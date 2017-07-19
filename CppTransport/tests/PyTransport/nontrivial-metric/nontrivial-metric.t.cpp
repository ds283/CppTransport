//
// Created by David Seery on 18/07/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#include "yvette_mpi.h"
#include "model.h"

#include "catch/catch.hpp"


using DataType = long double;

constexpr double FP_MATCH_TOLERANCE = 1E-6;
constexpr double FP_MATCH_MINIMUM = 1E-10;


template <typename Value>
constexpr bool fp_match(Value v1, Value v2)
  {
    if(std::abs(v1) < FP_MATCH_MINIMUM || std::abs(v2) < FP_MATCH_MINIMUM)
      return std::abs(v1 - v2) < FP_MATCH_TOLERANCE;
    
    return std::abs((v1 - v2)/v1) < FP_MATCH_TOLERANCE;
  }


SCENARIO( "Comparison of Yvette model between CppTransport and PyTransport", "[yvette-model]" )
  {
    
    // create CppTransport model instance, *avoiding* using the task manager
    // which doesn't play nicely with Catch
    transport::local_environment env;
    transport::argument_cache cache;

    auto CppTransport_model = std::make_shared< transport::yvette_mpi<DataType> >(env, cache);
    
    // create PyTransport model instance
    model PyTransport_model;
    
    // set up parameter and initial conditions information
    const DataType M_P = 1.0;     // PyTransport uses M_P=1, so we must match
    
    const DataType chi0 = 14.7 * M_P;
    const DataType m = std::sqrt(6.0) * 1.4E-5 * M_P*M_P / chi0;
    const DataType Gamma0 = 0.9;
    const DataType DeltaChi = 0.084 * M_P;
    const DataType M = std::sqrt(300.0/6.0) * m * chi0 / M_P;
    
    transport::parameters<DataType> params{M_P, {Gamma0, chi0, DeltaChi, m, M}, CppTransport_model};
    
    const DataType chi_init = chi0 + 3.0*M_P;
    const DataType psi_init = 0.0;
    
    const DataType H0 = CppTransport_model->H(params, {chi_init, psi_init, 0.0, 0.0});
    
    const DataType dchi_init = -1E-6 * M_P*M_P / H0;
    const DataType dpsi_init = 0.0;
    
    const double N_init = 0.0;
    const double N_stop = 79.091;
    const double N_max = 55.0;
    const double N_cross = N_stop - 55.0;
    
    transport::initial_conditions<DataType> ics{"yvette", params, {chi_init, psi_init, dchi_init, dpsi_init}, N_init, N_cross};
    
    transport::basic_range<double> times{N_init, N_max, 500, transport::spacing::linear};
    
    transport::basic_range<double> ks{1.0, std::exp(2.0), 4, transport::spacing::log_bottom};
    
    // construct background task
    transport::background_task<DataType> bgtk{ics, times};
    
    // evolve solution
    transport::backg_history<DataType> history;
    CppTransport_model->backend_process_backg(&bgtk, history, true);
    
    // determine where to check equivalence
    unsigned int check_point = 200;
    
    // construct twopf task, needed to compute any of the u-tensors using CppTransport
    transport::twopf_task<DataType> tk2{"yvette.twopf", ics, times, ks};
    
    unsigned int Nf = CppTransport_model->get_N_fields();
    
    // copy parameter values into std::vector<double> for PyTransport
    // (it can't accept a long double vector)
    std::vector<double> PyTransport_params;
    std::copy(params.get_vector().begin(), params.get_vector().end(), std::back_inserter(PyTransport_params));

    // copy field-space coordinate values at check point into std::vector<double> for PyTransport
    // (it can't accept a long double vector)
    std::vector<double> PyTransport_fields;
    std::copy(history[check_point].begin(), history[check_point].end(), std::back_inserter(PyTransport_fields));

    // PyTransport expects momenta to be time derivatives, not momenta derivatives
    auto H = CppTransport_model->H(params, history[check_point]);
    for(auto t = PyTransport_fields.begin() + Nf; t != PyTransport_fields.end(); ++t)
      {
        *t = (*t) * static_cast<double>(H);
      }
    
    GIVEN( "calculation of Hubble rate" )
      {
    
        auto CppTransport_H = CppTransport_model->H(params, history[check_point]);
        auto PyTransport_H = PyTransport_model.H(PyTransport_fields, PyTransport_params);
        
        REQUIRE( fp_match(CppTransport_H, static_cast<DataType>(PyTransport_H)) );
        
      }
    
    
    GIVEN( "calculation of epsilon" )
      {
        
        auto CppTransport_eps = CppTransport_model->epsilon(params, history[check_point]);
        auto PyTransport_eps = PyTransport_model.Ep(PyTransport_fields, PyTransport_params);
    
        REQUIRE( fp_match(CppTransport_eps, static_cast<DataType>(PyTransport_eps)) );
        
      }


    // PyTransport evolves correlation functions for field *time* derivatives, but does so in e-fold time
    // this means that we need to adjust CppTransport's u-tensors by factors of H, as follows:
    // - if the first index is a momentum, multiply the CppTransport u-tensor by H
    // - if any second or subsequent indices are momenta, divide the CppTransport u-tensor by H
    

    GIVEN( "calculation of u2" )
      {
    
        const auto& k_db = tk2.get_twopf_database();
        const double k = (**k_db.lookup(0)).k_comoving;
    
        std::vector<DataType> CppTransport_u2(2*Nf * 2*Nf);
        CppTransport_model->u2(&tk2, history[check_point], k, times[check_point], CppTransport_u2);
        
        // get correct normalization of a(t) for PyTransport
        auto PyTransport_N = times[check_point] - tk2.get_N_horizon_crossing() + tk2.get_astar_normalization();
        auto PyTransport_u2 = PyTransport_model.u(PyTransport_fields, PyTransport_params, k, PyTransport_N);
        
        for(unsigned int i = 0; i < 2*Nf; ++i)
          {
            for(unsigned int j = 0; j < 2*Nf; ++j)
              {
                if(i < Nf || j < Nf) // ignore momentum-momentum block, which differs by a factor of epsilon
                  {
                    int Hpower = (i >= Nf ? 1 : 0) + (j >= Nf ? -1 : 0);
                    DataType Hfactor = std::pow(H, Hpower);
    
                    REQUIRE( fp_match(Hfactor * CppTransport_u2[transport::FLATTEN(i,j)], static_cast<DataType>(PyTransport_u2[i + j*2*Nf])) );
                  }
              }
          }

      }
    
    
    GIVEN( "calculation of u3" )
      {
        
        const auto& k_db = tk2.get_twopf_database();
        
        const double k0 = (**k_db.lookup(0)).k_comoving;
        const double k1 = (**k_db.lookup(1)).k_comoving;
        const double k2 = (**k_db.lookup(2)).k_comoving;
        
        std::vector<DataType> CppTransport_u3(2*Nf * 2*Nf * 2*Nf);
        CppTransport_model->u3(&tk2, history[check_point], k0, k1, k2, times[check_point], CppTransport_u3);
        
        // get correct normalization of a(t) for PyTransport
        auto PyTransport_N = times[check_point] - tk2.get_N_horizon_crossing() + tk2.get_astar_normalization();
        auto PyTransport_u3 = PyTransport_model.u(PyTransport_fields, PyTransport_params, k0, k1, k2, PyTransport_N);
    
        for(unsigned int i = 0; i < 2*Nf; ++i)
          {
            for(unsigned int j = 0; j < 2*Nf; ++j)
              {
                for(unsigned int k = 0; k < 2*Nf; ++k)
                  {
                    int Hpower = (i >= Nf ? 1 : 0) + (j >= Nf ? -1 : 0) + (k >= Nf ? -1 : 0);
                    DataType Hfactor = std::pow(H, Hpower);
                    
                    REQUIRE( fp_match(Hfactor * CppTransport_u3[transport::FLATTEN(i,j,k)], static_cast<DataType>(PyTransport_u3[i + j*2*Nf + k*2*Nf*2*Nf])) );
                  }
              }
          }

      }
  
  }
