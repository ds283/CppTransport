//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <ctype.h>

#include "boost/algorithm/string.hpp"

#include "macro.h"
#include "macro_tokenizer.h"


// **************************************************************************************


macro_package::macro_package(translation_unit* u, package_group* pkg, std::string pf, std::string sp, unsigned int dm)
  : unit(u), package(pkg), prefix(pf), split(sp),
    recursion_max(dm), recursion_depth(0),
    pre_rule_cache(pkg->get_pre_ruleset()), post_rule_cache(pkg->get_post_ruleset()), index_rule_cache(pkg->get_index_ruleset())
  {
    assert(unit != nullptr);
    assert(package != nullptr);
    assert(recursion_max > 0);

    if(recursion_max == 0) recursion_max = 1;

    fields     = unit->get_number_fields();
    parameters = unit->get_number_parameters();
    order      = unit->get_index_order();
  }


std::shared_ptr< std::vector<std::string> > macro_package::apply(std::string& line, unsigned int& replacements)
  {
		// set up CPU timer to measure how long execution takes
    boost::timer::cpu_timer timer;

    std::shared_ptr< std::vector<std::string> > r_list(new std::vector<std::string>());

    if(++this->recursion_depth < this->recursion_max)
      {
        r_list = this->apply_line(line, replacements, timer);
        --this->recursion_depth;
      }
    else
      {
        std::ostringstream msg;
        msg << WARNING_RECURSION_DEPTH << "=" << this->recursion_max << ")";
        this->package->warn(msg.str());
      }

    return(r_list);
  }


std::shared_ptr< std::vector<std::string> > macro_package::apply_line(std::string& line, unsigned int& replacements, boost::timer::cpu_timer& timer)
  {
    std::shared_ptr< std::vector<std::string> > r_list(new std::vector<std::string>());

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

		token_list left_tokens(left, this->prefix, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache);
		token_list right_tokens(right, this->prefix, this->pre_rule_cache, this->post_rule_cache, this->index_rule_cache);

    unsigned int counter = 0;

    // evaluate pre macros and cache the results
		// we'd like to do this only once if possible, because evaluation may be expensive
		counter += left_tokens.evaluate_macros(token_list::simple_macro_token::pre);
		counter += right_tokens.evaluate_macros(token_list::simple_macro_token::pre);

    // generate an assignment for each LHS index
    assignment_package assigner(this->fields, this->parameters, this->order);
    std::vector< std::vector<struct index_assignment> > LHS_assignments = assigner.assign(left_tokens.get_indices());

		// LHS_assignments may be empty if there are no valid assignments (probably one of the index
		// ranges is empty). If there are no LHS indices then it will contain one empty vector
		// (the trivial index assignment, ie. no indices to assign).
		// It's important to distinguish these two cases!

		// generate an assignment for each RHS index
    std::vector<index_abstract> RHS_indices = assigner.difference(right_tokens.get_indices(), left_tokens.get_indices());
    std::vector< std::vector<struct index_assignment> > RHS_assignments = assigner.assign(RHS_indices);

		if(LHS_assignments.size() > 0)
			{
				for(std::vector< std::vector<struct index_assignment> >::iterator t = LHS_assignments.begin(); t != LHS_assignments.end(); t++)
			    {
				    // evaluate LHS macros on this index assignment
				    counter += left_tokens.evaluate_macros(*t);
						counter += left_tokens.evaluate_macros(token_list::simple_macro_token::post);

						if(RHS_assignments.size() > 1)   // multiple RHS assignments
							{
						    // push the LHS evaluated on this assignment into the output list, if it is non-empty
						    if(left_tokens.size() > 0)
							    {
						        r_list->push_back(left_tokens.to_string());
							    }

						    // now generate a set of RHS evaluations for this LHS evaluation
						    for(std::vector< std::vector<struct index_assignment> >::iterator u = RHS_assignments.begin(); u != RHS_assignments.end(); u++)
							    {
						        std::vector<index_assignment> LHS_RHS_assignment = assigner.merge(*t, *u);

						        counter += right_tokens.evaluate_macros(LHS_RHS_assignment);
						        counter += right_tokens.evaluate_macros(token_list::simple_macro_token::post);

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
						    std::vector<index_assignment> LHS_RHS_assignment = assigner.merge(*t, RHS_assignments.front());

						    counter += right_tokens.evaluate_macros(LHS_RHS_assignment);
						    counter += right_tokens.evaluate_macros(token_list::simple_macro_token::post);

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

    return(r_list);
	}

