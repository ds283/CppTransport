//
// Created by David Seery on 25/08/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "powerlaw_basic_unrolled.h"

#include "transport-runtime-api/repository/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck   = 1.0;
const double W0         = 1E-14 * M_Planck * M_Planck * M_Planck * M_Planck;
const double m_phi      = std::sqrt(0.02);
const double m_sigma1   = std::sqrt(0.5);
const double m_sigma2   = std::sqrt(0.05);
const double sigma_c    = 6E-6 * M_Planck;
const double sigma_grad = 1E-10 * M_Planck;

const double phi_init   = 0.00827 * M_Planck;
const double sigma_init = 5.8304E-10 * M_Planck;

// ****************************************************************************


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
    transport::powerlaw_basic<double>* model = new transport::powerlaw_basic<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { W0, m_phi, m_sigma1, m_sigma2, sigma_c, sigma_grad };
    transport::parameters<double> params      =	transport::parameters<double>(M_Planck, init_params, model);

    const std::vector<double> init_values = { phi_init, sigma_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 10.0; // horizon-crossing occurs at N=5
    const double Npre   = 10.0; // number of e-folds of subhorizon evolution
    const double Nsplit = 20.0; // split point between early and late
    const double Nmax   = 50.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions with the specified horizon-crossing time Ncross and Npre
    // e-folds of subhorizon evolution.
    // The resulting initial conditions apply at time Ncross-Npre
    transport::initial_conditions<double> ics("powerlaw-1", model, params, init_values, Ninit, Ncross, Npre);

    const unsigned int early_t_samples = 200;
    const unsigned int late_t_samples  = 100;

    transport::stepping_range<double>    early_times(Ncross - Npre, Ncross + Nsplit, early_t_samples, transport::range_spacing_type::logarithmic_bottom_stepping);
    transport::stepping_range<double>    late_times(Ncross + Nsplit, Ncross + Nmax, late_t_samples, transport::range_spacing_type::linear_stepping);
    transport::aggregation_range<double> times(early_times, late_times);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kt_min_exp    = 0.0;
    const double        kt_max_exp    = 5.0;
    const double       kmin      = exp(kt_min_exp);   // begin with the mode which crosses the horizon at N=N*+1
    const double       kmax      = exp(kt_max_exp);   // end with the mode which exits the horizon at N=N*+5
    const unsigned int k_samples = 50;         // number of k-points

    struct ThreepfStoragePolicy
	    {
      public:
        bool operator()(const transport::threepf_kconfig& data)
	        {
            return (true);
	        }
	    };

    transport::stepping_range<double> ks(kmin, kmax, k_samples, transport::range_spacing_type::linear_stepping);

    // construct a twopf task
    transport::twopf_task<double> tk2("powerlaw.twopf-1", ics, times, ks);

    // construct a zeta twopf task
    transport::zeta_twopf_task<double> ztk2("powerlaw.twopf-1.zeta", tk2);
    ztk2.set_paired(true);

    // construct a threepf task
    transport::threepf_cubic_task<double> tk3("powerlaw.threepf-1", ics, times, ks, ThreepfStoragePolicy());
		tk3.set_collect_initial_conditions(true);

    // construct zeta threepf task
    transport::zeta_threepf_task<double> ztk3("powerlaw.threepf-1.zeta", tk3);
    ztk3.set_paired(true);

    transport::fNL_task<double> ztk3_fNL_local("powerlaw.threepf-1.fNL_local", ztk3, transport::derived_data::template_type::fNL_local_template);
    transport::fNL_task<double> ztk3_fNL_equi("powerlaw.threepf-1.fNL_equi", ztk3, transport::derived_data::template_type::fNL_equi_template);
    transport::fNL_task<double> ztk3_fNL_ortho("powerlaw.threepf-1.fNL_ortho", ztk3, transport::derived_data::template_type::fNL_ortho_template);


		// SET UP SQL QUERIES

		// filter for all times
    transport::derived_data::SQL_time_config_query all_times("1=1");

		// filter: pick just two times
    transport::derived_data::SQL_time_config_query two_times("serial=90 OR serial=250");

		// filter for all twopf wavenumbers
    transport::derived_data::SQL_twopf_kconfig_query all_twopfs("1=1");

		// filter: twopf with largest k
    transport::derived_data::SQL_twopf_kconfig_query largest_twopf("conventional IN (SELECT MAX(conventional) FROM twopf_samples)");

		// filter: threepf with largest k_t
    transport::derived_data::SQL_threepf_kconfig_query largest_threepf("kt_conventional IN (SELECT MAX(kt_conventional) FROM threepf_samples)");

		// filter: equilateral threepf with largest and smallest k_t
    transport::derived_data::SQL_threepf_kconfig_query equilateral_extreme_threepf("ABS(alpha) < 0.01 AND ABS(beta - 1.0/3.0) < 0.01 AND (kt_conventional IN (SELECT MAX(kt_conventional) FROM threepf_samples) OR kt_conventional IN (SELECT MIN(kt_conventional) FROM threepf_samples))");

		// filter: extremely squeezed threepf configurations
    transport::derived_data::SQL_threepf_kconfig_query extreme_squeezed_threepf("beta > 0.95");

		// filter: most squeezed threepf configuration
    transport::derived_data::SQL_threepf_kconfig_query most_squeezed_threepf("beta in (SELECT MAX(beta) FROM threepf_samples)");

		// filter: equilateral threepf with largest k_t
    transport::derived_data::SQL_threepf_kconfig_query equilateral_largest_threepf("ABS(alpha) < 0.01 AND ABS(beta - 1.0/3.0) < 0.01 AND kt_conventional IN (SELECT MAX(kt_conventional) FROM threepf_samples)");

		// filter: all equilateral configurations
    transport::derived_data::SQL_threepf_kconfig_query all_equilateral("ABS(alpha) < 0.01 AND ABS(beta - 1.0/3.0) < 0.01");

    // construct some derived data products; first, simply plots of the background

    transport::index_selector<1> bg_sel(model->get_N_fields());
    bg_sel.all();

    transport::derived_data::background_time_series<double> tk2_bg(tk2, bg_sel, all_times);

    transport::derived_data::background_time_series<double> tk3_bg(tk3, bg_sel, all_times);

    transport::derived_data::time_series_plot<double> tk2_bg_plot("powerlaw.twopf-1.background", "background.pdf");
    tk2_bg_plot.add_line(tk2_bg);
    tk2_bg_plot.set_title_text("Background fields");
    tk2_bg_plot.set_y_label(true);

    transport::derived_data::time_series_plot<double> tk3_bg_plot("powerlaw.threepf-1.background", "background.pdf");
    tk3_bg_plot.add_line(tk3_bg);
    tk3_bg_plot.set_title_text("Background fields");
    tk3_bg_plot.set_y_label(true);

    // plots of some components of the twopf

    transport::index_selector<2> twopf_fields(model->get_N_fields());
    transport::index_selector<2> twopf_cross(model->get_N_fields());
    transport::index_selector<2> twopf_phis(model->get_N_fields());
    twopf_fields.none();
    twopf_cross.none();
    twopf_phis.none();

    // field-field correlations
    std::array<unsigned int, 2> index_set_a = { 0, 0 };
    std::array<unsigned int, 2> index_set_b = { 0, 1 };
    std::array<unsigned int, 2> index_set_c = { 1, 1 };

    // momenta-field correlations; the imaginary 2pf only has these cross-terms
    std::array<unsigned int, 2> index_set_d = { 2, 0 };
    std::array<unsigned int, 2> index_set_e = { 2, 1 };
    std::array<unsigned int, 2> index_set_f = { 3, 0 };
    std::array<unsigned int, 2> index_set_g = { 3, 1 };

    // phi correlation functions
    std::array<unsigned int, 2> index_set_h = { 0, 0 };
    std::array<unsigned int, 2> index_set_i = { 0, 2 };
    std::array<unsigned int, 2> index_set_j = { 2, 0 };
    std::array<unsigned int, 2> index_set_k = { 2, 2 };

    twopf_fields.set_on(index_set_a);
    twopf_fields.set_on(index_set_b);
    twopf_fields.set_on(index_set_c);

    twopf_cross.set_on(index_set_d);
    twopf_cross.set_on(index_set_e);
    twopf_cross.set_on(index_set_f);
    twopf_cross.set_on(index_set_g);

    twopf_phis.set_on(index_set_h);
    twopf_phis.set_on(index_set_i);
    twopf_phis.set_on(index_set_j);
    twopf_phis.set_on(index_set_k);

    transport::index_selector<2> tensor_twopf_fields(2);
    tensor_twopf_fields.all();

    // tensor-tensor correlation function only
    std::array<unsigned int, 2> index_set_l = { 0, 0 };

    transport::index_selector<2> tensor_only(2);
    tensor_only.none();
    tensor_only.set_on(index_set_l);

    transport::derived_data::twopf_time_series<double> tk2_twopf_group(tk2, twopf_fields, all_times, largest_twopf);
    tk2_twopf_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::twopf_time_series<double> tk2_phis_group(tk2, twopf_phis, all_times, largest_twopf);
    tk2_phis_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::tensor_twopf_time_series<double> tk2_tensor_twopf_group(tk2, tensor_twopf_fields, all_times, largest_twopf);
    tk2_tensor_twopf_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::tensor_twopf_time_series<double> tk2_tensor_only_group(tk2, tensor_only, all_times, largest_twopf);
    tk2_tensor_twopf_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::tensor_twopf_wavenumber_series<double> tk2_tensor_spectrum(tk2, tensor_twopf_fields, two_times, all_twopfs);
    tk2_tensor_spectrum.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::twopf_time_series<double> tk3_twopf_real_group(tk3, twopf_fields, all_times, largest_twopf);
    tk3_twopf_real_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::twopf_time_series<double> tk3_twopf_imag_group(tk3, twopf_cross, all_times, largest_twopf);
    tk3_twopf_imag_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);
    tk3_twopf_imag_group.set_type(transport::derived_data::twopf_type::imaginary);

    transport::derived_data::twopf_wavenumber_series<double> tk3_twopf_real_kgp(tk3, twopf_fields, two_times, all_twopfs);
    tk3_twopf_real_kgp.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::twopf_wavenumber_series<double> tk3_twopf_imag_kgp(tk3, twopf_cross, two_times, all_twopfs);
    tk3_twopf_imag_kgp.set_klabel_meaning(transport::derived_data::klabel_type::conventional);
    tk3_twopf_imag_kgp.set_type(transport::derived_data::twopf_type::imaginary);

    transport::derived_data::tensor_twopf_time_series<double> tensor_twopf_group(tk3, tensor_twopf_fields, all_times, largest_twopf);
    tensor_twopf_group.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::r_time_series<double> r_group(ztk3, all_times, largest_twopf);

    transport::derived_data::r_wavenumber_series<double> r_spectrum_group(ztk3, two_times, all_twopfs);


    transport::derived_data::time_series_plot<double> tk2_twopf_real_plot("powerlaw.twopf-1.twopf-real", "twopf-real.pdf");
    tk2_twopf_real_plot.add_line(tk2_twopf_group);
    tk2_twopf_real_plot.set_title_text("Real two-point function");
    tk2_twopf_real_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_twopf_total_plot("powerlaw.twopf-1.twopf-total", "twopf-total.pdf");
    tk2_twopf_total_plot.add_line(tk2_twopf_group);
    tk2_twopf_total_plot.add_line(tk2_tensor_only_group);
    tk2_twopf_total_plot.set_title_text("Two-point functions (fields and tensors)");
    tk2_twopf_total_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_compare_plot("powerlaw.twopf-1.twopf-compare", "twopf-compare.pdf");
    tk2_compare_plot.add_line(tk2_phis_group);
    tk2_compare_plot.add_line(tk2_tensor_twopf_group);
    tk2_compare_plot.set_title_text("Comparison of two-point functions");
    tk2_compare_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> tk2_tensor_twopf_plot("powerlaw.twopf-1.tensor-twopf", "tensor-twopf.pdf");
    tk2_tensor_twopf_plot.add_line(tk2_tensor_twopf_group);
    tk2_tensor_twopf_plot.set_title_text("Tensor two-point function");
    tk2_tensor_twopf_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::wavenumber_series_plot<double> tk2_tensor_spectrum_plot("powerlaw.twopf-1.tensor-spectrum", "tensor-spectrum.pdf");
    tk2_tensor_spectrum_plot.add_line(tk2_tensor_spectrum);
    tk2_tensor_spectrum_plot.set_title_text("Tensor power spectra");
    tk2_tensor_spectrum_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);
    tk2_tensor_spectrum_plot.set_log_x(true);


    transport::derived_data::time_series_plot<double> tk3_twopf_real_plot("powerlaw.threepf-1.twopf-real", "twopf-real.pdf");
    tk3_twopf_real_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_real_plot.set_title_text("Real two-point function");
    tk3_twopf_real_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_imag_plot("powerlaw.threepf-1.twopf-imag", "twopf-imag.pdf");
    tk3_twopf_imag_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_imag_plot.set_title_text("Imaginary two-point function");
    tk3_twopf_imag_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> tk3_twopf_total_plot("powerlaw.threepf-1.twopf-total", "twopf-total.pdf");
    tk3_twopf_total_plot.add_line(tk3_twopf_real_group);
    tk3_twopf_total_plot.add_line(tk3_twopf_imag_group);
    tk3_twopf_total_plot.set_title_text("Two-point function");
    tk3_twopf_total_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_real_spec("powerlaw.threepf-1.twopf-re-spec", "twopf-re-spec.pdf");
    tk3_twopf_real_spec.add_line(tk3_twopf_real_kgp);
    tk3_twopf_real_spec.set_log_x(true);
    tk3_twopf_real_spec.set_title_text("Real two-point function");

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_imag_spec("powerlaw.threepf-1.twopf-im-spec", "twopf-im-spec.pdf");
    tk3_twopf_imag_spec.add_line(tk3_twopf_imag_kgp);
    tk3_twopf_imag_spec.set_log_x(true);
    tk3_twopf_imag_spec.set_title_text("Imaginary two-point function");

    transport::derived_data::wavenumber_series_plot<double> tk3_twopf_total_spec("powerlaw.threepf-1.twopf-tot-spec", "twopf-tot-spec.pdf");
    tk3_twopf_total_spec.add_line(tk3_twopf_real_kgp);
    tk3_twopf_total_spec.add_line(tk3_twopf_imag_kgp);
    tk3_twopf_total_spec.set_log_x(true);
    tk3_twopf_total_spec.set_title_text("Two-point function");

    transport::derived_data::wavenumber_series_table<double> tk3_twopf_total_tab("powerlaw.threepf-1.twopf-tot-spec-table", "twopf-tot-spec-table.txt");
    tk3_twopf_total_tab.add_line(tk3_twopf_real_kgp);
    tk3_twopf_total_tab.add_line(tk3_twopf_imag_kgp);

    transport::derived_data::time_series_plot<double> tensor_twopf_plot("powerlaw.threepf-1.tensor-twopf", "tensor-twopf.pdf");
    tensor_twopf_plot.add_line(tensor_twopf_group);
    tensor_twopf_plot.set_title_text("Tensor two-point function");
    tensor_twopf_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_plot<double> r_plot("powerlaw.threepf-1.r-time", "r-time.pdf");
    r_plot.add_line(r_group);
    r_plot.set_title_text("Tensor-to-scalar ratio");
    r_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::wavenumber_series_plot<double> r_spectrum("powerlaw.threepf-1.r-spectrum", "r-spectrum.pdf");
    r_spectrum.add_line(r_spectrum_group);
    r_spectrum.set_title_text("Tensor-to-scalar ratio");
    r_spectrum.set_legend_position(transport::derived_data::legend_pos::bottom_left);


    // plots of some components of the threepf
    transport::index_selector<3> threepf_fields(model->get_N_fields());
    transport::index_selector<3> threepf_momenta(model->get_N_fields());
    threepf_fields.none();
    threepf_momenta.none();

    std::array<unsigned int, 3> sq_set_a = { 0, 0, 0 };
    std::array<unsigned int, 3> sq_set_b = { 0, 1, 0 };
    std::array<unsigned int, 3> sq_set_c = { 1, 1, 0 };
    std::array<unsigned int, 3> sq_set_d = { 0, 0, 1 };
    std::array<unsigned int, 3> sq_set_e = { 0, 1, 1 };
    std::array<unsigned int, 3> sq_set_f = { 1, 1, 1 };
    threepf_fields.set_on(sq_set_a);
    threepf_fields.set_on(sq_set_b);
    threepf_fields.set_on(sq_set_c);
    threepf_fields.set_on(sq_set_d);
    threepf_fields.set_on(sq_set_e);
    threepf_fields.set_on(sq_set_f);

    std::array<unsigned int, 3> sq_set_g = { 2, 2, 2 };
    std::array<unsigned int, 3> sq_set_h = { 2, 3, 2 };
    std::array<unsigned int, 3> sq_set_i = { 3, 3, 2 };
    std::array<unsigned int, 3> sq_set_j = { 2, 2, 3 };
    std::array<unsigned int, 3> sq_set_k = { 2, 3, 3 };
    std::array<unsigned int, 3> sq_set_l = { 3, 3, 3 };
    threepf_momenta.set_on(sq_set_g);
    threepf_momenta.set_on(sq_set_h);
    threepf_momenta.set_on(sq_set_i);
    threepf_momenta.set_on(sq_set_j);
    threepf_momenta.set_on(sq_set_k);
    threepf_momenta.set_on(sq_set_l);

    // THREEPF FIELDS
    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_equi(tk3, threepf_fields, all_times, equilateral_largest_threepf);
    tk3_threepf_fields_equi.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_fields_equi.set_dot_meaning(transport::derived_data::dot_type::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_equi_plot("powerlaw.threepf-1.f-equi", "f-equi.pdf");
    tk3_threepf_field_equi_plot.add_line(tk3_threepf_fields_equi);
    tk3_threepf_field_equi_plot.set_title_text("Three-point function");
    tk3_threepf_field_equi_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_equi_table("powerlaw.threepf-1.f-equi.table", "f-equi-table.txt");
    tk3_threepf_field_equi_table.add_line(tk3_threepf_fields_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_fields_sq(tk3, threepf_fields, all_times, most_squeezed_threepf);
    tk3_threepf_fields_sq.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_fields_sq.set_dot_meaning(transport::derived_data::dot_type::derivatives);
    tk3_threepf_fields_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_field_sq_plot("powerlaw.threepf-1.f-sq", "f-sq.pdf");
    tk3_threepf_field_sq_plot.add_line(tk3_threepf_fields_sq);
    tk3_threepf_field_sq_plot.set_title_text("Three-point function");
    tk3_threepf_field_sq_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_field_sq_table("powerlaw.threepf-1.f-sq.table", "f-sq-table.txt");
    tk3_threepf_field_sq_table.add_line(tk3_threepf_fields_sq);


    // THREEPF DERIVATIVES
    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_equi(tk3, threepf_momenta, all_times, equilateral_largest_threepf);
    tk3_threepf_deriv_equi.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_deriv_equi.set_dot_meaning(transport::derived_data::dot_type::derivatives);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_equi_plot("powerlaw.threepf-1.d-equi", "d-equi.pdf");
    tk3_threepf_deriv_equi_plot.add_line(tk3_threepf_deriv_equi);
    tk3_threepf_deriv_equi_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_equi_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_equi_table("powerlaw.threepf-1.d-equi.table", "d-equi-table.txt");
    tk3_threepf_deriv_equi_table.add_line(tk3_threepf_deriv_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_deriv_sq(tk3, threepf_momenta, all_times, extreme_squeezed_threepf);
    tk3_threepf_deriv_sq.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_deriv_sq.set_dot_meaning(transport::derived_data::dot_type::derivatives);
    tk3_threepf_deriv_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_deriv_sq_plot("powerlaw.threepf-1.d-sq", "d-sq.pdf");
    tk3_threepf_deriv_sq_plot.add_line(tk3_threepf_deriv_sq);
    tk3_threepf_deriv_sq_plot.set_title_text("Three-point function");
    tk3_threepf_deriv_sq_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_deriv_sq_table("powerlaw.threepf-1.d-sq.table", "d-sq-table.txt");
    tk3_threepf_deriv_sq_table.add_line(tk3_threepf_deriv_sq);

    // THREEPF MOMENTA
    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_equi(tk3, threepf_momenta, all_times, equilateral_largest_threepf);
    tk3_threepf_mma_equi.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_mma_equi.set_dot_meaning(transport::derived_data::dot_type::momenta);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_equi_plot("powerlaw.threepf-1.m-equi", "m-equi.pdf");
    tk3_threepf_mma_equi_plot.add_line(tk3_threepf_mma_equi);
    tk3_threepf_mma_equi_plot.set_title_text("Three-point function");
    tk3_threepf_mma_equi_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_equi_table("powerlaw.threepf-1.m-equi.table", "m-equi-table.txt");
    tk3_threepf_mma_equi_table.add_line(tk3_threepf_mma_equi);

    transport::derived_data::threepf_time_series<double> tk3_threepf_mma_sq(tk3, threepf_momenta, all_times, extreme_squeezed_threepf);
    tk3_threepf_mma_sq.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_mma_sq.set_dot_meaning(transport::derived_data::dot_type::momenta);
    tk3_threepf_mma_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_threepf_mma_sq_plot("powerlaw.threepf-1.m-sq", "m-sq.pdf");
    tk3_threepf_mma_sq_plot.add_line(tk3_threepf_mma_sq);
    tk3_threepf_mma_sq_plot.set_title_text("Three-point function");
    tk3_threepf_mma_sq_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_threepf_mma_sq_table("powerlaw.threepf-1.m-sq.table", "m-sq-table.txt");
    tk3_threepf_mma_sq_table.add_line(tk3_threepf_mma_sq);


    transport::derived_data::time_series_plot<double> tk3_mixed_plot("powerlaw.threepf-1.mixed", "mixed.pdf");

    tk3_mixed_plot.add_line(tk3_threepf_fields_equi);
    tk3_mixed_plot.add_line(tk3_twopf_real_group);
    tk3_mixed_plot.set_title_text("Two- and three-point functions");
    tk3_mixed_plot.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    // pick out the shift between derivative and momenta 3pfs
    transport::index_selector<3> threepf_mmta(model->get_N_fields());
    threepf_mmta.none();
    std::array<unsigned int, 3> sq_mmta_a = { 2, 0, 0 };
    std::array<unsigned int, 3> sq_mmta_b = { 3, 0, 0 };
    threepf_mmta.set_on(sq_mmta_a);
    threepf_mmta.set_on(sq_mmta_b);

    transport::derived_data::threepf_time_series<double> tk3_threepf_derivs(tk3, threepf_mmta, all_times, largest_threepf);
    tk3_threepf_derivs.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_derivs.set_dot_meaning(transport::derived_data::dot_type::derivatives);
    tk3_threepf_derivs.set_use_alpha_label(true);
    tk3_threepf_derivs.set_use_beta_label(true);

    transport::derived_data::threepf_time_series<double> tk3_threepf_momenta(tk3, threepf_mmta, all_times, largest_threepf);
    tk3_threepf_momenta.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_threepf_momenta.set_dot_meaning(transport::derived_data::dot_type::momenta);
    tk3_threepf_momenta.set_use_alpha_label(true);
    tk3_threepf_momenta.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_check_shift("powerlaw.threepf-1.checkshift", "checkshift.pdf");

    tk3_check_shift.add_line(tk3_threepf_derivs);
    tk3_check_shift.add_line(tk3_threepf_momenta);
    tk3_check_shift.set_title_text("Comparison of derivative and momenta 3pf");
    tk3_check_shift.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    transport::derived_data::time_series_table<double> tk3_check_shift_table("powerlaw.threepf-1.checkshift.table", "checkshift-table.txt");

    tk3_check_shift_table.add_line(tk3_threepf_derivs);
    tk3_check_shift_table.add_line(tk3_threepf_momenta);

    transport::derived_data::threepf_wavenumber_series<double> tk3_threepf_equi_kgp(tk3, threepf_fields, two_times, all_equilateral);
    tk3_threepf_equi_kgp.set_dot_meaning(transport::derived_data::dot_type::derivatives);
    tk3_threepf_equi_kgp.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_threepf_equi_spec("powerlaw.threepf-1.threepf-equi-spec", "threepf-equi-spec.pdf");
    tk3_threepf_equi_spec.add_line(tk3_threepf_equi_kgp);
    tk3_threepf_equi_spec.set_log_x(true);
    tk3_threepf_equi_spec.set_title_text("Spectrum of 3pf in equilateral configurations");

    transport::derived_data::wavenumber_series_table<double> tk3_threepf_equi_spec_tab("powerlaw.threepf-1.threepf-equi-spec-tab", "threepf-equi-spec-tab.txt");
    tk3_threepf_equi_spec_tab.add_line(tk3_threepf_equi_kgp);

    // check the zeta twopf
    transport::derived_data::zeta_twopf_time_series<double> tk3_zeta_twopf_group(ztk3, all_times, largest_twopf);
    tk3_zeta_twopf_group.set_dimensionless(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_twopf("powerlaw.threepf-1.zeta-twopf", "zeta-twopf.pdf");

    tk3_zeta_twopf.add_line(tk3_zeta_twopf_group);
    tk3_zeta_twopf.add_line(tk3_twopf_real_group);
    tk3_zeta_twopf.set_title_text("Comparison of $\\zeta$ and field 2pfs");
    tk3_zeta_twopf.set_legend_position(transport::derived_data::legend_pos::bottom_left);

    // check the zeta threepf
    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_equi_group(ztk3, all_times, equilateral_extreme_threepf);
    tk3_zeta_equi_group.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_zeta_equi_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_equi("powerlaw.threepf-1.zeta-equi", "zeta-equi.pdf");
    tk3_zeta_equi.add_line(tk3_zeta_equi_group);
    tk3_zeta_equi.set_legend_position(transport::derived_data::legend_pos::centre_right);
    tk3_zeta_equi.set_title_text("3pf of $\\zeta$ near equilateral configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_equi_table("powerlaw.threepf-1.zeta-equi.table", "zeta-equi-table.txt");
    tk3_zeta_equi_table.add_line(tk3_zeta_equi_group);

    transport::derived_data::zeta_threepf_time_series<double> tk3_zeta_sq_group(ztk3, all_times, extreme_squeezed_threepf);
    tk3_zeta_sq_group.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_zeta_sq_group.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_zeta_sq("powerlaw.threepf-1.zeta-sq", "zeta-sq.pdf");
    tk3_zeta_sq.add_line(tk3_zeta_sq_group);
    tk3_zeta_sq.set_legend(false);
    tk3_zeta_sq.set_title_text("3pf of $\\zeta$ near squeezed configurations");

    // set up a table too
    transport::derived_data::time_series_table<double> tk3_zeta_sq_table("powerlaw.threepf-1.zeta-sq.table", "zeta-sq-table.txt");
    tk3_zeta_sq_table.add_line(tk3_zeta_sq_group);

    // compute the reduced bispectrum in a few squeezed configurations
    transport::derived_data::zeta_reduced_bispectrum_time_series<double> tk3_zeta_redbsp_sq(ztk3, all_times, extreme_squeezed_threepf);
    tk3_zeta_redbsp_sq.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_zeta_redbsp_sq.set_use_beta_label(true);

    // and a few equilateral configurations
    transport::derived_data::zeta_reduced_bispectrum_time_series<double> tk3_zeta_redbsp_eq(ztk3, all_times, equilateral_extreme_threepf);
    tk3_zeta_redbsp_sq.set_klabel_meaning(transport::derived_data::klabel_type::comoving);
    tk3_zeta_redbsp_sq.set_use_beta_label(true);

    transport::derived_data::time_series_plot<double> tk3_redbsp_sq("powerlaw.threepf-1.redbsp-sq", "redbsp-sq.pdf");
    tk3_redbsp_sq.set_log_y(false);
    tk3_redbsp_sq.set_abs_y(false);
    tk3_redbsp_sq.add_line(tk3_zeta_redbsp_sq);
    tk3_redbsp_sq.set_legend(false);
//    tk3_redbsp_sq.set_legend_position(transport::derived_data::legend_pos::bottom_right);
    tk3_redbsp_sq.set_title_text("Reduced bispectrum near squeezed configurations");

    transport::derived_data::time_series_plot<double> tk3_redbsp_eq("powerlaw.threepf-1.redbsp-eq", "redbsp-eq.pdf");
    tk3_redbsp_eq.set_log_y(false);
    tk3_redbsp_eq.set_abs_y(false);
    tk3_redbsp_eq.add_line(tk3_zeta_redbsp_eq);
    tk3_redbsp_eq.set_legend_position(transport::derived_data::legend_pos::bottom_right);
    tk3_redbsp_eq.set_title_text("Reduced bispectrum near equilateral configurations");

    transport::derived_data::time_series_table<double> tk3_redbsp_table("powerlaw.threepf-1.redbsp-table", "redbsp-table.txt");
    tk3_redbsp_table.add_line(tk3_zeta_redbsp_sq);
    tk3_redbsp_table.add_line(tk3_zeta_redbsp_eq);

    transport::derived_data::zeta_twopf_wavenumber_series<double> tk3_zeta_2spec(ztk3, two_times, all_twopfs);
    tk3_zeta_2spec.set_klabel_meaning(transport::derived_data::klabel_type::conventional);
    tk3_zeta_2spec.set_dimensionless(true);

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_2spec_plot("powerlaw.threepf-1.zeta-2pec", "zeta-2spec.pdf");
    tk3_zeta_2spec_plot.add_line(tk3_zeta_2spec);
    tk3_zeta_2spec_plot.set_log_x(true);
    tk3_zeta_2spec_plot.set_title("$\\langle \\zeta \\zeta \\rangle$ power spectrum");

    transport::derived_data::wavenumber_series_table<double> tk3_zeta_2spec_table("powerlaw.threepf-1.zeta-2spec.table", "zeta-2spec-table.txt");
    tk3_zeta_2spec_table.add_line(tk3_zeta_2spec);

    transport::derived_data::zeta_threepf_wavenumber_series<double> tk3_zeta_3equspec(ztk3, two_times, all_equilateral);
    tk3_zeta_3equspec.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_zeta_3equspec_plot("powerlaw.threepf-1.zeta-3equspec", "zeta-3equspec.pdf");
    tk3_zeta_3equspec_plot.add_line(tk3_zeta_3equspec);
    tk3_zeta_3equspec_plot.set_log_x(true);
    tk3_zeta_3equspec_plot.set_title("$\\langle \\zeta \\zeta \\zeta \\rangle$ equilateral configuration");

    transport::derived_data::wavenumber_series_table<double> tk3_zeta_3equspec_table("powerlaw.threepf-1.zeta-3equspec.table", "zeta-3equspec-table.txt");
    tk3_zeta_3equspec_table.add_line(tk3_zeta_3equspec);

    transport::derived_data::zeta_reduced_bispectrum_wavenumber_series<double> tk3_zeta_redbsp_spec(ztk3, two_times, extreme_squeezed_threepf);
    tk3_zeta_redbsp_spec.set_klabel_meaning(transport::derived_data::klabel_type::conventional);

    transport::derived_data::wavenumber_series_plot<double> tk3_redbsp_spec_plot("powerlaw.threepf-1.redbsp-spec", "redbsp-spec.pdf");
    tk3_redbsp_spec_plot.add_line(tk3_zeta_redbsp_spec);
    tk3_redbsp_spec_plot.set_log_x(true);
    tk3_redbsp_spec_plot.set_title("Reduced bispectrum in the squeezed limit");

    transport::derived_data::wavenumber_series_table<double> tk3_redbsp_spec_table("powerlaw.threepf-1.redbsp-spec-table", "redbsp-spec-table.txt");
    tk3_redbsp_spec_table.add_line(tk3_zeta_redbsp_spec);

    transport::derived_data::fNL_time_series<double> fNLloc_time_series(ztk3_fNL_local, all_times);

    transport::derived_data::fNL_time_series<double> fNLequi_time_series(ztk3_fNL_equi, all_times);

    transport::derived_data::fNL_time_series<double> fNLortho_time_series(ztk3_fNL_ortho, all_times);

    transport::derived_data::time_series_plot<double> fNL_plot("powerlaw.threepf-1.fNL", "fNL.pdf");
    fNL_plot.add_line(fNLloc_time_series);
    fNL_plot.add_line(fNLequi_time_series);
    fNL_plot.add_line(r_group);
//    fNL_plot.add_line(fNLortho_time_series);
//    fNL_plot.add_line(tk3_zeta_redbsp_sq);
    fNL_plot.set_log_y(false);
    fNL_plot.set_abs_y(false);
    fNL_plot.set_title(false);
    fNL_plot.set_y_label(true);
    fNL_plot.set_typeset_with_LaTeX(true);

    transport::derived_data::time_series_table<double> fNL_table("powerlaw.threepf-1.fNL-table", "fNL-table.txt");
    fNL_table.add_line(fNLloc_time_series);
    fNL_table.add_line(fNLequi_time_series);
    fNL_table.add_line(fNLortho_time_series);

    std::cout << "3pf equilateral plot:" << '\n' << tk3_zeta_equi << '\n';

    std::cout << "3pf squeezed plot:" << '\n' << tk3_zeta_sq << '\n';


    // construct output tasks
    transport::output_task<double> twopf_output("powerlaw.twopf-1.output", tk2_bg_plot);
    twopf_output.add_element(tk2_twopf_real_plot);
    twopf_output.add_element(tk2_twopf_total_plot);
    twopf_output.add_element(tk2_tensor_twopf_plot);
    twopf_output.add_element(tk2_compare_plot);
    twopf_output.add_element(tk2_tensor_spectrum_plot);

    transport::output_task<double> threepf_output("powerlaw.threepf-1.output", tk3_bg_plot);
    threepf_output.add_element(tk3_twopf_real_plot);
    threepf_output.add_element(tk3_twopf_imag_plot);
    threepf_output.add_element(tk3_twopf_total_plot);
    threepf_output.add_element(tk3_threepf_field_equi_plot);
//		threepf_output.add_element(tk3_threepf_field_equi_table);
//		threepf_output.add_element(tk3_threepf_mma_equi_plot);
//		threepf_output.add_element(tk3_threepf_mma_equi_table);
//		threepf_output.add_element(tk3_threepf_deriv_equi_plot);
//		threepf_output.add_element(tk3_threepf_deriv_equi_table);
    threepf_output.add_element(tk3_threepf_field_sq_plot);
//    threepf_output.add_element(tk3_threepf_field_sq_table);
//    threepf_output.add_element(tk3_threepf_mma_sq_plot);
//    threepf_output.add_element(tk3_threepf_mma_sq_table);
//    threepf_output.add_element(tk3_threepf_deriv_sq_plot);
//    threepf_output.add_element(tk3_threepf_deriv_sq_table);
//		threepf_output.add_element(tk3_mixed_plot);
//    threepf_output.add_element(tk3_check_shift);
//		threepf_output.add_element(tk3_check_shift_table);
    threepf_output.add_element(tensor_twopf_plot);
    threepf_output.add_element(r_plot);
    threepf_output.add_element(r_spectrum);
    threepf_output.add_element(tk3_zeta_twopf);
    threepf_output.add_element(tk3_zeta_equi);
    threepf_output.add_element(tk3_zeta_equi_table);
    threepf_output.add_element(tk3_zeta_sq);
    threepf_output.add_element(tk3_zeta_sq_table);
    threepf_output.add_element(tk3_redbsp_sq);
    threepf_output.add_element(tk3_redbsp_eq);
    threepf_output.add_element(tk3_redbsp_table);
    threepf_output.add_element(tk3_twopf_real_spec);
//    threepf_output.add_element(tk3_twopf_imag_spec);
//    threepf_output.add_element(tk3_twopf_total_spec);
//    threepf_output.add_element(tk3_twopf_total_tab);
    threepf_output.add_element(tk3_threepf_equi_spec);
    threepf_output.add_element(tk3_threepf_equi_spec_tab);
    threepf_output.add_element(tk3_zeta_2spec_plot);
    threepf_output.add_element(tk3_zeta_2spec_table);
    threepf_output.add_element(tk3_zeta_3equspec_plot);
    threepf_output.add_element(tk3_zeta_3equspec_table);
    threepf_output.add_element(tk3_redbsp_spec_plot);
    threepf_output.add_element(tk3_redbsp_spec_table);

    transport::output_task<double> fNLloc_task("powerlaw.threepf-1.fNL", fNL_plot);
    fNLloc_task.add_element(fNL_table);

    std::cout << "powerlaw.threepf-1 output task:" << '\n' << threepf_output << '\n';

    // write output tasks to the database
//		repo->commit_task(twopf_output);
		repo->commit_task(ztk2);
    repo->commit_task(ztk3);
    repo->commit_task(ztk3_fNL_local);
    repo->commit_task(ztk3_fNL_equi);
    repo->commit_task(ztk3_fNL_ortho);
    repo->commit_task(twopf_output);
    repo->commit_task(threepf_output);
    repo->commit_task(fNLloc_task);

    return (EXIT_SUCCESS);
	}
