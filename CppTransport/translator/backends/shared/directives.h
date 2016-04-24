//
// Created by David Seery on 05/01/2016.
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


#ifndef CPPTRANSPORT_DIRECTIVES_H
#define CPPTRANSPORT_DIRECTIVES_H


#include <stack>

#include "replacement_rule_package.h"

#include "macro_tokenizer.h"


namespace macro_packages
  {

    constexpr unsigned int SET_DIRECTIVE_NAME_ARGUMENT = 0;
    constexpr unsigned int SET_DIRECTIVE_DEFINITION_ARGUMENT = 1;
    constexpr unsigned int SET_DIRECTIVE_TOTAL_ARGUMENTS = 2;

    constexpr unsigned int IF_DIRECTIVE_CONDITION_ARGUMENT = 0;
    constexpr unsigned int IF_DIRECTIVE_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int ELSE_DIRECTIVE_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int ENDIF_DIRECTIVE_TOTAL_ARGUMENTS = 0;


    namespace directives_impl
      {

        class user_macro: public replacement_rule_index
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            user_macro(std::string n, std::unique_ptr<token_list> t, const abstract_index_list& i, error_context d)
              : replacement_rule_index(n, 0, i.size()),
                tokens(std::move(t)),
                indices(i),
                declaration_point(std::move(d))
              {
              }

            //! destructor
            virtual ~user_macro() = default;


            // INTERFACE

          public:

            //! determined unroll status, which is inherited from the unroll status of the token list we contain
            enum unroll_behaviour get_unroll() const override { return this->tokens->unroll_status(); }

            //! get declaration point
            const error_context& get_declaration_point() const { return(this->declaration_point); }


            // INTERNAL API -- implements a 'replacement_rule_index' interface

          protected:

            //! pre-hook required to call all pre-macros
            virtual void pre_hook(const macro_argument_list& args) override;

            //! post-hook, required to call post-hook of all tokens we contain
            //! should reset token list to pristine state for next evaluation
            virtual void post_hook(const macro_argument_list& args) override;

            //! unrolled evaluation
            virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

            //! roll-up evaluation
            virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


            // INTERNAL DATA

          private:

            //! tokenized version of macro definition
            std::unique_ptr<token_list> tokens;

            //! list of indices defining this macro
            abstract_index_list indices;

            //! record declaration point
            const error_context declaration_point;

          };


        class if_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            if_record(std::string c, bool v)
              : condition(std::move(c)),
                value(v),
                if_branch(true)
              {
                if(value) enabled = true;
                else      enabled = false;
              }

            ~if_record() = default;


            // INTERFACE

          public:

            //! get truth value
            bool get_value() const { return(this->value); }

            //! get conditions
            const std::string& get_condition() const { return(this->condition); }

            //! currently in the if-branch? if not, assume we are in else-branch
            bool in_if_branch() const { return(this->if_branch); }

            //! mark as in else-branch
            void mark_else_branch() { this->if_branch = false; if(value) enabled = false; else enabled = true; }

            //! get current output-enabled status
            bool is_enabled() const { return(this->enabled); }


            // INTERNAL DATA

          private:

            //! string version of condition
            std::string condition;

            //! record truth value
            bool value;

            //! which branch are we in?
            bool if_branch;

            //! is output currently enabled?
            bool enabled;

          };

      }   // namespace directives_impl


    class set_directive: public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_directive(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_index(n, SET_DIRECTIVE_TOTAL_ARGUMENTS),
            payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_directive() = default;


        // INTERFACE

      public:

        //! determine unroll status; meaningless for a directive, so we always return 'prevent'
        //! in order to guarantee being provided with an abstract index list
        enum unroll_behaviour get_unroll() const override { return unroll_behaviour::prevent; }


        // INTERNAL API -- implements a 'replacement_rule_index' interface

      protected:

        //! evaluate unrolled; has no meaning here, so throws an exception
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! evaluate directive
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL API

      protected:

        //! validate indices discovered during tokenization against a supplied index list
        void validate_discovered_indices(const abstract_index_list& supplied, const abstract_index_list& discovered);


        // INTERNAL DATA

      private:

        //! reference to data payload provided by translator
        translator_data& payload;

        //! reference to language printer object
        language_printer& printer;

        typedef std::unordered_map< std::string, std::unique_ptr<directives_impl::user_macro> > macro_table;

        //! symbol table for macros
        macro_table macros;

      };


    typedef std::stack< directives_impl::if_record > if_stack;


    class if_directive: public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        if_directive(std::string n, translator_data& p, language_printer& prn, if_stack& is)
          : replacement_rule_simple(n, IF_DIRECTIVE_TOTAL_ARGUMENTS),
            payload(p),
            printer(prn),
            istack(is)
          {
          }

        //! destructor
        virtual ~if_directive() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to data payload provided by translator
        translator_data& payload;

        //! reference to language printer object
        language_printer& printer;

        //! reference to parent if_stack
        if_stack& istack;

      };


    class else_directive: public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        else_directive(std::string n, translator_data& p, language_printer& prn, if_stack& is)
          : replacement_rule_simple(n, ELSE_DIRECTIVE_TOTAL_ARGUMENTS),
            payload(p),
            printer(prn),
            istack(is)
          {
          }

        //! destructor
        virtual ~else_directive() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        //! evaluate else clause after tokenization
        virtual void post_tokenize_hook(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to data payload provided by translator
        translator_data& payload;

        //! reference to language printer object
        language_printer& printer;

        //! reference to parent if_stack
        if_stack& istack;

      };


    class endif_directive: public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        endif_directive(std::string n, translator_data& p, language_printer& prn, if_stack& is)
          : replacement_rule_simple(n, ENDIF_DIRECTIVE_TOTAL_ARGUMENTS),
            payload(p),
            printer(prn),
            istack(is)
          {
          }

        //! destructor
        virtual ~endif_directive() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        //! evaluate endif clause after tokenization
        virtual void post_tokenize_hook(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to data payload provided by translator
        translator_data& payload;

        //! reference to language printer object
        language_printer& printer;

        //! reference to parent if_stack
        if_stack& istack;

        //! condition cache, used for formatting comment string for output
        std::string condition_cache;

      };


    class directives: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        directives(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~directives() = default;


        // INTERNAL DATA

      private:

        //! stack of records corresponding to open if statements
        if_stack istack;

      };

  }   // namespace macro_packages




#endif //CPPTRANSPORT_DIRECTIVES_H
