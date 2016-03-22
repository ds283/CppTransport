//
// Created by David Seery on 20/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H
#define CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H


constexpr auto CPPTRANSPORT_REPORT_YES                            = "Yes";
constexpr auto CPPTRANSPORT_REPORT_NO                             = "No";

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
constexpr auto CPPTRANSPORT_REPORT_LAST_EDIT_DATE                 = "Last update";
constexpr auto CPPTRANSPORT_REPORT_API_VERSION                    = "Runtime version";

constexpr auto CPPTRANSPORT_REPORT_PACKAGE_N_INIT                 = "N_init";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_N_CROSSING             = "N*";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_NAME             = "Model";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHORS          = "Authors";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_TAG              = "Tagline";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MP                     = "M_Planck";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_PARAMETERS             = "Model parameters";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_ICS                    = "Initial conditions";

constexpr auto CPPTRANSPORT_REPORT_TASK_TYPE                      = "Task type";
constexpr auto CPPTRANSPORT_REPORT_PARENT_TASK                    = "Parent task";
constexpr auto CPPTRANSPORT_REPORT_KCONFIG_DB                     = "k-config database";
constexpr auto CPPTRANSPORT_REPORT_USES_PACKAGE                   = "Initial conditions package";

constexpr auto CPPTRANSPORT_REPORT_OUTPUT_GROUP_NAME              = "Output group";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_CREATED                 = "Created";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_EDITED                  = "Last update";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_FAILED                  = "Complete";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_SIZE                    = "Size";

constexpr auto CPPTRANSPORT_REPORT_PRODUCT_NAME                   = "Derived product";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_TYPE                   = "Type";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_TAGS                   = "Matches tags";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_FILENAME               = "Filename";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_DEPENDS_ON             = "Depends on content from";


#endif //CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H
