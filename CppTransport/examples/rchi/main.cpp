//
//  main.cpp (rchi)
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

#include "rchi.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double lambda = 1E-2;
const double xi = 1E6;
const double mu = 1.0;
const double A = 0.8;
const double C = 0.6;

const double h_init = 0.8;


static void output_info(transport::canonical_model<double>& model);


// ****************************************************************************


int main(int argc, const char* argv[])
  {
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    const std::vector<double> init_params = { lambda, xi, mu, A, C };
    transport::rad_higgs<double> model(M_Planck, init_params);

    output_info(model);

    if(argc != 3)
      {
        std::cerr << "syntax: rchi-openmp <output directory> <python interpreter>" << '\n';
      }

    std::string output(argv[1]);
    std::string python(argv[2]);

    python_plot_gadget<double> plt(python);
//    gnuplot_plot_gadget<double> plt;
//    asciitable_plot_gadget<double> plt;

    const std::vector<double> init_values = { h_init };

    const double       tmin = 0;          // begin at time t = 0
    const double       tmax = 50;         // end at time t = 50
    const unsigned int tN   = 1000;       // record 1000 samples
    std::vector<double> times;
    for(int i = 0; i <= tN; ++i)
      {
        times.push_back(tmin + ((tmax-tmin)/tN)*i);
      }

    std::cout << "Integrating background" << '\n';
    {
      boost::timer::auto_cpu_timer timer;

      // integrate background
      transport::background<double> backg = model.background(init_values, times);

      timer.stop();
      timer.report();
      // This line writes the solution to the screen, if desired
//      std::cout << backg;

      // This line produces the plot of the background
      backg.plot(&plt, output + "/background", "Double-quadratic inflation");
    }

    // integrate two-point function
    const double       kmin = exp(0.0);   // begin with the mode corresponding the horizon at the start of integration
    const double       kmax = exp(2.0);   // end with the mode which exited the horizon 2 e-folds later
    const unsigned int kN   = 3;          // number of k-points
    std::vector<double> ks;
    for(int i = 0; i <= kN; ++i)
      {
        ks.push_back(kmin * pow(kmax/kmin, ((double)i/(double)kN)));
      }

    std::cout << '\n';
    std::cout << "Integrating two-point function" << '\n';
    {
      boost::timer::auto_cpu_timer timer;

      transport::twopf<double> tpf = model.twopf(ks, 7.0, init_values, times);

      timer.stop();
      timer.report();
//    std::cout << tpf;

      // this set of lines produce the two-point function plots
      transport::index_selector<2>* selector = tpf.manufacture_selector();
      tpf.components_time_history(&plt, output + "/k_mode", selector, "pdf", false);
      tpf.zeta_time_history(&plt, output + "/zeta_k_mode");
      delete selector;
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
    for(int i = 0; i < fields.size(); ++i)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << fields[i];
      }
    std::cout << "\n";

    std::cout << "Parameters (" << model.get_N_params() << "): ";
    for(int i = 0; i < params.size(); ++i)
      {
        if(i > 0)
          {
            std::cout << ", ";
          }
        std::cout << params[i] << " = " << r_p[i];
      }
    std::cout << "\n";

    const std::vector<double> f = { h_init };
    std::cout << "V* = " << model.V(f) << "\n";

    std::cout << "\n";
  }
