//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "package_group.h"

#include "u_tensor_factory.h"
#include "flatten.h"


package_group::package_group(macro_packages::replacement_data& d)
  : data(d), cse_worker(nullptr)
  {
    u_factory = make_u_tensor_factory(data.parse_tree);
    fl        = new flattener(1);
  }


package_group::~package_group()
  {
    delete u_factory;
    delete fl;
  }


void package_group::push_back(macro_packages::replacement_rule_package* package)
  {
    assert(this->u_factory != nullptr);
    assert(this->fl != nullptr);
    assert(this->cse_worker != nullptr);

    // store this rule package in our list
    this->packages.push_back(package);

    // populate the rule package with information about the package environment
    package->set_u_factory(this->u_factory);
    package->set_flattener(this->fl);
    package->set_cse_worker(this->cse_worker);    // ! warning: assumes cse_worker has been set by the derived class constructor
  }


std::vector<macro_packages::simple_rule> package_group::get_pre_ruleset()
  {
    std::vector<macro_packages::simple_rule> ruleset;

    for(std::list<macro_packages::replacement_rule_package*>::iterator t = this->packages.begin(); t != this->packages.end(); t++)
      {
        std::vector<macro_packages::simple_rule> rules = (*t)->get_pre_rules();
        ruleset.reserve(ruleset.size() + rules.size());
        ruleset.insert(ruleset.end(), rules.begin(), rules.end());
      }

    return(ruleset);
  }


std::vector<macro_packages::simple_rule> package_group::get_post_ruleset()
  {
    std::vector<macro_packages::simple_rule> ruleset;

    for(std::list<macro_packages::replacement_rule_package*>::iterator t = this->packages.begin(); t != this->packages.end(); t++)
      {
        std::vector<macro_packages::simple_rule> rules = (*t)->get_post_rules();
        ruleset.reserve(ruleset.size() + rules.size());
        ruleset.insert(ruleset.end(), rules.begin(), rules.end());
      }

    return(ruleset);
  }


std::vector<macro_packages::index_rule> package_group::get_index_ruleset()
  {
    std::vector<macro_packages::index_rule> ruleset;

    for(std::list<macro_packages::replacement_rule_package*>::iterator t = this->packages.begin(); t != this->packages.end(); t++)
      {
        std::vector<macro_packages::index_rule> rules = (*t)->get_index_rules();
        ruleset.reserve(ruleset.size() + rules.size());
        ruleset.insert(ruleset.end(), rules.begin(), rules.end());
      }

    return(ruleset);
  }
