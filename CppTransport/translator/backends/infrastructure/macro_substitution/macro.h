//
// Created by David Seery on 27/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
#include "index_literal.h"
#include "cse.h"
#include "translator_data.h"
#include "package_group.h"
#include "replacement_rule_definitions.h"
#include "token_list.h"

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
        
        // CONSTRUCTOR, DESTRUCTOR

      public:
    
        //! constructor splits std::string into its components
        split_string(const std::string& line, const std::string& split_equal,
                     const std::string& split_sum_equal);

        //! destructor is default
        ~split_string() = default;
        
        
        // INTERFACE
        
      public:
        
        //! get left-hand string
        const std::string& get_left() const { return this->left; }
        
        //! get right-hand string
        const std::string& get_right() const { return this->right; }
        
        //! get position of split point, if one exists
        size_t get_split_point() const { return this->split_point; }
        
        //! get type of split point
        split_type get_split_type() const { return this->type; }
        
        //! has a trailing comma on LHS?
        bool has_trailing_comma() const { return this->comma; }
        
        //! has a trailing semicolon on RHS?
        bool has_trailing_semicolon() const { return this->semicolon; }
        
        
        // INTERNAL DATA
        
      private:

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
        split_type type;

      };

  }


class macro_agent
	{

  public:

		//! constructor
		macro_agent(translator_data& p, package_group& pkg, std::string pf, std::string speq, std::string spsumeq,
		            unsigned int dm = DEFAULT_RECURSION_DEPTH);


		// INTERFACE

  public:

    //! apply macro substitution to a line, provided this does not bring the total number
    //! of recursive applications above the maximum
    std::unique_ptr< std::list<std::string> > apply(const std::string& line, unsigned int& replacements);

    //! tokenize a line; used internally, but also available as a service to clients which may need
    //! to inspect tokenized strings (eg. directive implementations)
    std::unique_ptr<token_list>
    tokenize(const std::string& line, boost::optional<index_literal_database&> validate_db=boost::none,
             bool strict=false);

    //! inject a new macro definition
    void inject_macro(std::reference_wrapper< macro_packages::replacement_rule_index > rule);


    // INTERFACE -- OUTPUT CONTROL

  public:

    //! temporarily disable output
    void disable_output() { this->output_enabled = false; }

    //! re-enable output
    void enable_output() { this->output_enabled = true; }
    
    //! query whether output is enabled
    bool is_enabled() const { return this->output_enabled; }


		// INTERFACE - STATISTICS

  public:

		//! get total time spent doing macro replacement
		boost::timer::nanosecond_type get_total_work_time() const { return(this->macro_apply_timer.elapsed().wall); }

		//! get time spent doing tokenization
		boost::timer::nanosecond_type get_tokenization_time() const { return(this->tokenization_timer.elapsed().wall); }


		// INTERNAL API

  protected:

    //! do the heavy lifting of applying macro substitution to a line
    std::unique_ptr< std::list<std::string> > apply_line(const std::string& line, unsigned int& replacements);

    //! perform tokenization of a split line
    std::pair< std::unique_ptr<token_list>, std::unique_ptr<token_list> >
    perform_tokenization(const macro_impl::split_string& split_result);

    //! construct error_context associated with split point
    error_context make_split_point_context(const std::string& line, const macro_impl::split_string& split_result);
    
    //! determine whether the current unroll policy mandates unrolling or roll-up of the given assignment set
    bool apply_unroll_policy(const token_list& left_tokens, const token_list& right_tokens,
                             const assignment_set& LHS_assignments, const assignment_set& RHS_assignments,
                             const error_context& ctx) const;
    
    //! report on replacement rules that influenced the application of the unroll policy
    void notify_unroll_outcome(const token_list& left_tokens, const token_list& right_tokens) const;


    // INTERNAL API -- VALIDATION

  protected:

    //! perform validation of a set of RHS indices
    void validate_RHS_indices(token_list& left_tokens, token_list& right_tokens);

    //! check number of occurrrences of each RHS index, and flag warnings for those that occur only once
    void validate_RHS_count(const index_literal_list& right_indices);

    //! check index positioning for nontrivial-metric cases
    void validate_RHS_variances(const index_literal_list& decls);


    // INTERNAL API -- HANDLE INDEX SET BY UNROLLING

  protected:

    //! unroll an index assignment
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

    // INTERNAL STATE

    //! output currently enabled?
    bool output_enabled;


    // TRANSLATOR-SUPPLIED PAYLOAD AND CONFIGURATION DATA

    //! data payload
    translator_data& data_payload;

    //! cache number of fields
    unsigned int fields;

    //! cache number of parameters
    unsigned int parameters;

    //! cache index ordering convention
    index_order order;

    //! current recursion depth
    unsigned int recursion_depth;

    //! maximum recursive replacement depth
    unsigned int recursion_max;


    // MACRO PACKAGES

    //! indexes to macro package
    package_group& package;

    //! local index rule cache, used for storing macros dynamically defined within the template
    //! (this still contains references to rule objects; the objects themselves are owned by the
    //! set_directive class within the package_group, which maintains a symbol table of
    //! user-defined macros)
    index_ruleset local_index_rules;


    // MACRO CONFIGURATION

    //! macro prefix string (usually '$')
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
