//
// Created by David Seery on 20/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H
#define CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H


constexpr auto CPPTRANSPORT_REPORT_STATUS_TASKS                   = "Available tasks:";
constexpr auto CPPTRANSPORT_REPORT_STATUS_TYPE                    = "Type";
constexpr auto CPPTRANSPORT_REPORT_STATUS_TASK_NAME               = "Task";
constexpr auto CPPTRANSPORT_REPORT_STATUS_LAST_EDIT               = "Last activity";
constexpr auto CPPTRANSPORT_REPORT_STATUS_NUM_GROUPS              = "Outputs";

constexpr auto CPPTRANSPORT_REPORT_STATUS_INFLIGHT                = "In-flight content:";
constexpr auto CPPTRANSPORT_REPORT_STATUS_CONTENT                 = "Name";
constexpr auto CPPTRANSPORT_REPORT_STATUS_PARENT                  = "Task";
constexpr auto CPPTRANSPORT_REPORT_STATUS_INITIATED               = "Initiated";
constexpr auto CPPTRANSPORT_REPORT_STATUS_DURATION                = "Duration";
constexpr auto CPPTRANSPORT_REPORT_STATUS_CORES                   = "Cores";
constexpr auto CPPTRANSPORT_REPORT_STATUS_COMPLETION              = "Completion";

constexpr auto CPPTRANSPORT_REPORT_MISSING_OBJECT                 = "Could not find repository record for item";

constexpr auto CPPTRANSPORT_REPORT_RECORD_PACKAGE                 = "initial conditions package";
constexpr auto CPPTRANSPORT_REPORT_RECORD_TASK                    = "task";
constexpr auto CPPTRANSPORT_REPORT_RECORD_INTEGRATION_TASK        = "integration task";
constexpr auto CPPTRANSPORT_REPORT_RECORD_POSTINTEGRATION_TASK    = "postintegration task";
constexpr auto CPPTRANSPORT_REPORT_RECORD_OUTPUT_TASK             = "output task";
constexpr auto CPPTRANSPORT_REPORT_RECORD_DERIVED                 = "derived product";
constexpr auto CPPTRANSPORT_REPORT_RECORD_INTEGRATION             = "integration content";
constexpr auto CPPTRANSPORT_REPORT_RECORD_POSTINTEGRATION         = "postintegration content";
constexpr auto CPPTRANSPORT_REPORT_RECORD_OUTPUT                  = "derived content";

constexpr auto CPPTRANSPORT_REPORT_CREATION_DATE                  = "Created";
constexpr auto CPPTRANSPORT_REPORT_LAST_EDIT_DATE                 = "Last updated";
constexpr auto CPPTRANSPORT_REPORT_API_VERSION                    = "Runtime version";


#endif //CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H
