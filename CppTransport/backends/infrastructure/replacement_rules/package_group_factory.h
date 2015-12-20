//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef PACKAGE_GROUP_FACTORY_H
#define PACKAGE_GROUP_FACTORY_H


#include <string>
#include <memory>

#include "translator_data.h"
#include "package_group.h"
#include "buffer.h"
#include "backend_data.h"

#include "concepts/tensor_factory.h"
#include "symbol_factory.h"
#include "ginac_cache.h"


std::unique_ptr<package_group> package_group_factory(const boost::filesystem::path& in, const backend_data& backend,
                                                     translator_data& payload, tensor_factory& fctry);


#endif //PACKAGE_GROUP_FACTORY_H
