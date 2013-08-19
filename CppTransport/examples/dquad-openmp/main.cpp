//
//  main.cpp (dquad-openmp)
//  dquad
//
//  Created by David Seery on 26/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#include <iostream>

#include <boost/timer/timer.hpp>

#include "dq_basic.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double mass		  = 1E-5 * M_Planck;
const double m_phi    = 9.0 * mass;
const double m_chi    = 1.0 * mass;

const double phi_init = 8.2;
const double chi_init = 12.9;


static void output_info(transport::canonical_model<double>& model);


// ****************************************************************************


int main(int argc, const char* argv[])
  {
    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    const std::vector<double> init_params = { m_phi, m_chi };
    transport::dquad_basic<double> model(M_Planck, init_params);

    output_info(model);

    if(argc != 3)
      {
        std::cerr << "syntax: dquad-openmp <output directory> <python interpreter>" << std::endl;
      }

    std::string output(argv[1]);
    std::string python(argv[2]);

    transport::python_plot_gadget<double>     py_plt(python);
    transport::asciitable_plot_gadget<double> text_plt;
//    gnuplot_plot_gadget<double> plt;

//    py_plt.set_use_latex(true);

    const std::vector<double> init_values = { phi_init, chi_init };

    const double        tmin = 0;          // begin at time t = 0
    const double        tmax = 55;         // end at time t = 50
    const unsigned int  tN   = 1000;        // record 500 samples

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

    // integrate background, 2pf and 3pf together
    const double Nstar = 7.0;
    transport::threepf<double> threepf = model.threepf(ks, Nstar, init_values, times);

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

    std::array<unsigned int, 3> three_set_a = { 0, 0, 0 };
    std::array<unsigned int, 3> three_set_b = { 0, 0, 1 };
    std::array<unsigned int, 3> three_set_c = { 0, 1, 1 };
    std::array<unsigned int, 3> three_set_d = { 1, 1, 1 };
    std::array<unsigned int, 3> three_set_e = { 0, 2, 0 };
    std::array<unsigned int, 3> three_set_f = { 0, 2, 1 };
    std::array<unsigned int, 3> three_set_g = { 0, 3, 0 };
    std::array<unsigned int, 3> three_set_h = { 0, 3, 1 };
    std::array<unsigned int, 3> three_set_i = { 1, 2, 0 };
    std::array<unsigned int, 3> three_set_j = { 1, 2, 1 };
    std::array<unsigned int, 3> three_set_k = { 1, 3, 0 };
    std::array<unsigned int, 3> three_set_l = { 1, 3, 1 };

    std::array<unsigned int, 3> three_set_m = { 2, 0, 0 };
    std::array<unsigned int, 3> three_set_n = { 2, 0, 1 };
    std::array<unsigned int, 3> three_set_o = { 2, 1, 1 };
    std::array<unsigned int, 3> three_set_p = { 3, 0, 0 };
    std::array<unsigned int, 3> three_set_q = { 3, 0, 1 };
    std::array<unsigned int, 3> three_set_r = { 3, 1, 1 };

    transport::index_selector<1>* backg_selector    = backg.manufacture_selector();
    transport::index_selector<2>* twopf_re_selector = twopf_re.manufacture_selector();
    transport::index_selector<2>* twopf_im_selector = twopf_im.manufacture_selector();
    transport::index_selector<3>* threepf_selector  = threepf.manufacture_selector();
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
    threepf_selector->set_on(three_set_a);
    threepf_selector->set_on(three_set_b);
    threepf_selector->set_on(three_set_c);
    threepf_selector->set_on(three_set_d);

    backg.plot(&py_plt, output + "/background", backg_selector);

    u2_selector->none();
    u2_selector->set_on(index_set_d);
    u2_selector->set_on(index_set_e);
    u2_selector->set_on(index_set_f);
    u2_selector->set_on(index_set_g);
    backg.plot_u2(&py_plt, 0.3, output + "/u2_mom_k=pt3", u2_selector);

    u3_selector->none();
    u3_selector->set_on(three_set_e);
    u3_selector->set_on(three_set_f);
    u3_selector->set_on(three_set_g);
    u3_selector->set_on(three_set_h);
    u3_selector->set_on(three_set_i);
    u3_selector->set_on(three_set_j);
    u3_selector->set_on(three_set_k);
    u3_selector->set_on(three_set_l);
    backg.plot_u3(&py_plt, 0.3, 0.3, 0.3, output + "/u3_fields_k=pt3", u3_selector);

    u3_selector->none();
    u3_selector->set_on(three_set_m);
    u3_selector->set_on(three_set_n);
    u3_selector->set_on(three_set_o);
    u3_selector->set_on(three_set_p);
    u3_selector->set_on(three_set_q);
    u3_selector->set_on(three_set_r);
    backg.plot_u3(&py_plt, 0.3, 0.3, 0.3, output + "/u3_mom_k=pt3", u3_selector);

    twopf_re.components_time_history(&py_plt, output + "/re_k_mode", twopf_re_selector);
    twopf_im.components_time_history(&py_plt, output + "/im_k_mode", twopf_im_selector);
    twopf_re.zeta_time_history(&py_plt, output + "/zeta_twopf_mode");

    threepf.components_time_history(&py_plt, output + "/threepf_mode", threepf_selector);
    threepf.components_dotphi_time_history(&py_plt, output + "/threepf_dotphi_mode", threepf_selector);
    threepf.zeta_time_history(&py_plt, output + "/zeta_threepf_mode");
    threepf.reduced_bispectrum_time_history(&py_plt, output + "/redbisp");
    threepf.reduced_bispectrum_time_history(&text_plt, output + "/redbisp");

    delete backg_selector;
    delete twopf_re_selector;
    delete twopf_im_selector;
    delete threepf_selector;
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
