//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_B_H
#define CPPTRANSPORT_CANONICAL_B_H


#include <memory>

#include "concepts/B.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace canonical
  {

    class canonical_B: public B
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_B(language_printer& p, expression_cache& c, resources& r, shared_resources& s,
                     boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
          : B(),
            printer(p),
            cache(c),
            res(r),
            shared(s),
            fl(f),
            traits(t),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_B() = default;


        // INTERFACE -- IMPLEMENTS A 'B' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! evaluate component of tensor
        virtual GiNaC::ex compute_component(field_index i, field_index j, field_index k,
                                            GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! invalidate cache
        virtual void reset_cache() override { this->cached = false; }


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual enum unroll_behaviour get_unroll() override;


        // INTERNAL API

      private:

        //! populate workspace
        void populate_cache();

        //! underlying symbolic expression
        GiNaC::ex expr(GiNaC::idx& i, GiNaC::idx& j, GiNaC::idx& k,
                       GiNaC::ex& Vi, GiNaC::ex& Vj, GiNaC::ex& Vk,
                       GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j, GiNaC::symbol& deriv_k,
                       GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a);


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

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

        //! list of momentum symbols
        std::unique_ptr<symbol_list> derivs;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! epsilon
        GiNaC::ex eps;

        //! Planck mass
        GiNaC::symbol Mp;

        //! cache status
        bool cached;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_B_H
