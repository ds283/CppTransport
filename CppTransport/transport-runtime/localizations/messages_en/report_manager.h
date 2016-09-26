//
// Created by David Seery on 26/09/16.
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


#ifndef CPPTRANSPORT_REPORT_MANAGER_MESSAGES_H
#define CPPTRANSPORT_REPORT_MANAGER_MESSAGES_H


#define CPPTRANSPORT_WORKER_DATA_NUMBER              "Worker number"
#define CPPTRANSPORT_WORKER_DATA_PROCESSED_ITEMS     "Items processed"
#define CPPTRANSPORT_WORKER_DATA_ASSIGNED            "Is assigned?"
#define CPPTRANSPORT_WORKER_DATA_INTEGRATION_TIME    "Reported integration time"
#define CPPTRANSPORT_WORKER_DATA_MEAN_TIME_PER_ITEM  "Mean time per item"
#define CPPTRANSPORT_WORKER_DATA_LAST_CONTACT        "Last contact at"
#define CPPTRANSPORT_WORKER_DATA_LOAD_AVERAGE        "Load average"
#define CPPTRANSPORT_WORKER_DATA_YES                 "Yes"
#define CPPTRANSPORT_WORKER_DATA_NO                  "No"

#define CPPTRANSPORT_REPORT_WORK_ITEMS_PROCESSED     "Items processed"
#define CPPTRANSPORT_REPORT_WORK_ITEMS_INFLIGHT      "In flight"
#define CPPTRANSPORT_REPORT_REMAIN                   "Remaining"
#define CPPTRANSPORT_REPORT_COMPLETE                 "Complete"
#define CPPTRANSPORT_REPORT_MEAN_TIME_PER_ITEM       "Mean CPU/item"
#define CPPTRANSPORT_REPORT_TARGET_DURATION          "Assignment"
#define CPPTRANSPORT_REPORT_COMPLETION_ESTIMATE      "Estimated completion"
#define CPPTRANSPORT_REPORT_FROM_NOW                 "from now"
#define CPPTRANSPORT_REPORT_CPU_TIME_ESTIMATE        "Estimated CPU time"

#define CPPTRANSPORT_REPORT_WORK_COMPLETE            "All work items processed"

#define CPPTRANSPORT_PROCESSING_TASK_A               "processing task"
#define CPPTRANSPORT_PROCESSING_TASK_OF              "of"

#define CPPTRANSPORT_PROCESSED_TASKS_A               "processed"
#define CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR      "database task"
#define CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL        "database tasks"
#define CPPTRANSPORT_PROCESSED_TASKS_WALLCLOCK       "in wallclock time"
#define CPPTRANSPORT_PROCESSED_TASKS_MASTER_LOAD     "master load"
#define CPPTRANSPORT_PROCESSED_TASKS_WORKER_LOAD     "worker load"


#endif //CPPTRANSPORT_REPORT_MANAGER_MESSAGES_H
