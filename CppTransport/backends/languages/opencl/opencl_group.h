//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_OPENCL_GROUP_H
#define CPPTRANSPORT_OPENCL_GROUP_H


#include "package_group.h"
#include "concepts/tensor_factory.h"


class opencl_group: public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    opencl_group(translator_data& p, tensor_factory& fctry);

    //! destructor is default
    ~opencl_group() = default;

  };


#endif //CPPTRANSPORT_OPENCL_GROUP_H
