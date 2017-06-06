//
// Created by David Seery on 11/03/15.
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


#ifndef CPPTRANSPORT_MACRO_TOKENIZER_H
#define CPPTRANSPORT_MACRO_TOKENIZER_H

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <functional>

#include "core.h"
#include "error_context.h"
#include "index_assignment.h"
#include "replacement_rule_definitions.h"
#include "macro_types.h"
#include "translator_data.h"


namespace std
  {

    // provide std::equal_to implementation for abstract_index
    template <>
    struct equal_to<abstract_index>
      {

        bool operator()(const abstract_index& lhs, const abstract_index& rhs) const
          {
            return (lhs.get_label() == rhs.get_label()) && (lhs.get_class() == rhs.get_class());
          }

      };

    // provide std::hash implementation for abstract_index, so it can be used in a std::unordered_map
    template <>
    struct hash<abstract_index>
      {

        size_t operator()(const abstract_index& idx) const
          {
            return static_cast<size_t>(idx.get_label());
          }

      };

  }   // namespace std


typedef std::unordered_map<abstract_index, abstract_index> index_remap_rule;


namespace token_list_impl
  {

    class generic_token
      {

      public:

        //! constructor
        generic_token(const std::string& c, error_context ec);


        //! destructor is default, but icpc fails with explictly-defaulted destructor
        virtual ~generic_token()
          {
          }


        // TOKEN INTERFACE

      public:

        //! convert this token to its string equivalent
        std::string to_string() const
          { return (conversion); }


        //! raise error
        void error(const std::string& msg);


        //! mark this token as silent for error reporting
        void mark_silent()
          { this->silent = true; }


        // INTERNAL DATA

      protected:

        //! converted value of this macro
        std::string conversion;

        //! context for error messages involving this token
        error_context err_ctx;

        //! number of errors raised by this token
        unsigned int num_errors;

        //! error reported silenced?
        bool silent;

      };

    class text_token : public generic_token
      {

      public:

        //! constructor
        text_token(const std::string& l, error_context ec);


        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~text_token()
          {
          }

      };

    class index_literal_token : public generic_token
      {

      public:

        //! constructor
        index_literal_token(abstract_index_list::const_iterator& it, error_context ec);


        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~index_literal_token()
          {
          }

        // INTERFACE

      public:

        //! evaluate on a specific assignment and cache the result;
        //! used when unrolling an index set
        void evaluate(const assignment_list& a);

        //! evaluate on an abstract index and cache the result; used when
        //! planting code as a 'for'-loop
        void evaluate();

        //! evaluate on an abstract index and cache the result, first performing
        //! substitution according to an index remapping rule
        void evaluate(const index_remap_rule& rule);


        // INTERNAL DATA

      protected:

        //! record index
        abstract_index index;

      };

    class simple_macro_token : public generic_token
      {

      public:

        //! constructor
        simple_macro_token(const std::string& m, const macro_argument_list& a,
                           macro_packages::replacement_rule_simple& r, simple_macro_type t, error_context ec);


        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~simple_macro_token()
          {
          }


        // INTERFACE

      public:

        //! get type
        simple_macro_type get_type() const
          { return (this->type); }


        //! evaluate and cache the result
        void evaluate();


        // INTERNAL DATA

      protected:

        const std::string name;
        const macro_argument_list args;
        macro_packages::replacement_rule_simple& rule;

        simple_macro_type type;

        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;

      };

    class index_macro_token : public generic_token
      {

      public:

        //! constructor
        index_macro_token(const std::string& m, const abstract_index_list i, const macro_argument_list& a,
                          macro_packages::replacement_rule_index& r, error_context ec);

        //! destructor
        virtual ~index_macro_token();


        // INTERFACE

      public:

        //! evaluate (and cache the result) given a list of concrete index assignments;
        //! used to replace this macro while unrolling an index set
        void evaluate_unroll(const assignment_list& a);

        //! evaluate (and cache the result) on an abstract index assignment;
        //! used to replace this macro while handling an index set by
        //! rolling up into a for-loop
        void evaluate_roll();

        //! evaluate (and cache the result) on an abstract index assignment determined by the given
        //! substitution rule
        void evaluate_roll(const index_remap_rule& rule);

        //! call post-hook and reset initialization status
        void reset();


        // INTERNAL DATA

      protected:

        //! name of this macro token
        const std::string name;

        //! argument list for this token
        const macro_argument_list args;

        //! index list for this token
        const abstract_index_list indices;

        //! reference to replacement rule object for this token
        macro_packages::replacement_rule_index& rule;

        //! flag to determine whether 'pre' handler has been called
        bool initialized;

        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;

        //! have index-related errors been reported yet? if so, silence further errors
        bool index_error;

      };

  }   // namespace token_list_impl


