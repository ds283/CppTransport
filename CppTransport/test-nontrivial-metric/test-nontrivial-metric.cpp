//
// Created by David Seery on 27/06/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#include "transport-runtime/transport.h"
#include "gelaton_mpi.h"


void write_tasks(transport::repository<>& repo, transport::gelaton_mpi<>* model);


void write_tasks(transport::repository<>& repo, transport::gelaton_mpi<>* model)
  {
    const double M_P           = 1.0;
    const double P_zeta        = 1E-9;                                                          // desired amplitude of fluctuations
    const double omega         = M_PI / 30.0;                                                   // try to get round pi radians in ~ 30 e-folds
    
    const double V0            = 0.1 * P_zeta * M_P*M_P*M_P*M_P;                                // adjust uplift to get sufficient inflation
    const double eta_R         = 1.0/std::sqrt(3.0);                                            // adjust radial mass to be of order Hubble
    const double g_R           = M_P*M_P / std::sqrt(V0);                                       // adjust radial cubic coupling to be of order Hubble
    const double lambda_R      = 0.5 * M_P*M_P*M_P / std::pow(V0, 3.0/4.0) / std::sqrt(omega);  // adjust radial quartic coupling to dominate the displacement
    
    const double R0            = std::sqrt(V0/3.0) / (M_P * omega * std::sqrt(P_zeta));         // adjust radial minimum to give desired P_zeta normalization
    
    const double x_init        = -R0;
    const double y_init        = (1E-2)*R0;
    const double R_init        = std::sqrt(x_init*x_init + y_init*y_init);
    const double theta_init    = std::atan2(y_init, x_init);
    
    const double N_init        = 0.0;
    const double N_pre         = 8.0;
    const double N_max         = 28.0;
    
    transport::parameters<> params{M_P, { R0, V0, eta_R, g_R, lambda_R, omega }, model};
    transport::initial_conditions<> ics{"gelaton", params, { R_init, theta_init, 0.0, 0.0 }, N_init, N_pre};
    
    transport::basic_range<> times{N_init, N_max, 500, transport::spacing::linear};
    
    transport::basic_range<> ks{exp(10.0), exp(10.0), 0, transport::spacing::log_bottom};
    transport::basic_range<> kts{exp(10.0), exp(17.0), 100, transport::spacing::log_bottom};
    transport::basic_range<> alphas{0.0, 0.0, 0, transport::spacing::linear};
    transport::basic_range<> betas{1.0/3.0, 1.0/3.0, 0, transport::spacing::linear};
    
    // construct a twopf task
    transport::twopf_task<> tk2{"gelaton.twopf", ics, times, ks};
    tk2.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);
    
    // construct a threepf task
    transport::threepf_alphabeta_task<> tk3{"gelaton.threepf", ics, times, kts, alphas, betas};
    tk3.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);
    
    transport::zeta_twopf_task<> ztk2{"gelaton.twopf-zeta", tk2};
    transport::zeta_threepf_task<> ztk3{"gelaton.threepf-zeta", tk3};
    
    ztk2.set_paired(true);
    ztk3.set_paired(true);
    
    vis_toolkit::SQL_time_query tquery{"1=1"};
    vis_toolkit::SQL_twopf_query k2query{"comoving IN (SELECT MAX(comoving) FROM twopf_samples UNION SELECT MIN(comoving) FROM twopf_samples)"};
    vis_toolkit::SQL_threepf_query k3query{"kt_comoving IN (SELECT MAX(kt_comoving) FROM threepf_samples UNION SELECT MIN(kt_comoving) FROM threepf_samples)"};
    
    vis_toolkit::zeta_twopf_time_series<> z2pf{ztk3, tquery, k2query};    // defaults to dimensionless
    vis_toolkit::largest_u2_line<> u2line{tk3, tquery, k2query};
    vis_toolkit::largest_u3_line<> u3line{tk3, tquery, k3query};
    vis_toolkit::background_line<> hubble{tk3, tquery, vis_toolkit::background_quantity::Hubble};
    
    vis_toolkit::index_selector<2> f2_fields{model->get_N_fields()};
    f2_fields.all();
