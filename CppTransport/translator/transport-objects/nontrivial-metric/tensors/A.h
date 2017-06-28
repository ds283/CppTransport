//
// Created by David Seery on 20/12/2015.
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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_A_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_A_H


#include <memory>

#include "concepts/tensors/A.h"
#include "shared/shared_resources.h"
#include "shared/variance_tensor_cache.h"
#include "nontrivial-metric/resources.h"
#include "nontrivial-metric/metric_resource.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace nontrivial_metric
  {
    
    class A : public ::A
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        A(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
          boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t);

        //! destructor is default
        virtual ~A() = default;


        // INTERFACE -- IMPLEMENTS AN 'A' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        std::unique_ptr<flattened_tensor>
        compute(const index_literal_list& indices, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3,
                GiNaC::symbol& a) override;

        //! evaluate component of tensor
        GiNaC::ex
        compute_component(field_index i, field_index j, field_index k,
                          GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! evaluate lambda for tensor
        std::unique_ptr<atomic_lambda>
        compute_lambda(const index_literal& i, const index_literal& j, const index_literal& k,
                       GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        unroll_behaviour get_unroll(const index_literal_list& idx_list) override;


        // INTERFACE -- JANITORIAL API

        //! cache resources required for evaluation on an explicit index assignment
        void pre_explicit(const index_literal_list& indices) override;

        //! cache resources required for evaluation on a lambda
        void pre_lambda();

        //! release resources
        void post() override;


        // INTERNAL API

      private:

        //! underlying symbolic expression
        GiNaC::ex expr(const GiNaC::ex& delta_ij, const GiNaC::ex& delta_jk, const GiNaC::ex& delta_ik,
                       const GiNaC::ex& Vijk, const GiNaC::ex& Vij, const GiNaC::ex& Vjk, const GiNaC::ex& Vik,
                       const GiNaC::ex& Vi, const GiNaC::ex& Vj, const GiNaC::ex& Vk, const GiNaC::ex& A2_ij,
                       const GiNaC::ex& A2_jk, const GiNaC::ex& A2_ik, const GiNaC::ex& A3_ijk,
                       const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const GiNaC::ex& deriv_k,
                       const GiNaC::symbol& k1, const GiNaC::symbol& k2, const GiNaC::symbol& k3, const GiNaC::symbol& a);


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to expression cache
        expression_cache& cache;

        //! reference to resource object
        resources& res;

        //! reference to shared resource object
        shared_resources& shared;


        // AGENTS

        //! index flattener
        index_flatten& fl;

        //! index introspection
        index_traits& traits;


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE AND CACHE

        //! list of momenta
        deriv_cache derivs;

        //! flattened dV tensor
        dV_cache dV;

        //! flattened ddV tensor
        ddV_cache ddV;

        //! flattened dddV tensor
        dddV_cache dddV;
        
        //! flattened Riemann A2 tensor
        Riemann_A2_cache A2;
        
        //! flattened Riemann A3 tensor
        Riemann_A3_cache A3;
        
        //! generator for metric components
        metric_resource G;

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! epsilon
        GiNaC::ex eps;

        //! Planck mass
        GiNaC::symbol Mp;

        //! cache status
        bool cached;

      };

  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_A_H
