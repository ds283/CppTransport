//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <cctype>

#include "boost/algorithm/string.hpp"

#include "macro.h"


// **************************************************************************************


macro_agent::macro_agent(translator_data& p, package_group& pkg, std::string pf, std::string speq, std::string spsumeq, unsigned int dm)
  : data_payload(p),
    prefix(std::move(pf)),
    split_equal(std::move(speq)),
    split_sum_equal(std::move(spsumeq)),
    recursion_max(dm),
    recursion_depth(0),
    package(pkg),
    pre_rule_cache(pkg.get_pre_ruleset()),
    post_rule_cache(pkg.get_post_ruleset()),
    index_rule_cache(pkg.get_index_ruleset())
  {
    assert(recursion_max > 0);

		// pause timers
		timer.stop();
		tokenization_timer.stop();

    if(recursion_max == 0) recursion_max = 1;

    fields     = data_payload.get_number_fields();
    parameters = data_payload.get_number_parameters();
    order      = data_payload.get_index_order();
  }


std::unique_ptr< std::list<std::string> > macro_agent::apply(std::string& line, unsigned int& replacements)
  {
		// if timer is stopped, restart it
		bool stopped = this->timer.is_stopped();
		if(stopped) timer.resume();

		// the result of macro substitution is potentially large, and we'd rather not copy
		// a very large array of strings while moving the result around.
		// So, use a std::unique_ptr<> to manage the result object
    std::unique_ptr< std::list<std::string> > r_list;

    if(++this->recursion_depth < this->recursion_max)
      {
        r_list = this->apply_line(line, replacements);
        --this->recursion_depth;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_RECURSION_DEPTH << "=" << this->recursion_max << ")";

        error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        err_context.warn(msg.str());
      }

		// if timer was stopped, stop it again
		if(stopped) timer.stop();

    return(r_list);
  }


std::unique_ptr< std::list<std::string> > macro_agent::apply_line(const std::string& line, unsigned int& replacements)
  {
    std::unique_ptr< std::list<std::string> > r_list = std::make_unique<std::list<std::string> >();

    // break the line at the split point, if it exists, to get a 'left-hand' side and a 'right-hand' side
    macro_impl::split_string split_result = this->split(line);

    tokenization_timer.resume();
    token_list left_tokens(split_result.left, this->prefix, this->fields, this->parameters, this->pre_rule_cache,
                           this->post_rule_cache, this->index_rule_cache, this->data_payload);
    token_list right_tokens(split_result.right, this->prefix, this->fields, this->parameters, this->pre_rule_cache,
                            this->post_rule_cache, this->index_rule_cache, this->data_payload);
    tokenization_timer.stop();

    // running total of number of macro replacements
    unsigned int counter = 0;

    // evaluate pre macros and cache the results
    // we'd like to do this only once if possible, because evaluation may be expensive
    counter += left_tokens.evaluate_macros(simple_macro_type::pre);
    counter += right_tokens.evaluate_macros(simple_macro_type::pre);

    // generate assignments for the LHS indices
    assignment_set LHS_assignments(left_tokens.get_indices());

    // generate an assignment for each RHS index; first get RHS indices which are not also LHS indices
    abstract_index_list RHS_indices;
    error_context ctx(this->data_payload.get_stack(), this->data_payload.get_error_handler(),
                      this->data_payload.get_warning_handler());

    try
      {
        RHS_indices = right_tokens.get_indices() - left_tokens.get_indices();
      }
    catch(index_exception& xe)
      {
        std::ostringstream msg;
        msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
        ctx.error(msg.str());
      }

    assignment_set RHS_assignments(RHS_indices);

    // decide on total size of assignment set and apply policy for unrolling
    unsigned int total_assignment_size = LHS_assignments.size() * RHS_assignments.size();
    bool unroll_by_policy = total_assignment_size <= this->data_payload.unroll_policy();
    bool unroll = unroll_by_policy;

    unsigned int prevent = 0;
    unsigned int force = 0;
    if(left_tokens.unroll_status() == unroll_behaviour::prevent) ++prevent;
    if(right_tokens.unroll_status() == unroll_behaviour::prevent) ++prevent;
    if(left_tokens.unroll_status() == unroll_behaviour::force) ++force;
    if(right_tokens.unroll_status() == unroll_behaviour::force) ++force;

    if(force > 0 && prevent > 0)
      {
        ctx.error(ERROR_LHS_RHS_INCOMPATIBLE_UNROLL);
      }
    else
      {
        if(force > 0 || (unroll_by_policy && prevent == 0)) unroll = true;
        else unroll = false;
      }

    if(unroll)
      {
        this->unroll_index_assignment(left_tokens, right_tokens, LHS_assignments, RHS_assignments, counter,
                                      split_result, ctx, *r_list);
      }
    else
      {
        this->forloop_index_assignment(left_tokens, right_tokens, LHS_assignments, RHS_assignments, counter,
                                       split_result, ctx, *r_list);
      }

    replacements = counter;

    return(r_list);
  }


