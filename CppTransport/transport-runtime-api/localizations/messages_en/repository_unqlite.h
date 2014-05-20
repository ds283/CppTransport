//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H


#define __CPP_TRANSPORT_REPO_FAIL_PKG_OPEN                "Repository error: failed to open packages container"
#define __CPP_TRANSPORT_REPO_FAIL_TASKS_OPEN              "Repository error: failed to open tasks container"
#define __CPP_TRANSPORT_REPO_FAIL_DERIVED_PRODUCTS_OPEN   "Repository error: failed to open derived products container"

#define __CPP_TRANSPORT_REPO_DATABASES_OPEN               "Repository error: UnQLite database handles unexpectedly open"
#define __CPP_TRANSPORT_REPO_DATABASES_CLOSED             "Repository error: UnQLite database handles unexpectedly closed"
#define __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN           "Repository error: UnQLite database handles closed, but should be open"

#define __CPP_TRANSPORT_REPO_MISSING_COLLECTION           "Repository error: missing UnQLite database collection"

#define __CPP_TRANSPORT_REPO_JSON_FAIL                    "Repository error: failure when extracting JSON fields from unqlite"
#define __CPP_TRANSPORT_REPO_JSON_NO_ID                   "Repository error: missing UnQLite ID when deserializing JSON block"
#define __CPP_TRANSPORT_REPO_TASK_EXTRACT_FAIL            "Repository error: failure when extracting task record from JSON array"
#define __CPP_TRANSPORT_REPO_PKG_EXTRACT_FAIL             "Repository error: failure when extracting package record from JSON array"
#define __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXTRACT_FAIL "Repository error: failure when extracting derived package record from JSON array"

#define __CPP_TRANSPORT_BADLY_FORMED_RANGE                "Repository error: badly formed JSON document for 'range' group"
#define __CPP_TRANSPORT_BADLY_FORMED_PARAMS               "Repository error: badly formed JSON for 'parameters' group"
#define __CPP_TRANSPORT_BADLY_FORMED_ICS                  "Repository error: badly formed JSON for 'initial_conditions' group"
#define __CPP_TRANSPORT_BADLY_FORMED_TASK                 "Repository error: badly formed JSON for 'task' group"

#define __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN      "Repository error: task named in output task is not of integration-type"

#define __CPP_TRANSPORT_REPO_NULL_DERIVED_PRODUCT         "Repository error: null derived product specifier"

#define __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS    "Repository error: no matching output groups for derived product"
#define __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS_A  "task"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
