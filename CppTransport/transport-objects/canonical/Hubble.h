//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_HUBBLE_H
#define CPPTRANSPORT_CANONICAL_HUBBLE_H


#include "concepts/Hubble.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {

    class canonical_Hubble: public Hubble
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_Hubble(language_printer& p, cse& cw, resources& r)
          : Hubble(),
            printer(p),
            cse_worker(cw),
            res(r)
          {
          }

        //! destructor is default
        virtual ~canonical_Hubble() = default;


        // INTERFACE -- IMPLEMENTS A 'Hubble' TENSOR CONCEPT

      public:

        //! evaluate potential
        virtual GiNaC::ex compute_V() override;

        //! evaluate Hubble parameter
        virtual GiNaC::ex compute_Hsq() override;

        //! evaluate epsilon parameter
        virtual GiNaC::ex compute_eps() override;


        // INTERNAL DATA

      private:

        // AGENTS AND CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to resource object
        resources& res;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_HUBBLE_H