void macro_agent::unroll_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                          assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                          unsigned int& counter, macro_impl::split_string& split_result,
                                          error_context& ctx, std::list<std::string>& r_list)
  {
    // LHS_assignments may be *empty* (ie size=0) if there are no valid assignments (probably one of the index
    // ranges is empty).
    // If there are no LHS indices then it will report size=1, ie. the trivial index assignment
    // It's important to distinguish these two cases!

		if(LHS_assignments.size() > 0)
			{
        // compute raw indent for LHS
        std::string raw_indent = this->compute_prefix(split_result);

        for(std::unique_ptr<assignment_list> LHS_assign : LHS_assignments)
			    {
				    // evaluate LHS macros on this index assignment;
            // only need index- and post-macros; pre-macros were evaluated earlier
				    counter += left_tokens.evaluate_macros(*LHS_assign);
						counter += left_tokens.evaluate_macros(simple_macro_type::post);

						if(RHS_assignments.size() > 1)   // multiple RHS assignments
							{
						    // push the LHS evaluated on this assignment into the output list, if it is non-empty
						    if(left_tokens.size() > 0)
							    {
                    std::string lhs = left_tokens.to_string();
                    if(split_result.type == macro_impl::split_type::sum)       lhs += " = ";
                    if(split_result.type == macro_impl::split_type::sum_equal) lhs += " += ";
						        r_list.push_back(lhs);
							    }

						    // now generate a set of RHS evaluations for this LHS evaluation
                for(std::unique_ptr<assignment_list> RHS_assign : RHS_assignments)
							    {
                    assignment_list total_assignment;
                    try
                      {
                        total_assignment = *LHS_assign + *RHS_assign;
                      }
                    catch(index_exception& xe)
                      {
                        std::ostringstream msg;
                        msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
                        ctx.error(msg.str());
                      }

						        counter += right_tokens.evaluate_macros(total_assignment);
						        counter += right_tokens.evaluate_macros(simple_macro_type::post);

								    // set up replacement right hand side; add trailing ; and , only if the LHS is empty
						        std::string this_line = right_tokens.to_string() + (left_tokens.size() == 0 && split_result.semicolon ? ";" : "") + (left_tokens.size() == 0 && split_result.comma ? "," : "");

                    if(left_tokens.size() == 0)   // no need to format for indentation if no LHS; RHS will already include indentation
                      {
                        r_list.push_back(this_line);
                      }
                    else
                      {
                        r_list.push_back(this->dress(this_line, raw_indent, 3));
                      }
							    }

								// add a trailing ; and , if the LHS is nonempty
						    if(left_tokens.size() > 0 && r_list.size() > 0)
							    {
						        if(split_result.semicolon)
							        {
						            r_list.back() += ";";
							        }
						        if(split_result.comma)
							        {
						            r_list.back() += ",";
							        }
							    }
							}
						else if(RHS_assignments.size() == 1)  // just one RHS assignment, so coalesce with LHS
							{
                std::unique_ptr<assignment_list> RHS_assign = *RHS_assignments.begin();

                assignment_list total_assignment;
                try
                  {
                    total_assignment = *LHS_assign + *RHS_assign;
                  }
                catch(index_exception& xe)
                  {
                    std::ostringstream msg;
                    msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
                    ctx.error(msg.str());
                  }

						    counter += right_tokens.evaluate_macros(total_assignment);
						    counter += right_tokens.evaluate_macros(simple_macro_type::post);

								// set up line with macro replacements, and add trailing ; and , if necessary;
                // since the line includes the full LHS it needs no special formatting to account for indentation
						    std::string full_line = left_tokens.to_string();
                if(split_result.type == macro_impl::split_type::sum)       full_line += " =";
                if(split_result.type == macro_impl::split_type::sum_equal) full_line += " +=";

                full_line += (left_tokens.size() > 0 ? " " : "") + right_tokens.to_string() + (split_result.semicolon ? ";" : "") + (split_result.comma ? "," : "");
								r_list.push_back(full_line);
							}
			    }
			}
		else
			{
				r_list.push_back(std::string("// Skipped: empty index range (LHS index set is empty)"));
			}
	}


