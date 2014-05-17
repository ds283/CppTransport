//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "dq_basic.h"

#include "transport-runtime-api/manager/repository_creation_key.h"


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


// ****************************************************************************


// filter to determine which time values are included on plots
bool time_filter(double N)
	{
    return(true); // plot all values of the time
	}


int main(int argc, char* argv[])
  {
		if(argc != 2)
			{
		    std::cerr << "makerepo: Too few arguments. Expected repository name" << std::endl;
		    exit(EXIT_FAILURE);
			}

    transport::repository_creation_key key;

    transport::repository<double>* repo = transport::repository_factory<double>(argv[1], key);

    // set up an instance of a manager
    transport::task_manager<double>* mgr = new transport::task_manager<double>(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::dquad_basic<double>* model = new transport::dquad_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { m_phi, m_chi };
    transport::parameters<double> params      =
                                    transport::parameters<double>(M_Planck, init_params, model->get_param_names(),
                                                                  model->params_validator_factory());

    const std::vector<double> init_values = { phi_init, chi_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 9.0;  // horizon-crossing occurs at 9 e-folds from init_values
    const double Npre   = 5.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 59.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
    transport::initial_conditions<double> ics =
                                            transport::initial_conditions<double>("dquad-1", params, init_values, model->get_state_names(),
                                                                                  Ninit, Ncross, Npre,
                                                                                  model->ics_validator_factory(),
                                                                                  model->ics_finder_factory());

    const unsigned int t_samples = 5000;       // record 5000 samples - enough to find a good stepsize

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(2.0);   // end with the mode which exits the horizon at N=N*+2
    const unsigned int  k_samples = 10;         // number of k-points

    transport::range<double> ks = transport::range<double>(kmin, kmax, k_samples);

    // construct a threepf task
    transport::threepf_task<double> tk3 = transport::threepf_task<double>("dquad.threepf-1", ics, times, ks, model->kconfig_kstar_factory());

    // construct a twopf task
    transport::twopf_task<double> tk2 = transport::twopf_task<double>("dquad.twopf-1", ics, times, ks, model->kconfig_kstar_factory());

    // write each initial conditions/parameter specification and integration specification into the model repository
    repo->write_task(tk2, model);
    repo->write_task(tk3, model);

		// construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
		bg_sel.all();
    transport::derived_data::background<double> twopf_bg_plot =
	                                                transport::derived_data::background<double>("dquad.twopf-1.background", "background.pdf", tk2,
                                                                                              typename transport::derived_data::plot2d_product<double>::time_filter(time_filter),
                                                                                              bg_sel, model);
		twopf_bg_plot.set_title_text("Background fields");

    transport::derived_data::background<double> threepf_bg_plot =
	                                                transport::derived_data::background<double>("dquad.threepf-1.background", "background.pdf", tk3,
                                                                                              typename transport::derived_data::plot2d_product<double>::time_filter(time_filter),
	                                                                                            bg_sel, model);
		threepf_bg_plot.set_title_text("Background fields");

//    std::cout << "2pf background plot:" << std::endl << twopf_bg_plot << std::endl;
//    std::cout << "3pf background plot:" << std::endl << threepf_bg_plot << std::endl;

		// write derived data products representing these background plots to the database
		repo->write_derived_data(twopf_bg_plot);
		repo->write_derived_data(threepf_bg_plot);

		// construct an output task
    transport::output_task<double> twopf_output   = transport::output_task<double>("dquad.twopf-1.output", twopf_bg_plot);
    transport::output_task<double> threepf_output = transport::output_task<double>("dquad.threepf-1.output", threepf_bg_plot);

		// write output tasks to the database
		repo->write_task(twopf_output);
		repo->write_task(threepf_output);

    std::string package_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_package_document(ics.get_name());
    std::cout << "Package JSON document:" << std::endl << package_json << std::endl << std::endl;

    std::string task2_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(tk2.get_name());
    std::cout << "2pf integration JSON document:" << std::endl << task2_json << std::endl << std::endl;

    std::string task3_json = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(tk3.get_name());
    std::cout << "3pf integration JSON document:" << std::endl << task3_json << std::endl << std::endl;

    std::string out3_json  = dynamic_cast<transport::repository_unqlite<double>*>(repo)->json_task_document(threepf_output.get_name());
    std::cout << "3pf output task document:" << std::endl << out3_json << std::endl << std::endl;

    delete mgr;     // task_manager adopts its repository and destroys it silently; also destroys any registered models

    return(EXIT_SUCCESS);
  }
