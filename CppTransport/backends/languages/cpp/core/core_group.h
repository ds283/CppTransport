//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __core_group_H_
#define __core_group_H_


#include "package_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "core_macros.h"
#include "cpp_printer.h"


class core_group: public package_group
  {
  public:
    core_group(macro_packages::replacement_data& d, bool do_cse=true);
    ~core_group();

    void set_buffer(buffer* b)        { this->tp->set_buffer(b); }
    void set_macros(macro_package* m) { this->tp->set_macros(m); }

  protected:
    cpp::cpp_printer                    printer;

    macro_packages::fundamental*        f;
    macro_packages::flow_tensors*       ft;
    macro_packages::lagrangian_tensors* lt;
    macro_packages::utensors*           ut;
    macro_packages::gauge_xfm*          xf;
    macro_packages::temporary_pool*     tp;
    cpp::core_macros*                   cm;
  };


#endif //__core_group_H_
