//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __unqlite_operations_H_
#define __unqlite_operations_H_


#include "transport/models/model.h"

#include "transport/messages.h"
#include "transport/exceptions.h"

#include "unqlite/unqlite.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"



#define __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION "default-packages"
#define __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION   "twopf-tasks"
#define __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION "threepf-tasks"

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

// #define __CPP_TRANSPORT_NODE_PACKAGE_ROOT       "package-specification"
#define __CPP_TRANSPORT_NODE_PACKAGE_NAME       "package-name"
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

#define __CPP_TRANSPORT_NODE_INTGRTN_NAME       "integration-name"
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

        // default consumer for Jx9 virtual machine output -- throw it as an exception
        int default_unqlite_consumer(const void* data, unsigned int length, void* handle)
          {
            std::string jx9_msg(data, length);

            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_VM_OUPTUT << " '" << jx9_msg << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());

            return(UNQLITE_OK);
          }


        // execute a Jx9 script
        void exec_jx9(unqlite* db, const std::string& jx9)
          {
            // Compile the script
            unqlite_vm *vm;
            int err;

            std::cerr << std::endl << "Executing Jx9 script:" << std::endl
                      << jx9 << std::endl;

            err = unqlite_compile(db, jx9.c_str(), jx9.length(), &vm);

            if(err != UNQLITE_OK)
              {
                if(err == UNQLITE_COMPILE_ERR)
                  {
                    const char *zbuf;
                    int length;

                    // extract compile-time error log
                    unqlite_config(db, UNQLITE_CONFIG_JX9_ERR_LOG, &zbuf, &length);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE_LOG;
                    if(length > 0) msg << " '" << zbuf << "'";
                    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE << err << ")";
                    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
              }

            // install a VM output consumer
            err = unqlite_vm_config(vm, UNQLITE_VM_CONFIG_OUTPUT, default_unqlite_consumer, 0);

            if(err != UNQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNQLITE_FAIL_REGISTER_CONSUMER;
                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            // execute the script
            err = unqlite_vm_exec(vm);

            if(err != UNQLITE_OK)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNQLITE_FAIL_EXECUTE_SCRIPT;
                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            // release the virtual machine
            unqlite_vm_release(vm);
          }


        // create a collection within a specified database
        void create_collection(unqlite* db, const std::string& collection)
          {
            std::ostringstream jx9;

            jx9 << "if( !db_exists('" << collection << "') )"
                << "  {"
                << "    $rc = db_create('" << collection << ""');"
                << "    if ( !$rc )"
                << "      {"
                << "        print db_errlog();"
                << "        return;"
                << "      }"
                << "  }";

            exec_jx9(db, jx9.str());
          }


        // ensure a collection is present within a specified database
        void ensure_collection(unqlite* db, const std::string& collection)
          {
            std::ostringstream jx9;

            jx9 << "if( !db_exists('" << collection << "') )"
                << "  {"
                << "    print '" << __CPP_TRANSPORT_REPO_MISSING_CLCTN << " " << collection << "';"
                << "  }";

            exec_jx9(db, jx9.str());
          }


        // store a record within a specified collection
        void store(unqlite* db, const std::string& collection, const std::string& record)
          {
            // first, ensure the named collection is present in the database
            ensure_collection(db, collection);

            std::ostringstream jx9;

            // insert this record in the database; commit as soon as we are done
            // for batch insert, it would be preferable to wait until the end of the
            // batch before committing, but we don't expect the repository to be used that way
            jx9 << "$rec = [ " << record << " ];"
                << "$rc  = db_store('" << collection << "', $rec);"
                << "if ( !$rc )"
                << "  {"
                << "    print '" << __CPP_TRANSPORT_REPO_INSERT_ERROR "', $rc, ')';"
                << "  }"
                << "$rc = db_commit();"
                << "if ( !$rc )"
                << "  {"
                << "    print '" << __CPP_TRANSPORT_REPO_INSERT_ERROR "', $rc, ')';"
                << "  }";

            exec_jx9(db, jx9.str());
          }


      }   // unqlite_operations

  }   // transport

#endif //__unqlite_operations_H_
