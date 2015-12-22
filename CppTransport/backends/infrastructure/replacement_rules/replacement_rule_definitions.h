//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
#define CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H


#include <stdexcept>

#include "index_assignment.h"
#include "macro_types.h"
#include "cse_map.h"


//! replacement rule for an 'index' macro, which takes both arguments and needs an
//! index set; we need to be able to evaluate it concretely on a specific index
//! assignment, if the index set is unrolled, and abstractly on a loop variable if
//! it is not. This rule deals with the unrolling
typedef std::function<std::string(const macro_argument_list&, const assignment_list&, cse_map*)> replacement_rule_unroll;

//! hook called before index set unrolling, usually to set up a CSE map to the expressions
//! which will be used by the unroll replacement rule
typedef std::function<std::unique_ptr<cse_map>(const macro_argument_list&)> replacement_pre_unroll;

//! hook called after index set unrolling. There is no requirement to release the CSE map
//! whose lifetime is managed by the platform
typedef std::function<void(cse_map*)> replacement_post_unroll;

//! replacement rule for an 'index' macro on an abstract index set, used when planting
//! 'for'-loop based implementations of an index set
typedef std::function<std::string(const macro_argument_list&, const abstract_index_list&)> replacement_rule_abstract;


namespace macro_packages
  {

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
        unsigned int get_number_args() const { return(this->num_args); };

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }


        // INTERNAL API

      protected:

        //! evaluation function; has to be supplied by implementation
        virtual std::string evaluate(const macro_argument_list& args) = 0;


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

        //! constructor
        replacement_rule_index(std::string nm, unsigned int a, unsigned int i, enum index_class c)
          : name(std::move(nm)),
            num_args(a),
            num_indices(i),
            idx_class(c)
          {
          }

        //! destructor
        virtual ~replacement_rule_index() = default;


        // INTERFACE -- EVALUATION

      public:

        //! evaluate the macro
        std::string operator()(const macro_argument_list& args, const assignment_list& indices);

        //! pre-evaluation
        void pre(const macro_argument_list& args);

        //! post-evaluation
        void post(const macro_argument_list& args);


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return(this->num_args); }

        //! get number of indices associated with this macro
        unsigned int get_number_indices() const { return(this->num_indices); }

        //! get index class associated with this macro
        enum index_class get_index_class() const { return(this->idx_class); }

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }

        //! get unroll status for this macro -- must be handled by implementation
        virtual enum unroll_behaviour get_unroll() const = 0;


        // INTERNAL API

      protected:

        //! evaluation function; has to be supplied by implementation
        virtual std::string evaluate(const macro_argument_list& args, const assignment_list& indices) = 0;

        //! pre-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void pre_evaluate(const macro_argument_list& args) { return; }

        //! post-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void post_evaluate(const macro_argument_list& args) { return; }


        // INTERNAL DATA

      protected:

        //! name of this macro
        std::string name;

        //! number of arguments expected
        unsigned int num_args;

        //! number of indices expected
        unsigned int num_indices;

        //! class of index expected
        enum index_class idx_class;

      };


    class rule_apply_fail: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        rule_apply_fail(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~rule_apply_fail() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
