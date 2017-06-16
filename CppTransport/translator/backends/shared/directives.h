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

#include "directive_package.h"

#include "token_list.h"


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
            user_macro(std::string n, std::unique_ptr<token_list> t, index_literal_list i, error_context d)
              : replacement_rule_index(n, 0, i.size()),
                tokens(std::move(t)),
                indices(std::move(i)),
                declaration_point(std::move(d))
              {
              }

            //! destructor
            virtual ~user_macro() = default;


            // INTERFACE

          public:

            //! determined unroll status, which is inherited from the unroll status of the token list we contain
            unroll_behaviour get_unroll() const override { return this->tokens->unroll_status(); }

            //! get declaration point
            const error_context& get_declaration_point() const { return(this->declaration_point); }


            // INTERNAL API -- implements a 'replacement_rule_index' interface

          protected:

            //! pre-hook required to call all pre-macros
            void pre_hook(const macro_argument_list& args, const index_literal_list& indices) override;

            //! post-hook, required to call post-hook of all tokens we contain
            //! should reset token list to pristine state for next evaluation
            void post_hook(const macro_argument_list& args) override;

            //! unrolled evaluation
            std::string unroll(const macro_argument_list& args, const index_literal_assignment& indices) override;

            //! roll-up evaluation
            std::string roll(const macro_argument_list& args, const index_literal_list& indices) override;


            // INTERNAL DATA

          private:

            //! tokenized version of macro definition
            //! (contains an abstract_index_database that defines the indices associated with this macro)
            std::unique_ptr<token_list> tokens;

            //! index literal list for this macro
            const index_literal_list indices;

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


    class set_directive: public directive_index
      {
        
        // TYPES
        
      protected:
    
        //! symbol table for user-defined replacement rules
        typedef std::unordered_map< std::string, std::unique_ptr<directives_impl::user_macro> > macro_table;

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_directive(std::string n, translator_data& p)
          : directive_index(n, SET_DIRECTIVE_TOTAL_ARGUMENTS, p)
          {
          }

        //! destructor
        virtual ~set_directive() = default;


        // INTERNAL API -- implements a 'directive_index' interface

      protected:

        //! apply directive
        std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;


        // INTERNAL DATA

      private:

        //! symbol table for macros
        macro_table rules;

      };


    typedef std::stack< directives_impl::if_record > if_stack;


    class if_directive: public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        if_directive(std::string n, translator_data& p, if_stack& is)
          : directive_simple(n, IF_DIRECTIVE_TOTAL_ARGUMENTS, p),
            istack(is)
          {
          }

        //! destructor
        virtual ~if_directive() = default;


        // INTERNAL API

      protected:

        //! apply
        std::string apply(const macro_argument_list& args) override;

        //! force evaluation even when output is disabled
        bool always_apply() const override { return true; }
        

        // INTERNAL DATA

      private:

        //! reference to parent if_stack
        if_stack& istack;

      };


    class else_directive: public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        else_directive(std::string n, translator_data& p, if_stack& is)
          : directive_simple(n, ELSE_DIRECTIVE_TOTAL_ARGUMENTS, p),
            istack(is)
          {
          }

        //! destructor
        virtual ~else_directive() = default;


        // INTERNAL API

      protected:

        //! apply
        std::string apply(const macro_argument_list& args) override;
    
        //! force evaluation even when output is disabled
        bool always_apply() const override { return true; }
    
    
        // INTERNAL DATA

      private:

        //! reference to parent if_stack
        if_stack& istack;

      };


    class endif_directive: public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        endif_directive(std::string n, translator_data& p, if_stack& is)
          : directive_simple(n, ENDIF_DIRECTIVE_TOTAL_ARGUMENTS, p),
            istack(is)
          {
          }

        //! destructor
        virtual ~endif_directive() = default;


        // INTERNAL API

      protected:

        //! apply
        std::string apply(const macro_argument_list& args) override;
        
        //! force evaluation even when output is disabled
        bool always_apply() const override { return true; }
    
    
        // INTERNAL DATA

      private:

        //! reference to parent if_stack
        if_stack& istack;

      };


    class directives: public directive_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        directives(translator_data& p);

        //! destructor
        virtual ~directives() = default;


        // INTERNAL DATA

      private:

        //! stack of records corresponding to open if statements
        if_stack istack;

      };

  }   // namespace macro_packages




#endif //CPPTRANSPORT_DIRECTIVES_H
