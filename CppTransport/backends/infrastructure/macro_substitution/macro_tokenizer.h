//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __macro_tokenizer_H_
#define __macro_tokenizer_H_

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


class token_list
	{

  public:

    class generic_token
	    {

      public:

        //! constructor
		    generic_token(const std::string& c);

        //! destructor is default
        virtual ~generic_token() = default;


        // TOKEN INTERFACE

      public:

		    //! convert this token to its string equivalent
        std::string to_string() const { return(conversion); }


		    // INTERNAL DATA

      protected:

		    //! converted value of this macro
		    std::string conversion;

	    };

    class text_token: public generic_token
	    {

      public:

        //! constructor
        text_token(const std::string& l);

        //! destructor is default
        virtual ~text_token() = default;

	    };

    class free_index_token: public generic_token
	    {

      public:

        //! constructor
        free_index_token(index_abstract_list::const_iterator& it);

        //! destructor is default
        virtual ~free_index_token() = default;

        // INTERFACE

      public:

        //! evaluate and cache the result given a list of index assignments
        void evaluate(const assignment_list& a);


		    // INTERNAL DATA

      protected:

		    index_abstract index;

	    };

    class simple_macro_token: public generic_token
	    {

      public:

        //! constructor
        simple_macro_token(const std::string& m, const macro_argument_list& a, const macro_packages::simple_rule& r, simple_macro_type t);

        //! destructor is default
        virtual ~simple_macro_token() = default;


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
		    index_macro_token(const std::string& m, const index_abstract_list i, const macro_argument_list& a, const macro_packages::index_rule& r);

        //! destructor is default
				virtual ~index_macro_token();


		    // INTERFACE

		  public:

		    //! evaluate and cache the result given a list of index assignments
		    void evaluate(const assignment_list& a);


		    // INTERNAL DATA

		  protected:

				const std::string name;
				const macro_argument_list args;
				const index_abstract_list indices;
				macro_packages::index_rule rule;

				void* state;

			};

  public:

		//! build a token list from an input string
    token_list(const std::string& input, const std::string& prefix,
               unsigned int nf, unsigned int np,
               const std::vector<macro_packages::simple_rule>& pre,
               const std::vector<macro_packages::simple_rule>& post,
               const std::vector<macro_packages::index_rule>& index,
               error_context& ec);

		// suppress default copy constructor
		token_list(const token_list& obj) = delete;

    //! destructor is default
		~token_list() = default;


		// INTERFACE

  public:

		//! convert to string form, using evaluated versions of each macro
		std::string to_string();

		//! get number of tokens
		unsigned int size() const { return(this->tokens.size()); }

		//! evaluate simple macros of a specific type, and cache the result.
		//! We only want to do this once if possible, since macro evaluation may be expensive.
		unsigned int evaluate_macros(simple_macro_type type);

		//! evaluate index macros and cache the result
		unsigned int evaluate_macros(const assignment_list& a);

		//! get list of indices identified during tokenization
		const index_abstract_list& get_indices() { return(this->indices); }


		// INTERNAL API

  protected:

		//! check no index list exists in the input string
    void check_no_index_list(const std::string& input, const std::string& candidate, size_t& position);

		//! check an argument list of correct size exists in the input string, and return it (tokenized)
		macro_argument_list get_argument_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_args);

		//! check an index list of correct size exists in the input string, and return it (tokenized)
		index_abstract_list get_index_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_indices, enum index_class range);

		//! check whether the current candidate is a potential match for a macro
		template <typename Rule>
		bool check_for_match(const std::string& candidate, const std::vector<Rule>& rule_list, bool allow_substring=true);

		//! find the replacement rule for the current candidate
		template <typename Rule>
		const Rule& find_match(const std::string& candidate, const std::vector<Rule>& rule_list);

		//! add an index to our internal list
		index_abstract_list::const_iterator add_index(char label);

		//! add an index to our internal list
		index_abstract_list::const_iterator add_index(const index_abstract& index);


    // INTERNAL DATA

  protected:

    //! reference to error context
    error_context& err_ctx;

		//! tokenized version of input
		std::list< std::unique_ptr<generic_token> > tokens;

    //! auxiliary list of simple macro tokens
    std::list< simple_macro_token* > simple_macro_tokens;

    //! auxiliary list of index macro tokens
    std::list< index_macro_token* > index_macro_tokens;

    //! auxiliary list of free index tokens
    std::list< free_index_token* > free_index_tokens;

		//! list of indices found in input
		index_abstract_list indices;

    //! cache number of fields
    unsigned int num_fields;

    //! cache number of parameters
    unsigned int num_params;

	};


#endif //__macro_tokenizer_H_
