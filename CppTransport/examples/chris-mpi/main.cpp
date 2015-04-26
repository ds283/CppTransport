//
//  main.cpp (chris-openmp)
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

#include "chris_basic.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;
const double W0       = 1e-5;
const double lambda   = 0.05;

const double phi_init = 0.001;
const double chi_init = 16.5;

static void output_info(transport::canonical_model<double>& model);


// ****************************************************************************


int main(int argc, const char* argv[])
  {
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    const std::vector<double> init_params = { W0, lambda };
    transport::chris_basic<double> model(M_Planck, init_params);

    output_info(model);

    if(argc != 3)
      {
        std::cerr << "syntax: chris-openmp <output directory> <python interpreter>" << std::endl;
      }

    std::string output(argv[1]);
    std::string python(argv[2]);

    // ensure output directory exists
    boost::filesystem::path output_path(output);
//    boost::filesystem::create_directories(root_path);

    transport::python_plot_gadget<double>     py_plt(python);
    transport::asciitable_plot_gadget<double> text_plt;
//    gnuplot_plot_gadget<double> plt;

//    py_plt.set_use_latex(true);
//    py_plt.set_min_x(10);
//    py_plt.set_max_x(28);

    const std::vector<double> init_values = { phi_init, chi_init };

    const double Ncross = 9.0; // horizon-crossing occurs at 9 e-folds from init_values
    const double Npre   = 4.0; // how many e-folds do we wish to track the mode prior to horizon exit?
    const std::vector<double> ics = model.find_ics(init_values, Ncross, Npre);

    const double        tmin = 0;          // begin at time t = 0
    const double        tmax = 60+Npre;    // end at time t = 50
    const unsigned int  tN   = 5000;       // record 500 samples

    std::vector<double> times;
    for(int i = 0; i < tN; ++i)
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
    for(int i = 0; i < kN; ++i)
      {
        ks.push_back(kmin * pow(kmax/kmin, (double)i/(double)kN));
      }

    boost::timer::auto_cpu_timer timer;

    // integrate background, 2pf and 3pf together
    const double Nstar = 7.0;
    transport::threepf<double> threepf = model.threepf(ks, Npre, ics, times);

    timer.stop();
    timer.report();
//      std::cout << threepf;

    transport::background<double> backg = threepf.get_background();

    transport::twopf<double> twopf_re   = threepf.get_real_twopf();
    transport::twopf<double> twopf_im   = threepf.get_imag_twopf();

    std::array<unsigned int, 2> index_set_a = { 0, 0 };
    std::array<unsigned int, 2> index_set_b = { 0, 1 };
    std::array<unsigned int, 2> index_set_c = { 1, 1 };

    std::array<unsigned int, 2> index_set_d = { 2, 0 };
    std::array<unsigned int, 2> index_set_e = { 2, 1 };
    std::array<unsigned int, 2> index_set_f = { 3, 0 };
    std::array<unsigned int, 2> index_set_g = { 3, 1 };

    std::array<unsigned int, 3> sq_set_a    = { 0, 0, 0 };
    std::array<unsigned int, 3> sq_set_b    = { 0, 1, 0 };
    std::array<unsigned int, 3> sq_set_c    = { 1, 1, 0 };
    std::array<unsigned int, 3> sq_set_d    = { 0, 0, 1 };
    std::array<unsigned int, 3> sq_set_e    = { 0, 1, 1 };
    std::array<unsigned int, 3> sq_set_f    = { 1, 1, 1 };

    transport::index_selector<1>* backg_selector    = backg.manufacture_selector();
    transport::index_selector<2>* twopf_re_selector = twopf_re.manufacture_selector();
    transport::index_selector<2>* twopf_im_selector = twopf_im.manufacture_selector();
    transport::index_selector<3>* threepf_selector  = threepf.manufacture_selector();
    transport::index_selector<3>* sq_selector_a     = threepf.manufacture_selector();
    transport::index_selector<3>* sq_selector_b     = threepf.manufacture_selector();
    transport::index_selector<2>* u2_selector       = backg.manufacture_2_selector();
    transport::index_selector<3>* u3_selector       = backg.manufacture_3_selector();

    twopf_re_selector->none();
    twopf_re_selector->set_on(index_set_a);
    twopf_re_selector->set_on(index_set_b);
    twopf_re_selector->set_on(index_set_c);

    twopf_im_selector->none();
    twopf_im_selector->set_on(index_set_d);
    twopf_im_selector->set_on(index_set_e);
    twopf_im_selector->set_on(index_set_f);
    twopf_im_selector->set_on(index_set_g);

    threepf_selector->none();
    threepf_selector->set_on(sq_set_a);
    threepf_selector->set_on(sq_set_b);
    threepf_selector->set_on(sq_set_c);
    threepf_selector->set_on(sq_set_d);
    threepf_selector->set_on(sq_set_e);
    threepf_selector->set_on(sq_set_f);

    sq_selector_a->none();
    sq_selector_a->set_on(sq_set_a);
    sq_selector_a->set_on(sq_set_b);
    sq_selector_a->set_on(sq_set_c);
    sq_selector_b->none();
    sq_selector_b->set_on(sq_set_d);
    sq_selector_b->set_on(sq_set_e);
    sq_selector_b->set_on(sq_set_f);

    backg.plot(&py_plt, output_path.string() + "/background", backg_selector);

    twopf_re.components_time_history(       &py_plt,   output_path.string() + "/re_k_mode",          twopf_re_selector);
    twopf_im.components_time_history(       &py_plt,   output_path.string() + "/im_k_mode",          twopf_im_selector);

    twopf_re.zeta_time_history(             &py_plt,   output_path.string() + "/zeta_twopf_mode");
    twopf_re.zeta_time_history(             &text_plt, output_path.string() + "/zeta_twopf_mode");

    threepf.components_time_history(        &py_plt,   output_path.string() + "/threepf_mode",       threepf_selector);
    threepf.components_time_history(        &py_plt,   output_path.string() + "/threepf_shape_mode", transport::threepf_local_shape(), threepf_selector);
    threepf.components_time_history(        &text_plt, output_path.string() + "/threepf_shape_mode", transport::threepf_local_shape(), threepf_selector);

    threepf.components_time_history(        &py_plt,   output_path.string() + "/sq_config_a_mode",   transport::threepf_local_shape(), sq_selector_a);
    threepf.components_time_history(        &text_plt, output_path.string() + "/sq_config_a_mode",   transport::threepf_local_shape(), sq_selector_a);
    threepf.components_time_history(        &py_plt,   output_path.string() + "/sq_config_b_mode",   transport::threepf_local_shape(), sq_selector_b);
    threepf.components_time_history(        &text_plt, output_path.string() + "/sq_config_b_mode",   transport::threepf_local_shape(), sq_selector_b);

    threepf.zeta_time_history(              &py_plt,   output_path.string() + "/zeta_threepf_mode");

    threepf.reduced_bispectrum_time_history(&py_plt,   output_path.string() + "/redbisp");
    threepf.reduced_bispectrum_time_history(&text_plt, output_path.string() + "/redbisp");

    delete backg_selector;
    delete twopf_re_selector;
    delete twopf_im_selector;
    delete threepf_selector;
    delete sq_selector_a;
    delete sq_selector_b;
    delete u2_selector;
    delete u3_selector;

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

    const std::vector<double> f = { phi_init, chi_init };
    std::cout << "V* = " << model.V(f) << "\n";

    std::cout << "\n";
  }
