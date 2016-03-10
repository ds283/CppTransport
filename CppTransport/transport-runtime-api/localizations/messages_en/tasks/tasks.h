//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_TASKS_H
#define CPPTRANSPORT_MESSAGES_EN_TASKS_H


#define CPPTRANSPORT_TASK_TYPE_UNKNOWN                "Internal error: unknown task type"

#define CPPTRANSPORT_OUTPUT_TASK_RANGE                "Internal error: Out of range when accessing 'output_task' object"

#define CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A "Output task error: attempt to add two derived products with the filename"
#define CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B "to output task"

#define CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_A     "Output task error: attempt to add two derived products with the same name"
#define CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_B     "to output task"

#define CPPTRANSPORT_TASK_TIMES                       "Sampling times: "
#define CPPTRANSPORT_SERIALIZE_BACKGROUND_TASK        "Internal error: attempt to serialize background 'task' object"

#define CPPTRANSPORT_OUTPUT_ELEMENTS                  "Derived data tasks: "
#define CPPTRANSPORT_OUTPUT_ELEMENT_TASK              "task"
#define CPPTRANSPORT_OUTPUT_ELEMENT_OUTPUT            "output"
#define CPPTRANSPORT_OUTPUT_ELEMENT_TAGS              "tags"

#define CPPTRANSPORT_ZETA_INTEGRATION_CAST_FAIL       "Internal error: expected postintegration task parent to be castable to integration task, but dynamic cast failed"
#define CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL        "Internal error: expected zeta_twopf_list_task parent to be castable to twopf_list_task, but dynamic cast failed"
#define CPPTRANSPORT_ZETA_THREEPF_CAST_FAIL           "Internal error: expected zeta_threepf_task parent to be castable to threepf_task, but dynamic cast failed"

#define CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE        "Internal error: unknown bispectrum template"

#define CPPTRANSPORT_FNL_TASK_NOT_INTEGRABLE          "fNL_task: can't construct fNL product from non-integrable task"

#define CPPTRANSPORT_TASK_THREEPF_ELEMENTS_A          "task with"
#define CPPTRANSPORT_TASK_THREEPF_ELEMENTS_B          "3pf k-configurations and"
#define CPPTRANSPORT_TASK_THREEPF_ELEMENTS_C          "2pf k-configurations"

#define CPPTRANSPORT_TASK_THREEPF_DATABASE_MISS       "Internal error: missing database entry for k_t ="

#define CPPTRANSPORT_TASK_TWOPF_ELEMENTS_A            "task with"
#define CPPTRANSPORT_TASK_TWOPF_ELEMENTS_B            "2pf k-configurations"

#define CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_A     "largest scale kmin="
#define CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_B     "crosses horizon at time N*"
#define CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_C     "smallest scale kmax="
#define CPPTRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_D     "crosses horizon at time N*"

#define CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A      "earliest required time N="
#define CPPTRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B      "is earlier than time of initial conditions N="

#define CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A     "warning: initial conditions set at time N="
#define CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B     "allow only"
#define CPPTRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C     "e-folds of subhorizon evolution for largest scale"

#define CPPTRANSPORT_TASK_TWOPF_LIST_EARLIEST_STORED  "earliest stored time N="

#define CPPTRANSPORT_TASK_TWOPF_LIST_END_OF_INFLATION "end of inflation occurs near time N="
#define CPPTRANSPORT_TASK_TWOPF_LIST_WARN_LATE_END    "warning: end time for integration is later than end of inflation"
#define CPPTRANSPORT_TASK_TWOPF_LIST_NO_END_INFLATION "warning: end of inflation not found within search window"

#define CPPTRANSPORT_TASK_TWOPF_LIST_NO_TIMES         "no times to be stored"

#define CPPTRANSPORT_TASK_TWOPF_LIST_DATABASE_MISS    "Internal error: missing database entry for k ="

#define CPPTRANSPORT_TASK_FAIL_COMPUTE_HEXIT          "error: failed to compute horizon exit times for all k-configurations"
#define CPPTRANSPORT_TASK_SEARCH_FROM                 "searched from N="
#define CPPTRANSPORT_TASK_SEARCH_TO                   "to N="
#define CPPTRANSPORT_TASK_SEARCH_FOUND_END            "[end-of-search fixed by end of inflation]"
#define CPPTRANSPORT_TASK_SEARCH_NO_FOUND_END         "[did not find end of inflation: end-of-search is a fixed window]"
#define CPPTRANSPORT_TASK_SEARCH_RECORDED             "search recorded"
#define CPPTRANSPORT_TASK_SEARCH_SAMPLES              "samples"
#define CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE          "last sample log(aH)="
#define CPPTRANSPORT_TASK_SEARCH_LAST_SAMPLE_TIME     "at N="
#define CPPTRANSPORT_TASK_SEARCH_LARGEST_K            "largest k-mode log(k)="
#define CPPTRANSPORT_TASK_SEARCH_KAH                  "k/aH="
#define CPPTRANSPORT_TASK_SEARCH_GUESS_FAIL           "(search ended before last k-configuration left the horizon, probably because inflation ended before that could occur)"
#define CPPTRANSPORT_TASK_SEARCH_TOO_CLOSE_FAIL       "(search ended after last k-configuration left the horizon, but end of inflation is probably too close)"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_ACCURACY        "could not compute root of k-aH=0 to sufficient accuracy"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_INF             "received std::inf result while computing root of k-aH=0"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_NAN             "received std::nan result while computing root of k-aH=0"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_BRACKET         "extreme values of N did not bracket root of k-aH"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_ITERATIONS      "iterations"
#define CPPTRANSPORT_TASK_SEARCH_ROOT_ZERO_EQ         "abs(y)"


#endif // CPPTRANSPORT_MESSAGES_EN_TASKS_H
