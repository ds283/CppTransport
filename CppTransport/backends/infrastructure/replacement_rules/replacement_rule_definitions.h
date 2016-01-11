//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
#define CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H


#include <stdexcept>

#include "index_assignment.h"
#include "macro_types.h"
#include "cse_map.h"


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

        //! perform any required setup after tokenization; here a no-op but can be overridden if needed
        virtual void post_tokenize_hook(const macro_argument_list& args) { return; }

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

        //! constructor for a 'traditional' index macro with a fixed number of arguments and indices,
        //! and enforcing a specific class for the index type
        replacement_rule_index(std::string nm, unsigned int a, unsigned int i, enum index_class c)
          : name(std::move(nm)),
            num_args(a),
            num_indices(i),
            idx_class(c)
          {
            // traditional rules are always macros
            rule_class = replacement_rule_class::macro;
          }

        //! constructor for a 'traditional' index macro with a fixed number of arguments and indices,
        //! but no specific class for the index types
        replacement_rule_index(std::string nm, unsigned int a, unsigned int i)
          : name(std::move(nm)),
            num_args(a),
            num_indices(i)
          {
            // traditional rules are always macros
            rule_class = replacement_rule_class::macro;
          }

        //! constructor for a 'variable' index macro which can accept a variable number of indices
        replacement_rule_index(std::string nm, unsigned int a)
          : name(std::move(nm)),
            num_args(a)
          {
            // for the time being, variable rules are always directives
            rule_class = replacement_rule_class::directive;
          }

        //! destructor
        virtual ~replacement_rule_index() = default;


        // INTERFACE -- EVALUATION

      public:

        //! evaluate the macro on a concrete (unrolled) index assignment
        std::string evaluate_unroll(const macro_argument_list& args, const assignment_list& indices);

        //! evaluate the macro on an abstract (rolled-up) index assignment
        std::string evaluate_roll(const macro_argument_list& args, const abstract_index_list& indices);

        //! pre-evaluation
        void pre(const macro_argument_list& args);

        //! post-evaluation
        void post(const macro_argument_list& args);


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        unsigned int get_number_args() const { return(this->num_args); }

        //! get number of indices associated with this macro;
        //! returned as a boost::optional which will be empty if the macro can accept a variable
        //! number of indices
        boost::optional<unsigned int> get_number_indices() const { return(this->num_indices); }

        //! get index class associated with this macro;
        //! returned as a boost::optional which will be empty if the macro can accept variable
        //! index types
        boost::optional<enum index_class> get_index_class() const { return(this->idx_class); }

        //! get name associated with this macro
        const std::string& get_name() const { return(this->name); }

        //! get unroll status for this macro -- must be handled by implementation
        virtual enum unroll_behaviour get_unroll() const = 0;

        //! determine whether this rule is a directive
        bool is_directive() const { return(this->rule_class == replacement_rule_class::directive); }


        // INTERNAL API

      protected:

        //! evaluation function for unrolled index sets; has to be supplied by implementation
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) = 0;

        //! pre-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void pre_hook(const macro_argument_list& args) { return; }

        //! post-evaluation; if needed, can be supplied by implementation; default is no-op
        virtual void post_hook(const macro_argument_list& args) { return; }

        //! evaluation function for rolled-up index sets; has to be supplied by implementation
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) = 0;

        //! validate supplies arguments and index assignments
        template <typename IndexDatabase>
        void check(const macro_argument_list& args, const IndexDatabase& indices);


        // INTERNAL DATA

      protected:

        //! name of this replacement rule
        std::string name;

        //! class of this replacement rule
        enum replacement_rule_class rule_class;

        //! number of arguments expected
        unsigned int num_args;

        //! number of indices expected
        boost::optional<unsigned int> num_indices;

        //! class of index expected
        boost::optional<enum index_class> idx_class;

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


    class argument_mismatch: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        argument_mismatch(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~argument_mismatch() = default;

      };


    class index_mismatch: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        index_mismatch(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~index_mismatch() = default;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
