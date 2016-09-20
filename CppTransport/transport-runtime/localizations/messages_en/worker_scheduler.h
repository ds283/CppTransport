//
// Created by David Seery on 09/04/15.
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


#ifndef CPPTRANSPORT_WORKER_SCHEDULER_MESSAGES_H
#define CPPTRANSPORT_WORKER_SCHEDULER_MESSAGES_H


#define CPPTRANSPORT_SCHEDULING_INDEX_OUT_OF_RANGE         "Internal error: worker index out of range in worker scheduler"

#define CPPTRANSPORT_SCHEDULING_ALREADY_ASSIGNED           "Internal error: attempt to schedule work to an already-assigned worker process"
#define CPPTRANSPORT_SCHEDULING_NOT_ALREADY_ASSIGNED       "Internal error: attempt to de-assign a worker which is not already assigned"
#define CPPTRANSPORT_SCHEDULING_OVERRELEASE_INFLIGHT       "Internal error: over-release of number of in-flight work items"
#define CPPTRANSPORT_SCHEDULING_NO_UNASSIGNED              "Internal error: attempt to schedule work, but no unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_NOT_EXIST           "Internal error: attempt to assign non-existent work item"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_WORKER              "to worker"
#define CPPTRANSPORT_SCHEDULING_UNASSIGNED_MISMATCH        "Internal error: mismatch in number of unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ALREADY_INACTIVE           "Internal error: attempt to deactivate a worker which is already inactive"
#define CPPTRANSPORT_SCHEDULING_UNDER_INFLIGHT             "Internal error: under-release of number of in-flight work items"

#define CPPTRANSPORT_WORKER_SCHEDULER_WORK_ITEMS_PROCESSED "Items processed"
#define CPPTRANSPORT_WORKER_SCHEDULER_WORK_ITEMS_INFLIGHT  "In flight"
#define CPPTRANSPORT_WORKER_SCHEDULER_REMAIN               "Remaining"
#define CPPTRANSPORT_WORKER_SCHEDULER_COMPLETE             "Complete"
#define CPPTRANSPORT_WORKER_SCHEDULER_MEAN_TIME_PER_ITEM   "Mean CPU/item"
#define CPPTRANSPORT_WORKER_SCHEDULER_TARGET_DURATION      "Assignment"
#define CPPTRANSPORT_WORKER_SCHEDULER_COMPLETION_ESTIMATE  "Estimated completion"
#define CPPTRANSPORT_WORKER_SCHEDULER_FROM_NOW             "from now"
#define CPPTRANSPORT_WORKER_SCHEDULER_CPU_TIME_ESTIMATE    "Estimated CPU time"

#define CPPTRANSPORT_WORKER_SCHEDULER_WORK_COMPLETE        "All work items processed"


#endif //CPPTRANSPORT_WORKER_SCHEDULER_MESSAGES_H
