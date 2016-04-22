//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_VEXCL_GROUP_H
#define CPPTRANSPORT_VEXCL_GROUP_H


#include "package_group.h"
#include "concepts/tensor_factory.h"


class vexcl_group: public package_group
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    vexcl_group(translator_data& p, tensor_factory& fctry);

    //! destructor is default
    ~vexcl_group() = default;

  };


#endif //CPPTRANSPORT_VEXCL_GROUP_H
