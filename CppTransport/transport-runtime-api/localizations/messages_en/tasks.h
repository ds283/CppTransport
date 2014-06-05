//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
#define __CPP_TRANSPORT_MESSAGES_EN_TASKS_H


#define __CPP_TRANSPORT_NULL_SERIALIZATION_READER        "Internal error: null serialization reader passed to 'task' object or derived class"
#define __CPP_TRANSPORT_TIME_CONFIG_SN_TOO_BIG           "Internal error: time configuration serial number out of range when deserializing 'integration_task' object"

#define __CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE            "Internal error: Out of range when accessing 'twopf_list_task' object"
#define __CPP_TRANSPORT_TWOPF_TASK_LIST_NORM             "Internal error: Attempt to reset comoving normalization in 'twopf_task_list' object"
#define __CPP_TRANSPORT_TWOPF_TASK_LIST_UNSET            "Internal error: Attempt to push to 'twopf_task_list' before comoving normalization has been set"

#define __CPP_TRANSPORT_OUTPUT_TASK_RANGE                "Internal error: Out of range when accessing 'output_task' object"

#define __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A "Ouput task error: attempt to add two derived products with the filename"
#define __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B "to output task"

#define __CPP_TRANSPORT_TASK_TIMES                       "Sampling times: "
#define __CPP_TRANSPORT_TASK_THREEPF_TYPE                "Unknown wavenumber grid type when attempting to write threepf task to repository"
#define __CPP_TRANSPORT_SERIALIZE_BACKGROUND_TASK        "Internal Error: attempt to serialize base 'task' object"

#define __CPP_TRANSPORT_OUTPUT_ELEMENTS                  "Derived data tasks: "
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_TASK              "task"
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_OUTPUT            "output"
#define __CPP_TRANSPORT_OUTPUT_ELEMENT_TAGS              "tags"


#endif // __CPP_TRANSPORT_MESSAGES_EN_TASKS_H
