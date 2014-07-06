//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
#define __CPP_TRANSPORT_MESSAGES_EN_TASKS_H


#define __CPP_TRANSPORT_REFINEMENT_TOO_DEEP              "Integration error: too many levels of mesh refinement were required; decrease the underlying time step"

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

#define __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE        "Internal error: unknown bispectrum template"


#endif // __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
