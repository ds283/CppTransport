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


#ifndef CPPTRANSPORT_CORE_GROUP_H
#define CPPTRANSPORT_CORE_GROUP_H


#include "package_group.h"
#include "concepts/tensor_factory.h"


class cpp_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    cpp_group(translator_data& p, tensor_factory& fctry);

    //! destructor is default
    virtual ~cpp_group() = default;

  };


#endif //CPPTRANSPORT_CORE_GROUP_H
