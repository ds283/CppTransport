//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __replacement_rule_definitions_H_
#define __replacement_rule_definitions_H_


#include "index_assignment.h"


typedef std::function<std::string(const std::vector<std::string>&)>                                       replacement_rule_simple;
typedef std::function<std::string(const std::vector<std::string>&, std::vector<index_assignment>, void*)> replacement_rule_index;
typedef std::function<void*      (const std::vector<std::string>&)>                                       replacement_rule_pre;
typedef std::function<void       (void*)>                                                                 replacement_rule_post;


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
        unsigned int             range;
        std::string              name;

      };

  } // namespace macro_packages


#endif //__replacement_rule_definitions_H_
