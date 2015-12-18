//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __package_group_factory_H_
#define __package_group_factory_H_


#include <string>
#include <memory>

#include "translator_data.h"
#include "package_group.h"
#include "buffer.h"
#include "backend_data.h"

#include "symbol_factory.h"
#include "ginac_cache.h"


std::unique_ptr<package_group> package_group_factory(const boost::filesystem::path& in, const backend_data& backend, translator_data& payload, u_tensor_factory& factory);


#endif //__package_group_factory_H_
