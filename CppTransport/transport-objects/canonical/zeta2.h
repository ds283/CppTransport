//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_ZETA2_H
#define CPPTRANSPORT_CANONICAL_ZETA2_H


#include <memory>

#include "concepts/zeta2.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace canonical
  {

    class canonical_zeta2: public zeta2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_zeta2(language_printer& p, expression_cache& c, resources& r, shared_resources& s,
                        boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
          : zeta2(),
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
        virtual ~canonical_zeta2() = default;


        // INTERFACE -- IMPLEMENTS A 'zeta2' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k, GiNaC::symbol& k1,
                                                          GiNaC::symbol& k2, GiNaC::symbol& a) override;

        //! evaluate component of tensor
        virtual GiNaC::ex compute_component(phase_index i, phase_index j,
                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a) override;

        //! invalidate cache
        virtual void reset_cache() override { this->cached = false; }


        // INTERNAL API

      private:

        //! populate workspace
        void populate_cache();

        //! compute field-field entry
        GiNaC::ex compute_field_field(field_index i, field_index j,
                                      GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a);

        //! compute field-momentum or momentum-field entry;
        //! field index is i, momentum index is j
        //! likewise, corresponding momenta are k1, k2 respectively
        GiNaC::ex compute_field_momentum(field_index i, field_index j,
                                         GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a);


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

        //! quantity p
        GiNaC::ex p;

        //! cache status
        bool cached;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_ZETA2_H
