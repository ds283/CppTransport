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


#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H


#define CPPTRANSPORT_REPO_FAIL_DATABASE_OPEN                "Repository error: failed to open repository database"
#define CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN        "Repository error: failed to open kconfiguration database for task"

#define CPPTRANSPORT_REPO_FAIL_INTEGRATION_TASK_TYPE        "Repository error: unknown integration task type"

#define CPPTRANSPORT_REPO_DATABASES_OPEN                    "Repository error: SQLite database handles unexpectedly open"
#define CPPTRANSPORT_REPO_DATABASES_CLOSED                  "Repository error: SQLite database handles unexpectedly closed"
#define CPPTRANSPORT_REPO_DATABASES_NOT_OPEN                "Repository error: SQLite database handles closed, but should be open"

#define CPPTRANSPORT_REPO_BACKEND_FAIL                      "(backend code="

#define CPPTRANSPORT_REPO_COUNT_FAIL                        "Repository error: database count failed (backend code="
#define CPPTRANSPORT_REPO_COUNT_MULTIPLE_RETURN             "Repository error: database count returned multiple matches"
#define CPPTRANSPORT_REPO_SEARCH_FAIL                       "Repository error: database search failed (backend code="
#define CPPTRANSPORT_REPO_SEARCH_MULTIPLE_RETURN            "Repository error: database search returned multiple matches"

#define CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL                "Repository error: failed to store package record"
#define CPPTRANSPORT_REPO_STORE_TASK_FAIL                   "Repository error: failed to store task record"
#define CPPTRANSPORT_REPO_STORE_PRODUCT_FAIL                "Repository error: failed to store derived product record"
#define CPPTRANSPORT_REPO_STORE_OUTPUT_FAIL                 "Repository error: failed to store content group record"
#define CPPTRANSPORT_REPO_DELETE_OUTPUT_FAIL                "Repository error: failed to delete content group record"

#define CPPTRANSPORT_REPO_STORE_RESERVE_FAIL                "Repository error: failed to store content group name reservation (backend code="
#define CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED        "Repository error: attempt to commit content group for unreserved name"
#define CPPTRANSPORT_REPO_DELETE_OUTPUT_UNSTORED            "Repository error: attempt to delete content group for unstored name"
#define CPPTRANSPORT_REPO_UPDATE_COMPLETION_FAIL            "Repository error: failed to update completion time for content group (backend code="
#define CPPTRANSPORT_REPO_DEREGISTER_FAIL                   "Repository error: failed to release content group name reservation (backend code="

#define CPPTRANSPORT_REPO_STORE_INTEGRATION_WRITER_FAIL     "Repository error: failed to register integration writer (backend code="
#define CPPTRANSPORT_REPO_STORE_POSTINTEGRATION_WRITER_FAIL "Repository error: failed to register postintegration writer (backend code="
#define CPPTRANSPORT_REPO_STORE_DERIVED_CONTENT_WRITER_FAIL "Repository error: failed to register derived content writer (backend code="

#define CPPTRANSPORT_BADLY_FORMED_RANGE                     "Repository error: badly formed JSON document for 'range' group"
#define CPPTRANSPORT_BADLY_FORMED_PARAMS                    "Repository error: badly formed JSON for 'parameters' group"
#define CPPTRANSPORT_BADLY_FORMED_ICS                       "Repository error: badly formed JSON for 'initial_conditions' group"

#define CPPTRANSPORT_REPO_CANT_VACUUM                       "Repository error: could not perform database maintenance"


#endif // CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
