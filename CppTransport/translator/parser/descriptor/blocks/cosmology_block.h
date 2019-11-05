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

#ifndef CPPTRANSPORT_COSMOLOGY_BLOCK_H
#define CPPTRANSPORT_COSMOLOGY_BLOCK_H

#include <string>
#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"

#include "symbol_tables.h"
#include "declarations.h"
#include "symbol_factory.h"
#include "symbol_list.h"
#include "version_policy.h"
#include "disable_warnings.h"


class cosmology_block {

  // cosmology_block takes no constructor arguments, and so we can get away with the default constructor & destructor
  public:
    cosmology_block() = default;

    virtual ~cosmology_block() = default;

  // Getters and setters for the cosmological parameters
  public:
    const std::string &get_template();

    bool set_template(std::string &tmplt);


    double get_H0() const;

    void set_H0(double H_0);


    double get_OmegaBh2() const;

    void set_OmegaBh2(double omegaB);


    double get_OmegaCDMh2() const;

    void set_OmegaCDM(double omegaCDM);


    double get_tau() const;

    void set_tau(double Tau);


    double get_Tcmb() const;

    void set_Tcmb(double TCMB);

  // Internal cosmological data -- defaults to Planck 2018 if nothing's specified
  // Data comes from the TT, TE, EE + LowE + lensing 68% limits
  private:
    // string that represents the starting template for the model.
    std::string Template = "Planck 2018";

    // Hubble constant in km/s Mpc^-1
    double H0 = 67.36;

    // The baryon density today multiplied by h^2
    double omega_bh2 = 0.02237;

    // The cold dark matter density today multiplied by h^2
    double omega_cdmh2 = 0.1200;

    // Thomson scattering optical depth due to reionization
    double tau = 0.0544;

    // Temperature of the CMB now
    double Tcmb = 2.7260;

    int ell_max = 2500;

};


#endif //CPPTRANSPORT_COSMOLOGY_BLOCK_H
