//
//  main.cpp (axion-mpi)
//  axion
//
//  Created by David Seery on 26/06/2013.
//  Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include "axion_mpi.h"


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    std::unique_ptr< transport::task_manager<double> > mgr = std::make_unique< transport::task_manager<double> >(argc, argv);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    std::shared_ptr< transport::axion_mpi<double> > model = mgr->create_model< transport::axion_mpi<double> >();

    mgr->process();

    return(EXIT_SUCCESS);
  }
