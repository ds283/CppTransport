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


namespace transport
  {
    
    constexpr auto CPPTRANSPORT_WORKER_DATA_NUMBER = "Worker number";
    constexpr auto CPPTRANSPORT_WORKER_DATA_PROCESSED_ITEMS = "Items processed";
    constexpr auto CPPTRANSPORT_WORKER_DATA_ASSIGNED = "Is assigned?";
    constexpr auto CPPTRANSPORT_WORKER_DATA_INTEGRATION_TIME = "Reported work time";
    constexpr auto CPPTRANSPORT_WORKER_DATA_MEAN_TIME_PER_ITEM = "Mean time per item";
    constexpr auto CPPTRANSPORT_WORKER_DATA_LAST_CONTACT = "Last contact";
    constexpr auto CPPTRANSPORT_WORKER_DATA_LOAD_AVERAGE = "Load average";
    constexpr auto CPPTRANSPORT_WORKER_DATA_YES = "Yes";
    constexpr auto CPPTRANSPORT_WORKER_DATA_NO = "No";
    
    constexpr auto CPPTRANSPORT_REPORT_MASTER_LOAD_TASK = "Master load this task";
    constexpr auto CPPTRANSPORT_REPORT_MASTER_LOAD_GLOBAL = "Master load global";
    
    constexpr auto CPPTRANSPORT_REPORT_STATISTICS = "STATISTICS REPORT";
    constexpr auto CPPTRANSPORT_REPORT_RESOURCES = "RESOURCES REPORT";
    constexpr auto CPPTRANSPORT_REPORT_WORKERS = "WORKER STATUS";
    constexpr auto CPPTRANSPORT_REPORT_ALERTS = "ALERTS ISSUED";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE = "DATABASE PERFORMANCE";
    
    constexpr auto CPPTRANSPORT_REPORT_ALERT_MULTIPLE_A = "issued";
    constexpr auto CPPTRANSPORT_REPORT_ALERT_MULTIPLE_B = "times in this period, last at";
    
    constexpr auto CPPTRANSPORT_REPORT_WORK_ITEMS_PROCESSED = "Items processed";
    constexpr auto CPPTRANSPORT_REPORT_WORK_ITEMS_INFLIGHT = "In flight";
    constexpr auto CPPTRANSPORT_REPORT_REMAIN = "Remaining";
    constexpr auto CPPTRANSPORT_REPORT_COMPLETE = "Complete";
    constexpr auto CPPTRANSPORT_REPORT_MEAN_TIME_PER_ITEM = "Mean CPU/item";
    constexpr auto CPPTRANSPORT_REPORT_TARGET_DURATION = "Assignment";
    constexpr auto CPPTRANSPORT_REPORT_COMPLETION_ESTIMATE = "Estimated completion";
    constexpr auto CPPTRANSPORT_REPORT_FROM_NOW = "from now";
    constexpr auto CPPTRANSPORT_REPORT_CPU_TIME_ESTIMATE = "Estimated CPU time";
    
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_EVENT_TIME = "Aggregation at";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_ROWS = "Rows";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_TIME = "Time taken";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_ROWS_PER_SEC = "Rows/sec";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_CTR_SIZE = "Container size";
    constexpr auto CPPTRANSPORT_REPORT_DATABASE_TEMP_SIZE = "Temporary size";
    
    constexpr auto CPPTRANSPORT_REPORT_WORK_COMPLETE = "All work items processed";
    
    constexpr auto CPPTRANSPORT_PROCESSING_TASK_A = "processing task";
    constexpr auto CPPTRANSPORT_PROCESSING_TASK_OF = "of";
    
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_A = "processed";
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR = "database task";
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL = "database tasks";
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_WALLCLOCK = "in wallclock time";
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_MASTER_LOAD = "master load";
    constexpr auto CPPTRANSPORT_PROCESSED_TASKS_WORKER_LOAD = "worker load";
    
    constexpr auto CPPTRANSPORT_REPORT_HEADING_A = "Progress report for task";
    constexpr auto CPPTRANSPORT_REPORT_HEADING_B = "issued at";
    
    constexpr auto CPPTRANSPORT_REPORT_HEADING_C = "Final database report for task";
    
  }   // namespace transport


#endif //CPPTRANSPORT_REPORT_MANAGER_MESSAGES_H
