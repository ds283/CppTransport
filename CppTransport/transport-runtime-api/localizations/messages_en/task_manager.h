//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H


#define CPPTRANSPORT_EXEC_SLAVE                      "Internal error: task_manager::execute_tasks() called on slave process"
#define CPPTRANSPORT_WAIT_MASTER                     "Internal error: task_manager::wait_for_tasks() called on master process"
#define CPPTRANSPORT_UNKNOWN_DERIVED_TASK            "Internal error: unknown derived 'task<number>' class for task"
#define CPPTRANSPORT_TOO_FEW_WORKERS                 "Too few workers: require at least two worker processes to process a task"
#define CPPTRANSPORT_UNEXPECTED_MPI                  "Internal error: unexpected MPI message received"
#define CPPTRANSPORT_TASK_NULL_DERIVED_PRODUCT       "Internal error: null derived product specifier for task"

#define CPPTRANSPORT_FILTER_TAG                      "Filter for work items:"

#define CPPTRANSPORT_EXPECTED_TWOPF_TASK             "Internal error: expected postintegration parent task to be of type twopf_task, but instead found task"
#define CPPTRANSPORT_EXPECTED_THREEPF_TASK           "Internal error: expected postintegration parent task to be of type threepf_task, but instead found task"
#define CPPTRANSPORT_EXPECTED_ZETA_THREEPF_TASK      "Internal error: expected postintegration parent task to be of type zeta_threepf_task, but instead found task"

#define CPPTRANSPORT_SCHEDULING_ALREADY_ASSIGNED     "Internal error: attempt to schedule work to an already-assigned worker process"
#define CPPTRANSPORT_SCHEDULING_NOT_ALREADY_ASSIGNED "Internal error: attempt to de-assign a worker which is not already assigned"
#define CPPTRANSPORT_SCHEDULING_OVERRELEASE_INFLIGHT "Internal error: over-release of number of in-flight work items"
#define CPPTRANSPORT_SCHEDULING_NO_UNASSIGNED        "Internal error: attempt to schedule work, but no unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_NOT_EXIST     "Internal error: attempt to assign non-existent work item"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_WORKER        "to worker"
#define CPPTRANSPORT_SCHEDULING_UNASSIGNED_MISMATCH  "Internal error: mismatch in number of unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ALREADY_INACTIVE     "Internal error: attempt to deactivate a worker which is already inactive"
#define CPPTRANSPORT_SCHEDULING_UNDER_INFLIGHT       "Internal error: under-release of number of in-flight work items"

#define CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A          "Could not find a matching content group"
#define CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B          "to seed task"

#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A "Paired groups"
#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B "and"
#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C "do not have the same missing k-configurations and cannot be used to seed a paired integration"

#define CPPTRANSPORT_PROCESSED_TASKS_A               "Task manager: processed"
#define CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR      "database task"
#define CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL        "database tasks"
#define CPPTRANSPORT_PROCESSED_TASKS_C               "in wallclock time"
#define CPPTRANSPORT_PROCESSED_TASKS_D               "time now"

#define CPPTRANSPORT_TASK_MANAGER_LABEL              "Task manager:"
#define CPPTRANSPORT_TASK_MANAGER_WARNING_LABEL      "Warning:"

#define CPPTRANSPORT_UNKNOWN_SWITCH                  "Ignored unknown command-line switch"

#endif // CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
