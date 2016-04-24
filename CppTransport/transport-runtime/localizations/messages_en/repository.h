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


#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H


#define CPPTRANSPORT_REPO_MISSING_ROOT                "Repository error: Root directory does not exist"
#define CPPTRANSPORT_REPO_MISSING_DATABASE            "Repository error: Repository database not present or inaccessible:"
#define CPPTRANSPORT_REPO_MISSING_PACKAGE_STORE       "Repository error: Package store not present or inaccessible"
#define CPPTRANSPORT_REPO_MISSING_TASK_STORE          "Repository error: Task store not present or inaccessible"
#define CPPTRANSPORT_REPO_MISSING_PRODUCT_STORE       "Repository error: Product store not present or inaccessible"
#define CPPTRANSPORT_REPO_MISSING_OUTPUT_STORE        "Repository error: Output store not present or inaccessible"

#define CPPTRANSPORT_REPO_PACKAGE_EXISTS              "Repository error: attempt to insert duplicate package record"
#define CPPTRANSPORT_REPO_TASK_EXISTS                 "Repository error: attempt to insert duplicate task record"
#define CPPTRANSPORT_REPO_PRODUCT_EXISTS              "Repository error: attempt to insert duplicate derived product record"
#define CPPTRANSPORT_REPO_OUTPUT_EXISTS               "Repository error: attempt to insert duplicate output record"

#define CPPTRANSPORT_REPO_PACKAGE_MISSING             "Repository error: missing record for package"
#define CPPTRANSPORT_REPO_TASK_MISSING                "Repository error: missing record for task"
#define CPPTRANSPORT_REPO_PRODUCT_MISSING             "Repository error: missing record for derived product"
#define CPPTRANSPORT_REPO_OUTPUT_MISSING              "Repository error: missing record for output"

#define CPPTRANSPORT_REPO_TASK_DUPLICATE              "Repository error: attempt to add record for already existing task"
#define CPPTRANSPORT_REPO_PACKAGE_DUPLICATE           "Repository error: attempt to add record for already existing package"
#define CPPTRANSPORT_REPO_PRODUCT_DUPLICATE           "Repository error: attempt to add record for already existing derived product"
#define CPPTRANSPORT_REPO_CONTENT_DUPLICATE           "Repository error: attempt to add record for already existing content group"

#define CPPTRANSPORT_REPO_GROUP_IS_LOCKED             "Repository error: attempt to delete locked content group"

#define CPPTRANSPORT_REPO_COMMIT_FAILURE              "Repository error: could not write database record for"
#define CPPTRANSPORT_REPO_DESERIALIZE_CANT_OPEN       "Repository error: could not locate database record for"
#define CPPTRANSPORT_REPO_DESERIALIZE_FAILURE         "Repository error: could not read database record for"

#define CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH     "Repository error: Error while attempting to move content group to failure cache"

#define CPPTRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_A    "Autocommit: integration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_B    "requires package"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A        "Autocommit: derived product"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B        "requires integration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_C        "Autocommit: derived product"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_D        "requires postintegration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_OUTPUT_A         "Autocommit: output task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_OUTPUT_B         "requires derived product"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A     "Autocommit: postintegration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B     "requires integration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_C     "Autocommit: postintegration task"
#define CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_D     "requires postintegration task"

#define CPPTRANSPORT_REPO_NOTE_NO_STATISTICS          "Collection of per-configuration timing statistics disabled because not supported by backend"
#define CPPTRANSPORT_REPO_NOTE_NO_INTEGRATION         "Cannot be used for integrations over the spectrum or bispectrum because task does not linearly step in k-configurations or the storage policy disables some configurations"

#define CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_A   "Committed content group"
#define CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_B   "for task"
#define CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_C   "tags"
#define CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_D   "at"

#define CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_A         "Content group"
#define CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_B         "has missing content"

#define CPPTRANSPORT_REPO_CONTENT_FAILED_A            "Content group"
#define CPPTRANSPORT_REPO_CONTENT_FAILED_B            "moved to fail cache in repository"

#define CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_A "Task"
#define CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_B "Errors were encountered during integration"

#define CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_A "Task"
#define CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_B "Some derived content could not be generated"

#define CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_A      "Task"
#define CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_B      "Some postprocessing could not be completed"

#define CPPTRANSPORT_REPO_NOT_SET                     "Internal error: attempt to access repository when unset in 'task_manager'"

#define CPPTRANSPORT_REPO_MISSING_RECORD              "Repository error: Could not find database record"

#define CPPTRANSPORT_REPO_SKIPPING_TASK               "; skipping this task"
#define CPPTRANSPORT_REPO_NONE                        "Nothing to do: no repository specified"
#define CPPTRANSPORT_REPO_FOR_TASK                    "while processing task"
#define CPPTRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE   "Internal error: attempt to read an unset 'data_manager' data-container handle in 'integration_writer'"
#define CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET     "Internal error: aggregator unset in writer object"
#define CPPTRANSPORT_REPO_WRITER_FAILURE_UNSUPPORTED  "Internal error: writer doesn't support failure lists"

#define CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN "Repository error: attempt to enumerate derived contents for non-integration task"
#define CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT "Repository error: attempt to enumerate derived contents for non-postintegration task"
#define CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT  "Repository error: attempt to enumerate derived contents for non-output task"

#define CPPTRANSPORT_REPO_TASK_NOT_INTEGRATION        "Repository error: derived product requires an integration task"
#define CPPTRANSPORT_REPO_ZETA_TASK_NOT_DERIVABLE     "Repository error: task named in zeta task is not of derivable-type"
#define CPPTRANSPORT_REPO_NO_MATCHING_CONTENT_GROUPS   "Repository error: no matching content groups for task"

#define CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL       "Repository error: failed to deserialize task"
#define CPPTRANSPORT_REPO_PRODUCT_DESERIALIZE_FAIL    "Repository error: failed to deserialize derived product"

#define CPPTRANSPORT_REPO_RECORD_CAST_FAILED          "Internal error: dynamic cast of repository record failed"

#define CPPTRANSPORT_REPO_COMMIT_READONLY             "Internal error: Attempt to commit read-only record"


#endif // CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H
