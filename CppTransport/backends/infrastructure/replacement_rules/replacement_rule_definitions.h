//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __replacement_rule_definitions_H_
#define __replacement_rule_definitions_H_


#include "index_assignment.h"
#include "macro_types.h"


typedef std::function<std::string(const macro_argument_list&)>                                replacement_rule_simple;
typedef std::function<std::string(const macro_argument_list&, const assignment_list&, void*)> replacement_rule_index;
typedef std::function<void*      (const macro_argument_list&)>                                replacement_rule_pre;
typedef std::function<void       (void*)>                                                     replacement_rule_post;


namespace macro_packages
  {

    class simple_rule
      {

      public:

        replacement_rule_simple  rule;
        unsigned int             args;
        std::string              name;

      };


    class index_rule

      {

      public:

        replacement_rule_index   rule;
        replacement_rule_pre     pre;
        replacement_rule_post    post;
        unsigned int             args;
        unsigned int             indices;
        enum index_class         range;
        std::string              name;

      };

  } // namespace macro_packages


#endif //__replacement_rule_definitions_H_
