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

#include "cosmology.h"

transport::cosmology::cosmology() = default;

const std::string &transport::cosmology::get_template() const {
  return this->Template;
}

void transport::cosmology::set_template(const std::string &tmplt){
  this->Template = tmplt;
}


double transport::cosmology::get_H0() const {
  return this->H0;
}

void transport::cosmology::set_H0(double H_0) {
  this->H0 = H_0;
}


double transport::cosmology::get_Omega_Bh2() const {
  return this->Omega_Bh2;
}

void transport::cosmology::set_Omega_Bh2(double omegaBh2) {
  this->Omega_Bh2 = omegaBh2;
}


double transport::cosmology::get_Omega_CDMh2() const {
  return this->Omega_CDMh2;
}

void transport::cosmology::set_Omega_CDMh2(double omegaCDMh2) {
  this->Omega_CDMh2 = omegaCDMh2;
}


double transport::cosmology::get_tau() const {
  return this->tau;
}

void transport::cosmology::set_tau(double Tau) {
  this->tau = Tau;
}
