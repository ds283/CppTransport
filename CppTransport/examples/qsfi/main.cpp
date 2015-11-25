//
//  main.cpp (qsfi-mpi)
//  qsfi
//
//  Created by David Seery on 12/06/2015.
//  Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#include <iostream>

#include <boost/timer/timer.hpp>
#include <boost/filesystem/operations.hpp>

#include "qsfi_basic_unrolled.h"


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    std::unique_ptr< transport::task_manager<double> > mgr = std::make_unique< transport::task_manager<double> >(argc, argv);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    std::shared_ptr< transport::QSFI_basic<double> > model = mgr->create_model< transport::QSFI_basic<double> >();

		mgr->process();

    return(EXIT_SUCCESS);
  }
