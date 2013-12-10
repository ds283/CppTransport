//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __package_group_factory_H_
#define __package_group_factory_H_


#include <string>

#include "translation_unit.h"
#include "package_group.h"
#include "buffer.h"


package_group* package_group_factory(std::string backend, translation_unit* unit, buffer* buf);


#endif //__package_group_factory_H_
