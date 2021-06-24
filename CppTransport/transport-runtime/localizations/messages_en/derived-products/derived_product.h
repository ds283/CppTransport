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


#define CPPTRANSPORT_PRODUCT_DATALINE_AXIS_MISMATCH  "Axis and sample size mismatch in 'time_data_line'"

#define CPPTRANSPORT_PRODUCT_UNKNOWN_TYPE            "Unknown derived-product type"

#define CPPTRANSPORT_INDEX_UNKNOWN_RANGE_TYPE        "Unknown 'index_selector' range type"
#define CPPTRANSPORT_INDEX_PRESET_MISMATCH           "Mismatched number of presets provided to 'index_selector'; expected"
#define CPPTRANSPORT_INDEX_PRESET_MISMATCH_A         "received"

#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH          "Unexpected number of fields in index selector object"
#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A        "received"
#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B        "expected"

#define CPPTRANSPORT_PRODUCT_INDEX_BOUNDS            "Out-of-bounds index value"

#define CPPTRANSPORT_DERIVED_PRODUCT_FAILED          "Error: I/O error; failed to generate derived product"

#define CPPTRANSPORT_DERIVED_PRODUCT_FILENAME        "Filename"

#define CPPTRANSPORT_DERIVED_PRODUCT_LINE_2D_PLOT    "2d plot"
#define CPPTRANSPORT_DERIVED_PRODUCT_LINE_TABLE      "table"

#define CPPTRANSPORT_PRODUCT_CANT_BUILD_SPLINE_A     "Could not build successful spline for line with label"
#define CPPTRANSPORT_PRODUCT_CANT_BUILD_SPLINE_B     "(plot shows original line)"

#endif // CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H
