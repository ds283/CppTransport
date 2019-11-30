//
// Created by Alessandro Maraio on 30/11/2019.
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//

//#include "boost/multiprecision/mpfr.hpp"

//using mpfr50 = boost::multiprecision::number< boost::multiprecision::mpfr_float_backend<50>, boost::multiprecision::et_off >;
//using mpfr50 = boost::multiprecision::mpfr_float_50;

//namespace std
//  {
//
//    using boost::multiprecision::sqrt;
//    using boost::multiprecision::log;
//    using boost::multiprecision::pow;
//    using boost::multiprecision::exp;
//
//    using boost::multiprecision::abs;
//    using boost::multiprecision::sinh;
//    using boost::multiprecision::cosh;
//    using boost::multiprecision::tanh;
//
//    bool isnan(const mpfr50& x) { return boost::math::isnan(x); }
//    bool isinf(const mpfr50& x) { return boost::math::isinf(x); }
//    mpfr50 max(const mpfr50& a, const mpfr50& b) { return (a < b) ? b : a; }
//
//  }

//#include "boost/numeric/odeint/external/mkl/mkl_operations.hpp"
//
//#define CPPTRANSPORT_ALGEBRA_NAME(state) boost::numeric::odeint::vector_space_algebra
//#define CPPTRANSPORT_OPERATIONS_NAME(state) boost::numeric::odeint::mkl_operations

#include "gelaton_mpi.h"

using DataType = double;
using StateType = std::vector<DataType>;

//namespace transport
//  {
//
//    template <>
//    inline std::string data_type_name<mpfr50>() { return std::string{"boost::multiprecision::mpfr_float_50 without expression templates"}; }
//
//  }


