//
//  main.cpp (dquad-openmp)
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


static void output_info(transport::canonical_model<double>* model, transport::task<double>* tk);


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

		if(mgr->is_master())
			{
				transport::repository<double>* repo = mgr->get_repository();
		    std::list<typename transport::repository<double>::output_group> output = repo->enumerate_task_output("dquad.threepf-1");

		    for(std::list<typename transport::repository<double>::output_group>::iterator t = output.begin(); t != output.end(); t++)
			    {
		        t->write(std::cout);
		        std::cout << std::endl;
			    }
			}

//    transport::python_plot_gadget<double>     py_plt(python);
//    transport::asciitable_plot_gadget<double> text_plt;
//    gnuplot_plot_gadget<double> plt;

//    py_plt.set_use_latex(true);
//    py_plt.set_min_x(10);
//    py_plt.set_max_x(28);


//    output_info(model, &tk);

//    boost::timer::auto_cpu_timer timer;
//
//    // integrate background, 2pf and 3pf together
//    transport::threepf<double> threepf = model->int_threepf(tk);
//
//    timer.stop();
//    timer.report();
////      std::cout << threepf;
//
//    transport::background<double> backg = threepf.get_background();
//
//    transport::twopf<double> twopf_re   = threepf.get_real_twopf();
//    transport::twopf<double> twopf_im   = threepf.get_imag_twopf();
//
//    std::array<unsigned int, 2> index_set_a = { 0, 0 };
//    std::array<unsigned int, 2> index_set_b = { 0, 1 };
//    std::array<unsigned int, 2> index_set_c = { 1, 1 };
//
//    std::array<unsigned int, 2> index_set_d = { 2, 0 };
//    std::array<unsigned int, 2> index_set_e = { 2, 1 };
//    std::array<unsigned int, 2> index_set_f = { 3, 0 };
//    std::array<unsigned int, 2> index_set_g = { 3, 1 };
//
//    std::array<unsigned int, 2> index_set_h = { 2, 2 };
//    std::array<unsigned int, 2> index_set_i = { 2, 3 };
//    std::array<unsigned int, 2> index_set_j = { 3, 3 };
//
//    std::array<unsigned int, 3> sq_set_a    = { 0, 0, 0 };
//    std::array<unsigned int, 3> sq_set_b    = { 0, 1, 0 };
//    std::array<unsigned int, 3> sq_set_c    = { 1, 1, 0 };
//    std::array<unsigned int, 3> sq_set_d    = { 0, 0, 1 };
//    std::array<unsigned int, 3> sq_set_e    = { 0, 1, 1 };
//    std::array<unsigned int, 3> sq_set_f    = { 1, 1, 1 };
//
//    transport::index_selector<1>* backg_selector    = backg.manufacture_selector();
//    transport::index_selector<2>* twopf_fields      = twopf_re.manufacture_selector();
//    transport::index_selector<2>* twopf_cross       = twopf_re.manufacture_selector();
//    transport::index_selector<2>* twopf_momenta     = twopf_re.manufacture_selector();
//    transport::index_selector<3>* threepf_selector  = threepf.manufacture_selector();
//    transport::index_selector<3>* sq_selector_a     = threepf.manufacture_selector();
//    transport::index_selector<3>* sq_selector_b     = threepf.manufacture_selector();
//    transport::index_selector<2>* u2_selector       = backg.manufacture_2_selector();
//    transport::index_selector<3>* u3_selector       = backg.manufacture_3_selector();
//
//    twopf_fields->none();
//    twopf_fields->set_on(index_set_a);
//    twopf_fields->set_on(index_set_b);
//    twopf_fields->set_on(index_set_c);
//
//    twopf_cross->none();
//    twopf_cross->set_on(index_set_d);
//    twopf_cross->set_on(index_set_e);
//    twopf_cross->set_on(index_set_f);
//    twopf_cross->set_on(index_set_g);
//
//    twopf_momenta->none();
//    twopf_momenta->set_on(index_set_h);
//    twopf_momenta->set_on(index_set_i);
//    twopf_momenta->set_on(index_set_j);
//
//    threepf_selector->none();
//    threepf_selector->set_on(sq_set_a);
//    threepf_selector->set_on(sq_set_b);
//    threepf_selector->set_on(sq_set_c);
//    threepf_selector->set_on(sq_set_d);
//    threepf_selector->set_on(sq_set_e);
//    threepf_selector->set_on(sq_set_f);
//
//    sq_selector_a->none();
//    sq_selector_a->set_on(sq_set_a);
//    sq_selector_a->set_on(sq_set_b);
//    sq_selector_a->set_on(sq_set_c);
//    sq_selector_b->none();
//    sq_selector_b->set_on(sq_set_d);
//    sq_selector_b->set_on(sq_set_e);
//    sq_selector_b->set_on(sq_set_f);
//
//    backg.plot(&py_plt, output_path.string() + "/background", backg_selector);
//
//    twopf_re.components_time_history(       &py_plt,   output_path.string() + "/re_k_fields",        twopf_fields);
//    twopf_re.components_time_history(       &py_plt,   output_path.string() + "/re_k_cross",         twopf_cross);
//    twopf_re.components_time_history(       &py_plt,   output_path.string() + "/re_k_momenta",       twopf_momenta);
//
//    twopf_im.components_time_history(       &py_plt,   output_path.string() + "/im_k_fields",        twopf_fields);
//    twopf_im.components_time_history(       &py_plt,   output_path.string() + "/im_k_cross",         twopf_cross);
//    twopf_im.components_time_history(       &py_plt,   output_path.string() + "/im_k_momenta",       twopf_momenta);
//
//    twopf_re.zeta_time_history(             &py_plt,   output_path.string() + "/zeta_twopf_mode");
//    twopf_re.zeta_time_history(             &text_plt, output_path.string() + "/zeta_twopf_mode");
//
//    threepf.components_time_history(        &py_plt,   output_path.string() + "/threepf_mode",       threepf_selector);
//    threepf.components_time_history(        &text_plt, output_path.string() + "/threepf_mode",       threepf_selector);
//    threepf.components_time_history(        &py_plt,   output_path.string() + "/threepf_shape_mode", transport::threepf_local_shape(), threepf_selector);
//    threepf.components_time_history(        &text_plt, output_path.string() + "/threepf_shape_mode", transport::threepf_local_shape(), threepf_selector);
//
//    threepf.components_time_history(        &py_plt,   output_path.string() + "/sq_config_a_mode",   transport::threepf_local_shape(), sq_selector_a);
//    threepf.components_time_history(        &text_plt, output_path.string() + "/sq_config_a_mode",   transport::threepf_local_shape(), sq_selector_a);
//    threepf.components_time_history(        &py_plt,   output_path.string() + "/sq_config_b_mode",   transport::threepf_local_shape(), sq_selector_b);
//    threepf.components_time_history(        &text_plt, output_path.string() + "/sq_config_b_mode",   transport::threepf_local_shape(), sq_selector_b);
//
//    threepf.zeta_time_history(              &py_plt,   output_path.string() + "/zeta_threepf_mode");
//
//    threepf.reduced_bispectrum_time_history(&py_plt,   output_path.string() + "/redbisp");
//    threepf.reduced_bispectrum_time_history(&text_plt, output_path.string() + "/redbisp");
//
//    delete backg_selector;
//    delete twopf_fields;
//    delete twopf_cross;
//    delete twopf_momenta;
//    delete threepf_selector;
//    delete sq_selector_a;
//    delete sq_selector_b;
//    delete u2_selector;
//    delete u3_selector;

    // models must all be destroyed before the corresponding manager
    delete mgr;   // task manager adopts and destroys its repository, and any registered models

    return(EXIT_SUCCESS);
  }


// ****************************************************************************


// interrogate an arbitrary canonical_model object and print information about it
void output_info(transport::canonical_model<double>* model, transport::task<double>* tk)
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
