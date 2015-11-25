//
//  main.cpp (quadratic10-mpi)
//  axion
//
//  Created by David Seery on 10/06/2013.
//  Copyright (c) 2013-14 University of Sussex. All rights reserved.
//

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#include <iostream>

#include "quadratic10_basic.h"


// ****************************************************************************


static void output_info(transport::canonical_model<double>* model, transport::integration_task<double>* tk);


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    std::unique_ptr< transport::task_manager<double> > mgr = std::make_unique< transport::task_manager<double> >(argc, argv);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    std::shared_ptr< transport::quadratic10_basic<double> > model = mgr->create_model< transport::quadratic10_basic<double> >();

		mgr->process();

    return(EXIT_SUCCESS);
  }
