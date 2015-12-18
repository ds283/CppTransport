//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_CORE_GROUP_H
#define CPPTRANSPORT_CORE_GROUP_H


#include "package_group.h"
#include "u_tensor_factory.h"


class cpp_group : public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    cpp_group(translator_data& p, u_tensor_factory& factory);

    //! destructor is default
    virtual ~cpp_group() = default;

  };


#endif //CPPTRANSPORT_CORE_GROUP_H
