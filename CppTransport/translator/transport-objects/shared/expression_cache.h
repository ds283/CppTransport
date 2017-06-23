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

#ifndef CPPTRANSPORT_EXPRESSION_CACHE_H
#define CPPTRANSPORT_EXPRESSION_CACHE_H


#include "ginac_cache.h"


enum class expression_item_types
  {
    sr_U_item, sr_U_lambda,
    U1_item, U1_lambda,
    U2_item, U2_lambda,
    U3_item, U3_lambda,
    A_item, A_lambda,
    Atilde_item, Atilde_lambda,
    B_item, B_lambda,
    C_item, C_lambda,
    M_item, M_lambda,
    zxfm1_item, zxfm1_lambda,
    zxfm2_item, zxfm2_lambda,
    dN1_item,
    dN2_item,
    V_item,
    dV_item, dV_lambda,
    ddV_item, ddV_lambda,
    dddV_item, dddV_lambda,
    Hubble2_item,
    epsilon_item,
    parameters_lambda, coordinates_lambda, fields_lambda
  };


typedef ginac_cache<expression_item_types> expression_cache;


#endif //CPPTRANSPORT_EXPRESSION_CACHE_H
