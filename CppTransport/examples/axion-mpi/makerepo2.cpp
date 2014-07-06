//
// Created by David Seery on 6/6/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#include "axion_basic.h"

#include "transport-runtime-api/manager/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;
const double m        = 1e-5;
const double f        = M_Planck;
const double Lambda   = pow(5*m*f/(2*M_PI),1.0/2.0);

const double phi_init = 16.0 * M_Planck;
const double chi_init = f/2.0 - 0.001*M_Planck;


// ****************************************************************************


// filter to determine which time values are included on time-series plots - we just use them all
bool timeseries_axis_filter(const transport::derived_data::filter::time_filter_data& data)
	{
    return(true); // plot all points
	}

// filter to determine which twopf-kconfigs are plotted - we use the largest and smallest
bool timeseries_twopf_kconfig_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
		return(data.min || data.max);
	}

// filter for near-squeezed 3pf k-configurations
bool threepf_kconfig_near_squeezed(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(fabs(data.beta - 0.95) < 0.01); // use beta = 0.9 only
	}

bool twopf_kseries_axis_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
		return(true);   // plot all 2pf configuraitons
	}

bool kseries_last_time(const transport::derived_data::filter::time_filter_data& data)
	{
		return(data.max);
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

    // set up an instance of the axion-quadratic model,
    // using doubles, with given parameter choices
    transport::axion_basic<double>* model = new transport::axion_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { m, Lambda, f, M_PI };
    transport::parameters<double> params      =
                                    transport::parameters<double>(M_Planck, init_params, model);

    const std::vector<double> init_values = { phi_init, chi_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 4.0;  // horizon-crossing occurs at 4 e-folds from init_values
    const double Npre   = 4.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 61.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
    transport::initial_conditions<double> ics =
                                            transport::initial_conditions<double>("axion-1", model, params, init_values, Ninit, Ncross, Npre);

    const unsigned int t_samples = 50000;       // record 5000 samples - enough to find a good stepsize

    // time storage policy is to store only 500 samples
    struct TimeStoragePolicy
      {
      public:
        bool operator() (const transport::integration_task<double>::time_config_storage_policy_data& data) { return((data.serial % 100) == 0); }
      };

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        ktmin      = exp(3.0);
    const double        ktmax      = exp(8.0);
    const unsigned int  k_samples = 30;

		const double        alphamin   = -1.0;
		const double        alphamax   = +1.0;
		const unsigned int  a_samples  = 30;

		const double        betamin    = 0.0;
		const double        betamax    = 1.0;
		const unsigned int  b_samples  = 20;

		struct ThreepfStoragePolicy
			{
		  public:
				bool operator() (const transport::threepf_task<double>::threepf_kconfig_storage_policy_data& data) { return(true); }
			};

    transport::range<double> kts    = transport::range<double>(ktmin, ktmax, k_samples, transport::range<double>::linear);
    transport::range<double> alphas = transport::range<double>(alphamin, alphamax, a_samples, transport::range<double>::linear);
    transport::range<double> betas  = transport::range<double>(betamin, betamax, b_samples, transport::range<double>::linear);

    // construct a threepf task
    transport::threepf_fls_task<double> tk3 = transport::threepf_fls_task<double>("axion.threepf-1", ics, times, kts, alphas, betas, TimeStoragePolicy(), ThreepfStoragePolicy());
		tk3.set_fast_forward_efolds(4.0);

    std::cout << tk3;

    // check the zeta twopf
    transport::derived_data::zeta_twopf_time_series<double> tk3_zeta_twopf_group = transport::derived_data::zeta_twopf_time_series<double>(tk3,
                                                                                                                                           transport::derived_data::filter::time_filter(timeseries_axis_filter),
                                                                                                                                           transport::derived_data::filter::twopf_kconfig_filter(timeseries_twopf_kconfig_filter));

    transport::derived_data::time_series_plot<double> tk3_zeta_twopf = transport::derived_data::time_series_plot<double>("axion.threepf-1.zeta-twopf", "zeta-twopf.pdf");

    tk3_zeta_twopf.add_line(tk3_zeta_twopf_group);
    tk3_zeta_twopf.set_title_text("$\\zeta$ two-point function");
    tk3_zeta_twopf.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_left);

    // check the zeta threepf
    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_sq_group = transport::derived_data::zeta_threepf_time_series<double>(tk3,
                                                                                                                                            transport::derived_data::filter::time_filter(timeseries_axis_filter),
                                                                                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
    tk3_zeta_sq_group.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_zeta_sq_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_sq = transport::derived_data::time_series_plot<double>("axion.threepf-1.zeta-sq", "zeta-sq.pdf");
    tk3_zeta_sq.add_line(tk3_zeta_sq_group);
    tk3_zeta_sq.set_title_text("3pf of $\\zeta$ near squeezed configurations");

		// set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_sq_table = transport::derived_data::time_series_table<double>("axion.threepf-1.zeta-sq.table", "zeta-sq-table.txt");
		tk3_zeta_sq_table.add_line(tk3_zeta_sq_group);

    // compute the reduced bispectrum in a few squeezed configurations
    transport::derived_data::zeta_reduced_bispectrum_time_series<double> tk3_zeta_redbsp = transport::derived_data::zeta_reduced_bispectrum_time_series<double>(tk3,
                                                                                                                                                                transport::derived_data::filter::time_filter(timeseries_axis_filter),
                                                                                                                                                                transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
    tk3_zeta_redbsp.set_klabel_meaning(transport::derived_data::derived_line<double>::comoving);
    tk3_zeta_redbsp.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_redbsp = transport::derived_data::time_series_plot<double>("axion.threepf-1.redbsp-sq", "redbsp-sq.pdf");
    tk3_redbsp.set_log_y(false);
    tk3_redbsp.set_abs_y(false);
    tk3_redbsp.add_line(tk3_zeta_redbsp);
    tk3_redbsp.set_legend_position(transport::derived_data::line_plot2d<double>::bottom_right);
    tk3_redbsp.set_title_text("Reduced bispectrum near squeezed configurations");

    transport::derived_data::time_series_table<double> tk3_redbsp_table = transport::derived_data::time_series_table<double>("axion.threepf-1.redbsp-dq.table", "redbsp-sq-table.txt");
    tk3_redbsp_table.add_line(tk3_zeta_redbsp);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk3_zeta_2spec = transport::derived_data::zeta_twopf_wavenumber_series<double>(tk3,
                                                                                                                                                 transport::derived_data::filter::time_filter(kseries_last_time),
                                                                                                                                                 transport::derived_data::filter::twopf_kconfig_filter(twopf_kseries_axis_filter));
    tk3_zeta_2spec.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_2spec_plot = transport::derived_data::wavenumber_series_plot<double>("axion.threepf-1.zeta-2spec", "zeta-2spec.pdf");
    tk3_zeta_2spec_plot.add_line(tk3_zeta_2spec);
    tk3_zeta_2spec_plot.set_log_x(true);
    tk3_zeta_2spec_plot.set_title("$\\langle \\zeta \\zeta \\rangle$ power spectrum");

    transport::derived_data::wavenumber_series_table<double> tk3_zeta_2spec_table = transport::derived_data::wavenumber_series_table<double>("axion.threepf-1.zeta-2spec.table", "zeta-2spec-table.txt");
    tk3_zeta_2spec_table.add_line(tk3_zeta_2spec);

    transport::derived_data::zeta_reduced_bispectrum_wavenumber_series<double> tk3_zeta_redbsp_spec = transport::derived_data::zeta_reduced_bispectrum_wavenumber_series<double>(tk3,
                                                                                                                                                                                 transport::derived_data::filter::time_filter(kseries_last_time),
                                                                                                                                                                                 transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_near_squeezed));
		tk3_zeta_redbsp_spec.set_klabel_meaning(transport::derived_data::derived_line<double>::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_redbsp_spec_plot = transport::derived_data::wavenumber_series_plot<double>("axion.threepf-1.redbsp-spec", "redbsp-spec.pdf");
		tk3_redbsp_spec_plot.add_line(tk3_zeta_redbsp_spec);
		tk3_redbsp_spec_plot.set_title("Reduced bispectrum in the squeezed limit");

    transport::derived_data::wavenumber_series_table<double> tk3_redbsp_spec_table = transport::derived_data::wavenumber_series_table<double>("axion.threepf-1.redbsp-spec-table", "redbsp-spec-table.txt");
		tk3_redbsp_spec_table.add_line(tk3_zeta_redbsp_spec);


		// construct output tasks

    transport::output_task<double> threepf_output = transport::output_task<double>("axion.threepf-1.output", tk3_zeta_twopf);
    threepf_output.add_element(tk3_zeta_sq);
		threepf_output.add_element(tk3_zeta_sq_table);
    threepf_output.add_element(tk3_redbsp);
    threepf_output.add_element(tk3_redbsp_table);
    threepf_output.add_element(tk3_zeta_2spec_plot);
    threepf_output.add_element(tk3_zeta_2spec_table);
		threepf_output.add_element(tk3_redbsp_spec_plot);
		threepf_output.add_element(tk3_redbsp_spec_table);

    std::cout << "axion.threepf-1 output task:" << std::endl << threepf_output << std::endl;

		// write output tasks to the database
    repo->commit_task(threepf_output);

    delete mgr;     // task_manager adopts its repository and destroys it silently; also destroys any registered models

    return(EXIT_SUCCESS);
  }
