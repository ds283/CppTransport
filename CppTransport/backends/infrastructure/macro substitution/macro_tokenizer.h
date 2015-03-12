//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __macro_tokenizer_H_
#define __macro_tokenizer_H_

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

#include "core.h"
#include "index_assignment.h"
#include "replacement_rule_definitions.h"


class token_list
	{

  public:

    class generic_token
	    {

      public:

		    generic_token(const std::string& c);

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

        text_token(const std::string& l);

        virtual ~text_token() = default;

	    };

    class free_index_token: public generic_token
	    {

      public:

        free_index_token(const struct index_abstract& i);

		    free_index_token(char label)
		      : generic_token(std::string(1, label))
			    {
				    index.label = label;
				    index.range = identify_index(label);
			    }

		    virtual ~free_index_token() = default;


        // INTERFACE

      public:

        //! evaluate and cache the result given a list of index assignments
        void evaluate(const std::vector<index_assignment>& a);


		    // INTERNAL DATA

      protected:

		    struct index_abstract index;

	    };

    class simple_macro_token: public generic_token
	    {

      public:

	      enum macro_type { pre, post };

      public:

        simple_macro_token(const std::string& m, const std::vector<std::string>& a, const macro_packages::simple_rule& r, macro_type t);

		    virtual ~simple_macro_token() = default;


		    // INTERFACE

      public:

		    //! get type
		    macro_type get_type() const { return(this->type); }

		    //! evaluate and cache the result
		    void evaluate();


		    // INTERNAL DATA

      protected:

		    const std::string name;
		    const std::vector<std::string> args;
		    macro_packages::simple_rule rule;

		    enum macro_type type;

	    };

		class index_macro_token: public generic_token
			{

		  public:

		    index_macro_token(const std::string& m, const std::vector<index_abstract>& i, const std::vector<std::string>& a, const macro_packages::index_rule& r);

				virtual ~index_macro_token();


		    // INTERFACE

		  public:

		    //! evaluate and cache the result given a list of index assignments
		    void evaluate(const std::vector<index_assignment>& a);


		    // INTERNAL DATA

		  protected:

				const std::string name;
				const std::vector<std::string> args;
				const std::vector<index_abstract> indices;
				macro_packages::index_rule rule;

				void* state;

			};

  public:

		//! build a token list from an input string
		token_list(const std::string& input,
		           const std::string& prefix,
		           const std::vector<macro_packages::simple_rule>& pre,
		           const std::vector<macro_packages::simple_rule>& post,
		           const std::vector<macro_packages::index_rule>& index);

		// suppress default copy constructor
		token_list(const token_list& obj) = delete;

		~token_list();


		// INTERFACE

  public:

		//! convert to string form, using evaluated versions of each macro
		std::string to_string();

		//! get number of tokens
		unsigned int size() const { return(this->tokens.size()); }

		//! evaluate simple macros of a specific type, and cache the result.
		//! We only want to do this once if possible, since macro evaluation may be expensive.
		unsigned int evaluate_macros(simple_macro_token::macro_type type);

		//! evaluate index macros and cache the result
		unsigned int evaluate_macros(const std::vector<index_assignment>& a);

		//! get list of indices identified during tokenization
		const std::vector<index_abstract>& get_indices() { return(this->indices); }


		// INTERNAL API

  protected:

		//! check no index list exists in the input string
    void check_no_index_list(const std::string& input, const std::string& candidate, size_t& position);

		//! check an argument list of correct size exists in the input string, and return it (tokenized)
		std::vector<std::string> get_argument_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_args);

		//! check an index list of correct size exists in the input string, and return it (tokenized)
		std::vector<index_abstract> get_index_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_indices, unsigned int range);

		//! check whether the current candidate is a potential match for a macro
		template <typename Rule>
		bool check_for_match(const std::string& candidate, const std::vector<Rule>& rule_list, bool allow_substring=true);

		//! find the replacement rule for the current candidate
		template <typename Rule>
		const Rule& find_match(const std::string& candidate, const std::vector<Rule>& rule_list);

		//! add an index to our internal list
		void add_index(char label);

		//! add an index to our internal list
		void add_index(const index_abstract& index);


    // INTERNAL DATA

  protected:

		//! tokenized version of input
		std::list<generic_token*> tokens;

		//! list of indices found in input
		std::vector<index_abstract> indices;

	};


#endif //__macro_tokenizer_H_
