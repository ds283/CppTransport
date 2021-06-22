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


#ifndef CPPTRANSPORT_MESSAGES_EN_TASKS_H
#define CPPTRANSPORT_MESSAGES_EN_TASKS_H


constexpr auto CPPTRANSPORT_TASK_TYPE_UNKNOWN = "Internal error: unknown task type";

constexpr auto CPPTRANSPORT_OUTPUT_TASK_RANGE = "Internal error: Out of range when accessing 'output_task' object";

constexpr auto CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A = "Output task error: attempt to add two derived products with the filename";
constexpr auto CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B = "to output task";

constexpr auto CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_A = "Output task error: attempt to add two derived products with the same name";
constexpr auto CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_B = "to output task";

constexpr auto CPPTRANSPORT_TASK_TIMES = "Sampling times: ";
constexpr auto CPPTRANSPORT_SERIALIZE_BACKGROUND_TASK = "Internal error: attempt to serialize background task object";
constexpr auto CPPTRANSPORT_KCONFIG_BACKGROUND_TASK = "Internal error: attempt to query background task object for k-configuration database";
constexpr auto CPPTRANSPORT_QUERY_BACKGROUND_TASK = "Internal error: attempt to query background task object for type";

constexpr auto CPPTRANSPORT_OUTPUT_ELEMENTS = "Derived data tasks: ";
constexpr auto CPPTRANSPORT_OUTPUT_ELEMENT_TASK = "task";
constexpr auto CPPTRANSPORT_OUTPUT_ELEMENT_OUTPUT = "output";
constexpr auto CPPTRANSPORT_OUTPUT_ELEMENT_TAGS = "tags";

constexpr auto CPPTRANSPORT_ZETA_INTEGRATION_CAST_FAIL = "Internal error: expected postintegration task parent to be castable to integration task, but dynamic cast failed";
constexpr auto CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL = "Internal error: expected zeta_twopf_list_task parent to be castable to twopf_list_task, but dynamic cast failed";
constexpr auto CPPTRANSPORT_ZETA_THREEPF_CAST_FAIL = "Internal error: expected zeta_threepf_task parent to be castable to threepf_task, but dynamic cast failed";

constexpr auto CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE = "Internal error: unknown bispectrum template";

constexpr auto CPPTRANSPORT_FNL_TASK_NOT_INTEGRABLE = "fNL_task: can't construct fNL product from non-integrable task";

constexpr auto CPPTRANSPORT_TASK_DATA_NSTAR = "N*";
constexpr auto CPPTRANSPORT_TASK_DATA_N = "N=";

constexpr auto CPPTRANSPORT_TASK_DATA_THREEPF = "3pf configs";
constexpr auto CPPTRANSPORT_TASK_DATA_TWOPF = "2pf configs";

constexpr auto CPPTRANSPORT_TASK_DATA_LARGEST = "Largest k";
constexpr auto CPPTRANSPORT_TASK_DATA_SMALLEST = "Smallest k";
constexpr auto CPPTRANSPORT_TASK_DATA_EARLIEST = "Earliest N_exit";
constexpr auto CPPTRANSPORT_TASK_DATA_LATEST = "Latest N_exit";
constexpr auto CPPTRANSPORT_TASK_DATA_END_INFLATION = "Inflation ends";

constexpr auto CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS = "Internal error: missing database entry for k =";

constexpr auto CPPTRANSPORT_TASK_TWOPF_VALIDATE_INCONSISTENT = "Internal error: validation of subhorizon efolds is inconsistent";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A = "N=";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B = "adaptive ics -- earliest required time N=";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_C = "is earlier than time of initial conditions N=";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_SETTLING_A = "allows only";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_SETTLING_B = "e-folds for some scales to settle initial conditions; consider checking whether this is sufficient";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_SETTLING_C = "earliest massless time N=";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A = "uniform ics -- earliest massless time N=";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B = "allows only";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C = "e-folds of massless evolution for some scales";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_EARLIEST_STORED = "earliest stored time N=";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_END_OF_INFLATION = "end of inflation occurs near time N=";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_WARN_LATE_END = "end time for integration is later than end of inflation";
constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_NO_END_INFLATION = "end of inflation not found within search window";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_NO_TIMES = "no times to be stored";

constexpr auto CPPTRANSPORT_TASK_TWOPF_LIST_DATABASE_MISS = "Internal error: missing database entry for k =";

constexpr auto CPPTRANSPORT_TASK_FAIL_COMPUTE_HEXIT = "Error: failed to compute horizon exit times for all k-configurations";
constexpr auto CPPTRANSPORT_TASK_SEARCH_FROM = "searched from N=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_TO = "to N=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_FOUND_END = "[end-of-search fixed by end of inflation]";
constexpr auto CPPTRANSPORT_TASK_SEARCH_NO_FOUND_END = "[did not find end of inflation: end-of-search is a fixed window]";
constexpr auto CPPTRANSPORT_TASK_SEARCH_RECORDED = "search recorded";
constexpr auto CPPTRANSPORT_TASK_SEARCH_SAMPLES = "samples";
constexpr auto CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE = "last sample log(aH)=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE_TIME = "at N=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_LARGEST_K = "largest k-mode log(k)=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_KAH = "k/aH=";
constexpr auto CPPTRANSPORT_TASK_SEARCH_GUESS_FAIL = "search ended before last k-configuration left the horizon, probably because inflation ended before that could occur";
constexpr auto CPPTRANSPORT_TASK_SEARCH_TOO_CLOSE_FAIL = "search ended after last k-configuration left the horizon, but end of inflation is probably too close";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY = "could not compute root to sufficient accuracy";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_INF = "received std::inf result while computing root";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_NAN = "received std::nan result while computing root";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET_EXIT = "extreme values of N did not bracket time of horizon exit; check whether range of N contains horizon exit times for all configurations";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET_MASSLESS = "extreme values of N did not bracket massless point; check whether range of N contains massless time for all configurations";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_ITERATIONS = "iterations";
constexpr auto CPPTRANSPORT_TASK_SEARCH_ROOT_ZERO_EQ = "abs(y)";

constexpr auto CPPTRANSPORT_TASK_TYPE_INTEGRATION = "integration";
constexpr auto CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION = "postintegration";
constexpr auto CPPTRANSPORT_TASK_TYPE_OUTPUT = "output";

constexpr auto CPPTRANSPORT_TASK_TYPE_INTEGRATION_TWOPF = "twopf";
constexpr auto CPPTRANSPORT_TASK_TYPE_INTEGRATION_THREEPF = "threepf";
constexpr auto CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_TWOPF = "zeta twopf";
constexpr auto CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_THREEPF = "zeta threepf";
constexpr auto CPPTRANSPORT_TASK_TYPE_POSTINTEGRATION_FNL = "fNL";

constexpr auto CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE = "Attempt to access out-of-range payload flag for this content group type";

#endif // CPPTRANSPORT_MESSAGES_EN_TASKS_H
