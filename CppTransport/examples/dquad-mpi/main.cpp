//
//  main.cpp (dquad-mpi)
//  dquad
//
//  Created by David Seery on 26/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#include <iostream>

#include <boost/timer/timer.hpp>
#include <boost/filesystem/operations.hpp>

#include "dq_basic.h"


// ****************************************************************************


static void output_info(transport::canonical_model<double>* model, transport::integration_task<double>* tk);


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    transport::task_manager<double>* mgr = new transport::task_manager<double>(argc, argv);
 
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::dquad_basic<double>* model = new transport::dquad_basic<double>(mgr);

    if(mgr->is_master()) mgr->execute_tasks();
    else                 mgr->wait_for_tasks();

    // models must all be destroyed before the corresponding manager
    delete mgr;   // task manager adopts and destroys its repository, and any registered models

    return(EXIT_SUCCESS);
  }


// ****************************************************************************


// interrogate an arbitrary canonical_model object and print information about it
void output_info(transport::canonical_model<double>* model, transport::integration_task<double>* tk)
  {
    std::cout << "Model:   " << model->get_name() << "\n";
    std::cout << "Authors: " << model->get_author() << "\n";
    std::cout << "  -- "     << model->get_tag() << "\n\n";

    std::vector<std::string>  fields = model->get_field_names();
    std::vector<std::string>  params = model->get_param_names();
    const std::vector<double> r_p    = tk->get_params().get_vector();

    std::cout << "Fields (" << model->get_N_fields() << "): ";
    for(int i = 0; i < model->get_N_fields(); i++)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << fields[i];
      }
    std::cout << std::endl;

    std::cout << "Parameters (" << model->get_N_params() << "): ";
    for(int i = 0; i < model->get_N_params(); i++)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << params[i] << " = " << r_p[i];
      }
    std::cout << std::endl;

    std::cout << "V* = " << model->V(tk->get_params(), tk->get_ics().get_vector()) << std::endl;

    std::cout << std::endl;
  }
