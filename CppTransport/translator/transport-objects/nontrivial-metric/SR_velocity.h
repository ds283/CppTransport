//
// Created by David Seery on 19/12/2015.
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

#ifndef CPPTRANSPORT_NONCANONICAL_SR_VELOCITY_H
#define CPPTRANSPORT_NONCANONICAL_SR_VELOCITY_H


#include <memory>

#include "concepts/SR_velocity.h"
#include "utilities/shared_resources.h"
#include "nontrivial_metric/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace nontrivial_metric
  {

    class canonical_SR_velocity: public SR_velocity
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_SR_velocity(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                    boost::timer::cpu_timer& tm, index_flatten& f)
          : SR_velocity(),
            printer(p),
            cse_worker(cw),
            cache(c),
            res(r),
            shared(s),
            fl(f),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_SR_velocity() = default;


        // INTERFACE -- IMPLEMENTS A 'SR_velocity' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute() override;

        //! evaluate a component of the tensor
        virtual GiNaC::ex compute_component(field_index i) override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i) override;

        //! invalidate cache
        virtual void reset_cache() override { this->cached = false; }


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual unroll_behaviour get_unroll() override;


        // INTERNAL API

      private:

        //! cache symbols
        void cache_symbols();

        //! populate workspace
        void populate_workspace();

        //! underlying symbolic expression
        GiNaC::ex expr(GiNaC::ex& Vi);



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


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE AND CACHE

        //! potential
        GiNaC::ex V;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! flattened ddV tensor
        std::unique_ptr<flattened_tensor> ddV;

        //! Planck mass
        GiNaC::symbol Mp;

        //! cache status
        bool cached;

      };

  }   // namespace nontrivial_metric



#endif //CPPTRANSPORT_CANONICAL_SR_VELOCITY_H
