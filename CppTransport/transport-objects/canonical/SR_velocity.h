//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_SR_VELOCITY_H
#define CPPTRANSPORT_CANONICAL_SR_VELOCITY_H


#include <memory>

#include "concepts/SR_velocity.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {

    class canonical_SR_velocity: public SR_velocity
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_SR_velocity(language_printer& p, expression_cache& c, resources& r, shared_resources& s,
                    boost::timer::cpu_timer& tm, index_flatten& f)
          : SR_velocity(),
            printer(p),
            cache(c),
            res(r),
            shared(s),
            fl(f),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_SR_velocity() = default;


        // INTERFACE -- IMPLEMENTS A 'dV' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute() override;

        //! evaluate a component of the tensor
        virtual GiNaC::ex compute_component(field_index i) override;

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

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! Planck mass
        GiNaC::symbol Mp;

        //! cache status
        bool cached;

      };

  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_SR_VELOCITY_H
