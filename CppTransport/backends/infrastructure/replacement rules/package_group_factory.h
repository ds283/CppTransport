//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __package_group_factory_H_
#define __package_group_factory_H_


#include <string>

#include "replacement_data.h"
#include "package_group.h"
#include "buffer.h"


package_group* package_group_factory(std::string backend, macro_packages::replacement_data& data, buffer* buf, bool do_cse=true);


#endif //__package_group_factory_H_
