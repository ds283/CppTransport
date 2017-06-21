//
// Created by David Seery on 07/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_DIRECTIVE_DEFINITIONS_H
#define CPPTRANSPORT_DIRECTIVE_DEFINITIONS_H


#include <stdexcept>
#include <functional>

#include "translator_data.h"
#include "index_assignment.h"
#include "index_literal.h"
#include "macro_types.h"
#include "rules_common.h"


namespace macro_packages
  {

    //! base class for a 'simple' directive that takes arguments but not indices
    class directive_simple
      {
      
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
      
        //! constructor
        directive_simple(std::string nm, unsigned int n, translator_data& p)
          : name(std::move(nm)),
            num_args(n),
            payload(p)
          {
          }
          
        //! destructor
        virtual ~directive_simple() = default;
        
        
        // INTERFACE -- EVALUATION
        
      public:
      
        //! evaluate the directive
        std::string operator()(const macro_argument_list& args);
        
        
        // INTERFACE -- QUERY FOR DATA
        
      public:
        
        //! get number of arguments associated with this directive
        unsigned int get_number_args() const { return this->num_args; }
        
        //! get name associated with this directive
        const std::string& get_name() const { return this->name; }


        // INTERNAL API
        
      protected:
        
        //! apply function; has to be supplied by implementation
        //! unlike for a macro, apply() for a directive is guaranteed to be called exactly once
        virtual std::string apply(const macro_argument_list& args) = 0;
    
        //! should this directive always be evaluated, even if output is disabled?
        //! defaults to false, but can be overridden if evaluation must always be performed,
        //! eg. as for $IF, $ELSE, $ENDIF
        virtual bool always_apply() const { return false; }

    
        // VALIDATION
  
      protected:
    
        //! validate supplied arguments
        void validate(const macro_argument_list& args);
    
    
        // INTERNAL DATA
        
      protected:
        
        //! name of this directive
        std::string name;
        
        //! number of arguments expected
        unsigned int num_args;
        
        //! translator data payload
        translator_data& payload;
      
      };
    
    
    //! base class for an 'index' directive that takes both arguments and indices
    class directive_index
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor that accepts a variable number of indices
        directive_index(std::string nm, unsigned int a, translator_data& p,
                        boost::optional<unsigned int> i = boost::none,
                        boost::optional< std::vector<index_class> > c = boost::none)
          : name(std::move(nm)),
            num_args(a),
            payload(p),
            num_indices(i),
            idx_classes(c)
          {
            if(idx_classes && !num_indices) throw std::runtime_error(ERROR_DIRECTIVE_RULE_INDEX_COUNT);
            
            if(!idx_classes) return;
            
            if(num_indices.get() != idx_classes.get().size()) throw std::runtime_error(ERROR_DIRECTIVE_RULE_INDEX_COUNT);
          }
        
        //! destructor is default
        virtual ~directive_index() = default;

        
        // INTERFACE -- EVALUATION
        
      public:
        
        //! apply this directive
        std::string operator()(const macro_argument_list& args, const index_literal_list& indices);
    
    
        // INTERFACE -- QUERY FOR DATA
        
      public:
        
        //! get number of arguments associated with this directive
        unsigned int get_number_args() const { return this->num_args; }
        
        //! get number of indices associated with this macro;
        //! returned as a boost::optional which will be empty if the directive can accept a variable
        //! number of indices
        boost::optional<unsigned int> get_number_indices() const { return this->num_indices; }
        
        //! get index class associated with this macro;
        //! returned as a boost::optional which will be empty if the directive can accept variable
        //! index types
        const boost::optional< std::vector<index_class> >& get_index_class() const { return this->idx_classes; }
        
        //! get name associated with this directive
        const std::string& get_name() const { return this->name; }


        // INTERNAL API
        
      protected:
    
        //! apply function; has to be supplied by implementation
        //! unlike for a macro, apply() for a directive is guaranteed to be called exactly once
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) = 0;
    
        //! should this directive always be evaluated, even if output is disabled?
        //! defaults to false, but can be overridden if evaluation must always be performed,
        //! eg. as for $IF, $ELSE, $ENDIF
        virtual bool always_apply() const { return false; }


        // VALIDATION
  
      protected:
    
        //! validate supplied arguments
        void validate(const macro_argument_list& args);
    
        //! validate supplied index assignments
        template <typename IndexDatabase>
        void validate(const IndexDatabase& indices);
        
        
        // INTERNAL DATA
        
      protected:
        
        //! name of this directive
        std::string name;
        
        //! number of arguments expected
        unsigned int num_args;
    
        //! translator data payload
        translator_data& payload;
        
        //! number of indices expected
        boost::optional<unsigned int> num_indices;
        
        //! class of index expected
        boost::optional< std::vector<index_class> > idx_classes;
    
      };
    
  }   // namespace macro_packages


// containers for directive-sets; since we can't store references in the STL
// containers, they must be wrapped in std::refernce_wrapper
typedef std::vector< std::reference_wrapper<macro_packages::directive_simple> > simple_directiveset;
typedef std::vector< std::reference_wrapper<macro_packages::directive_index> > index_directiveset;


#endif //CPPTRANSPORT_DIRECTIVE_DEFINITIONS_H
