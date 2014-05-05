//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H


#define __CPP_TRANSPORT_REPO_MISSING_ROOT     "Repository error: Root directory does not exist"
#define __CPP_TRANSPORT_REPO_MISSING_ENV      "Repository error: Environment directory not present"
#define __CPP_TRANSPORT_REPO_MISSING_CNTR     "Repository error: Containers directory not present"
#define __CPP_TRANSPORT_REPO_MISSING_PACKAGES "Repository error: Container for model definitions not present or inaccessible"
#define __CPP_TRANSPORT_REPO_MISSING_INTGRTNS "Repository error: Container for integration definitions not present or inaccessible"
#define __CPP_TRANSPORT_REPO_PACKAGE_EXISTS   "Repository error: Attempt to insert duplicate model specification"
#define __CPP_TRANSPORT_REPO_INTGRTN_EXISTS   "Repository error: Attempt to insert duplicate integration specification"
#define __CPP_TRANSPORT_REPO_INSERT_ERROR     "Repository error: Error while inserting document into repository (backend code="
#define __CPP_TRANSPORT_REPO_QUERY_ERROR      "Repository error: Error while querying document from repository (backend code="
#define __CPP_TRANSPORT_REPO_ROOT_EXISTS      "Repository error: Root directory already exists"
#define __CPP_TRANSPORT_REPO_WRITE_SLAVE      "Internal error: Attempt to write to repository from MPI slave process"
#define __CPP_TRANSPORT_REPO_NOT_SET          "Internal error: Attempt to access repository when unset in 'task_manager'"
#define __CPP_TRANSPORT_REPO_NULL_MODEL       "Internal error: Null model when attempting to write to repository"
#define __CPP_TRANSPORT_REPO_NULL_TASK        "Internal error: Null task when attempting to add output to repository"
#define __CPP_TRANSPORT_REPO_MISSING_TASK     "Repository error: Could not find specified task"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL    "Repository error: Coult not find specified package"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_A  "Repository error: Could not find model"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_B  "corresponding to specified task"
#define __CPP_TRANSPORT_REPO_SKIPPING_TASK    "; skipping this task"
#define __CPP_TRANSPORT_REPO_MISSING_DATA     "Repository error: could not find data block"
#define __CPP_TRANSPORT_REPO_NONE             "Nothing to do: no repository specified"
#define __CPP_TRANSPORT_RUN_REPAIR            "; consider checking database integrity"
#define __CPP_TRANSPORT_REPO_FOR_TASK         "for task"
#define __CPP_TRANSPORT_UNKNOWN_JOB_TYPE      "Internal error: Unexpected job type in 'task_manager'"
#define __CPP_TRANSPORT_REPO_EDITTIME_FAIL    "Repository error: Failed to set last-edit time"
#define __CPP_TRANSPORT_REPO_INSERT_FAIL      "Repository error: Could not log new integration output in the database"
#define __CPP_TRANSPORT_REPO_NO_SERIALNO      "Repository error: Could not allocate serial number for integration output group"
#define __CPP_TRANSPORT_REPO_INTCTR_UNSETHANDLE "Repository error: Attempt to read an unset 'data_manager' data-container handle in 'integration_container'"
#define __CPP_TRANSPORT_REPO_INTCTR_UNSETTASK   "Repository error: Attempt to read an unset 'data_manager' taskfile handle in 'integration_container'"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
