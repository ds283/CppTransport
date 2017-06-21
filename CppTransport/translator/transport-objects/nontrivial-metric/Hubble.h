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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_HUBBLE_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_HUBBLE_H


#include "concepts/Hubble.h"
#include "utilities/shared_resources.h"
#include "nontrivial-metric/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace nontrivial_metric
  {
    
    class nontrivial_metric_Hubble : public ::Hubble
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        nontrivial_metric_Hubble(language_printer& p, cse& cw, resources& r)
          : ::Hubble(),
            printer(p),
            cse_worker(cw),
            res(r)
          {
          }

        //! destructor is default
        virtual ~nontrivial_metric_Hubble() = default;


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

  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_HUBBLE_H
