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


#define CPPTRANSPORT_SCHEDULING_INDEX_OUT_OF_RANGE         "Worker index out of range in worker scheduler"

#define CPPTRANSPORT_SCHEDULING_ALREADY_ASSIGNED           "Attempt to schedule work to an already-assigned worker process"
#define CPPTRANSPORT_SCHEDULING_NOT_ALREADY_ASSIGNED       "Attempt to de-assign a worker which is not already assigned"
#define CPPTRANSPORT_SCHEDULING_OVERRELEASE_INFLIGHT       "Over-release of number of in-flight work items"
#define CPPTRANSPORT_SCHEDULING_NO_UNASSIGNED              "Attempt to schedule work, but no unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_NOT_EXIST           "Attempt to assign non-existent work item"
#define CPPTRANSPORT_SCHEDULING_ASSIGN_WORKER              "to worker"
#define CPPTRANSPORT_SCHEDULING_UNASSIGNED_MISMATCH        "Mismatch in number of unassigned workers"
#define CPPTRANSPORT_SCHEDULING_ALREADY_INACTIVE           "Attempt to deactivate a worker which is already inactive"
#define CPPTRANSPORT_SCHEDULING_UNDER_INFLIGHT             "Under-release of number of in-flight work items"


#endif //CPPTRANSPORT_WORKER_SCHEDULER_MESSAGES_H