void macro_agent::forloop_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                           assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                           unsigned int& counter, macro_impl::split_string& split_result,
                                           error_context& ctx, std::list<std::string>& r_list)
  {
    std::string raw_indent = this->compute_prefix(split_result);
    unsigned int current_indent = 0;

    if(LHS_assignments.size() > 0)
      {
        this->plant_LHS_forloop(LHS_assignments.idx_set_begin(), LHS_assignments.idx_set_end(),
                                RHS_assignments, left_tokens, right_tokens, counter, split_result,
                                ctx, r_list, raw_indent, current_indent);
      }
    else
      {
        r_list.push_back(std::string("// Skipped: empty index range (LHS index set is empty)"));
      }
  }


macro_impl::split_string macro_agent::split(const std::string& line)
  {
    macro_impl::split_string rval;

    // check for sum split point
    if((rval.split_point = line.find(this->split_equal)) != std::string::npos)
      {
        rval.left  = line.substr(0, rval.split_point);
        rval.right = line.substr(rval.split_point + this->split_equal.length());
        rval.type  = macro_impl::split_type::sum;
      }
    else    // not sum, but possibly sum-equals?
      {
        if((rval.split_point = line.find(this->split_sum_equal)) != std::string::npos)
          {
            rval.left  = line.substr(0, rval.split_point);
            rval.right = line.substr(rval.split_point + this->split_sum_equal.length());
            rval.type  = macro_impl::split_type::sum_equal;
          }
        else    // no split-point; everything counts as the right-hand side
          {
            rval.right = line;
            rval.type  = macro_impl::split_type::none;
          }
      }

    // trim trailing white space on the right-hand side,
    // and leading white space if there is a nontrivial left-hand side
    boost::algorithm::trim_right(rval.right);
    if(rval.left.length() > 0) boost::algorithm::trim_left(rval.right);

    // check if the last component is a semicolon
    // note std:string::back() and std::string::pop_back() require C++11
    if(rval.right.size() > 0 && rval.right.back() == ';')
      {
        rval.semicolon = true;
        rval.right.pop_back();
      }

    // check if the last component is a comma
    if(rval.right.size() > 0 && rval.right.back() == ',')
      {
        rval.comma = true;
        rval.right.pop_back();
      }

    return(rval);
  }


std::string macro_agent::compute_prefix(macro_impl::split_string& split_string)
  {
    std::string prefix;
    size_t pos = 0;

    std::string raw = (split_string.left.length() > 0) ? split_string.left : split_string.right;

    while(pos < raw.length() && std::isspace(raw[pos]))
      {
        prefix += raw[pos];
        ++pos;
      }

    return(prefix);
  }


