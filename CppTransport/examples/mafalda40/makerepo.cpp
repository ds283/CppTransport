//
// Created by David Seery on 10/06/2014.
// Copyright (c) 2013-14 University of Sussex. All rights reserved.
//


#include "mafalda40_basic_twopf.h"

#include "transport-runtime-api/repository/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

#define INIT_VALUE_LIST { 3.5623994785877113, -5.939408670155248, -2.0992141982240935, \
                                   4.298878339454238, 3.2630727863108313, -3.8233456409111355, 5.720635369231927, \
                                   4.316092925616966, 1.2358063474515841, -0.35717453801930366, \
                                   -1.2965806350819717, 1.9483683292641143, -1.1402076723394798, \
                                   0.25241081137733035, 2.8526182435002903, 2.1020473441601197, 3.56818001993791, \
                                   -0.3304286006069248, -0.5327965911255463, 0.4437314994936759, \
                                   2.0555690549413153, -2.0053246205815536, -2.120303046765226, \
                                   -0.1413937994572644, -2.0947415513247947, -1.9983292650876643, \
                                   -2.0430505775940375, 0.2304834083121715, 1.0329761159612512, \
                                   0.022419681272499315, -1.5043508561729206, 0.44470683528284294, \
                                   0.25620753210752095, -2.4823731442400296, 1.1652989233612543, \
                                   1.6918503953200772, 0.16661997764471065, 2.8349425152737022, 2.4850230423021435, \
                                   1.6200875823145793 }

// ****************************************************************************


bool timeseries_filter(const transport::derived_data::filter::time_filter_data& data)
	{
    return(true); // plot all points
	}


bool twopf_timeseries_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
    return(data.min || data.max); // plot all values of k
	}


bool spectrum_timefilter(const transport::derived_data::filter::time_filter_data& data)
	{
    return(data.max);
	}


bool all_k_modes(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
    return(true); // plot only the largest k
	}


int main(int argc, char* argv[])
  {
		if(argc != 2)
			{
		    std::cerr << "makerepo: Too few arguments. Expected repository name" << std::endl;
		    exit(EXIT_FAILURE);
			}

    transport::repository_creation_key key;

    transport::json_interface_repository<double>* repo = transport::repository_factory<double>(argv[1], key);

    // set up an instance of a manager
    transport::task_manager<double>* mgr = new transport::task_manager<double>(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::mafalda40_basic<double>* model = new transport::mafalda40_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params;
    transport::parameters<double> params      =
                                    transport::parameters<double>(M_Planck, init_params, model);

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 5.0;  // horizon-crossing occurs at 5 e-folds from init_values
    const double Npre   = 5.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 55.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
		const std::vector<double> init_values = INIT_VALUE_LIST;
    transport::initial_conditions<double> ics =
                                            transport::initial_conditions<double>("mafalda40", model, params, init_values, Ninit, Ncross, Npre);

    const unsigned int t_samples = 1000;       // record 2000 samples - enough to find a good stepsize

    struct TimeStoragePolicy
      {
      public:
        bool operator() (const transport::integration_task<double>::time_config_storage_policy_data& data) { return((data.serial % 1) == 0); }
      };

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(10.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int  k_samples = 100;         // number of k-points

    transport::range<double> ks = transport::range<double>(kmin, kmax, k_samples, transport::logarithmic_bottom_stepping);

    // construct a twopf task
    transport::twopf_task<double> tk2 = transport::twopf_task<double>("mafalda40.twopf-1", ics, times, ks, TimeStoragePolicy());

		// construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
		bg_sel.all();

    transport::derived_data::background_time_series<double> tk2_bg = transport::derived_data::background_time_series<double>(tk2, bg_sel, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::time_series_plot<double> tk2_bg_plot =
	                                                       transport::derived_data::time_series_plot<double>("mafalda40.twopf-1.background", "background.pdf");
		tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");


    transport::derived_data::zeta_twopf_time_series<double> tk2_zeta_times = transport::derived_data::zeta_twopf_time_series<double>(tk2,
                                                                                                                                     transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                                                                     transport::derived_data::filter::twopf_kconfig_filter(twopf_timeseries_filter));

    transport::derived_data::time_series_plot<double> tk2_zeta_timeplot = transport::derived_data::time_series_plot<double>("mafalda40.twopf-1.zeta-twopf", "zeta-twopf.pdf");
		tk2_zeta_timeplot.add_line(tk2_zeta_times);
		tk2_zeta_timeplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ time evolution");
		tk2_zeta_timeplot.set_legend_position(transport::derived_data::bottom_right);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk2_zeta_spec = transport::derived_data::zeta_twopf_wavenumber_series<double>(tk2,
                                                                                                                                                transport::derived_data::filter::time_filter(spectrum_timefilter),
                                                                                                                                                transport::derived_data::filter::twopf_kconfig_filter(all_k_modes));

    transport::derived_data::wavenumber_series_plot<double> tk2_zeta_specplot = transport::derived_data::wavenumber_series_plot<double>("mafalda40.twopf-1.zeta-spec", "zeta-spec.pdf");
		tk2_zeta_specplot.add_line(tk2_zeta_spec);
		tk2_zeta_specplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ spectrum");

    transport::derived_data::wavenumber_series_table<double> tk2_zeta_spectable = transport::derived_data::wavenumber_series_table<double>("mafalda40.twopf-1.zeta-spec.table", "zeta-spec-table.txt");
		tk2_zeta_spectable.add_line(tk2_zeta_spec);

		// construct output tasks
    transport::output_task<double> twopf_output   = transport::output_task<double>("mafalda40.twopf-1.output", tk2_bg_plot);
		twopf_output.add_element(tk2_zeta_timeplot);
		twopf_output.add_element(tk2_zeta_specplot);
		twopf_output.add_element(tk2_zeta_spectable);

		// write output tasks to the database
		repo->commit_task(twopf_output);

    delete mgr;     // task_manager adopts its repository and destroys it silently; also destroys any registered models

    return(EXIT_SUCCESS);
  }
