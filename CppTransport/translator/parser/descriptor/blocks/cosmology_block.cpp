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

#include <algorithm>
#include <cctype>
#include <string>
#include "cosmology_block.h"

bool cosmology_block::set_template(std::string &tmplt) {

  // Transform the template string into all lower case so that way 'Planck' = 'planck' to avoid any errors
  std::transform(tmplt.begin(), tmplt.end(), tmplt.begin(),
                 [](unsigned char c){ return std::tolower(c); });

  // All data comes from the TT, TE, EE + LowE + lensing 68% limits for the respective releases
  if (tmplt == "planck 2018" || tmplt == "planck2018" || tmplt == "planck18")
  {
    this->Template = "Planck 2018";
    this->H0 = 67.36;
    this->omega_bh2 = 0.02237;
    this->omega_cdmh2 = 0.1200;
    this->tau = 0.0544;
    return true;
  }
  else if (tmplt == "planck 2015" || tmplt == "planck2015" || tmplt == "planck15")
  {
    this->Template = "Planck 2015";
    this->H0 = 67.51;
    this->omega_bh2 = 0.02226;
    this->omega_cdmh2 = 0.1193;
    this->tau = 0.063;
    return true;
  }
  else if (tmplt == "planck 2013" || tmplt == "planck2013" || tmplt == "planck13")
  {
    this->Template = "Planck 2013";
    this->H0 = 67.9;
    this->omega_bh2 = 0.02218;
    this->omega_cdmh2 = 0.1186;
    this->tau = 0.090;
    return true;
  }
  else if (tmplt == "wmap 9" || tmplt == "wmap9")
  {
    this->Template = "WMAP 9";
    this->H0 = 70.0;
    this->omega_bh2 = 0.02264;
    this->omega_cdmh2 = 0.1138;
    this->tau = 0.089;
    return true;
  }

  // If we get here, the we haven't found the specified template. Throw an exception for the template provided
  // and default back to Planck 2018 values.
  throw std::exception();
}

const std::string &cosmology_block::get_template() {
  return this->Template;
}


double cosmology_block::get_H0() const {
  return this->H0;
}

void cosmology_block::set_H0(double H_0) {
  this->H0 = H_0;
}


double cosmology_block::get_OmegaBh2() const {
  return this->omega_bh2;
}

void cosmology_block::set_OmegaBh2(double omegaB) {
  this->omega_bh2 = omegaB;
}


double cosmology_block::get_OmegaCDMh2() const {
  return this->omega_cdmh2;
}

void cosmology_block::set_OmegaCDM(double omegaCDM) {
  this->omega_cdmh2 = omegaCDM;
}


double cosmology_block::get_tau() const {
  return this->tau;
}

void cosmology_block::set_tau(double Tau) {
  this->tau = Tau;
}


double cosmology_block::get_Tcmb() const {
  return this->Tcmb;
}

void cosmology_block::set_Tcmb(double TCMB) {
  this->Tcmb = TCMB;
}
