//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_DV_H
#define CPPTRANSPORT_CANONICAL_DV_H


#include <memory>

#include "concepts/dV.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {

    class canonical_dV: public dV
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_dV(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
          : dV(),
            printer(p),
            cse_worker(cw),
            res(r),
            shared(s),
            fl(f)
          {
          }

        //! destructor is default
        virtual ~canonical_dV() = default;


        // INTERFACE -- IMPLEMENTS A 'dV' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute() override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i) override;


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual enum unroll_behaviour get_unroll() override;


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to shaed resource object
        shared_resources& shared;

        //! reference to resource object
        resources& res;


        // AGENTS

        //! index flattener
        index_flatten& fl;

      };

  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_DV_H
