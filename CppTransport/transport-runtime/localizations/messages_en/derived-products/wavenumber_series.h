//
// Created by David Seery on 07/06/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_WAVENUMBER_SERIES_PRODUCT_H
#define CPPTRANSPORT_MESSAGES_EN_WAVENUMBER_SERIES_PRODUCT_H


#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_BACKGROUND         "background line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TWOPF              "twopf line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_THREEPF            "threepf line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TENSOR_TWOPF       "tensor twopf line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_TWOPF         "zeta twopf line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_THREEPF       "zeta threepf line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_REDUCED_BISPECTRUM "reduced bispectrum line group"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_R                  "tensor-to-scalar ratio line group"

#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_KCONFIG_SN_LABEL         "k-configurations: including serial numbers"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_TSAMPLE_SN_LABEL         "time sample: including serial numbers"

#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_DATAPIPE            "Internal error: Null datapipe in wavenumber_series or derived class"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_MODEL               "Internal error: Null model in wavenumber_series or derived class"
#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_READER              "Internal error: Null serialization reader in wavenumber_series or derived class"

#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_EMPTY_FILTER             "k-configuration series filtering produced no results when constructing derived line from task"

#define CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_UNKNOWN_OPPOS            "Internal error: Unknown operator position for three-point function shift"


#endif // CPPTRANSPORT_MESSAGES_EN_WAVENUMBER_SERIES_PRODUCT_H
