//
// Created by David Seery on 12/06/2013.
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

#ifndef CPPTRANSPORT_CORE_H
#define CPPTRANSPORT_CORE_H

#include <string>

#include "msg_en.h"

constexpr auto CPPTRANSPORT_NAME                     = "CppTransport";
constexpr auto CPPTRANSPORT_VERSION                  = "2016.1 RC2";
constexpr auto CPPTRANSPORT_NUMERIC_VERSION          = 201601;
constexpr auto CPPTRANSPORT_COPYRIGHT                = "(c) University of Sussex 2016";

constexpr auto TEMPLATE_TAG_SUFFIX                   = "_";

constexpr auto NEWLINE_CHAR                          = "\n";

constexpr auto INVALID_GUARD_CHARACTERS              = "-/\\|@£$%^&*()`~+=[]{}:;<>,§±";

constexpr auto MPLANCK_SYMBOL                        = "__Mp";
constexpr auto MPLANCK_LATEX_SYMBOL                  = "M_{\\mathrm{P}}";
constexpr auto MPLANCK_TEXT_NAME                     = "M_P";

constexpr auto HSQ_SYMBOL_NAME                       = "__InternalHsq";
constexpr auto EPS_SYMBOL_NAME                       = "__InternalEps";
constexpr auto V_SYMBOL_NAME                         = "__InternalV";
constexpr auto ZETA2_P_SYMBOL_NAME                   = "__InternalZ2p";

constexpr auto DEFAULT_RECURSION_DEPTH               = (127);
constexpr auto DEFAULT_MODEL_NAME                    = "UNKNOWN_MODEL";

constexpr auto CPPTRANSPORT_PATH_ENV                 = "CPPTRANSPORT_PATH";
constexpr auto CPPTRANSPORT_TEMPLATE_PATH            = "templates";
constexpr auto HOME_ENV                              = "HOME";

constexpr auto CONFIG_FILE_LOCATION                  = ".cpptransport";

constexpr auto DEFAULT_UNROLL_MAX                    = 1000;


// macro strings

constexpr auto BACKEND_MACRO_PREFIX                  = "$";
constexpr auto BACKEND_LINE_SPLIT_EQUAL              = "$=";
constexpr auto BACKEND_LINE_SPLIT_SUM_EQUAL          = "$+=";


// output strings

constexpr auto OUTPUT_CPPTRANSPORT_TAG               = "CPPTRANSPORT";
constexpr auto OUTPUT_TIME_FORMAT                    = "%X on %d %m %Y";
constexpr auto OUTPUT_DEFAULT_POOL_TEMPLATE          = "auto $1 = $2;";

constexpr auto OUTPUT_DEFAULT_CSE_TEMPORARY_NAME     = "__t";
constexpr auto OUTPUT_DEFAULT_LAMBDA_TEMPORARY_NAME  = "__l";

constexpr auto OUTPUT_TEMPORARY_POOL_START           = "BEGIN TEMPORARY POOL";
constexpr auto OUTPUT_TEMPORARY_POOL_END             = "END TEMPORARY POOL";
constexpr auto OUTPUT_TEMPORARY_POOL_SEQUENCE        = "sequence";
constexpr auto OUTPUT_KERNEL_LOCATION                = "VEXCL KERNEL INSERTION";

constexpr auto OUTPUT_VEXCL_KERNEL_PRE               = ", \"";
constexpr auto OUTPUT_VEXCL_KERNEL_POST              = "\"";

constexpr auto OUTPUT_VEXCL_KERNEL_LINE_PRE          = "\"";
constexpr auto OUTPUT_VEXCL_KERNEL_LINE_POST         = "\\n\"";

constexpr auto OUTPUT_OPENCL_QUALIFIER               = "global";

#endif //CPPTRANSPORT_CORE_H