//    f2_fields.none().set_on({0,0}).set_on({0,1}).set_on({1,1});
    vis_toolkit::index_selector<2> f2_momenta{model->get_N_fields()};
    f2_momenta.none().set_on({2,2}).set_on({2,3}).set_on({3,3});
    
    vis_toolkit::twopf_time_series<> tk2_fields{tk2, f2_fields, tquery, k2query};
    vis_toolkit::twopf_time_series<> tk2_momenta{tk2, f2_momenta, tquery, k2query};
    
    vis_toolkit::time_series_plot<> tk2_fplot{"gelaton.twopf-1.fields-plot", "fields-plot.pdf"};
    tk2_fplot += tk2_fields;
    vis_toolkit::time_series_plot<> tk2_mplot{"gelaton.twopf-1.momenta-plot", "momenta-plot.pdf"};
    tk2_mplot += tk2_momenta;
    
    transport::output_task<> tk2_out{"gelaton.test"};
    tk2_out += tk2_fplot + tk2_mplot;
    repo.commit(tk2_out);
    
    vis_toolkit::time_series_table<> table{"gelaton.utable", "utable.csv"};
    table += z2pf + u2line + u3line + hubble;
    
    vis_toolkit::index_selector<3> fsel{model->get_N_fields()};
    fsel.none().set_on({0,0,0}).set_on({1,1,1});
    vis_toolkit::threepf_time_series<> tpf{tk3, fsel, tquery, k3query};
    
    vis_toolkit::time_series_plot<> plot{"gelaton.threepf-fields", "threepf-fields.pdf"};
    plot += tpf;
    
    vis_toolkit::zeta_threepf_time_series<> ztpf{ztk3, tquery, k3query};
    
    vis_toolkit::time_series_plot<> plot2{"gelaton.threepf-zeta", "threepf-zeta.pdf"};
    plot2 += ztpf;
    
    vis_toolkit::zeta_reduced_bispectrum_time_series<> zrbsp(ztk3, tquery, k3query);
    
    vis_toolkit::time_series_plot<> plot3("gelaton.threepf-redbsp", "threepf-redbsp.pdf");
    plot3.set_log_y(false);
    plot3 += zrbsp;
    
    vis_toolkit::twopf_time_series<> tk3_fields{tk3, f2_fields, tquery, k2query};
    vis_toolkit::twopf_time_series<> tk3_momenta{tk3, f2_momenta, tquery, k2query};
    
    vis_toolkit::time_series_plot<> tk3_fplot{"gelaton.threepf-1.fields-plot", "fields-plot.pdf"};
    tk3_fplot += tk3_fields;
    vis_toolkit::time_series_plot<> tk3_mplot{"gelaton.threepf-1.momenta-plot", "momenta-plot.pdf"};
    tk3_mplot += tk3_momenta;
    
    vis_toolkit::SQL_time_query tquery2{"serial IN (SELECT MAX(serial) FROM time_samples)"};
    vis_toolkit::SQL_threepf_query k3query2("1=1");
    
    vis_toolkit::zeta_reduced_bispectrum_wavenumber_series<> fNL{ztk3, tquery2, k3query2};
    fNL.set_current_x_axis_value(vis_toolkit::axis_value::efolds_exit);
    
    vis_toolkit::wavenumber_series_plot<> fNLplot{"gelaton.fNL", "fNL.pdf"};
    fNLplot += fNL;
    
    transport::output_task<> otk{"gelaton.output"};
    otk += table + plot + plot2 + plot3 + tk3_fplot + tk3_mplot + fNLplot;
    
    repo.commit(ztk2);
    repo.commit(ztk3);
    repo.commit(otk);
  }


int main(int argc, char* argv[])
  {
    // create task manager instance
    transport::task_manager<> mgr(argc, argv);
    
    // create model instance
    auto model = mgr.create_model< transport::gelaton_mpi<> >();
    
    // write tasks to repository
    mgr.add_generator([=](transport::repository<>& repo) -> void { write_tasks(repo, model.get()); });
    
    // hand off control to task manager
    mgr.process();
    
    return(EXIT_SUCCESS);
  }
