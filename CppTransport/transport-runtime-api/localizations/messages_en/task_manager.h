//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H
#define __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H


#define __CPP_TRANSPORT_EXEC_SLAVE                "Internal error: task_manager::execute_tasks() called on slave process"
#define __CPP_TRANSPORT_WAIT_MASTER               "Internal error: task_manager::wait_for_tasks() called on master process"
#define __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK      "Internal error: unknown derived 'task<number>' class for task"
#define __CPP_TRANSPORT_TOO_FEW_WORKERS           "Too few workers: require at least two worker processes to process a task"
#define __CPP_TRANSPORT_UNEXPECTED_MPI            "Internal error: unexpected MPI message received"
#define __CPP_TRANSPORT_TASK_NULL_DERIVED_PRODUCT "Internal error: null derived product specifier for task"

#define __CPP_TRANSPORT_FILTER_TAG                "Filter for work items:"

#define __CPP_TRANSPORT_EXPECTED_TWOPF_TASK       "Internal error: expected postintegration parent task to be of type twopf_task, but instead found task"
#define __CPP_TRANSPORT_EXPECTED_THREEPF_TASK     "Internal error: expected postintegration parent task to be of type threepf_task, but instead found task"


#endif // __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H
