//
// Created by David Seery on 10/06/2014.
// Copyright (c) 2013-14 University of Sussex. All rights reserved.
//


#include "mafalda40_basic_twopf.h"


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




int main(int argc, char* argv[])
  {
		if(argc != 2)
			{
		    std::cerr << "makerepo: Too few arguments. Expected repository name" << '\n';
		    exit(EXIT_FAILURE);
			}

    std::shared_ptr< transport::json_repository<double> > repo = transport::repository_factory<double>(argv[1]);

    // set up an instance of a manager
    std::unique_ptr< transport::task_manager<double> > mgr = std::make_unique< transport::task_manager<double> >(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    std::shared_ptr< transport::mafalda40_basic<double> > model = mgr->create_model< transport::mafalda40_basic<double> >();

    // set up parameter choices
    const std::vector<double>     init_params;
    transport::parameters<double> params(M_Planck, init_params, model);

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 5.0;  // horizon-crossing occurs at 5 e-folds from init_values
    const double Npre   = 5.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 55.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
		const std::vector<double> init_values = INIT_VALUE_LIST;
    transport::initial_conditions<double> ics = transport::initial_conditions<double>("mafalda40", model, params, init_values, Ninit, Ncross, Npre);

    const unsigned int t_samples = 100;

    transport::range<double> times(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(10.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int  k_samples = 100;         // number of k-points

    transport::range<double> ks(kmin, kmax, k_samples, transport::range_spacing_type::logarithmic_bottom_stepping);

    // construct a twopf task
    transport::twopf_task<double> tk2("mafalda40.twopf-1", ics, times, ks);
    transport::zeta_twopf_task<double> ztk2("mafalda40.twopf-1.zeta", tk2);
    ztk2->set_paired(true);

    // SET UP SQL QUERIES

    // filter for all times
    transport::derived_data::SQL_time_config_query all_times("1=1");

    // filter for latest time
    transport::derived_data::SQL_time_config_query last_time("serial IN (SELECT MAX(serial) FROM time_samples)");

    // filter for all twopf wavenumbers
    transport::derived_data::SQL_twopf_kconfig_query all_twopfs("1=1");

    // filter: twopf with largest k
    transport::derived_data::SQL_twopf_kconfig_query largest_twopf("conventional IN (SELECT MAX(conventional) FROM twopf_samples)");


		// construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
		bg_sel.all();

    transport::derived_data::background_time_series<double> tk2_bg(tk2, bg_sel, all_times);

    transport::derived_data::time_series_plot<double> tk2_bg_plot("mafalda40.twopf-1.background", "background.pdf");
		tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");


    transport::derived_data::zeta_twopf_time_series<double> tk2_zeta_times(ztk2, all_times, largest_twopf);

    transport::derived_data::time_series_plot<double> tk2_zeta_timeplot("mafalda40.twopf-1.zeta-twopf", "zeta-twopf.pdf");
		tk2_zeta_timeplot.add_line(tk2_zeta_times);
		tk2_zeta_timeplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ time evolution");
		tk2_zeta_timeplot.set_legend_position(transport::derived_data::legend_pos::bottom_right);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk2_zeta_spec(ztk2, last_time, all_twopfs);

    transport::derived_data::wavenumber_series_plot<double> tk2_zeta_specplot("mafalda40.twopf-1.zeta-spec", "zeta-spec.pdf");
		tk2_zeta_specplot.add_line(tk2_zeta_spec);
		tk2_zeta_specplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ spectrum");

    transport::derived_data::wavenumber_series_table<double> tk2_zeta_spectable("mafalda40.twopf-1.zeta-spec.table", "zeta-spec-table.txt");
		tk2_zeta_spectable.add_line(tk2_zeta_spec);

		// construct output tasks
    transport::output_task<double> twopf_output("mafalda40.twopf-1.output", tk2_bg_plot);
		twopf_output.add_element(tk2_zeta_timeplot);
		twopf_output.add_element(tk2_zeta_specplot);
		twopf_output.add_element(tk2_zeta_spectable);

		// write output tasks to the database
		repo->commit_task(twopf_output);

    return(EXIT_SUCCESS);
  }
