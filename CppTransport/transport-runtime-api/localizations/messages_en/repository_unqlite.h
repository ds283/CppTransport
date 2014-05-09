//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H


#define __CPP_TRANSPORT_REPO_FAIL_PKG          "Repository error: failed to open package container"
#define __CPP_TRANSPORT_REPO_FAIL_INTGN        "Repository error: failed to open integration container"

#define __CPP_TRANSPORT_REPO_MISSING_DB        "Repository error: unqlite database handles are null"

#define __CPP_TRANSPORT_REPO_MISSING_CLCTN     "Repository error: missing unqlite database collection"

#define __CPP_TRANSPORT_REPO_JSON_FAIL         "Repository error: failure when extracting JSON fields from unqlite"
#define __CPP_TRANSPORT_REPO_JSON_NO_ID        "Repository error: missing UnQLite ID when deserializing JSON block"
#define __CPP_TRANSPORT_REPO_TASK_EXTRACT_FAIL "Repository error: failure when extracting task record from JSON array"
#define __CPP_TRANSPORT_REPO_PKG_EXTRACT_FAIL  "Repository error: failure when extracting package record from JSON array"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
