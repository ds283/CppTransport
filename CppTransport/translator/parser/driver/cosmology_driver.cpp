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

#include "cosmology_driver.h"
#include "ginac/ginac.h"

namespace y {

  cosmology_driver::cosmology_driver(model_descriptor &sc, symbol_factory &sf, argument_cache &ac,
                                     local_environment &le)
   :root(sc),
    sym_factory(sf),
    cache(ac),
    env(le)
    {
    }


  void cosmology_driver::set_template(const std::shared_ptr<lexeme_type>& como_tmplt) {
    auto tmp = como_tmplt->get_string();

    try
    {
      this->root.cosmo.set_template(*tmp);
      // Try setting the template to the string specified
    }

    catch (std::exception& e)
    {
      std::string message =  "The specified template " + *tmp + " does not exist, defaulting to Planck 2018 values";
      como_tmplt->warn(message);
      // If the specified template was not found, catch the exception and raise an issue to the terminal showing this.
    }
  }

  // In the model file, we have specified that the type of variable that H0, tau, etc. can be set to is of type
  // GiNaC expression which allows for much more ways of specifying a value than just a number.
  // Therefore, we need to evaluate the GiNaC expression into double form which can then be stored in the cosmology class

  void cosmology_driver::set_H0(const std::shared_ptr<GiNaC::ex>& H0) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(H0->evalf()).to_double();
    this->root.cosmo.set_H0(tmp);
  }

  // We store the value of H0 as the Hubble constant, and so we need to multiply h0 by 100 to store it correctly
  void cosmology_driver::set_h0(const std::shared_ptr<GiNaC::ex>& h0) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(h0->evalf()).to_double() * 100.0;
    this->root.cosmo.set_H0(tmp);
  }

  // The cosmic densities that CppTransport stores are multiplied by h^2, and so if just Omega_b is specified
  // we have to multiply by h^2 here to set the correct value.
  void cosmology_driver::set_OmegaB(const std::shared_ptr<GiNaC::ex>& OmegaB) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(OmegaB->evalf()).to_double() * (this->root.cosmo.get_H0() / 100.0)
                                                                         * (this->root.cosmo.get_H0() / 100.0);
    this->root.cosmo.set_OmegaBh2(tmp);
  }

  void cosmology_driver::set_OmegaBh2(const std::shared_ptr<GiNaC::ex>& OmegaBh2) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(OmegaBh2->evalf()).to_double();
    this->root.cosmo.set_OmegaBh2(tmp);
  }


  void cosmology_driver::set_OmegaCDM(const std::shared_ptr<GiNaC::ex>& OmegaCDM) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(OmegaCDM->evalf()).to_double() * (this->root.cosmo.get_H0() / 100.0)
                                                                           * (this->root.cosmo.get_H0() / 100.0);
    this->root.cosmo.set_OmegaCDM(tmp);
  }

  void cosmology_driver::set_OmegaCDMh2(const std::shared_ptr<GiNaC::ex>& OmegaCDMh2) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(OmegaCDMh2->evalf()).to_double();
    this->root.cosmo.set_OmegaCDM(tmp);
  }


  void cosmology_driver::set_tau(const std::shared_ptr<GiNaC::ex>& Tau) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(Tau->evalf()).to_double();
    this->root.cosmo.set_tau(tmp);
  }


  void cosmology_driver::set_Tcmb(const std::shared_ptr<GiNaC::ex>& Tcmb) {
    auto tmp = GiNaC::ex_to<GiNaC::numeric>(Tcmb->evalf()).to_double();
    this->root.cosmo.set_Tcmb(tmp);
  }

} // namespace y