// forward-declare package_group, if needed
class package_group;


class token_list
  {

  public:

    //! build a token list from an input string
    token_list(const std::string& input, const std::string& prefix, unsigned int nf, unsigned int np,
               const package_group& package, const index_ruleset& local_rules, translator_data& d);

    // suppress default copy constructor
    token_list(const token_list& obj) = delete;

    //! destructor is default
    ~token_list() = default;


    // INTERFACE -- UTILITIES

  public:

    //! get number of tokens
    unsigned int size() const
      { return (this->tokens.size()); }


    //! reset token list to pristine state, as if no evaluations had taken place
    //! principally used to force call to post-hooks and reset pre-hooks
    void reset();


    // INTERFACE -- MACRO EVALUATION AND CONVERSION

  public:

    //! convert to string form, using evaluated versions of each macro
    std::string to_string();

    //! evaluate simple macros of a specific type, and cache the result.
    //! We only want to do this once if possible, since macro evaluation may be expensive.
    unsigned int evaluate_macros(simple_macro_type type);

    //! evaluate index macros on a specific assignment and cache the result;
    //! used when unrolling index sets
    unsigned int evaluate_macros(const assignment_list& a);

    //! evaluate index macros on an abstract assignment and cache the result;
    //! used when planting code as a 'for'-loop
    unsigned int evaluate_macros();

    //! evaluate index macros on an abstract assignment determined by the given substitution rule
    unsigned int evaluate_macros(const index_remap_rule& rule);


    //! get list of indices identified during tokenization
    const abstract_index_list& get_indices()
      { return (this->indices); }


    // INTERFACE -- INDEX SET UNROLLING

  public:

    //! does this token set prevent, force or allow unrolling?
    unroll_behaviour unroll_status() const;


    // INTERNAL API

  protected:

    //! tokenize a macro or free index
    template <typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::generic_token>, size_t>
    match_macro_or_index(const std::string& input, const size_t position, const pre_ruleset& pre,
                             const post_ruleset& post, const index_ruleset& index, const index_ruleset& local_rules,
                             ContextFactory make_context);

    //! build an index literal token
    template <typename ContextFactory>
    std::unique_ptr<token_list_impl::index_literal_token>
    make_index_literal(const std::string& input, const size_t position, ContextFactory make_context);

    //! build a simple macro token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::simple_macro_token>, size_t>
    make_simple_macro(const std::string& input, const std::string& macro, const size_t position,
                      const RuleSet& rules, simple_macro_type type, ContextFactory make_context);

    //! build an index macro token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::index_macro_token>, size_t>
    make_index_macro(const std::string& input, const std::string& macro, const size_t position,
                     const RuleSet& rules, ContextFactory make_context);

    //! add an index to our internal list
    abstract_index_list::const_iterator add_index(char label);

    //! add an index to our internal list
    abstract_index_list::const_iterator add_index(const abstract_index& index, error_context& ctx);


    // INTERNAL DATA

  protected:


    // PAYLOAD DATA (provided to us by owning translator)

    //! tokenized input string; ownership is shared with any error contexts which we generate
    std::shared_ptr<std::string> input_string;

    //! reference to translator data payload
    translator_data& data_payload;


    // TOKEN DATA

    //! tokenized version of input
    std::list<std::unique_ptr<token_list_impl::generic_token> > tokens;

    //! auxiliary list of simple macro tokens
    std::list< std::reference_wrapper<token_list_impl::simple_macro_token> > simple_macro_tokens;

    //! auxiliary list of index macro tokens
    std::list< std::reference_wrapper<token_list_impl::index_macro_token> > index_macro_tokens;

    //! auxiliary list of index literal
    std::list< std::reference_wrapper<token_list_impl::index_literal_token> > index_literal_tokens;


    // MACRO DATA

    //! list of macro names preventing loop unroll
    std::list<std::string> prevent_unroll;

    //! list of macro names forcing loop unroll
    std::list<std::string> force_unroll;


    // INDEX DATA (maintains information about indices encountered in this entire line)

    //! list of indices found in input
    abstract_index_list indices;


    // CACHE DATA ABOUT MODEL

    //! cache number of fields
    unsigned int num_fields;

    //! cache number of parameters
    unsigned int num_params;

  };

#endif //CPPTRANSPORT_MACRO_TOKENIZER_H
