//
// Created by David Seery on 26/05/2014.
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



#ifndef CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H
#define CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_CONTENT_TYPE_UNKNOWN               = "Internal error: Cannot deserialize unknown 'derived_line' content type";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN                 = "Internal error: Unknown twopf type in 'derived_line'";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_PARENT_TASK                     = "Internal error: Cannot deserialize 'derived_line' content because no parent task was specified";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_BAD_PARENT_TASK_TYPE               = "Internal error: Bad parent task type for 'derived_line' content";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_DATAPIPE_ATTACH_BAD_TAG            = "Datapipe was not attached because the specified parent task could not be found";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_LABEL                              = "Derived content:";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME                          = "Derived from task";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME                         = "model";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_CLASS                       = "axis data represent";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_TIME_LABEL                   = "time";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_WAVENUMBER_LABEL             = "k-value";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_THREEPF_CONFIGURATION_LABEL  = "threepf configuration";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_SUPPORTED                   = "supported x-axis values:";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS                     = "current x-axis value:";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS                = "e-folds";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K                     = "k-value";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS_EXIT           = "efolds from horizone exit";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_ALPHA                 = "alpha";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_BETA                  = "beta";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K1          = "k1/kt";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K2          = "k2/kt";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K3          = "k3/kt";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING                        = "correlation functions involve";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE                     = "field derivatives";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA                        = "momenta";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING                     = "wavenumber normalization is";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING                    = "comoving";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL                = "conventional";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_CLASS               = "derived_line error: unknown x-axis class";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE               = "derived_line error: unknown x-axis value";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN                   = "derived_line error: unknown dot meaning";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN                = "derived_line error: unknown klabel meaning";

    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_AXIS_TYPES                      = "derived_line error: no supported axis types";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_NOT_SUPPORTED               = "derived_line error: unsupported x-axis type";
    constexpr auto CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNSUPPORTED_X_AXIS_TYPE            = "derived_line error: attempt to set x-axis to unsupported type";

  }   // namespace transport


#endif // CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H
