//
// Created by David Seery on 10/06/2014.
// Copyright (c) 2013-14 University of Sussex. All rights reserved.
//


#include "quadratic10_basic.h"

#include "transport-runtime-api/manager/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

#define MASS_LIST { 2.45391E-10, 1.72583E-10, 1.44638E-10, 1.06326E-10, 8.53891E-11, 7.74336E-11, 5.1783E-11, 4.34577E-11, 2.37254E-11, 1.77517E-11 }

#define INIT_VALUE_LIST { sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), sqrt(142.0/5.0), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

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
		    std::cerr << "makerepo: Too few arguments. Expected repository name" << '\n';
		    exit(EXIT_FAILURE);
			}

    transport::repository_creation_key key;

    std::shared_ptr< transport::json_repository<double> > repo = transport::repository_factory<double>(argv[1], key);

    // set up an instance of a manager
    std::shared_ptr< transport::task_manager<double> > mgr = std::make_shared< transport::task_manager<double> >(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::quadratic10_basic<double>* model = new transport::quadratic10_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = MASS_LIST;
    transport::parameters<double> params      = transport::parameters<double>(M_Planck, init_params, model);

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 5.0;  // horizon-crossing occurs at 5 e-folds from init_values
    const double Npre   = 5.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 60.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
		const std::vector<double> init_values     = INIT_VALUE_LIST;
    transport::initial_conditions<double> ics = transport::initial_conditions<double>("quadratic10", model, params, init_values, Ninit, Ncross, Npre);

    const unsigned int t_samples = 1000;       // record 2000 samples - enough to find a good stepsize

    struct TimeStoragePolicy
      {
      public:
        bool operator() (const transport::integration_task<double>::time_config_storage_policy_data& data) { return((data.serial % 10) == 0); }
      };

    struct ThreepfStoragePolicy
      {
      public:
        bool operator() (const transport::threepf_task<double>::threepf_kconfig_storage_policy_data& data) { return(true); }
      };

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(5.0);   // end with the mode which exits the horizon at N=N*+3
    const unsigned int  k_samples = 3;         // number of k-points

    transport::range<double> ks = transport::range<double>(kmin, kmax, k_samples, transport::linear_stepping);

    // construct a threepf task
    transport::threepf_cubic_task<double> tk3("quadratic10.threepf-1", ics, times, ks, TimeStoragePolicy(), ThreepfStoragePolicy());
    transport::zeta_threepf_task<double> zeta_tk3("quadratic10.threepf-1.zeta", tk3, true);

    transport::fNL_task<double> zeta_tk3_fNL_local("quadratic10.threepf-1.fNL_local", tk3, transport::derived_data::fNL_local_template, true);
    transport::fNL_task<double> zeta_tk3_fNL_equi("quadratic10.threepf-1.fNL_equi", tk3, transport::derived_data::fNL_equi_template, true);
    transport::fNL_task<double> zeta_tk3_fNL_ortho("quadratic10.threepf-1.fNL_ortho", tk3, transport::derived_data::fNL_ortho_template, true);

		// construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
		bg_sel.none();
		for(unsigned int m = 0; m < model->get_N_fields(); ++m)
			{
		    std::array<unsigned int, 1> set = { m };
				bg_sel.set_on(set);
			}

    transport::derived_data::background_time_series<double> tk3_bg(tk3, bg_sel, transport::derived_data::filter::time_filter(timeseries_filter));

    transport::derived_data::time_series_plot<double> tk3_bg_plot("quadratic10.threepf-1.background", "background.pdf");
		tk3_bg_plot.add_line(tk3_bg);
    tk3_bg_plot.set_title_text("Background fields");
		tk3_bg_plot.set_legend_position(transport::derived_data::bottom_left);

    transport::derived_data::zeta_twopf_time_series<double> tk3_zeta_times(tk3,
                                                                                                                                     transport::derived_data::filter::time_filter(timeseries_filter),
                                                                                                                                     transport::derived_data::filter::twopf_kconfig_filter(twopf_timeseries_filter));

    transport::derived_data::time_series_plot<double> tk3_zeta_timeplot("quadratic10.threepf-1.zeta-twopf", "zeta-twopf.pdf");
		tk3_zeta_timeplot.add_line(tk3_zeta_times);
		tk3_zeta_timeplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ time evolution");
		tk3_zeta_timeplot.set_legend_position(transport::derived_data::bottom_right);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk3_zeta_spec(tk3,
                                                                                                                                                transport::derived_data::filter::time_filter(spectrum_timefilter),
                                                                                                                                                transport::derived_data::filter::twopf_kconfig_filter(all_k_modes));

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_specplot("quadratic10.threepf-1.zeta-spec", "zeta-spec.pdf");
		tk3_zeta_specplot.add_line(tk3_zeta_spec);
		tk3_zeta_specplot.set_title_text("$\\langle \\zeta \\zeta \\rangle$ spectrum");

    transport::derived_data::r_wavenumber_series<double> tk3_r_spec(tk3,
                                                                                                                           transport::derived_data::filter::time_filter(spectrum_timefilter),
                                                                                                                           transport::derived_data::filter::twopf_kconfig_filter(all_k_modes));

    transport::derived_data::wavenumber_series_plot<double> tk3_r_specplot("quadratic10.threepf-1.r-spec", "r-spec.pdf");
		tk3_r_specplot.add_line(tk3_r_spec);
		tk3_r_specplot.set_title_text("Tensor-to-scalar ratio");

    transport::derived_data::fNL_time_series<double> fNL_local_line(tk3, transport::derived_data::filter::time_filter(timeseries_filter));
    fNL_local_line.set_type(transport::derived_data::fNL_local_template);
    transport::derived_data::fNL_time_series<double> fNL_ortho_line(tk3, transport::derived_data::filter::time_filter(timeseries_filter));
    fNL_ortho_line.set_type(transport::derived_data::fNL_ortho_template);
    transport::derived_data::fNL_time_series<double> fNL_equi_line(tk3, transport::derived_data::filter::time_filter(timeseries_filter));
    fNL_equi_line.set_type(transport::derived_data::fNL_equi_template);

    transport::derived_data::time_series_plot<double> fNL_plot("quadratic10.threepf-1.fNL", "fNLs.pdf");
    fNL_plot.add_line(fNL_local_line);
    fNL_plot.add_line(fNL_equi_line);
    fNL_plot.add_line(fNL_ortho_line);
    fNL_plot.set_log_y(false);
    fNL_plot.set_abs_y(false);
    fNL_plot.set_title(false);

		// construct output tasks
    transport::output_task<double> threepf_output("quadratic10.threepf-1.output", tk3_bg_plot);
		threepf_output.add_element(tk3_zeta_timeplot);
		threepf_output.add_element(tk3_zeta_specplot);
		threepf_output.add_element(tk3_r_specplot);
    threepf_output.add_element(fNL_plot);

		// write output tasks to the database
		repo->commit_task(tk3);
		repo->commit_task(zeta_tk3);
		repo->commit_task(threepf_output);

    return(EXIT_SUCCESS);
  }
