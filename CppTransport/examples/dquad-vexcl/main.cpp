//
//  main.cpp (dquad-vexcl)
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

#include "dq_vexcl.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double mass		  = 1E-5 * M_Planck;
const double m_phi    = 9.0 * mass;
const double m_chi    = 1.0 * mass;

const double phi_init = 10;
const double chi_init = 12.9;


static void output_info(transport::canonical_model<double>& model);


// ****************************************************************************


int main(int argc, const char* argv[])
  {
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    const std::vector<double> init_params = { m_phi, m_chi };
    transport::dquad_vexcl<double> model(M_Planck, init_params);

    output_info(model);

    if(argc != 3)
      {
        std::cerr << "syntax: dquad-vexcl <output directory> <python interpreter>" << std::endl;
      }

    std::string output(argv[1]);
    std::string python(argv[2]);

    // ensure output directory exists
    boost::filesystem::path output_path(output);
//    boost::filesystem::create_directories(output_path);

    transport::python_plot_gadget<double>     py_plt(python);
    transport::asciitable_plot_gadget<double> text_plt;
//    gnuplot_plot_gadget<double> plt;

    // set up an OpenCL context and work queue which can
    // be used by boost::odeint to schedule computations
    vex::Context ctx(vex::Filter::Type(CL_DEVICE_TYPE_GPU) && vex::Filter::DoublePrecision);
    std::cout << "Available OpenCL GPU devices:\n";
    std::cout << ctx << "\n";

    const std::vector<double> init_values = { phi_init, chi_init };

    const double Ncross = 9.0; // horizon-crossing occurs at 9 e-folds from init_values
    const double Npre   = 7;   // how many e-folds do we wish to track the mode prior to horizon exit?
    const std::vector<double> ics = model.find_ics(init_values, Ncross, Npre);

    const double        tmin = 0;          // begin at time t = 0
    const double        tmax = 60+Npre;    // end 60 e-folds after horizon-crossing
    const unsigned int  tN   = 5000;       // record 5000 samples

    std::vector<double> times;
    for(int i = 0; i < tN; i++)
      {
        times.push_back(tmin + (tmax - tmin)*(double)i/(double)tN);
      }

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax = exp(3.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int  kN   = 5;          // number of k-points

    std::vector<double> ks;
    for(int i = 0; i < kN; i++)
      {
        ks.push_back(kmin * pow(kmax/kmin, (double)i/(double)kN));
      }

    boost::timer::auto_cpu_timer timer;

    const double Nstar = 7.0;
    transport::twopf<double> tpf = model.twopf(ctx, ks, Nstar, init_values, times);

    timer.stop();
    timer.report();
//    std::cout << tpf;

    transport::background<double> backg = tpf.get_background();

    std::array<unsigned int, 2> index_set_a = { 0, 0 };
    std::array<unsigned int, 2> index_set_b = { 0, 1 };
    std::array<unsigned int, 2> index_set_c = { 1, 1 };

    std::array<unsigned int, 2> index_set_d = { 2, 0 };
    std::array<unsigned int, 2> index_set_e = { 2, 1 };
    std::array<unsigned int, 2> index_set_f = { 3, 0 };
    std::array<unsigned int, 2> index_set_g = { 3, 1 };

    std::array<unsigned int, 2> index_set_h = { 2, 2 };
    std::array<unsigned int, 2> index_set_i = { 2, 3 };
    std::array<unsigned int, 2> index_set_j = { 3, 3 };

    transport::index_selector<1>* backg_selector    = backg.manufacture_selector();

    transport::index_selector<2>* twopf_fields      = tpf.manufacture_selector();
    transport::index_selector<2>* twopf_cross       = tpf.manufacture_selector();
    transport::index_selector<2>* twopf_momenta     = tpf.manufacture_selector();

    twopf_fields->none();
    twopf_fields->set_on(index_set_a);
    twopf_fields->set_on(index_set_b);
    twopf_fields->set_on(index_set_c);

    twopf_cross->none();
    twopf_cross->set_on(index_set_d);
    twopf_cross->set_on(index_set_e);
    twopf_cross->set_on(index_set_f);
    twopf_cross->set_on(index_set_g);

    twopf_momenta->none();
    twopf_momenta->set_on(index_set_h);
    twopf_momenta->set_on(index_set_i);
    twopf_momenta->set_on(index_set_j);

    backg.plot(&py_plt, output_path.string() + "/background", backg_selector);

    tpf.components_time_history(&py_plt, output_path.string() + "/k_fields", twopf_fields);
    tpf.components_time_history(&py_plt, output_path.string() + "/k_cross", twopf_cross);
    tpf.components_time_history(&py_plt, output_path.string() + "/k_momenta", twopf_momenta);

    delete backg_selector;
    delete twopf_fields;
    delete twopf_cross;
    delete twopf_momenta;

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
