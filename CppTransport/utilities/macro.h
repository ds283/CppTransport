//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __macro_H_
#define __macro_H_

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

#include "core.h"
#include "parse_tree.h"
#include "index_assignment.h"
#include "u_tensor_factory.h"
#include "cse.h"
#include "flatten.h"


typedef std::function<std::string(const std::vector<std::string>&)>                                              replacement_rule_simple;
typedef std::function<std::string(const std::vector<std::string>&, std::vector<struct index_assignment>, void*)> replacement_rule_index;
typedef std::function<void*      (const std::vector<std::string>&)>                                              replacement_rule_pre;
typedef std::function<void       (void*)>                                                                        replacement_rule_post;


class package_group;

namespace macro_packages
  {
    class simple_rule;
    class index_rule;
  }

class macro_package
  {
    public:
      macro_package(unsigned int N_f, unsigned int N_p, enum indexorder o, std::string pf, std::string sp, package_group* pk);

      unsigned int                              apply                 (std::string& line);

    private:
      unsigned int                              apply_simple          (std::string& line, std::vector<macro_packages::simple_rule>& ruleset, bool blank=false);
      unsigned int                              apply_index           (std::string& line, const std::vector<struct index_abstract>& lhs_indices,
                                                                       const bool semicolon, const bool comma, const bool lhs_present,
                                                                       std::vector<macro_packages::index_rule>& ruleset);

      std::vector<struct index_abstract>        get_lhs_indices       (std::string lhs);
      void                                      assign_lhs_index_types(std::string rhs, std::vector<struct index_abstract>& lhs_indices,
                                                                      std::vector<macro_packages::index_rule>& ruleset);
		  void                                      assign_index_defaults (std::vector<struct index_abstract>& lhs_indices);

      std::vector<std::string>                  get_argument_list     (std::string& line, size_t pos, unsigned int num_args, std::string macro_name);

      std::vector<struct index_abstract>        get_index_set         (std::string line, size_t pos, std::string name, unsigned int indices, unsigned int range);

      void                                      map_indices           (std::string& line, std::string prefx, const std::vector<struct index_assignment>& assignment);


      unsigned int                              fields;
      unsigned int                              parameters;
      enum indexorder                           order;

      package_group*                            package;

      std::vector<macro_packages::simple_rule>& pre_rule_cache;
      std::vector<macro_packages::simple_rule>& post_rule_cache;
      std::vector<macro_packages::index_rule>&  index_rule_cache;

      const std::string                         prefix;
      const std::string                         split;
  };


#endif //__macro_H_
