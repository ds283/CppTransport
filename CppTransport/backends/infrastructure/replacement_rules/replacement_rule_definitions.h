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
        replacement_rule_simple(std::string n)
          : name(std::move(n))
          {
          }

        //! destructor
        virtual ~replacement_rule_simple() = default;


        // INTERFACE -- EVALUATION

      public:

        //! evaluate the macro
        virtual std::string operator()(const macro_argument_list& args) = 0;

        //! report end of input; here a no-op but but can be overridden if needed
        virtual void report_end_of_input() { return; }


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const = 0;

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }


        // INTERNAL DATA

      protected:

        //! name of this macro
        std::string name;

      };


    // base class for an 'index' macro which takes both arguments and needs an index set
    class replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replacement_rule_index() = default;

        //! destructor
        virtual ~replacement_rule_index() = default;


        // INTERFACE

      public:

        //! evaluate the macro
        virtual std::string operator()(const macro_argument_list& args, const assignment_list& indices) = 0;

        //! pre-evaluation
        virtual void pre(const macro_argument_list& args) = 0;

        //! post-evaluation
        virtual void post(const macro_argument_list& args) = 0;

      };


    class simple_rule
      {

      public:

        //! constructor enforces setup of all fields;
        //! we capture the replacement rule via a std::shared_ptr<> which may not be ideal
        // TODO: consider searching for a better solution
        simple_rule(std::string n, std::shared_ptr<replacement_rule_simple> r, unsigned int a)
          : rule(std::move(r)),
            args(a),
            name(std::move(n))
          {
          }


        // INTERFACE - GET DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return(this->args); }

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }


        // INTERFACE - EVALUATION

      public:

        //! apply replacement rule
        std::string operator()(const macro_argument_list& args);

        //! report end of input; used eg. to deposit temporaries when input is finished
        void report_end_of_input();


        // INTERNAL DATA

      private:

        //! function pointer to replacement rule
        std::shared_ptr<replacement_rule_simple> rule;

        //! number of arguments expected by this macro
        unsigned int args;

        //! name of this macro
        std::string name;

      };


    class index_rule
      {

      public:

        //! constructor enforces setup of all fields
        index_rule(std::string n, replacement_rule_unroll r, replacement_pre_unroll pr, replacement_post_unroll po,
                   unsigned int a, unsigned int i, enum index_class rn, enum unroll_behaviour u)
          : rule(std::move(r)),
            pre(std::move(pr)),
            post(std::move(po)),
            args(a),
            indices(i),
            range(rn),
            unroll(u),
            name(n)
          {
          }


        // INTERFACE - GET DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return(this->args); }

        //! get number of indices associated with this macro
        unsigned int get_number_indices() const { return(this->indices); }

        //! get index class associated with this macro
        enum index_class get_index_class() const { return(this->range); }

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }

        //! get unroll flag associated with this macro
        enum unroll_behaviour get_unroll() const { return(this->unroll); }


        // INTERFACE - EVALUATION

      public:

        //! apply replacement rule
        std::string operator()(const macro_argument_list& args, const assignment_list& indices, cse_map* state);

        //! pre-hook
        std::unique_ptr<cse_map> pre_evaluate(const macro_argument_list& args);

        //! post-hook
        void post_evaluate(cse_map* state);


        // INTERNAL DATA

      private:

        //! function pointer to replacement rule
        replacement_rule_unroll rule;

        //! function pointer to set-up method for this macro
        replacement_pre_unroll pre;

        //! function pointer to set-down method for this macro
        replacement_post_unroll post;

        //! number of arguments expected by this macro
        unsigned int args;

        //! number of indices expected by this macro
        unsigned int indices;

        //! type of index expected by this macro
        enum index_class range;

        //! does this macro require unrolling, prevent unrolling, or simply allow it?
        enum unroll_behaviour unroll;

        //! name of this macro
        std::string name;

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