void macro_agent::plant_LHS_forloop(index_database<abstract_index>::const_iterator current,
                                    index_database<abstract_index>::const_iterator end,
                                    assignment_set& RHS_assignments, token_list& left_tokens, token_list& right_tokens,
                                    unsigned int& counter, macro_impl::split_string& split_result, error_context& ctx,
                                    std::list<std::string>& r_list, const std::string& raw_indent,
                                    unsigned int current_indent)
  {
    if(current == end)
      {
        this->plant_RHS_forloop(RHS_assignments.idx_set_begin(), RHS_assignments.idx_set_end(), left_tokens,
                                right_tokens, counter, split_result, ctx, r_list, raw_indent, current_indent);
      }
    else
      {
        language_printer& printer = this->package.get_language_printer();
        boost::optional< std::string > start_delimiter = printer.get_start_block_delimiter();
        boost::optional< std::string > end_delimiter = printer.get_end_block_delimiter();

        r_list.push_back(this->dress(printer.plant_for_loop(current->get_loop_variable(), 0, current->numeric_range()), raw_indent, current_indent));

        std::ostringstream brace_indent;
        for(unsigned int i = 0; i < printer.get_block_delimiter_indent(); ++i) brace_indent << " ";

        if(start_delimiter) r_list.push_back(this->dress(brace_indent.str() + *start_delimiter, raw_indent, current_indent));
        this->plant_LHS_forloop(++current, end, RHS_assignments, left_tokens, right_tokens, counter, split_result, ctx,
                                r_list, raw_indent, current_indent + printer.get_block_indent());
        if(end_delimiter) r_list.push_back(this->dress(brace_indent.str() + *end_delimiter, raw_indent, current_indent));
      }
  }


void macro_agent::plant_RHS_forloop(index_database<abstract_index>::const_iterator current,
                                    index_database<abstract_index>::const_iterator end,
                                    token_list& left_tokens, token_list& right_tokens, unsigned int& counter,
                                    macro_impl::split_string& split_result, error_context& ctx,
                                    std::list<std::string>& r_list, const std::string& raw_indent,
                                    unsigned int current_indent)
  {
    if(current == end)
      {
        std::string this_line = left_tokens.to_string() + " " + right_tokens.to_string() + (left_tokens.size() == 0 && split_result.semicolon ? ";" : "") + (left_tokens.size() == 0 && split_result.comma ? "," : "");
        r_list.push_back(this->dress(this_line, raw_indent, current_indent));
      }
    else
      {
        language_printer& printer = this->package.get_language_printer();
        boost::optional< std::string > start_delimiter = printer.get_start_block_delimiter();
        boost::optional< std::string > end_delimiter = printer.get_end_block_delimiter();

        r_list.push_back(this->dress(printer.plant_for_loop(current->get_loop_variable(), 0, current->numeric_range()), raw_indent, current_indent));

        std::ostringstream brace_indent;
        for(unsigned int i = 0; i < printer.get_block_delimiter_indent(); ++i) brace_indent << " ";

        if(start_delimiter) r_list.push_back(this->dress(brace_indent.str() + *start_delimiter, raw_indent, current_indent));
        this->plant_RHS_forloop(++current, end, left_tokens, right_tokens, counter, split_result, ctx, r_list,
                                raw_indent, current_indent + printer.get_block_indent());
        if(end_delimiter) r_list.push_back(this->dress(brace_indent.str() + *end_delimiter, raw_indent, current_indent));
      }
  }


std::string macro_agent::dress(std::string out_str, const std::string& raw_indent, unsigned int current_indent)
  {
    std::ostringstream out;

    out << raw_indent;
    for(unsigned int i = 0; i < current_indent; ++i) out << " ";

    out << out_str;
    return(out.str());
  }
