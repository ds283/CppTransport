//
// Created by David Seery on 06/06/2017.
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


#ifndef CPPTRANSPORT_MACRO_TOKENS_H
#define CPPTRANSPORT_MACRO_TOKENS_H


#include <string>
#include <functional>

#include "core.h"
#include "error_context.h"
#include "index_assignment.h"
#include "replacement_rule_definitions.h"
#include "directive_definitions.h"
#include "macro_types.h"


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
        std::string to_string() const { return conversion; }
        
        //! raise error
        void error(const std::string& msg);
        
        //! mark this token as silent for error reporting
        void mark_silent() { this->silent = true; }


        // INTERNAL DATA

      protected:

        //! converted value of this token
        std::string conversion;

        //! context for error messages involving this token
        error_context err_ctx;

        //! number of errors raised by this token
        unsigned int num_errors;

        //! error reports silenced?
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
        index_literal_token(abstract_index_database::const_iterator& it, error_context ec);


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
        simple_macro_type get_type() const { return (this->type); }
        
        //! evaluate and cache the result
        void evaluate();


        // INTERNAL DATA

      protected:

        //! macro name
        const std::string name;
        
        //! macro argument list
        const macro_argument_list args;
        
        //! reference to owning replacement rule
        macro_packages::replacement_rule_simple& rule;

        //! macro type -- pre or post?
        simple_macro_type type;

        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;

      };

    
    class index_macro_token : public generic_token
      {

      public:

        //! constructor
        index_macro_token(const std::string& m, const abstract_index_database i, const macro_argument_list& a,
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
        const abstract_index_database indices;

        //! reference to replacement rule object for this token
        macro_packages::replacement_rule_index& rule;

        //! flag to determine whether 'pre' handler has been called
        bool initialized;

        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;

        //! have index-related errors been reported yet? if so, silence further errors
        bool index_error;

      };
    
    
    class simple_directive_token : public generic_token
      {
      
      public:
        
        //! constructor
        simple_directive_token(const std::string& m, const macro_argument_list& a,
                               macro_packages::directive_simple& r, error_context ec);
        
        //! destructor is default, but icpc fails with explicitly-default destructor
        virtual ~simple_directive_token() = default;
        
        
        // INTERFACE
      
      public:
        
        //! evaluate and cache the result
        void evaluate();
        
        
        // INTERNAL DATA
      
      protected:
        
        //! directive name
        const std::string name;
        
        //! macro argument list
        const macro_argument_list args;
        
        //! reference to owning replacement rule
        macro_packages::directive_simple& rule;
        
        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;
        
      };
    
    
    class index_directive_token : public generic_token
      {
      
      public:
        
        //! constructor
        index_directive_token(const std::string& m, const abstract_index_database i, const macro_argument_list& a,
                              macro_packages::directive_index& r, error_context ec);
        
        //! destructor
        virtual ~index_directive_token() = default;
        
        
        // INTERFACE
      
      public:
        
        //! evaluate and cache the result
        void evaluate();
        
        
        // INTERNAL DATA
      
      protected:
        
        //! name of this macro token
        const std::string name;
        
        //! argument list for this token
        const macro_argument_list args;
        
        //! index list for this token
        const abstract_index_database indices;
        
        //! reference to replacement rule object for this token
        macro_packages::directive_index& rule;
        
        //! have argument-related errors been reported yet? if so, silence further errors
        bool argument_error;
        
        //! have index-related errors been reported yet? if so, silence further errors
        bool index_error;
        
      };

  }   // namespace token_list_impl


#endif //CPPTRANSPORT_MACRO_TOKENS_H
