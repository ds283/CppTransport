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


#ifndef CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H


#define CPPTRANSPORT_EXEC_SLAVE                      "Internal error: task_manager::execute_tasks() called on slave process"
#define CPPTRANSPORT_WAIT_MASTER                     "Internal error: task_manager::wait_for_tasks() called on master process"
#define CPPTRANSPORT_UNKNOWN_DERIVED_TASK            "Internal error: unknown derived 'task<number>' class for task"
#define CPPTRANSPORT_TOO_FEW_WORKERS                 "Too few workers: require at least two worker processes to process a task"
#define CPPTRANSPORT_UNEXPECTED_MPI                  "Internal error: unexpected MPI message received"

#define CPPTRANSPORT_UNEXPECTED_UNHANDLED            "Internal error: unexpected unhandled exception"

#define CPPTRANSPORT_TASK_MISSING                    "Could not find repository record for task"
#define CPPTRANSPORT_TASK_CANT_BE_READ               "Can not read repository record for task"

#define CPPTRANSPORT_FILTER_TAG                      "Filter for work items:"

#define CPPTRANSPORT_EXPECTED_TWOPF_TASK             "Internal error: expected postintegration parent task to be of type twopf_task, but instead found task"
#define CPPTRANSPORT_EXPECTED_THREEPF_TASK           "Internal error: expected postintegration parent task to be of type threepf_task, but instead found task"
#define CPPTRANSPORT_EXPECTED_ZETA_THREEPF_TASK      "Internal error: expected postintegration parent task to be of type zeta_threepf_task, but instead found task"

#define CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A          "Could not find a matching content group"
#define CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B          "to seed task"

#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A "Paired groups"
#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B "and"
#define CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C "do not have the same missing k-configurations and cannot be used to seed a paired integration"

#define CPPTRANSPORT_PROCESSING_TASK_A               "Task manager: processing task"
#define CPPTRANSPORT_PROCESSING_TASK_OF              "of"

#define CPPTRANSPORT_PROCESSING_GANTT_CHART          "Task manager: generating process Gantt chart"
#define CPPTRANSPORT_PROCESSING_ACTIVITY_JOURNAL     "Task manager: generating activity journal"

#define CPPTRANSPORT_PROCESSED_TASKS_A               "Task manager: processed"
#define CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR      "database task"
#define CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL        "database tasks"
#define CPPTRANSPORT_PROCESSED_TASKS_WALLCLOCK       "in wallclock time"
#define CPPTRANSPORT_PROCESSED_TASKS_MASTER_LOAD     "master load"
#define CPPTRANSPORT_PROCESSED_TASKS_WORKER_LOAD     "worker load"

#define CPPTRANSPORT_TASK_MANAGER_LABEL              "Task manager:"
#define CPPTRANSPORT_TASK_MANAGER_WARNING_LABEL      "Warning:"

#define CPPTRANSPORT_UNKNOWN_SWITCH                  "Ignored unknown command-line switch"
#define CPPTRANSPORT_UNKNOWN_PLOT_STYLE              "Ignored unknown plot style"
#define CPPTRANSPORT_UNKNOWN_MPL_BACKEND             "Ignored unknown Matplotlib backend"


#define CPPTRANSPORT_WORKER_DATA_NUMBER              "Worker number"
#define CPPTRANSPORT_WORKER_DATA_PROCESSED_ITEMS     "Items processed"
#define CPPTRANSPORT_WORKER_DATA_ASSIGNED            "Is assigned?"
#define CPPTRANSPORT_WORKER_DATA_INTEGRATION_TIME    "Reported integration time"
#define CPPTRANSPORT_WORKER_DATA_MEAN_TIME_PER_ITEM  "Mean time per item"
#define CPPTRANSPORT_WORKER_DATA_LAST_CONTACT        "Last contact at"
#define CPPTRANSPORT_WORKER_DATA_LOAD_AVERAGE        "Load average"
#define CPPTRANSPORT_WORKER_DATA_YES                 "Yes"
#define CPPTRANSPORT_WORKER_DATA_NO                  "No"


#define CPPTRANSPORT_MASTER_REPORTED_BY_WORKER       "reported by worker"


#endif // CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
