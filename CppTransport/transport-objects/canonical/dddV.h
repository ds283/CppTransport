//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
        canonical_dddV(language_printer& p, resources& r, index_flatten& f)
          : dddV(),
            printer(p),
            res(r),
            fl(f)
          {
          }

        //! destructor is default
        virtual ~canonical_dddV() = default;


        // INTERFACE -- IMPLEMENTS A 'dV' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute() override;


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to resource object
        resources& res;


        // AGENTS

        //! index flattener
        index_flatten& fl;

      };

  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_DDDV_H
