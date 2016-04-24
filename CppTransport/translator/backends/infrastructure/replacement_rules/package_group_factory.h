//
// Created by David Seery on 05/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
