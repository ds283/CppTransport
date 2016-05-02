//
// Created by David Seery on 01/05/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

// include implementation header generated by translator
#include "dquad_mpi.h"

int main(int argc, char* argv[])
  {
    // set up a task_manager instance to control this process
    transport::task_manager<> mgr(argc, argv);

    // set up an instance of the double quadratic model
    std::shared_ptr< transport::dquad_mpi<> > model = mgr.create_model< transport::dquad_mpi<> >();

    // hand off control to the task manager
    mgr.process();

    return(EXIT_SUCCESS);
  }
