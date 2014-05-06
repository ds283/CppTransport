//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H
#define __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H


#define __CPP_TRANSPORT_EXEC_SLAVE            "Internal error: task_manager::execute_tasks() called on slave process"
#define __CPP_TRANSPORT_WAIT_MASTER           "Internal error: task_manager::wait_for_tasks() called on master process"
#define __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK  "Internal error: Unknown derived 'task<number>' class"
#define __CPP_TRANSPORT_TOO_FEW_WORKERS       "Too few workers: require at least two worker processes to integrate a task"
#define __CPP_TRANSPORT_UNEXPECTED_MPI        "Internal error: Unexpected MPI message received"
#define __CPP_TRANSPORT_FILTER_TAG            "Filter for work items:"


#endif // __CPP_TRANSPORT_MESSAGES_EN_TASK_MANAGER_H