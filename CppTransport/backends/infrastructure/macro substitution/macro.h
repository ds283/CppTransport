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
#include "index_assignment.h"
#include "u_tensor_factory.h"
#include "cse.h"
#include "flatten.h"
#include "package_group.h"
#include "translation_unit.h"
#include "replacement_rule_definitions.h"

#include "boost/timer/timer.hpp"


class package_group;


namespace macro_packages
	{
    class simple_rule;

    class index_rule;
	}


class macro_package
	{

  public:

		// constructor
    macro_package(translation_unit* u, package_group* pkg, std::string pf, std::string sp,
                  unsigned int dm = DEFAULT_RECURSION_DEPTH);


		// INTERFACE

    // apply macro substitution to a line, provided this does not bring the total number
    // of recursive applications above the maximum
    std::shared_ptr< std::vector<std::string> > apply(std::string& line, unsigned int& replacements);


		// INTERNAL API

  protected:

    // do the heavy lifting of applying macro substitution to a line
    std::shared_ptr< std::vector<std::string> > apply_line(std::string& line, unsigned int& replacements, boost::timer::cpu_timer& timer);

    // apply a ruleset of simple macros to a line
    unsigned int apply_simple(std::string& line, std::vector<macro_packages::simple_rule>& ruleset, bool blank = false);

    // apply a ruleset of index macros to a line
    unsigned int apply_index(std::string& line, const std::vector<struct index_abstract>& lhs_indices,
                             const bool semicolon, const bool comma, const bool lhs_present,
                             std::vector<macro_packages::index_rule>& ruleset);

    std::vector<struct index_abstract> get_lhs_indices(std::string lhs);

    void assign_lhs_index_types(std::string rhs, std::vector<struct index_abstract>& lhs_indices,
                                std::vector<macro_packages::index_rule>& ruleset);

    void assign_index_defaults(std::vector<struct index_abstract>& lhs_indices);

    std::vector<std::string> get_argument_list(std::string& line, size_t pos, unsigned int num_args, std::string macro_name);

    std::vector<struct index_abstract> get_index_set(std::string line, size_t pos, std::string name, unsigned int indices, unsigned int range);

    void map_indices(std::string& line, std::string prefx, const std::vector<struct index_assignment>& assignment);


    // INTERNAL DATA

  private:

    unsigned int    fields;
    unsigned int    parameters;
    enum indexorder order;

    unsigned int recursion_depth;
    unsigned int recursion_max;

    translation_unit* unit;
    package_group*    package;

    std::vector<macro_packages::simple_rule>& pre_rule_cache;
    std::vector<macro_packages::simple_rule>& post_rule_cache;
    std::vector<macro_packages::index_rule>&  index_rule_cache;

    const std::string prefix;
    const std::string split;
	};


#endif //__macro_H_