void write_tasks(transport::repository<DataType>& repo, transport::gelaton_mpi<DataType, StateType>* model)
  {
    const double N_init        = 0.0;
    const double N_pre         = 8.0;
    const double N_max         = 28.0;

    transport::basic_range<double> times{N_init, N_max, 500, transport::spacing::linear};
    
    transport::basic_range<double> ks{exp(10.0), exp(18.5), 1000, transport::spacing::log_bottom};
    transport::basic_range<double> kts{exp(10.0), exp(17.0), 100, transport::spacing::log_bottom};
    transport::basic_range<double> alphas{0.0, 0.0, 0, transport::spacing::linear};
    transport::basic_range<double> betas{1.0/3.0, 1.0/3.0, 0, transport::spacing::linear};

    // construct a twopf task using the model instance instead of explicit parameters and initial conditions
    transport::twopf_task<DataType> tk2{"gelaton.twopf", model, times, ks};
    tk2.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);
    
    // construct a threepf task again using the model class instead of explicit parameters and initial conditions
    transport::threepf_alphabeta_task<DataType> tk3{"gelaton.threepf", model, times, kts, alphas, betas};
    tk3.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);
    
    transport::zeta_twopf_task<DataType> ztk2{"gelaton.twopf-zeta", tk2};
    transport::zeta_threepf_task<DataType> ztk3{"gelaton.threepf-zeta", tk3};
    
    ztk2.set_paired(true);
    ztk3.set_paired(true);
    
    vis_toolkit::SQL_time_query tquery{"1=1"};
    vis_toolkit::SQL_time_query tlast{"serial IN (SELECT MAX(serial) FROM time_samples)"};
    
    vis_toolkit::SQL_twopf_query k2query{"comoving IN (SELECT MAX(comoving) FROM twopf_samples UNION SELECT MIN(comoving) FROM twopf_samples)"};
    vis_toolkit::SQL_threepf_query k3query{"kt_comoving IN (SELECT MAX(kt_comoving) FROM threepf_samples UNION SELECT MIN(kt_comoving) FROM threepf_samples)"};
    
    vis_toolkit::SQL_twopf_query k2all{"1=1"};
    vis_toolkit::SQL_threepf_query k3all("1=1");
    
    vis_toolkit::zeta_twopf_time_series<DataType> z2pf{ztk3, tquery, k2query};    // defaults to dimensionless
    vis_toolkit::largest_u2_line<DataType> u2line{tk3, tquery, k2query};
    vis_toolkit::largest_u3_line<DataType> u3line{tk3, tquery, k3query};
    vis_toolkit::background_line<DataType> hubble{tk3, tquery, vis_toolkit::background_quantity::Hubble};
    
    vis_toolkit::index_selector<2> f2_fields{model->get_N_fields()};
    f2_fields.none().set_on({0,0}).set_on({0,1}).set_on({1,0}).set_on({1,1});
    vis_toolkit::index_selector<2> f2_momenta{model->get_N_fields()};
    f2_momenta.none().set_on({2,2}).set_on({2,3}).set_on({3,2}).set_on({3,3});
    vis_toolkit::index_selector<2> f2_cross{model->get_N_fields()};
    f2_cross.none().set_on({2,0}).set_on({2,1}).set_on({3,0}).set_on({3,1});
    vis_toolkit::index_selector<2> f2_cross2{model->get_N_fields()};
    f2_cross2.none().set_on({0,2}).set_on({1,2}).set_on({0,3}).set_on({1,3});
    
    vis_toolkit::twopf_time_series<DataType> tk2_fields{tk2, f2_fields, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk2_momenta{tk2, f2_momenta, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk2_cross{tk2, f2_cross, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk2_cross2{tk2, f2_cross2, tquery, k2query};
    
    vis_toolkit::time_series_plot<DataType> tk2_fplot{"gelaton.twopf-1.fields-plot", "fields-plot.pdf"};
    tk2_fplot += tk2_fields;
    vis_toolkit::time_series_plot<DataType> tk2_mplot{"gelaton.twopf-1.momenta-plot", "momenta-plot.pdf"};
    tk2_mplot += tk2_momenta;
    vis_toolkit::time_series_plot<DataType> tk2_cplot("gelaton.twopf-1.cross-plot", "cross-plot.pdf");
    tk2_cplot += tk2_cross;
    vis_toolkit::time_series_plot<DataType> tk2_c2plot("gelaton.twopf-1.cross2-plot", "cross2-plot.pdf");
    tk2_c2plot += tk2_cross2;
    
    vis_toolkit::u2_line<DataType> tk2_u2_fields{tk2, f2_fields, tquery, k2query};
    vis_toolkit::u2_line<DataType> tk2_u2_momenta{tk2, f2_momenta, tquery, k2query};
    vis_toolkit::u2_line<DataType> tk2_u2_cross{tk2, f2_cross, tquery, k2query};
    vis_toolkit::u2_line<DataType> tk2_u2_cross2{tk2, f2_cross2, tquery, k2query};
    
    vis_toolkit::time_series_plot<DataType> tk2_u2_mplot{"gelaton.twopf-1.u2-momenta-plot", "u2-momenta-plot.pdf"};
    tk2_u2_mplot += tk2_u2_momenta;
    vis_toolkit::time_series_plot<DataType> tk2_u2_cplot{"gelaton.twopf-1.u2-cross-plot", "u2-cross-plot.pdf"};
    tk2_u2_cplot += tk2_u2_cross;
    vis_toolkit::time_series_plot<DataType> tk2_u2_c2plot{"gelaton.twopf-1.u2-cross2-plot", "u2-cross2-plot.pdf"};
    tk2_u2_c2plot += tk2_u2_cross2;
    
    vis_toolkit::time_series_table<DataType> tk2_u2_table{"gelaton.twopf-1.u2-table", "u2-table.txt"};
    tk2_u2_table += tk2_u2_fields + tk2_u2_momenta + tk2_u2_cross + tk2_u2_cross2;
    
    vis_toolkit::index_selector<1> Rindex{model->get_N_fields()};
    Rindex.none().set_on({0});
    vis_toolkit::index_selector<1> thetaindex{model->get_N_fields()};
    thetaindex.none().set_on({1});

    vis_toolkit::background_time_series<DataType> tk2_R{tk2, Rindex, tquery};
    vis_toolkit::background_time_series<DataType> tk2_theta{tk2, thetaindex, tquery};
    vis_toolkit::background_line<DataType> tk2_eps{tk2, tquery, vis_toolkit::background_quantity::epsilon};
    vis_toolkit::background_line<DataType> tk2_H{tk2, tquery, vis_toolkit::background_quantity::Hubble};
    
    vis_toolkit::time_series_plot<DataType> tk2_R_plot{"gelaton.twopf-1.bg-R", "bg-R.pdf"};
    tk2_R_plot += tk2_R;
    vis_toolkit::time_series_plot<DataType> tk2_theta_plot{"gelaton.twopf-1.bg-theta", "bg-theta.pdf"};
    tk2_theta_plot += tk2_theta;
    vis_toolkit::time_series_plot<DataType> tk2_eps_plot{"gelaton.twopf-1.epsilon", "epsilon.pdf"};
    tk2_eps_plot += tk2_eps;
    vis_toolkit::time_series_plot<DataType> tk2_H_plot{"gelaton.twopf-1.Hubble", "Hubble.pdf"};
    tk2_H_plot += tk2_H;
    
    vis_toolkit::time_series_table<DataType> tk2_bg_table{"gelaton.twopf-1.bg-table", "bg-table.txt"};
    tk2_bg_table += tk2_R + tk2_theta + tk2_eps + tk2_H;
    
    transport::output_task<DataType> tk2_out{"gelaton.test"};
    tk2_out += tk2_fplot + tk2_mplot + tk2_cplot + tk2_c2plot
               + tk2_u2_mplot + tk2_u2_cplot + tk2_u2_c2plot + tk2_u2_table
               + tk2_R_plot + tk2_theta_plot + tk2_eps_plot + tk2_H_plot + tk2_bg_table;
    repo.commit(tk2_out);
    
    vis_toolkit::time_series_table<DataType> table{"gelaton.utable", "utable.txt"};
    table += z2pf + u2line + u3line + hubble;
    
    vis_toolkit::zeta_twopf_wavenumber_series<DataType> zeta2pf{ztk3, tlast, k2all};
    zeta2pf.set_current_x_axis_value(vis_toolkit::axis_value::efolds_exit);
    
    vis_toolkit::wavenumber_series_plot<DataType> zeta2pf_plot{"gelaton.threepf-1.zeta_2pf", "zeta2pf-plot.pdf"};
    zeta2pf_plot += zeta2pf;
    
    vis_toolkit::index_selector<3> fsel{model->get_N_fields()};
    fsel.none().set_on({0,0,0}).set_on({1,1,1});
    vis_toolkit::threepf_time_series<DataType> tpf{tk3, fsel, tquery, k3query};
    
    vis_toolkit::time_series_plot<DataType> plot{"gelaton.threepf-fields", "threepf-fields.pdf"};
    plot += tpf;
    
    vis_toolkit::zeta_twopf_time_series<DataType> ztwpf{ztk3, tquery, k2query};
    vis_toolkit::zeta_threepf_time_series<DataType> zthpf{ztk3, tquery, k3query};
    vis_toolkit::zeta_reduced_bispectrum_time_series<DataType> zrbsp(ztk3, tquery, k3query);
    
    vis_toolkit::time_series_plot<DataType> plot2{"gelaton.threepf-zeta", "threepf-zeta.pdf"};
    plot2 += zthpf;
    vis_toolkit::time_series_plot<DataType> plot2a{"gelaton.twopf-zeta", "twopf-zeta.pdf"};
    plot2a += ztwpf;
    vis_toolkit::time_series_plot<DataType> plot3("gelaton.threepf-redbsp", "threepf-redbsp.pdf");
    plot3.set_log_y(false);
    plot3 += zrbsp;
    
    vis_toolkit::time_series_table<DataType> thpf_tab{"gelaton.threepf-table", "threepf-table.txt"};
    thpf_tab += ztwpf + zthpf + zrbsp;
    
    vis_toolkit::twopf_time_series<DataType> tk3_fields{tk3, f2_fields, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk3_momenta{tk3, f2_momenta, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk3_cross{tk3, f2_cross, tquery, k2query};
    vis_toolkit::twopf_time_series<DataType> tk3_cross2{tk3, f2_cross2, tquery, k2query};
    
    vis_toolkit::time_series_plot<DataType> tk3_fplot{"gelaton.threepf-1.fields-plot", "fields-plot.pdf"};
    tk3_fplot += tk3_fields;
    vis_toolkit::time_series_plot<DataType> tk3_mplot{"gelaton.threepf-1.momenta-plot", "momenta-plot.pdf"};
    tk3_mplot += tk3_momenta;
    vis_toolkit::time_series_plot<DataType> tk3_cplot{"gelaton.threepf-1.cross-plot", "cross-plot.pdf"};
    tk3_cplot += tk3_cross;
    vis_toolkit::time_series_plot<DataType> tk3_c2plot{"gelaton.threepf-1.cross2-plot", "cross2-plot.pdf"};
    tk3_c2plot += tk3_cross2;
    
    vis_toolkit::zeta_reduced_bispectrum_wavenumber_series<DataType> fNL{ztk3, tlast, k3all};
    fNL.set_current_x_axis_value(vis_toolkit::axis_value::efolds_exit);
    
    vis_toolkit::wavenumber_series_plot<DataType> fNLplot{"gelaton.fNL", "fNL.pdf"};
    fNLplot.set_log_y(false);
    fNLplot += fNL;
    
    transport::output_task<DataType> otk{"gelaton.output"};
    otk += zeta2pf_plot + table + plot + plot2 + plot2a + plot3 + tk3_fplot + tk3_mplot + tk3_cplot + tk3_c2plot
           + thpf_tab + fNLplot;
    
    repo.commit(ztk2);
    repo.commit(ztk3);
    repo.commit(otk);
  }


int main(int argc, char* argv[])
  {
    // create task manager instance
    transport::task_manager<DataType> mgr(argc, argv);
    
    // create model instance
    auto model = mgr.create_model< transport::gelaton_mpi<DataType, StateType> >();
    
    // write tasks to repository
    mgr.add_generator([=](transport::repository<DataType>& repo) -> void { write_tasks(repo, model.get()); });
    
    // hand off control to task manager
    mgr.process();
    
    return(EXIT_SUCCESS);
  }
