//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACRO_H
#define CPPTRANSPORT_MACRO_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

#include "core.h"
#include "index_assignment.h"
#include "cse.h"
#include "translator_data.h"
#include "package_group.h"
#include "replacement_rule_definitions.h"
#include "macro_tokenizer.h"

#include "boost/timer/timer.hpp"


// package_group.h includes macro.h
// macro.h includes package_group.h

// if we were included from package_group.h then no declaration for package_group will yet have been seen,
// so forward-declare here
class package_group;


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
    std::unique_ptr< std::list<std::string> > apply(const std::string& line, unsigned int& replacements);


		// INTERFACE - STATISTICS

  public:

		// get total time spent doing macro replacement
		boost::timer::nanosecond_type get_total_work_time() const { return(this->macro_apply_timer.elapsed().wall); }

		// get time spent doing tokenization
		boost::timer::nanosecond_type get_tokenization_time() const { return(this->tokenization_timer.elapsed().wall); }


		// INTERNAL API

  protected:

    // do the heavy lifting of applying macro substitution to a line
    std::unique_ptr< std::list<std::string> > apply_line(const std::string& line, unsigned int& replacements);

    //! find a split-point in a line, if one exists
    macro_impl::split_string split(const std::string& line);


    // INTERNAL API -- HANDLE INDEX SET BY UNROLLING
    //! unroll an index assignment

  protected:

    void unroll_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                 assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                 unsigned int& counter, macro_impl::split_string& split_result,
                                 error_context& ctx, std::list<std::string>& r_list);


    // INTERNAL API -- HANDLE INDEX SET BY FOR-LOOP

  protected:

    //! plant a for-loop implementation of an index assignment
    void forloop_index_assignment(token_list& left_tokens, token_list& right_tokens,
                                  assignment_set& LHS_assignments, assignment_set& RHS_assignments,
                                  unsigned int& counter, macro_impl::split_string& split_result,
                                  error_context& ctx, std::list<std::string>& r_list);

    //! compute leading indentation for planting for-loop code
    std::string compute_prefix(macro_impl::split_string& split_string);

    //! plant code representing LHS for-loops (recursive)
    void plant_LHS_forloop(index_database<abstract_index>::const_iterator current,
                           index_database<abstract_index>::const_iterator end,
                           assignment_set& RHS_assignments, token_list& left_tokens, token_list& right_tokens,
                           unsigned int& counter, macro_impl::split_string& split_result, error_context& ctx,
                           std::list<std::string>& r_list, const std::string& raw_indent, unsigned int current_indent);

    //! plant code representing RHS for-loops (recursive)
    void plant_RHS_forloop(index_database<abstract_index>::const_iterator current,
                           index_database<abstract_index>::const_iterator end,
                           token_list& left_tokens, token_list& right_tokens, unsigned int& counter,
                           macro_impl::split_string& split_result, error_context& ctx,
                           std::list<std::string>& r_list, const std::string& raw_indent, unsigned int current_indent,
                           bool coalesce);

    //! format an output string with correct indents
    std::string dress(std::string out_str, const std::string& raw_indent, unsigned int current_indent);


    // INTERNAL DATA

  private:


    // TRANSLATOR-SUPPLIED PAYLOAD AND CONFIGURATION DATA

    //! data payload
    translator_data& data_payload;

    //! cache number of fields
    unsigned int fields;

    //! cache number of parameters
    unsigned int parameters;

    //! cache index ordering convention
    enum index_order order;

    //! current recursion depth
    unsigned int recursion_depth;

    //! maximum recursive replacement depth
    unsigned int recursion_max;


    // MACRO PACKAGES

    //! indexes to macro package
    package_group& package;

    //! cache pre-rules
    std::vector<macro_packages::replacement_rule_simple*>& pre_rule_cache;

    //! cache post-rules
    std::vector<macro_packages::replacement_rule_simple*>& post_rule_cache;

    //! cache index rules
    std::vector<macro_packages::replacement_rule_index*>& index_rule_cache;


    // MACRO CONFIGURATION

    //! macro prefix string (usually '$$__')
    const std::string prefix;

    //! split-equality symbol (usually '$$=')
    const std::string split_equal;

    //! split-sum-equals symbol (usually '$$+=')
    const std::string split_sum_equal;


    // TIMERS

		// timer to measure performance during macro replacement
    boost::timer::cpu_timer macro_apply_timer;

		// measure time spent tokenizing
    boost::timer::cpu_timer tokenization_timer;

  };


#endif //CPPTRANSPORT_MACRO_H
