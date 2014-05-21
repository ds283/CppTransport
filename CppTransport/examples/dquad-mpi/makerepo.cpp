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


// filter to determine which time values are included on plots - we just use them all
bool time_filter(const transport::derived_data::filter::time_filter_data&)
	{
    return(true); // plot all values of the time
	}


// filter to determine which time values are included on plots - we just use them all
bool twopf_kconfig_filter(const transport::derived_data::filter::twopf_kconfig_filter_data& data)
	{
    return(data.max); // plot only the largest k
	}


// filter to determine which time values are included on plots - we just use them all
bool threepf_kconfig_filter(const transport::derived_data::filter::threepf_kconfig_filter_data& data)
	{
    return(data.kt_max); // plot only the largest k_t
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

    transport::derived_data::background_time_data<double> tk2_bg = transport::derived_data::background_time_data<double>(tk2, model, bg_sel, transport::derived_data::filter::time_filter(time_filter));

    transport::derived_data::background_time_data<double> tk3_bg = transport::derived_data::background_time_data<double>(tk3, model, bg_sel, transport::derived_data::filter::time_filter(time_filter));

    transport::derived_data::general_time_plot<double> tk2_bg_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.twopf-1.background", "background.pdf");
		tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");

    transport::derived_data::general_time_plot<double> tk3_bg_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.background", "background.pdf");
		tk3_bg_plot.add_line(tk3_bg);
		tk3_bg_plot.set_title_text("Background fields");

		// plots of some components of the twopf

    transport::index_selector<2> twopf_fields(model->get_N_fields());
    transport::index_selector<2> twopf_cross(model->get_N_fields());
		twopf_fields.none();
		twopf_cross.none();

		// field-field correlations
    std::array<unsigned int, 2> index_set_a = { 0, 0 };
    std::array<unsigned int, 2> index_set_b = { 0, 1 };
    std::array<unsigned int, 2> index_set_c = { 1, 1 };

		// momenta-field correlations; the imaginary 2pf only has these cross-terms
    std::array<unsigned int, 2> index_set_d = { 2, 0 };
    std::array<unsigned int, 2> index_set_e = { 2, 1 };
    std::array<unsigned int, 2> index_set_f = { 3, 0 };
    std::array<unsigned int, 2> index_set_g = { 3, 1 };

    twopf_fields.set_on(index_set_a);
		twopf_fields.set_on(index_set_b);
		twopf_fields.set_on(index_set_c);

		twopf_cross.set_on(index_set_d);
    twopf_cross.set_on(index_set_e);
    twopf_cross.set_on(index_set_f);
    twopf_cross.set_on(index_set_g);

    transport::derived_data::twopf_time_data<double> tk2_twopf_real_group =
	                                                     transport::derived_data::twopf_time_data<double>(tk2, model, twopf_fields,
	                                                                                                      transport::derived_data::filter::time_filter(time_filter),
	                                                                                                      transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
		tk2_twopf_real_group.set_klabel_meaning(transport::derived_data::general_time_data<double>::conventional);

    transport::derived_data::twopf_time_data<double> tk2_twopf_imag_group =
	                                                     transport::derived_data::twopf_time_data<double>(tk2, model, twopf_cross,
	                                                                                                      transport::derived_data::filter::time_filter(time_filter),
	                                                                                                      transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk2_twopf_imag_group.set_klabel_meaning(transport::derived_data::general_time_data<double>::conventional);
    tk2_twopf_imag_group.set_type(transport::derived_data::twopf_time_data<double>::imaginary);

    transport::derived_data::twopf_time_data<double> tk3_twopf_real_group =
	                                                     transport::derived_data::twopf_time_data<double>(tk3, model, twopf_fields,
	                                                                                                      transport::derived_data::filter::time_filter(time_filter),
	                                                                                                      transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_real_group.set_klabel_meaning(transport::derived_data::general_time_data<double>::conventional);

    transport::derived_data::twopf_time_data<double> tk3_twopf_imag_group =
	                                                     transport::derived_data::twopf_time_data<double>(tk3, model, twopf_cross,
	                                                                                                      transport::derived_data::filter::time_filter(time_filter),
	                                                                                                      transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));
    tk3_twopf_imag_group.set_klabel_meaning(transport::derived_data::general_time_data<double>::conventional);
    tk3_twopf_imag_group.set_type(transport::derived_data::twopf_time_data<double>::imaginary);

    transport::derived_data::general_time_plot<double> tk2_twopf_real_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.twopf-1.twopf-real", "twopf-real.pdf");
		tk2_twopf_real_plot.add_line(tk2_twopf_real_group);
		tk2_twopf_real_plot.set_title_text("Real two-point function");
		tk2_twopf_real_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk2_twopf_imag_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.twopf-1.twopf-imag", "twopf-imag.pdf");
    tk2_twopf_imag_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk2_twopf_imag_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk2_twopf_total_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.twopf-1.twopf-total", "twopf-total.pdf");
    tk2_twopf_total_plot.add_line(tk2_twopf_real_group);
		tk2_twopf_total_plot.add_line(tk2_twopf_imag_group);
    tk2_twopf_total_plot.set_title_text("Two-point function");
    tk2_twopf_total_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk3_twopf_real_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.twopf-real", "twopf-real.pdf");
    tk3_twopf_real_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_real_plot.set_title_text("Real two-point function");
    tk3_twopf_real_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk3_twopf_imag_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.twopf-imag", "twopf-imag.pdf");
    tk3_twopf_imag_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk3_twopf_imag_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk3_twopf_total_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.twopf-total", "twopf-total.pdf");
    tk3_twopf_total_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_total_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_total_plot.set_title_text("Two-point function");
    tk3_twopf_total_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    // plots of some components of the threepf
    transport::index_selector<3> threepf_fields(model->get_N_fields());
    threepf_fields.none();

    std::array<unsigned int, 3> sq_set_a    = { 0, 0, 0 };
    std::array<unsigned int, 3> sq_set_b    = { 0, 1, 0 };
    std::array<unsigned int, 3> sq_set_c    = { 1, 1, 0 };
    std::array<unsigned int, 3> sq_set_d    = { 0, 0, 1 };
    std::array<unsigned int, 3> sq_set_e    = { 0, 1, 1 };
    std::array<unsigned int, 3> sq_set_f    = { 1, 1, 1 };
    threepf_fields.set_on(sq_set_a);
    threepf_fields.set_on(sq_set_b);
    threepf_fields.set_on(sq_set_c);
    threepf_fields.set_on(sq_set_d);
    threepf_fields.set_on(sq_set_e);
    threepf_fields.set_on(sq_set_f);

    transport::derived_data::threepf_time_data<double> tk3_threepf_group =
	                                                       transport::derived_data::threepf_time_data<double>(tk3, model, threepf_fields,
	                                                                                                          transport::derived_data::filter::time_filter(time_filter),
	                                                                                                          transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
		tk3_threepf_group.set_klabel_meaning(transport::derived_data::general_time_data<double>::conventional);

    transport::derived_data::general_time_plot<double> tk3_threepf_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.threepf", "threepf.pdf");
    tk3_threepf_plot.add_line(tk3_threepf_group);
    tk3_threepf_plot.set_title_text("Three-point function");
    tk3_threepf_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    transport::derived_data::general_time_plot<double> tk3_mixed_plot =
	                                                       transport::derived_data::general_time_plot<double>("dquad.threepf-1.mixed", "mixed.pdf");

		tk3_mixed_plot.add_line(tk3_threepf_group);
		tk3_mixed_plot.add_line(tk3_twopf_real_group);
		tk3_mixed_plot.set_title_text("Two- and three-point functions");
		tk3_mixed_plot.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    // pick out the shift between derivative and moments 3pfs
    transport::index_selector<3> threepf_mmta(model->get_N_fields());
    threepf_mmta.none();
    std::array<unsigned int, 3> sq_mmta_a = { 2, 0, 0 };
    std::array<unsigned int, 3> sq_mmta_b = { 3, 0, 0 };
    threepf_mmta.set_on(sq_mmta_a);
    threepf_mmta.set_on(sq_mmta_b);

    transport::derived_data::threepf_time_data<double> tk3_threepf_derivs =
                                                         transport::derived_data::threepf_time_data<double>(tk3, model, threepf_mmta,
                                                                                                            transport::derived_data::filter::time_filter(time_filter),
                                                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_derivs.set_klabel_meaning(transport::derived_data::general_time_data<double>::comoving);
    tk3_threepf_derivs.set_dot_meaning(transport::derived_data::general_time_data<double>::derivatives);
    tk3_threepf_derivs.set_use_alpha_label(true);
    tk3_threepf_derivs.set_use_beta_label(true);

    transport::derived_data::threepf_time_data<double> tk3_threepf_momenta =
                                                         transport::derived_data::threepf_time_data<double>(tk3, model, threepf_mmta,
                                                                                                            transport::derived_data::filter::time_filter(time_filter),
                                                                                                            transport::derived_data::filter::threepf_kconfig_filter(threepf_kconfig_filter));
    tk3_threepf_momenta.set_klabel_meaning(transport::derived_data::general_time_data<double>::comoving);
    tk3_threepf_momenta.set_dot_meaning(transport::derived_data::general_time_data<double>::momenta);
    tk3_threepf_momenta.set_use_alpha_label(true);
    tk3_threepf_momenta.set_use_beta_label(true);

    transport::derived_data::general_time_plot<double> tk3_check_shift = transport::derived_data::general_time_plot<double>("dquad.threepf-1.checkshift", "checkshift.pdf");

    tk3_check_shift.add_line(tk3_threepf_derivs);
    tk3_check_shift.add_line(tk3_threepf_momenta);
    tk3_check_shift.set_title_text("Comparison of derivative and momenta 3pf");
    tk3_check_shift.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    // check the zeta twopf
    transport::derived_data::zeta_twopf_time_data<double> tk3_zeta_twopf_group = transport::derived_data::zeta_twopf_time_data<double>(tk3, model,
                                                                                                                                       transport::derived_data::filter::time_filter(time_filter),
                                                                                                                                       transport::derived_data::filter::twopf_kconfig_filter(twopf_kconfig_filter));

    transport::derived_data::general_time_plot<double> tk3_zeta_twopf = transport::derived_data::general_time_plot<double>("dquad.threepf-1.zeta-twopf", "zeta-twopf.pdf");

    tk3_zeta_twopf.add_line(tk3_zeta_twopf_group);
    tk3_zeta_twopf.add_line(tk3_twopf_real_group);
    tk3_zeta_twopf.set_title_text("Comparison of $\\zeta$ and field 2pfs");
    tk3_zeta_twopf.set_legend_position(transport::derived_data::plot2d_product<double>::bottom_left);

    std::cout << "3pf background plot:" << std::endl << tk3_mixed_plot << std::endl;

		// write derived data products representing these background plots to the database
    repo->write_derived_product(tk2_bg_plot);
    repo->write_derived_product(tk2_twopf_real_plot);
    repo->write_derived_product(tk2_twopf_imag_plot);
    repo->write_derived_product(tk2_twopf_total_plot);

    repo->write_derived_product(tk3_bg_plot);
    repo->write_derived_product(tk3_twopf_real_plot);
    repo->write_derived_product(tk3_twopf_imag_plot);
    repo->write_derived_product(tk3_twopf_total_plot);
    repo->write_derived_product(tk3_threepf_plot);
    repo->write_derived_product(tk3_mixed_plot);

    repo->write_derived_product(tk3_check_shift);

    repo->write_derived_product(tk3_zeta_twopf);

		// construct output tasks
    transport::output_task<double> twopf_output   = transport::output_task<double>("dquad.twopf-1.output", tk2_bg_plot);
		twopf_output.add_element(tk2_twopf_real_plot);
		twopf_output.add_element(tk2_twopf_imag_plot);
		twopf_output.add_element(tk2_twopf_total_plot);

    transport::output_task<double> threepf_output = transport::output_task<double>("dquad.threepf-1.output", tk3_bg_plot);
		threepf_output.add_element(tk3_twopf_real_plot);
    threepf_output.add_element(tk3_twopf_imag_plot);
    threepf_output.add_element(tk3_twopf_total_plot);
		threepf_output.add_element(tk3_threepf_plot);
		threepf_output.add_element(tk3_mixed_plot);
    threepf_output.add_element(tk3_check_shift);
    threepf_output.add_element(tk3_zeta_twopf);

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
