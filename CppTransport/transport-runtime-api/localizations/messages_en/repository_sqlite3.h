//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H


#define CPPTRANSPORT_REPO_FAIL_DATABASE_OPEN                "Repository error: failed to open repository database"
#define CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN        "Repository error: failed to open kconfiguration database for task"

#define CPPTRANSPORT_REPO_FAIL_INTEGRATION_TASK_TYPE        "Repository error: unknown integration task type"

#define CPPTRANSPORT_REPO_DATABASES_OPEN                    "Repository error: SQLite database handles unexpectedly open"
#define CPPTRANSPORT_REPO_DATABASES_CLOSED                  "Repository error: SQLite database handles unexpectedly closed"
#define CPPTRANSPORT_REPO_DATABASES_NOT_OPEN                "Repository error: SQLite database handles closed, but should be open"

#define CPPTRANSPORT_REPO_COUNT_FAIL                        "Repository error: database count failed (backend code="
#define CPPTRANSPORT_REPO_COUNT_MULTIPLE_RETURN             "Repository error: database count returned multiple matches"
#define CPPTRANSPORT_REPO_SEARCH_FAIL                       "Repository error: database search failed (backend code="
#define CPPTRANSPORT_REPO_SEARCH_MULTIPLE_RETURN            "Repository error: database search returned multiple matches"

#define CPPTRANSPORT_REPO_STORE_PACKAGE_FAIL                "Repository error: failed to store package record (backend code="
#define CPPTRANSPORT_REPO_STORE_TASK_FAIL                   "Repository error: failed to store task record (backend code="
#define CPPTRANSPORT_REPO_STORE_PRODUCT_FAIL                "Repository error: failed to store derived product record (backend code="
#define CPPTRANSPORT_REPO_STORE_OUTPUT_FAIL                 "Repository error: failed to store output record (backend code="

#define CPPTRANSPORT_REPO_STORE_RESERVE_FAIL                "Repository error: failed to store content group name reservation (backend code="
#define CPPTRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED        "Repository error: attempt to commit output group for unreserved name"

#define CPPTRANSPORT_REPO_STORE_INTEGRATION_WRITER_FAIL     "Repository error: failed to register integration writer (backend code="
#define CPPTRANSPORT_REPO_STORE_POSTINTEGRATION_WRITER_FAIL "Repository error: failed to register postintegration writer (backend code="
#define CPPTRANSPORT_REPO_STORE_DERIVED_CONTENT_WRITER_FAIL "Repository error: failed to register derived content writer (backend code="

#define CPPTRANSPORT_BADLY_FORMED_RANGE                     "Repository error: badly formed JSON document for 'range' group"
#define CPPTRANSPORT_BADLY_FORMED_PARAMS                    "Repository error: badly formed JSON for 'parameters' group"
#define CPPTRANSPORT_BADLY_FORMED_ICS                       "Repository error: badly formed JSON for 'initial_conditions' group"


#endif // CPPTRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
