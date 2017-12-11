//
// Created by David Seery on 05/05/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_INTERGRATION_H
#define CPPTRANSPORT_MESSAGES_EN_INTERGRATION_H


constexpr auto CPPTRANSPORT_OBSERVER_SLOW_INTEGRATION = "Detected slow integration in progress";
constexpr auto CPPTRANSPORT_OBSERVER_UPDATE           = "Progress update";
constexpr auto CPPTRANSPORT_OBSERVER_TIME             = ": current stepper position is N";
constexpr auto CPPTRANSPORT_OBSERVER_ELAPSED_FIRST    = "| time elapsed since beginning of integration";
constexpr auto CPPTRANSPORT_OBSERVER_ELAPSED_LATER    = "| time elapsed since last report";

constexpr auto CPPTRANSPORT_NO_TIMES                  = "Fatal: no times specified for integration";

constexpr auto CPPTRANSPORT_SAMPLES_START_TOO_EARLY_A = "Fatal: sample times begin before the initial time";
constexpr auto CPPTRANSPORT_SAMPLES_START_TOO_EARLY_B = "earliest sample time";
constexpr auto CPPTRANSPORT_SAMPLES_START_TOO_EARLY_C = "initial time";

constexpr auto CPPTRANSPORT_NO_DEVICES                = "Fatal: no devices specified in context";

constexpr auto CPPTRANSPORT_SINGLE_GPU_ONLY           = "GPU error: only a single GPU device is currently supported per worker process";

constexpr auto CPPTRANSPORT_INTEGRATION_FAIL          = "Fatal: integration failure";

constexpr auto CPPTRANSPORT_HSQ_IS_NEGATIVE           = "H^2 became negative";
constexpr auto CPPTRANSPORT_INTEGRATION_PRODUCED_NAN  = "Integration produced NaN";
constexpr auto CPPTRANSPORT_EPS_IS_NEGATIVE           = "epsilon became negative";
constexpr auto CPPTRANSPORT_EPS_TOO_LARGE             = "espilon became too large (exceeded eps=3.0)";
constexpr auto CPPTRANSPORT_V_IS_NEGATIVE             = "potential became negative";



#endif // CPPTRANSPORT_MESSAGES_EN_INTEGRATION_H
