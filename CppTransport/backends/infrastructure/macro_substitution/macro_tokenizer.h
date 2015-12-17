//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
        std::string to_string() const { return(conversion); }

        //! raise error
        void error(const std::string& msg);

        //! mark this token as silent for error reporting
        void mark_silent() { this->silent = true; }


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

    class text_token: public generic_token
      {

      public:

        //! constructor
        text_token(const std::string& l, error_context ec);

        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~text_token()
          {
          }

      };

    class free_index_token: public generic_token
      {

      public:

        //! constructor
        free_index_token(abstract_index_list::const_iterator& it, error_context ec);

        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~free_index_token()
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


        // INTERNAL DATA

      protected:

        abstract_index index;

      };

    class simple_macro_token: public generic_token
      {

      public:

        //! constructor
        simple_macro_token(const std::string& m, const macro_argument_list& a, const macro_packages::simple_rule& r, simple_macro_type t, error_context ec);

        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~simple_macro_token()
          {
          }


        // INTERFACE

      public:

        //! get type
        simple_macro_type get_type() const { return(this->type); }

        //! evaluate and cache the result
        void evaluate();


        // INTERNAL DATA

      protected:

        const std::string name;
        const macro_argument_list args;
        macro_packages::simple_rule rule;

        enum simple_macro_type type;

      };

    class index_macro_token: public generic_token
      {

      public:

        //! constructor
        index_macro_token(const std::string& m, const abstract_index_list i, const macro_argument_list& a, const macro_packages::index_rule& r, error_context ec);

        //! destructor
        virtual ~index_macro_token();


        // INTERFACE

      public:

        //! evaluate and cache the result given a list of index assignments
        void evaluate(const assignment_list& a);


        // INTERNAL DATA

      protected:

        const std::string          name;
        const macro_argument_list  args;
        const abstract_index_list  indices;
        macro_packages::index_rule rule;

        std::unique_ptr<cse_map>   state;

      };

  }   // namespace token_list_impl


class token_list
	{

  public:

		//! build a token list from an input string
    token_list(const std::string& input, const std::string& prefix,
               unsigned int nf, unsigned int np,
               const std::vector<macro_packages::simple_rule>& pre,
               const std::vector<macro_packages::simple_rule>& post,
               const std::vector<macro_packages::index_rule>& index,
               translator_data& d);

		// suppress default copy constructor
		token_list(const token_list& obj) = delete;

    //! destructor is default
		~token_list() = default;


    // INTERFACE -- METADATA

  public:

    //! get number of tokens
    unsigned int size() const { return(this->tokens.size()); }


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

		//! get list of indices identified during tokenization
		const abstract_index_list& get_indices() { return(this->indices); }


    // INTERFACE -- INDEX SET UNROLLING

  public:

    //! does this token set prevent, force or allow unrolling?
    enum unroll_behaviour unroll_status() const;


		// INTERNAL API

  protected:

		//! check no index list exists in the input string
    void check_no_index_list(const std::string& input, const std::string& candidate, size_t& position);

		//! check an argument list of correct size exists in the input string, and return it (tokenized)
		macro_argument_list get_argument_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_args);

		//! check an index list of correct size exists in the input string, and return it (tokenized)
    abstract_index_list get_index_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_indices, enum index_class range);

		//! check whether the current candidate is a potential match for a macro
		template <typename Rule>
		bool check_for_match(const std::string& candidate, const std::vector<Rule>& rule_list, bool allow_substring=true);

		//! find the replacement rule for the current candidate
		template <typename Rule>
		const Rule& find_match(const std::string& candidate, const std::vector<Rule>& rule_list);

		//! add an index to our internal list
		abstract_index_list::const_iterator add_index(char label);

		//! add an index to our internal list
		abstract_index_list::const_iterator add_index(const abstract_index& index, error_context& ctx);


    // INTERNAL DATA

  protected:


    // PAYLOAD DATA (provided to us by owning translator)

    //! tokenized input string; ownership is shared with any error contexts which we generate
    std::shared_ptr< std::string > input_string;

    //! reference to translator data payload
    translator_data& data_payload;


    // TOKEN DATA

		//! tokenized version of input
		std::list< std::unique_ptr< token_list_impl::generic_token > > tokens;

    //! auxiliary list of simple macro tokens
    std::list< token_list_impl::simple_macro_token* > simple_macro_tokens;

    //! auxiliary list of index macro tokens
    std::list< token_list_impl::index_macro_token* > index_macro_tokens;

    //! auxiliary list of free index tokens
    std::list< token_list_impl::free_index_token* > free_index_tokens;


    // MACRO DATA

    //! number of macros preventing loop unroll
    unsigned int num_prevent_unroll;

    //! number of macros forcing loop unroll
    unsigned int num_force_unroll;


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
