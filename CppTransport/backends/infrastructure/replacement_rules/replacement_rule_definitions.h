//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
#define CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H


#include "index_assignment.h"
#include "macro_types.h"
#include "cse_map.h"


typedef std::function<std::string             (const macro_argument_list&)>                                   replacement_rule_simple;
typedef std::function<std::string             (const macro_argument_list&, const assignment_list&, cse_map*)> replacement_rule_index;
typedef std::function<std::unique_ptr<cse_map>(const macro_argument_list&)>                                   replacement_rule_pre;
typedef std::function<void                    (std::unique_ptr<cse_map>&)>                                    replacement_rule_post;


namespace macro_packages
  {

    class simple_rule
      {

      public:

        //! constructor enforces setup of all fields
        simple_rule(std::string n, replacement_rule_simple r, unsigned int a)
          : rule(std::move(r)),
            args(a),
            name(std::move(n))
          {
          }


        //! function pointer to replacement rule
        replacement_rule_simple  rule;

        //! number of arguments expected by this macro
        unsigned int             args;

        //! name of this macro
        std::string              name;

      };


    class index_rule
      {

      public:

        //! constructor enforces setup of all fields
        index_rule(std::string n, replacement_rule_index r, replacement_rule_pre pr, replacement_rule_post po,
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

        //! function pointer to replacement rule
        replacement_rule_index   rule;

        //! function pointer to set-up method for this macro
        replacement_rule_pre     pre;

        //! function pointer to set-down method for this macro
        replacement_rule_post    post;

        //! number of arguments expected by this macro
        unsigned int             args;

        //! number of indices expected by this macro
        unsigned int             indices;

        //! type of index expected by this macro
        enum index_class         range;

        //! does this macro require unrolling, prevent unrolling, or simply allow it?
        enum unroll_behaviour    unroll;

        //! name of this macro
        std::string              name;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_REPLACEMENT_RULE_DEFINITIONS_H
