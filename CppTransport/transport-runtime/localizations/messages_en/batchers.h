//
// Created by David Seery on 03/11/2020.
// --@@
// Copyright (c) 2020 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_MESSAGES_EN_BATCHERS_H
#define CPPTRANSPORT_MESSAGES_EN_BATCHERS_H

constexpr auto CPPTRANSPORT_BATCHER_BACKG_SIZE           = "Internal error: background fields sample item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_ICS_SIZE             = "Internal error: per-configuration initial conditions item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_TWOPF_SIZE           = "Internal error: fields 2pf sample item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_TWOPF_SI_SIZE        = "Internal error: fields 2pf 'spectral index' sample item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_THREEPF_SIZE         = "Internal error: fields 3pf sample item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_TENSOR_TWOPF_SIZE    = "Internal error: tensor 2pf sample item has unexpected size";
constexpr auto CPPTRANSPORT_BATCHER_TENSOR_TWOPF_SI_SIZE = "Internal error: tensor 2pf 'spectral index' sample item has unexpected size";
constexpr auto CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM1_SIZE  = "Internal error: gauge_xfm1 sample item has unexpected size";
constexpr auto CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM2_SIZE  = "Internal error: gauge_xfm2 sample item has unexpected size";

#endif //CPPTRANSPORT_MESSAGES_EN_BATCHERS_H
