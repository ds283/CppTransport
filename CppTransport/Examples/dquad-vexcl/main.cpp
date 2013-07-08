//
//  main.cpp
//  dquad
//
//  Created by David Seery on 26/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#include <iostream>

#include <boost/timer/timer.hpp>

#include "dq.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double M		    = 1E-5 * M_Planck;
const double m_phi    = 9.0 * M;
const double m_chi    = 1.0 * M;

const double phi_init = 8.2;
const double chi_init = 12.9;


static void output_info(transport::canonical_model<double>& model);


// ****************************************************************************


int main(int argc, const char* argv[])
  {
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    const std::vector<double> init_params = { m_phi, m_chi };
    transport::dquad<double> model(M_Planck, init_params);
	 
	 python_plot_maker<double> plt("/opt/local/bin/python2.7");

    output_info(model);

    // set up an OpenCL context and work queue which can
    // be used by boost::odeint to schedule computations
    vex::Context ctx(vex::Filter::Type(CL_DEVICE_TYPE_GPU) && vex::Filter::DoublePrecision);
    std::cout << "Available OpenCL GPU devices:\n";
    std::cout << ctx << "\n";

    const std::vector<double> init_values = { phi_init, chi_init };

    const double       tmin = 0;          // begin at time t = 0
    const double       tmax = 55;         // end at time t = 55
    const unsigned int tN   = 100;        // record 100 samples
    std::vector<double> times;
    for(int i = 0; i <= tN; i++)
      {
        times.push_back(tmin + ((tmax-tmin)/tN)*i);
      }

    std::cout << "Integrating background" << std::endl;
    {
      boost::timer::auto_cpu_timer timer;

      // integrate background
      transport::background<double> backg = model.background(init_values, times);

      timer.stop();
      timer.report();
//    std::cout << backg;

      backg.plot(&plt, "/Users/ds283/Desktop/background.pdf", "Double-quadratic inflation");
    }

    // integrate two-point function
    const double       kmin = exp(0.0);   // begin with the mode corresponding the horizon at the start of integration
    const double       kmax = exp(2.0);   // end with the mode which exited the horizon 3 e-folds later
    const unsigned int kN   = 3;          // number of k-points
    std::vector<double> ks;
    for(int i = 0; i <= kN; i++)
      {
        ks.push_back(kmin * pow(kmax/kmin, ((double)i/(double)kN)));
      }

    std::cout << std::endl;
    std::cout << "Integrating two-point function" << std::endl;
    {
      boost::timer::auto_cpu_timer timer;

      transport::twopf<double> tpf = model.twopf(ks, 7.0, init_values, times);

      timer.stop();
      timer.report();
//    std::cout << tpf;

      tpf.time_history(&plt, "/Users/ds283/Desktop/kplots/k_mode");
    }

    return(EXIT_SUCCESS);
  }


// ****************************************************************************


// interrogate an arbitrary canonical_model object and print information about it
void output_info(transport::canonical_model<double>& model)
  {
    std::cout << "Model:   " << model.get_name() << "\n";
    std::cout << "Authors: " << model.get_author() << "\n";
    std::cout << "  -- "     << model.get_tag() << "\n\n";

    std::vector<std::string>  fields = model.get_field_names();
    std::vector<std::string>  params = model.get_param_names();
    const std::vector<double> r_p    = model.get_parameters();

    std::cout << "Fields (" << model.get_N_fields() << "): ";
    for(int i = 0; i < fields.size(); i++)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << fields[i];
      }
    std::cout << "\n";

    std::cout << "Parameters (" << model.get_N_params() << "): ";
    for(int i = 0; i < params.size(); i++)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << params[i] << " = " << r_p[i];
      }
    std::cout << "\n";

    const std::vector<double> f = { phi_init, chi_init };
    std::cout << "V* = " << model.V(f) << "\n";

    std::cout << "\n";
  }
