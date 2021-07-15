//
// Created by David Seery on 14/05/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H
#define CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H


namespace transport
  {

    namespace derived_data
      {

        constexpr auto CPPTRANSPORT_PRODUCT_DATALINE_AXIS_MISMATCH    = "Axis and sample size mismatch in data_line<>";
        constexpr auto CPPTRANSPORT_PRODUCT_DATALINE_UNEXPECTED_STORE = "Unexpected data store type in data_line<>";

        constexpr auto CPPTRANSPORT_PRODUCT_UNKNOWN_TYPE              = "Unknown derived-product type";

        constexpr auto CPPTRANSPORT_INDEX_UNKNOWN_RANGE_TYPE          = "Unknown 'index_selector' range type";
        constexpr auto CPPTRANSPORT_INDEX_PRESET_MISMATCH             = "Mismatched number of presets provided to 'index_selector'; expected";
        constexpr auto CPPTRANSPORT_INDEX_PRESET_MISMATCH_A           = "received";

        constexpr auto CPPTRANSPORT_PRODUCT_INDEX_MISMATCH            = "Unexpected number of fields in index selector object";
        constexpr auto CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A          = "received";
        constexpr auto CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B          = "expected";

        constexpr auto CPPTRANSPORT_PRODUCT_INDEX_BOUNDS              = "Out-of-bounds index value";

        constexpr auto CPPTRANSPORT_DERIVED_PRODUCT_FAILED            = "Error: I/O error; failed to generate derived product";

        constexpr auto CPPTRANSPORT_DERIVED_PRODUCT_FILENAME          = "Filename";

        constexpr auto CPPTRANSPORT_DERIVED_PRODUCT_LINE_2D_PLOT      = "2d plot";
        constexpr auto CPPTRANSPORT_DERIVED_PRODUCT_LINE_TABLE        = "table";

        constexpr auto CPPTRANSPORT_PRODUCT_CANT_BUILD_SPLINE_A       = "Could not build successful spline for line with label";
        constexpr auto CPPTRANSPORT_PRODUCT_CANT_BUILD_SPLINE_B       = "(plot shows original line)";

      }   // namespace derived_data

  }   // namespace transport


#endif // CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H
