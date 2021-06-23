//
// Created by David Seery on 31/03/2015.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_LINE_VALUES_H
#define CPPTRANSPORT_MESSAGES_EN_LINE_VALUES_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_FIELD                = "field value";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_MOMENTUM             = "momentum value";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_CORRELATION_FUNCTION = "correlation function";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_FNL                  = "fNL";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_R                    = "tensor-to-scalar ratio";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_SPECTRAL_INDEX       = "spectral index";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_MASS                 = "mass scale";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_SECONDS              = "seconds";
    constexpr auto CPPTRANSPORT_VALUE_NON_LATEX_STEPS                = "steps";

    constexpr auto CPPTRANSPORT_VALUE_LATEX_FIELD                    = "field value";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_MOMENTUM                 = "momentum value";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_CORRELATION_FUNCTION     = "correlation function";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_FNL                      = "$f_{\\mathrm{NL}}$";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_R                        = "tensor-to-scalar ratio";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_SPECTRAL_INDEX           = "spectral index";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_MASS                     = "mass scale";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_SECONDS                  = "seconds";
    constexpr auto CPPTRANSPORT_VALUE_LATEX_STEPS                    = "steps";

    constexpr auto CPPTRANSPORT_VALUE_UNKNOWN_TYPE                   = "lines_values: unknown value type";

  }   // namespace transport


#endif // CPPTRANSPORT_MESSAGES_EN_LINE_VALUES_H
