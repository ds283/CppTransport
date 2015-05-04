//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H


#define __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN           "Repository error: failed to open repository database"
#define __CPP_TRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN   "Repository error: failed to open kconfiguration database for task"

#define __CPP_TRANSPORT_REPO_DATABASES_OPEN               "Repository error: SQLite database handles unexpectedly open"
#define __CPP_TRANSPORT_REPO_DATABASES_CLOSED             "Repository error: SQLite database handles unexpectedly closed"
#define __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN           "Repository error: SQLite database handles closed, but should be open"

#define __CPP_TRANSPORT_REPO_COUNT_FAIL                   "Repository error: database count failed (backend code="
#define __CPP_TRANSPORT_REPO_COUNT_MULTIPLE_RETURN        "Repository error: database count returned multiple matches"
#define __CPP_TRANSPORT_REPO_SEARCH_FAIL                  "Repository error: database search failed (backend code="
#define __CPP_TRANSPORT_REPO_SEARCH_MULTIPLE_RETURN       "Repository error: database search returned multiple matches"

#define __CPP_TRANSPORT_REPO_STORE_PACKAGE_FAIL           "Repository error: failed to store package record (backend code="
#define __CPP_TRANSPORT_REPO_STORE_TASK_FAIL              "Repository error: failed to store task record (backend code="
#define __CPP_TRANSPORT_REPO_STORE_PRODUCT_FAIL           "Repository error: failed to store derived product record (backend code="
#define __CPP_TRANSPORT_REPO_STORE_OUTPUT_FAIL            "Repository error: failed to store output record (backend code="

#define __CPP_TRANSPORT_REPO_STORE_RESERVE_FAIL           "Repository error: failed to store content group name reservation (backend code="
#define __CPP_TRANSPORT_REPO_COMMIT_OUTPUT_NOT_RESERVED   "Repository error: attempt to commit output group for unreserved name"

#define __CPP_TRANSPORT_BADLY_FORMED_RANGE                "Repository error: badly formed JSON document for 'range' group"
#define __CPP_TRANSPORT_BADLY_FORMED_PARAMS               "Repository error: badly formed JSON for 'parameters' group"
#define __CPP_TRANSPORT_BADLY_FORMED_ICS                  "Repository error: badly formed JSON for 'initial_conditions' group"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_SQLITE_H
