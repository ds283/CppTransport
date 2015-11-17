//
//  main.cpp (dquad-mpi)
//  dquad
//
//  Created by David Seery on 26/06/2013.
//  Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#include <iostream>

#include <boost/timer/timer.hpp>
#include <boost/filesystem/operations.hpp>

#include "dq_basic_unrolled.h"


// ****************************************************************************


static void output_info(transport::canonical_model<double>* model, transport::integration_task<double>* tk);


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    std::unique_ptr< transport::task_manager<double> > mgr = std::make_unique< transport::task_manager<double> >(argc, argv);
 
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    std::shared_ptr< transport::dquad_basic<double> > model = mgr->create_model< transport::dquad_basic<double> >();

		mgr->process();

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
    for(int i = 0; i < model->get_N_fields(); ++i)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << fields[i];
      }
    std::cout << '\n';

    std::cout << "Parameters (" << model->get_N_params() << "): ";
    for(int i = 0; i < model->get_N_params(); ++i)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << params[i] << " = " << r_p[i];
      }
    std::cout << '\n';

    std::cout << "V* = " << model->V(tk->get_params(), tk->get_ics().get_vector()) << '\n';

    std::cout << '\n';
  }
