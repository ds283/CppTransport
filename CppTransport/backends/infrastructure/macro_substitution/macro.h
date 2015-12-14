//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __macro_H_
#define __macro_H_

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

#include "core.h"
#include "index_assignment.h"
#include "u_tensor_factory.h"
#include "cse.h"
#include "flatten.h"
#include "package_group.h"
#include "translator_data.h"
#include "replacement_rule_definitions.h"
#include "macro_tokenizer.h"

#include "boost/timer/timer.hpp"


class package_group;


namespace macro_packages
	{
    class simple_rule;

    class index_rule;
	}


namespace macro_impl
  {

    enum class split_type
      {
        none,
        sum,
        sum_equal
      };

    //! hold the result of breaking an input line at a split-point marker, if one is present
    class split_string
      {

      public:

        //! constructor applies sensible defaults
        split_string()
          : split_point(0),
            comma(false),
            semicolon(false),
            type(split_type::none)
          {
          }

        //! destructor is default
        ~split_string() = default;

        //! left-hand side
        std::string left;

        //! right-hand side
        std::string right;

        //! split point, if exists
        size_t split_point;

        //! does the right-hand side have a trailing comma?
        bool comma;

        //! does the right-hand side have a trailing semicolon?
        bool semicolon;

        //! split type
        enum split_type type;

      };

  }


class macro_agent
	{

  public:

		// constructor
		macro_agent(translator_data& p, package_group& pkg, std::string pf, std::string speq, std::string spsumeq,
		            unsigned int dm = DEFAULT_RECURSION_DEPTH);


		// INTERFACE

  public:

    // apply macro substitution to a line, provided this does not bring the total number
    // of recursive applications above the maximum
    std::unique_ptr< std::list<std::string> > apply(std::string& line, unsigned int& replacements);


		// INTERFACE - STATISTICS

  public:

		// get total time spent doing macro replacement
		boost::timer::nanosecond_type get_total_time() const { return(this->timer.elapsed().wall); }

		// get time spent doing tokenization
		boost::timer::nanosecond_type get_tokenization_time() const { return(this->tokenization_timer.elapsed().wall); }


		// INTERNAL API

  protected:

    // do the heavy lifting of applying macro substitution to a line
    std::unique_ptr< std::list<std::string> > apply_line(const std::string& line, unsigned int& replacements);

    //! find a split-point in a line, if one exists
    macro_impl::split_string split(const std::string& line);

    //! unroll an index assignment
    void unroll_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                 assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                 unsigned int& counter, macro_impl::split_string& split_result,
                                 error_context& ctx, std::list<std::string>& r_list);

    //! plant a for-loop implementation of an index assignment
    void forloop_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                  assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                  unsigned int& counter, macro_impl::split_string& split_result,
                                  error_context& ctx, std::list<std::string>& r_list);


    // INTERNAL DATA

  private:

    unsigned int                              fields;
    unsigned int                              parameters;
    enum indexorder                           order;

    unsigned int                              recursion_depth;
    unsigned int                              recursion_max;

    translator_data&                          data_payload;

    std::vector<macro_packages::simple_rule>& pre_rule_cache;
    std::vector<macro_packages::simple_rule>& post_rule_cache;
    std::vector<macro_packages::index_rule>&  index_rule_cache;

    const std::string                         prefix;
    const std::string                         split_equal;
    const std::string                         split_sum_equal;

		// timer to measure performance during macro replacement
		boost::timer::cpu_timer                   timer;

		// measure time spent tokenizing
		boost::timer::cpu_timer                   tokenization_timer;

	};


#endif //__macro_H_
