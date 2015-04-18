//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
#define __CPP_TRANSPORT_MESSAGES_EN_TASKS_H


#define __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER   "Internal error: null serialization reader passed to 'task' object or derived class"
#define __CPP_TRANSPORT_TASK_TIME_CONFIG_SN_TOO_BIG      "Internal error: time configuration serial number out of range when deserializing 'integration_task' object"
#define __CPP_TRANSPORT_TASK_TWOPF_CONFIG_SN_TOO_BIG     "Internal error: twopf k-configuration serial number out of range when deserializing 'integration_task' object"
#define __CPP_TRANSPORT_TASK_TYPE_UNKNOWN                "Internal error: unknown task type"

#define __CPP_TRANSPORT_OUTPUT_TASK_RANGE                "Internal error: Out of range when accessing 'output_task' object"

#define __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A "Output task error: attempt to add two derived products with the filename"
#define __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B "to output task"

#define __CPP_TRANSPORT_OUTPUT_TASK_NAME_COLLISION_A     "Output task error: attempt to add two derived products with the same name"
#define __CPP_TRANSPORT_OUTPUT_TASK_NAME_COLLISION_B     "to output task"

#define __CPP_TRANSPORT_TASK_TIMES                       "Sampling times: "
#define __CPP_TRANSPORT_SERIALIZE_BACKGROUND_TASK        "Internal error: attempt to serialize background 'task' object"

#define __CPP_TRANSPORT_OUTPUT_ELEMENTS                  "Derived data tasks: "
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_TASK              "task"
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_OUTPUT            "output"
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_TAGS              "tags"

#define __CPP_TRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL        "Internal error: expected zeta_twopf_list_task parent to be castable to twopf_list_task, but dynamic cast failed"
#define __CPP_TRANSPORT_ZETA_THREEPF_CAST_FAIL           "Internal error: expected zeta_threepf_task parent to be castable to threepf_task, but dynamic cast failed"

#define __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE        "Internal error: unknown bispectrum template"

#define __CPP_TRANSPORT_FNL_TASK_NOT_INTEGRABLE          "fNL_task: can't construct fNL product from non-integrable task"

#define __CPP_TRANSPORT_TASK_THREEPF_ELEMENTS_A          "task with"
#define __CPP_TRANSPORT_TASK_THREEPF_ELEMENTS_B          "3pf k-configurations and"
#define __CPP_TRANSPORT_TASK_THREEPF_ELEMENTS_C          "2pf k-configurations"

#define __CPP_TRANSPORT_TASK_TWOPF_ELEMENTS_A            "task with"
#define __CPP_TRANSPORT_TASK_TWOPF_ELEMENTS_B            "2pf k-configurations"

#define __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_A     "largest scale kmin="
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_B     "crosses horizon at time N*"
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_C     "smallest scale kmax="
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_MODE_RANGE_D     "crosses horizon at time N*"

#define __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_A      "earliest required time N="
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_TOO_EARLY_B      "is earlier than time of initial conditions N="

#define __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_A     "initial conditions set at time N="
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_B     "allow only"
#define __CPP_TRANSPORT_TASK_TWOPF_LIST_CROSS_WARN_C     "e-folds of subhorizon evolution for largest scale"

#define __CPP_TRANSPORT_TASK_TWOPF_LIST_EARLIEST_STORED  "earliest stored time N="

#define __CPP_TRANSPORT_TASK_TWOPF_LIST_NO_TIMES         "no times to be stored"


#endif // __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
