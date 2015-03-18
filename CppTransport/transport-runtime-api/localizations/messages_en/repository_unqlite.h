//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H


#define __CPP_TRANSPORT_REPO_FAIL_DATABASE_OPEN           "Repository error: failed to open repository database"

#define __CPP_TRANSPORT_REPO_DATABASES_OPEN               "Repository error: UnQLite database handles unexpectedly open"
#define __CPP_TRANSPORT_REPO_DATABASES_CLOSED             "Repository error: UnQLite database handles unexpectedly closed"
#define __CPP_TRANSPORT_REPO_DATABASES_NOT_OPEN           "Repository error: UnQLite database handles closed, but should be open"

#define __CPP_TRANSPORT_REPO_MISSING_COLLECTION           "Repository error: missing UnQLite database collection"

#define __CPP_TRANSPORT_REPO_JSON_FAIL                    "Repository error: failure when extracting JSON fields from unqlite"
#define __CPP_TRANSPORT_REPO_JSON_NO_ID                   "Repository error: missing UnQLite ID when deserializing JSON block"

#define __CPP_TRANSPORT_BADLY_FORMED_RANGE                "Repository error: badly formed JSON document for 'range' group"
#define __CPP_TRANSPORT_BADLY_FORMED_PARAMS               "Repository error: badly formed JSON for 'parameters' group"
#define __CPP_TRANSPORT_BADLY_FORMED_ICS                  "Repository error: badly formed JSON for 'initial_conditions' group"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_UNQLITE_H
