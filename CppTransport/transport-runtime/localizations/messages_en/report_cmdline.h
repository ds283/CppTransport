//
// Created by David Seery on 20/03/2016.
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
constexpr auto CPPTRANSPORT_REPORT_PROVENANCE_FAILURE             = "Could not find record for output content group";

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
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_CITEGUIDE        = "Cite guide";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_DESCRIPTION      = "Description";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_LICENSE          = "License";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_REVISION         = "Revision";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_REFERENCES       = "References";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_URLS             = "URLs";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MP                     = "M_Planck";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_PARAMETERS             = "Model parameters";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_ICS                    = "Initial conditions";

constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_NAME      = "Name";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_EMAIL     = "Email";
constexpr auto CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_INSTITUTE = "Institute";

constexpr auto CPPTRANSPORT_REPORT_TASK_TYPE                      = "Task type";
constexpr auto CPPTRANSPORT_REPORT_TASK_DESCRIPTION               = "Task description";
constexpr auto CPPTRANSPORT_REPORT_PARENT_TASK                    = "Parent task";
constexpr auto CPPTRANSPORT_REPORT_KCONFIG_DB                     = "k-config database";
constexpr auto CPPTRANSPORT_REPORT_USES_PACKAGE                   = "Initial conditions package";

constexpr auto CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME              = "Content group";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_CREATED                 = "Created";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_EDITED                  = "Last update";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_FAILED                  = "Complete";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_SIZE                    = "Size";

constexpr auto CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK             = "Task";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK_TYPE        = "Type";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_LOCKED                  = "Locked";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_TAGS                    = "Tags";
constexpr auto CPPTRANSPORT_REPORT_OUTPUT_NOTES                   = "Notes";

constexpr auto CPPTRANSPORT_REPORT_PRODUCT_NAME                   = "Derived product";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_DESCRIPTION            = "Product description";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_TYPE                   = "Type";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_TAGS                   = "Matches tags";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_TAG_SET                = "Tags";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_FILENAME               = "Filename";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_DEPENDS_ON             = "Depends on content from";
constexpr auto CPPTRANSPORT_REPORT_PRODUCT_CREATED                = "Created";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_CONTAINER              = "Container";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_COMPLETE               = "Complete";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_WORKGROUP              = "Workgroup";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_SEEDED                 = "Seeded";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_DATA_TYPE              = "Data type";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_SEED_GROUP             = "Seed group";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_STATISTICS             = "Has statistics";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_ICS                = "Initial conditions";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_SIZE                   = "Size";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_PARENT                 = "Parent group";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_PRECOMPUTED            = "Group contains precomputed values for";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_TWOPF         = "zeta twopf";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_THREEPF       = "zeta threepf";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_REDBSP        = "reduced bispectrum";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_LOCAL          = "fNL_local";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_EQUILATERAL    = "fNL_equi";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_ORTHOGONAL     = "fNL_ortho";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_DBI            = "fNL_DBI";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_METADATA               = "Metadata";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_TOTAL_WALLCLOCK        = "Wallclock time";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_TOTAL_TIME             = "Total time";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_GLOBAL_MIN             = "Min integration";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_GLOBAL_MAX             = "Max integration";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_CONFIGURATIONS         = "Configurations";
constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_FAILURES               = "Failures";

constexpr auto CPPTRANSPORT_REPORT_PAYLOAD_CONTENT_GROUPS         = "Summary of content groups used";


#endif //CPPTRANSPORT_MESSAGES_EN_REPORT_CMDLINE_H
