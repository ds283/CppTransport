//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include "boost/algorithm/string.hpp"

#include "macro.h"
#include "macro_tokenizer.h"


// **************************************************************************************


macro_agent::macro_agent(translator_data& p, package_group& pkg, std::string pf, std::string sp, unsigned int dm)
  : data_payload(p),
    prefix(std::move(pf)),
    split(std::move(sp)),
    recursion_max(dm),
    recursion_depth(0),
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


std::unique_ptr< std::list<std::string> > macro_agent::apply_line(std::string& line, unsigned int& replacements)
  {
    std::unique_ptr< std::list<std::string> > r_list = std::make_unique< std::list<std::string> >();

		// break the line at the split point, if it exists, to get a 'left-hand' side and a 'right-hand' side
    std::string left;
    std::string right;

		size_t split_point;
		if((split_point = line.find(this->split)) != std::string::npos)
			{
				left = line.substr(0, split_point);
				right = line.substr(split_point + this->split.size());
			}
		else  // no split-point; everything counts as the right-hand side
			{
				right = line;
			}

    // trim trailing white space on the right-hand side
    boost::algorithm::trim_right(right);

    // check if the last component is a semicolon
    // note std:string::back() and std::string::pop_back() require C++11
    bool semicolon = false;
    if(right.size() > 0 && right.back() == ';')
	    {
        semicolon = true;
        right.pop_back();
	    }

    // check if the last component is a comma
    bool comma = false;
    if(right.size() > 0 && right.back() == ',')
	    {
        comma = true;
        right.pop_back();
	    }

		tokenization_timer.resume();
    error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
		token_list left_tokens(left, this->prefix, this->fields, this->parameters, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache, err_context);
		token_list right_tokens(right, this->prefix, this->fields, this->parameters, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache, err_context);
		tokenization_timer.stop();

    // running total of number of macro replacements
    unsigned int counter = 0;

    // evaluate pre macros and cache the results
		// we'd like to do this only once if possible, because evaluation may be expensive
		counter += left_tokens.evaluate_macros(simple_macro_type::pre);
		counter += right_tokens.evaluate_macros(simple_macro_type::pre);

    // generate assignments for the LHS indices
    assignment_set LHS_assignments(left_tokens.get_indices());

		// LHS_assignments may be empty if there are no valid assignments (probably one of the index
		// ranges is empty). If there are no LHS indices then it will contain one empty vector
		// (the trivial index assignment, ie. no indices to assign).
		// It's important to distinguish these two cases!

		// generate an assignment for each RHS index
		// TODO: check that RHS assignments match the expected type for each macro (the previous implementation did this; the revised one doesn't yet)

    // first get RHS indices which are not also LHS indices
    index_abstract_list RHS_indices;
    try
      {
        RHS_indices = right_tokens.get_indices() - left_tokens.get_indices();
      }
    catch(index_exception& xe)
      {
        std::ostringstream msg;
        msg << ERROR_MACRO_LHS_RHS_MISMATCH << " '" << xe.what() << "'";
        err_context.error(msg.str());
      }

    assignment_set RHS_assignments(RHS_indices);

		if(LHS_assignments.size() > 0)
			{
        for(std::unique_ptr<assignment_list> LHS_assign : LHS_assignments)
			    {
				    // evaluate LHS macros on this index assignment
				    counter += left_tokens.evaluate_macros(*LHS_assign);
						counter += left_tokens.evaluate_macros(simple_macro_type::post);    // pre macros were evaluated earlier

						if(RHS_assignments.size() > 1)   // multiple RHS assignments
							{
						    // push the LHS evaluated on this assignment into the output list, if it is non-empty
						    if(left_tokens.size() > 0)
							    {
						        r_list->push_back(left_tokens.to_string());
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
                        err_context.error(msg.str());
                      }

						        counter += right_tokens.evaluate_macros(total_assignment);
						        counter += right_tokens.evaluate_macros(simple_macro_type::post);

								    // set up replacement right hand side; add trailing ; and , only if the LHS is empty
						        std::string this_line = right_tokens.to_string() + (left_tokens.size() == 0 && semicolon ? ";" : "") + (left_tokens.size() == 0 && comma ? "," : "");

						        r_list->push_back(this_line);
							    }

								// add a trailing ; and , if the LHS is nonempty
						    if(left_tokens.size() > 0 && r_list->size() > 0)
							    {
						        if(semicolon)
							        {
						            r_list->back() += ";";
							        }
						        if(comma)
							        {
						            r_list->back() += ",";
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
                    err_context.error(msg.str());
                  }

						    counter += right_tokens.evaluate_macros(total_assignment);
						    counter += right_tokens.evaluate_macros(simple_macro_type::post);

								// set up line with macro replacements, and add trailing ; and , if necessary
						    std::string full_line = left_tokens.to_string() + " " + right_tokens.to_string() + (semicolon ? ";" : "") + (comma ? "," : "");
								r_list->push_back(full_line);
							}
			    }
			}
		else
			{
				r_list->push_back(std::string("// Skipped: empty index range (LHS index set is empty)"));
			}

    replacements = counter;

    return(r_list);
	}
