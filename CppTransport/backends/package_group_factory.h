//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __package_group_factory_H_
#define __package_group_factory_H_


#include <string>

#include "replacement_rule_package.h"
#include "package_group.h"


package_group* package_group_factory(std::string backend, macro_packages::replacement_data& data);


#endif //__package_group_factory_H_
