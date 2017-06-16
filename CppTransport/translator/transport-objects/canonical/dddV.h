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

#ifndef CPPTRANSPORT_CANONICAL_DDDV_H
#define CPPTRANSPORT_CANONICAL_DDDV_H


#include <memory>

#include "concepts/dddV.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {

    class canonical_dddV: public dddV
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_dddV(language_printer& p, cse& cw, resources& r, shared_resources& s, index_flatten& f)
          : dddV(),
            printer(p),
            cse_worker(cw),
            res(r),
            shared(s),
            fl(f)
          {
          }

        //! destructor is default
        virtual ~canonical_dddV() = default;


        // INTERFACE -- IMPLEMENTS A 'dV' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(const index_literal_list& indices) override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<atomic_lambda> compute_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k) override;


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual unroll_behaviour get_unroll() override;


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to shared resource object
        shared_resources& shared;

        //! reference to resource object
        resources& res;


        // AGENTS

        //! index flattener
        index_flatten& fl;

      };

  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_DDDV_H
