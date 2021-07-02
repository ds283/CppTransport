//
// Created by David Seery on 01/07/2021.
// --@@
// Copyright (c) 2021 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_MESSAGES_EN_TENSOR_TWOPF_SPECTRAL_LINE_PRODUCT_H
#define CPPTRANSPORT_MESSAGES_EN_TENSOR_TWOPF_SPECTRAL_LINE_PRODUCT_H


namespace transport
  {

    namespace derived_data
      {

        constexpr auto CPPTRANSPORT_PRODUCT_TENSOR_NT_LINE_NO_SPECTRAL_DATA = "tensor_nt_line error: provided task does not collect spectral data for 2pf";

      }   // namespace derived_data

  }   // namespace transport

#endif //CPPTRANSPORT_MESSAGES_EN_TENSOR_TWOPF_SPECTRAL_LINE_PRODUCT_H
