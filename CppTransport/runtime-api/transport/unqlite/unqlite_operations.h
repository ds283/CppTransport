//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __unqlite_operations_H_
#define __unqlite_operations_H_


#include "transport/models/model.h"

#include "unqlite/unqlite.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"



#define __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF   "env"
#define __CPP_TRANSPORT_REPO_CONTAINERS_LEAF    "containers"
#define __CPP_TRANSPORT_REPO_INTOUTPUT_LEAF     "output"
#define __CPP_TRANSPORT_REPO_LOGDIR_LEAF        "logs"
#define __CPP_TRANSPORT_REPO_TEMPDIR_LEAF       "tempfiles"
#define __CPP_TRANSPORT_REPO_TASKFILE_LEAF      "tasks.sqlite"
#define __CPP_TRANSPORT_REPO_GROUP_STEM         "group"
#define __CPP_TRANSPORT_REPO_DATABASE_LEAF      "integration.sqlite"
#define __CPP_TRANSPORT_CNTR_PACKAGES_LEAF      "packages.unqlite"
#define __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF  "integrations.unqlite"

#define __CPP_TRANSPORT_NODE_PACKAGE_ROOT       "package-specification"
#define __CPP_TRANSPORT_NODE_PACKAGE_MODELUID   "package-model-uid"
#define __CPP_TRANSPORT_NODE_PACKAGE_DATA       "package-data"
#define __CPP_TRANSPORT_NODE_PACKAGE_ICS        "package-ics"

#define __CPP_TRANSPORT_NODE_PKGDATA_NAME       "model-name"
#define __CPP_TRANSPORT_NODE_PKGDATA_AUTHOR     "model-author"
#define __CPP_TRANSPORT_NODE_PKGDATA_TAG        "model-tag"
#define __CPP_TRANSPORT_NODE_PKGDATA_CREATED    "creation-time"
#define __CPP_TRANSPORT_NODE_PKGDATA_EDITED     "last-edit-time"
#define __CPP_TRANSPORT_NODE_PKGDATA_RUNTIMEAPI "runtime-api-version"

#define __CPP_TRANSPORT_NODE_TWOPF_ROOT         "twopf-specification"
#define __CPP_TRANSPORT_NODE_THREEPF_ROOT       "threepf-specification"
#define __CPP_TRANSPORT_NODE_INTGRTN_TASK       "integration-task"
#define __CPP_TRANSPORT_NODE_INTGRTN_DATA       "integration-data"
#define __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT     "integration-output"

#define __CPP_TRANSPORT_NODE_INTDATA_PACKAGE    "package-name"
#define __CPP_TRANSPORT_NODE_INTDATA_CREATED    "creation-time"
#define __CPP_TRANSPORT_NODE_INTDATA_EDITED     "last-edit-time"
#define __CPP_TRANSPORT_NODE_INTDATA_RUNTIMEAPI "runtime-api-version"

#define __CPP_TRANSPORT_NODE_INTOUTPUT_GROUP    "output-group"
#define __CPP_TRANSPORT_NODE_OUTPUT_ID          "serial-number"
#define __CPP_TRANSPORT_NODE_OUTPUT_PATH        "output-path"
#define __CPP_TRANSPORT_NODE_OUTPUT_DATABASE    "database-path"
#define __CPP_TRANSPORT_NODE_OUTPUT_CREATED     "creation-time"
#define __CPP_TRANSPORT_NODE_OUTPUT_LOCKED      "locked"
#define __CPP_TRANSPORT_NODE_OUTPUT_NOTES       "notes"
#define __CPP_TRANSPORT_NODE_OUTPUT_BACKEND     "backend"

#define __CPP_TRANSPORT_VALUE_TRUE              "true"
#define __CPP_TRANSPORT_VALUE_FALSE             "false"


#define __CPP_TRANSPORT_UPDATE_ALIAS            "updatectr"


namespace transport
  {

    // utility functions for manipulating XML documents, but which don't need to be methods for a repository object
    // embedding in an unnamed namespace makes these functions invisible outside this translation unit
    namespace unqlite_operations
      {

      }   // unqlite_operations

  }   // transport

#endif //__unqlite_operations_H_
