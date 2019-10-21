//
// Created by Alessandro Maraio on 20/10/2019.
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

#include "cosmology_macros.h"
#include "translation_unit.h"

#include <string>

#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }

namespace macro_packages
{
  cosmology_macros::cosmology_macros(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
    : replacement_rule_package(f, cw, lm, p, prn)
  {
    // Define the rules of which functions to run with which macros
    EMPLACE(pre_package, BIND(replace_template, "COSMO_TEMPLATE"))
    EMPLACE(pre_package, BIND(replace_hubble, "COSMO_H0"))
    EMPLACE(pre_package, BIND(replace_omegab, "COSMO_OMEGA_B"))
    EMPLACE(pre_package, BIND(replace_omegacdm, "COSMO_OMEGA_CDM"))
    EMPLACE(pre_package, BIND(replace_tau, "COSMO_TAU"))
    EMPLACE(pre_package, BIND(replace_tcmb, "COSMO_TCMB"))
  }

  // *******************************************************************

  // Replacement rules for the cosmology macros

  std::string replace_template::evaluate(const macro_argument_list& args)
  {
    return "\"" + this->data_payload.cosmo.get_template() + "\"";
  }

  std::string replace_hubble::evaluate(const macro_argument_list& args)
  {
    return std::to_string(this->data_payload.cosmo.get_H0());
  }

  std::string replace_omegab::evaluate(const macro_argument_list& args)
  {
    return std::to_string(this->data_payload.cosmo.get_OmegaBh2());
  }

  std::string replace_omegacdm::evaluate(const macro_argument_list& args)
  {
    return std::to_string(this->data_payload.cosmo.get_OmegaCDMh2());
  }

  std::string replace_tau::evaluate(const macro_argument_list& args)
  {
    return std::to_string(this->data_payload.cosmo.get_tau());
  }

  std::string replace_tcmb::evaluate(const macro_argument_list& args)
  {
    return std::to_string(this->data_payload.cosmo.get_Tcmb());
  }


} // namespace macro_packages
