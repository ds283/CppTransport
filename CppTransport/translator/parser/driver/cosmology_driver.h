//
// Created by Alessandro Maraio on 19/10/2019.
//
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_COSMOLOGY_DRIVER_H
#define CPPTRANSPORT_COSMOLOGY_DRIVER_H

#include <string>
#include <memory>

#include "semantic_values.h"
#include "model_descriptor.h"
#include "symbol_factory.h"
#include "argument_cache.h"
#include "local_environment.h"
#include "cosmology_block.h"

namespace y {

  // Class that describes the specific cosmology_driver for a model.
  class cosmology_driver {

    // Generic constructor and destructor
    public:
      cosmology_driver(model_descriptor& sc, symbol_factory& sf, argument_cache& ac, local_environment& le);

      virtual ~cosmology_driver() = default;

    // Functions that allow us to read in settings from the model file, and pass them onto the cosmology class to be set
    public:
      void set_template(const std::shared_ptr<lexeme_type>& como_tmplt);

      void set_H0(const std::shared_ptr<GiNaC::ex>& H0);

      void set_h0(const std::shared_ptr<GiNaC::ex>& h0);

      void set_OmegaB(const std::shared_ptr<GiNaC::ex>& OmegaB);

      void set_OmegaBh2(const std::shared_ptr<GiNaC::ex>& OmegaBh2);

      void set_OmegaCDM(const std::shared_ptr<GiNaC::ex>& OmegaCDM);

      void set_OmegaCDMh2(const std::shared_ptr<GiNaC::ex>& OmegaCDMh2);

      void set_tau(const std::shared_ptr<GiNaC::ex>& Tau);

      void set_Tcmb(const std::shared_ptr<GiNaC::ex>& Tcmb);


    // Internal data
    protected:
      //! model description container
      model_descriptor& root;

      //! delegated symbol factory
      symbol_factory& sym_factory;

      //! delegated argument cache
      argument_cache& cache;

      //! delegated local environment
      local_environment& env;
  };

} // namespace y

#endif //CPPTRANSPORT_COSMOLOGY_DRIVER_H
