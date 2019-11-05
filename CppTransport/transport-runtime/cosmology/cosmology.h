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

#ifndef CPPTRANSPORT_COSMOLOGY_H
#define CPPTRANSPORT_COSMOLOGY_H

#include <string>

namespace transport {

  class cosmology {
    public:
      cosmology();

      ~cosmology() = default;

  public:
    const std::string &get_template() const;

    void set_template(const std::string &tmplt);


    double get_H0() const;

    void set_H0(double H_0);


    double get_Omega_Bh2() const;

    void set_Omega_Bh2(double omegaBh2);


    double get_Omega_CDMh2() const;

    void set_Omega_CDMh2(double omegaCDMh2);


    double get_tau() const;

    void set_tau(double Tau);

  private:
    std::string Template;

    // Hubble constant in km/s Mpc^-1
    double H0 = 0;

    // The baryon density today multiplied by h^2
    double Omega_Bh2 = 0;

    // The cold dark matter density today multiplied by h^2
    double Omega_CDMh2 = 0;

    // Thomson scattering optical depth due to reionization
    double tau = 0;

    // Current temperature of the CMB
    double T_CMB = 0;

    int ell_max = 2500;
  };


}

#endif //CPPTRANSPORT_COSMOLOGY_H
