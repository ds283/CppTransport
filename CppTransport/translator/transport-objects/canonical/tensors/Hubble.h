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

#ifndef CPPTRANSPORT_CANONICAL_HUBBLE_H
#define CPPTRANSPORT_CANONICAL_HUBBLE_H


#include "concepts/tensors/Hubble.h"
#include "shared/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {
    
    class Hubble : public ::Hubble
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        Hubble(language_printer& p, cse& cw, resources& r)
          : ::Hubble(),
            printer(p),
            cse_worker(cw),
            res(r)
          {
          }

        //! destructor is default
        virtual ~Hubble() = default;


        // INTERFACE -- IMPLEMENTS A 'Hubble' TENSOR CONCEPT

      public:

        //! evaluate potential
        GiNaC::ex compute_V() override;

        //! evaluate Hubble parameter
        GiNaC::ex compute_Hsq() override;

        //! evaluate epsilon parameter
        GiNaC::ex compute_eps() override;

        //! evaluate eta parameter
        GiNaC::ex compute_eta() override;


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
