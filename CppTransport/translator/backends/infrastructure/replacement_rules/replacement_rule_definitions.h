//
// Created by David Seery on 10/12/2013.
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


#ifndef CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
#define CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H


#include <stdexcept>
#include <functional>
#include <vector>

#include "index_assignment.h"
#include "index_literal.h"
#include "macro_types.h"
#include "cse_map.h"
#include "rules_common.h"

#include "boost/optional.hpp"


namespace macro_packages
  {

    //! tag for type of replacement rule;
    //! either a macro, which evaluates to text and is replaced in the template,
    //! or a directive, which does not evaluate to text and instead causes a
    //! change of state in the translator
    enum class replacement_rule_class
      {
        macro, directive
      };


    //! base class for a 'simple' macro which takes arguments but not indices
    class replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replacement_rule_simple(std::string nm, unsigned int n)
          : name(std::move(nm)),
            num_args(n)
          {
          }

        //! destructor
        virtual ~replacement_rule_simple() = default;


        // INTERFACE -- EVALUATION

      public:

        //! evaluate the macro
        std::string operator()(const macro_argument_list& args);
    
        //! report end of input; here a no-op but but can be overridden if needed
        virtual void report_end_of_input() { return; }


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return this->num_args; };

        //! get name associated with this macro
        const std::string& get_name() const { return this->name; }


        // INTERNAL API

      protected:

        //! evaluation function; has to be supplied by implementation
        virtual std::string evaluate(const macro_argument_list& args) = 0;
    
    
        // VALIDATION
  
      protected:
    
        //! validate supplied arguments
        void validate(const macro_argument_list& args);


        // INTERNAL DATA

      protected:

        //! name of this macro
        std::string name;

        //! number of arguments expected
        unsigned int num_args;

      };


    // base class for an 'index' macro which takes both arguments and needs an index set
    class replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor for an index macro with a fixed number of arguments and indices.
        //! optionally enforces specific classes for the index types if the fourth argument
        //! 'c' is given
        replacement_rule_index(std::string nm, unsigned int a, unsigned int i,
                               boost::optional< std::vector<index_class> > c = boost::none)
          : name(std::move(nm)),
            num_args(a),
            num_indices(i),
            idx_classes(std::move(c))
          {
            if(num_indices == 0) throw std::runtime_error(ERROR_REPLACEMENT_RULE_ZERO_INDICES);

            // if no list of index classes supplied, then nothing to do
            if(!idx_classes) return;
            
            // if a list was supplied, its length should match the specified number of indices
            if(num_indices != idx_classes.get().size()) throw std::runtime_error(ERROR_REPLACEMENT_RULE_INDEX_COUNT);
          }

        //! destructor is default
        virtual ~replacement_rule_index() = default;


        // INTERFACE -- EVALUATION

      public:

        //! evaluate the macro on a concrete (unrolled) index assignment
        std::string evaluate_unroll(const macro_argument_list& args, const index_literal_assignment& indices);

        //! evaluate the macro on an abstract (rolled-up) index assignment
        std::string evaluate_roll(const macro_argument_list& args, const index_literal_list& indices);

        //! pre-evaluation
        void pre(const macro_argument_list& args);

        //! post-evaluation
        void post(const macro_argument_list& args);


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return this->num_args; }

        //! get number of indices associated with this macro
        //! returned as a boost::optional which will be empty if the macro can accept a variable
        //! number of indices
        unsigned int get_number_indices() const { return this->num_indices; }

        //! get index class associated with this macro;
        //! returned as a boost::optional which will be empty if the macro can accept variable
        //! index types
        const boost::optional< std::vector<index_class> >& get_index_class() const { return this->idx_classes; }

        //! get name associated with this macro
        const std::string& get_name() const { return this->name; }

        //! get unroll status for this macro -- must be handled by implementation
        virtual unroll_behaviour get_unroll() const = 0;
    
    
        // INTERNAL API

      protected:

        //! evaluation function for unrolled index sets; has to be supplied by implementation
        virtual std::string unroll(const macro_argument_list& args, const index_literal_assignment& indices) = 0;

        //! pre-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void pre_hook(const macro_argument_list& args) { return; }

        //! post-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void post_hook(const macro_argument_list& args) { return; }

        //! evaluation function for rolled-up index sets; has to be supplied by implementation
        virtual std::string roll(const macro_argument_list& args, const index_literal_list& indices) = 0;

        
        // VALIDATION
        
      protected:
        
        //! validate supplied arguments
        void validate(const macro_argument_list& args);
        
        //! validate supplied index assignments
        template <typename IndexDatabase>
        void validate(const IndexDatabase& indices);


        // INTERNAL DATA

      protected:

        //! name of this replacement rule
        std::string name;

        //! class of this replacement rule
        enum replacement_rule_class rule_class;

        //! number of arguments expected
        unsigned int num_args;

        //! number of indices expected
        unsigned int num_indices;

        //! class of index expected
        boost::optional< std::vector<index_class> > idx_classes;

      };

  } // namespace macro_packages


// containers for rulesets; since we can't store references in the STL
// cotainers, they must be wrapped in std::reference_wrapper<>
typedef std::vector< std::reference_wrapper<macro_packages::replacement_rule_simple> > pre_ruleset;
typedef std::vector< std::reference_wrapper<macro_packages::replacement_rule_simple> > post_ruleset;
typedef std::vector< std::reference_wrapper<macro_packages::replacement_rule_index> > index_ruleset;


#endif //CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
