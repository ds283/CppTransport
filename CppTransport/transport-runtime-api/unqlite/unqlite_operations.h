//
// Created by David Seery on 04/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __unqlite_operations_H_
#define __unqlite_operations_H_


#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


//#define __CPP_TRANSPORT_PRINT_JX9_SCRIPTS


extern "C"
{
#include "unqlite/unqlite.h"
}


#define __CPP_TRANSPORT_UNQLITE_PACKAGE_COLLECTION "default-packages"
#define __CPP_TRANSPORT_UNQLITE_TWOPF_COLLECTION   "twopf-tasks"
#define __CPP_TRANSPORT_UNQLITE_THREEPF_COLLECTION "threepf-tasks"
#define __CPP_TRANSPORT_UNQLITE_OUTPUT_COLLECTION  "output-tasks"

#define __CPP_TRANSPORT_REPO_CONTAINERS_LEAF       "containers"
#define __CPP_TRANSPORT_REPO_INTOUTPUT_LEAF        "output"
#define __CPP_TRANSPORT_REPO_LOGDIR_LEAF           "logs"
#define __CPP_TRANSPORT_REPO_TEMPDIR_LEAF          "tempfiles"
#define __CPP_TRANSPORT_REPO_TASKFILE_LEAF         "tasks.sqlite"
#define __CPP_TRANSPORT_REPO_GROUP_STEM            "group"
#define __CPP_TRANSPORT_REPO_DATABASE_LEAF         "integration.sqlite"
#define __CPP_TRANSPORT_CNTR_PACKAGES_LEAF         "packages.unqlite"
#define __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF     "integrations.unqlite"

#define __CPP_TRANSPORT_NODE_PACKAGE_NAME          "package-name"
#define __CPP_TRANSPORT_NODE_PACKAGE_MODELUID      "package-model-uid"
#define __CPP_TRANSPORT_NODE_PACKAGE_DATA          "package-data"
#define __CPP_TRANSPORT_NODE_PACKAGE_ICS           "package-ics"

#define __CPP_TRANSPORT_NODE_PKG_DATA_NAME         "model-name"
#define __CPP_TRANSPORT_NODE_PKG_DATA_AUTHOR       "model-author"
#define __CPP_TRANSPORT_NODE_PKG_DATA_TAG          "model-tag"
#define __CPP_TRANSPORT_NODE_PKG_DATA_CREATED      "creation-time"
#define __CPP_TRANSPORT_NODE_PKG_DATA_EDITED       "last-edit-time"
#define __CPP_TRANSPORT_NODE_PKG_DATA_RUNTIMEAPI   "runtime-api-version"

#define __CPP_TRANSPORT_NODE_TASK_NAME             "task-name"
#define __CPP_TRANSPORT_NODE_TASK_DATA_PACKAGE     "package-name"
#define __CPP_TRANSPORT_NODE_TASK_DATA_CREATED     "creation-time"
#define __CPP_TRANSPORT_NODE_TASK_DATA_EDITED      "last-edit-time"
#define __CPP_TRANSPORT_NODE_TASK_DATA_RUNTIMEAPI  "runtime-api-version"

#define __CPP_TRANSPORT_NODE_TASK_DATA             "task-data"

#define __CPP_TRANSPORT_NODE_INTGRTN_TASK          "integration-task"
#define __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT        "integration-output"

#define __CPP_TRANSPORT_NODE_OUTPUT_TASK           "output-task"

#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_ID        "serial-number"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_PATH      "output-path"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_DATABASE  "database-path"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_CREATED   "creation-time"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED    "locked"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES     "notes"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTE      "note"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS      "tags"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG       "tag"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_BACKEND   "backend"


namespace transport
  {

    // utility functions for manipulating XML documents, but which don't need to be methods for a repository object
    // embedding in an unnamed namespace makes these functions invisible outside this translation unit
    namespace unqlite_operations
      {

        // Build a JSON query from a set of fields and a value
        namespace json_query
          {
            typedef enum { equals, greater_than, less_than } query_type;

            std::string stringize_query(query_type type)
              {
                if(type == equals)       return("==");
                if(type == greater_than) return(">");
                if(type == less_than)    return("<");

                return("=");
              }

            // base case: string
            std::string build_query(const query_type type, const std::string& value, const std::string& field)
              {
                std::ostringstream r;

                r << "'" << field << "' " << stringize_query(type) << " '" << value << "'";

                return(r.str());
              }

            // base case: unsigned int
            std::string build_query(const query_type type, const unsigned int& value, const std::string& field)
              {
                std::ostringstream r;

                r << "'" << field << "' " << stringize_query(type) << " " << boost::lexical_cast<std::string>(value);

                return(r.str());
              }

            // base case: double
            std::string build_query(const query_type type, const double& value, const std::string& field)
              {
                std::ostringstream r;

                r << "'" << field << "' " << stringize_query(type) << " " << boost::lexical_cast<std::string>(value);

                return(r.str());
              }

            // base case: bool
            std::string build_query(const query_type type, const bool& value, const std::string& field)
              {
                std::ostringstream r;

                r << "'" << field << "' " << stringize_query(type) << " " << boost::lexical_cast<std::string>(value);

                return(r.str());
              }

            // inductive case
            template <typename T, typename... other_fields>
            std::string build_query(query_type type, const T& value, std::string field, other_fields... more_fields)
              {
                std::ostringstream r;

		            r << "'" << field << "'." << build_query(type, value, more_fields...);

		            return(r.str());
              }
          }   // namespace json_query


		    // debug helper utility: prints out array contents
		    int array_printer(unqlite_value* key, unqlite_value* value, void* handle)
			    {
				    assert(key != nullptr);
				    assert(value != nullptr);

		        std::cerr << "key   = " << unqlite_value_to_string(key, nullptr)
			                << " (is_integer = " << unqlite_value_is_int(key) << ","
			                << " is_string = " << unqlite_value_is_string(key)
			                << ")" << std::endl;
		        std::cerr << "value = " << unqlite_value_to_string(value, nullptr) << std::endl;

		        return(UNQLITE_OK);
			    }


        // default consumer for Jx9 virtual machine output;
		    // pass everything to the supplied stream buffer.
        int default_unqlite_consumer(const char* data, unsigned int length, void* handle)
          {
		        assert(data != nullptr);
		        assert(handle != nullptr);

            std::string jx9_msg = std::string(data, length);

            std::ostringstream* msg_buf = static_cast<std::ostringstream*>(handle);

            (*msg_buf) << (msg_buf->str().empty() ? "" : ", ") << "'" << jx9_msg << "'";
//            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
//            std::cerr << msg.str() << std::endl;

            return(UNQLITE_OK);
          }


        // execute a Jx9 script, returning the virtual machine for variables to be extracted if necessary
        unqlite_vm* exec_jx9_vm(unqlite* db, const std::string& jx9)
          {
            assert(db != nullptr);

            // Compile the script
            unqlite_vm* vm;
            int err;

		        #ifdef __CPP_TRANSPORT_PRINT_JX9_SCRIPTS
            std::cerr << std::endl << "Executing Jx9 script:" << std::endl
              << jx9 << std::endl << std::endl;
		        #endif

            err = unqlite_compile(db, jx9.c_str(), jx9.length(), &vm);

            if(err != UNQLITE_OK)
              {
                if(err == UNQLITE_COMPILE_ERR)
                  {
                    const char * buf;
                    int length;

                    // extract compile-time error log
                    unqlite_config(db, UNQLITE_CONFIG_JX9_ERR_LOG, &buf, &length);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE_LOG;
                    if(length > 0) msg << " '" << buf << "'" << std::endl;
		                msg << jx9 << std::endl;
                    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE << err << ")" << std::endl;
		                msg << jx9 << std::endl;
                    throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
                  }
              }

            // install a VM output consumer
            std::ostringstream output_buffer;
            err = unqlite_vm_config(vm, UNQLITE_VM_CONFIG_OUTPUT, default_unqlite_consumer, &output_buffer);

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
		        if(!output_buffer.str().empty())
			        {
		            std::ostringstream msg;
			          msg << __CPP_TRANSPORT_UNQLITE_VM_OUPTUT << " " << output_buffer.str();
		            throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
			        }

            return(vm);
          }


        // execute a Jx9 script which does not need output variables to be extracted
        void exec_jx9(unqlite* db, const std::string& jx9)
          {
            assert(db != nullptr);

            unqlite_vm* vm = exec_jx9_vm(db, jx9);

            // release the virtual machine
            unqlite_vm_release(vm);
          }


        // create a collection within a specified database
        void create_collection(unqlite* db, const std::string& collection)
          {
            assert(db != nullptr);

            std::ostringstream jx9;

            jx9 << "if( !db_exists('" << collection << "') )" << std::endl
                << "  {" << std::endl
                << "    $rc = db_create('" << collection << "');" << std::endl
                << "    if ( !$rc )" << std::endl
                << "      {" << std::endl
                << "        print db_errlog();" << std::endl
                << "        return;" << std::endl
                << "      }" << std::endl
                << "  }" << std::endl;

            exec_jx9(db, jx9.str());
          }


        // ensure a collection is present within a specified database
        void ensure_collection(unqlite* db, const std::string& collection)
          {
            assert(db != nullptr);

            std::ostringstream jx9;

            jx9 << "if( !db_exists('" << collection << "') )" << std::endl
                << "  {" << std::endl
                << "    print \"" << __CPP_TRANSPORT_REPO_MISSING_CLCTN << " '" << collection << "'\";" << std::endl
                << "  }" << std::endl;

            exec_jx9(db, jx9.str());
          }


        // store a record within a specified collection
        void store(unqlite* db, const std::string& collection, const std::string& record)
          {
            assert(db != nullptr);

            // first, ensure the named collection is present in the database
            ensure_collection(db, collection);

            std::ostringstream jx9;

            // insert this record in the database; commit as soon as we are done
            // for batch insert, it would be preferable to wait until the end of the
            // batch before committing, but we don't expect the repository to be used that way
            jx9 << "$rec = [ " << record << " ];" << std::endl
                << "$rc  = db_store('" << collection << "', $rec);" << std::endl
                << "if ( !$rc )" << std::endl
                << "  {" << std::endl
                << "    print \"" << __CPP_TRANSPORT_REPO_INSERT_ERROR << "'$rc')\", db_errlog();" << std::endl
                << "  }" << std::endl;
// FIXME: removed explicit commit statement which led to locking problems - might need to revert later, or remove this code if OK
//                << "$rc = db_commit();" << std::endl
//                << "if ( !$rc )" << std::endl
//                << "  {" << std::endl
//                << "    print \"" << __CPP_TRANSPORT_REPO_COMMIT_ERROR << "'$rc')\", db_errlog();" << std::endl
//                << "  }" << std::endl;

            exec_jx9(db, jx9.str());
          }


		    // drop a record within a specified collection
		    void drop(unqlite* db, const std::string& collection, unsigned int unqlite_id)
			    {
		        assert(db != nullptr);

				    // first, ensure the named collection is present in this database
				    ensure_collection(db, collection);

		        std::ostringstream jx9;

						// drop the specified record from the database
				    jx9 <<
// FIXME: removed explicit commit statement which led to locking problems - might need to revert later, or remove this code if OK
//					         "$rc = db_commit();" << std::endl
//					      << "if ( !$rc )" << std::endl
//					      << "  {" << std::endl
//					      << "    print \"" << __CPP_TRANSPORT_REPO_PRECOMMIT_ERROR << "'" << unqlite_id << "')\", db_errlog();" << std::endl
//					      << "  }" << std::endl
//	              <<
			             "$rc = db_drop_record('" << collection << "', " << unqlite_id << ");" << std::endl
					      << "if ( !$rc )" << std::endl
					      << "  {" << std::endl
					      << "    print \"" << __CPP_TRANSPORT_REPO_DELETE_ERROR << "'" << unqlite_id << "')\", db_errlog();" << std::endl
		            << "  }" << std::endl;
// FIXME: removed explicit commit statement which led to locking problems - might need to revert later, or remove this code if OK
//			          << "$rc = db_commit();" << std::endl
//			          << "if ( !$rc )" << std::endl
//						    << "  {" << std::endl
//						    << "    print \"" << __CPP_TRANSPORT_REPO_COMMIT_ERROR << "'" << unqlite_id << "')\", db_errlog();" << std::endl
//			          << "  }" << std::endl;

				    exec_jx9(db, jx9.str());
			    }


        // extract a JSON representation of a record within a specified collection
        template <typename T, typename... fields>
        std::string extract_json(unqlite* db, const std::string& collection, const T& value, fields... field_names)
          {
            assert(db != nullptr);

            std::string query = json_query::build_query(json_query::equals, value, field_names...);

            std::ostringstream jx9;

            jx9 << "$callback = function($rec)" << std::endl
                << "  {" << std::endl
                << "    if( $rec." + query + " )" << std::endl
                << "      { return TRUE; }" << std::endl
                << "    else" << std::endl
                << "      { return FALSE; }" << std::endl
                << "  };" << std::endl
                << "$data = db_fetch_all('" << collection << "', $callback);" << std::endl
	              << "$num_records = count($data);" << std::endl
                << "if( $num_records != 1 )" << std::endl
                << "  {" << std::endl
                << "    print \"" << __CPP_TRANSPORT_UNQLITE_MULTIPLE_JSON << "$num_records)\";" << std::endl
                << "    return;" << std::endl
                << "  }" << std::endl
                << "$ele = reset($data);" << std::endl         // reset() moves array pointer back to the beginning, and returns the value of the first object
                << "$json = json_encode($ele);" << std::endl;

            unqlite_vm *vm = exec_jx9_vm(db, jx9.str());

            // extract value of $json
            unqlite_value* json = unqlite_vm_extract_variable(vm, "json");

            if(json == nullptr)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNQLITE_UNDEFINED_VARIABLE << " '$json'";
                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            std::string json_document = unqlite_value_to_string(json, nullptr);

            // release the virtual machine
            unqlite_vm_release(vm);

            return(json_document);
          }


        // count number of records matching specified criteria
        template <typename T, typename... fields>
        unsigned int query_count(unqlite* db, const std::string& collection, const T& value, fields... field_names)
          {
            assert(db != nullptr);

            std::string query = json_query::build_query(json_query::equals, value, field_names...);

            std::ostringstream jx9;

            jx9 << "$callback = function($rec)" << std::endl
                << "  {" << std::endl
                << "    if( $rec." + query + " )" << std::endl
                << "      { return TRUE; }" << std::endl
                << "    else" << std::endl
                << "      { return FALSE; }" << std::endl
                << "    };" << std::endl
                << "$data = db_fetch_all('" << collection << "', $callback);" << std::endl
                << "$num_records = count($data);" << std::endl;

            unqlite_vm *vm = exec_jx9_vm(db, jx9.str());

            // extract value of $num_records
            unqlite_value* num_records = unqlite_vm_extract_variable(vm, "num_records");

            if(num_records == nullptr)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNQLITE_UNDEFINED_VARIABLE << " '$num_records'";
                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

		        int signed_num = unqlite_value_to_int(num_records);
		        if(signed_num < 0) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);
            unsigned int num = static_cast<unsigned int>(signed_num);

            // release the virtual machine
            unqlite_vm_release(vm);

            return(num);
          }


        // extract array records matching specified criteria
        // also returns virtual machine for further processing; it should be released when processing is complete
        template <typename T, typename... fields>
        unqlite_value* query(unqlite* db, unqlite_vm*& vm, const std::string& collection, const T& value, fields... field_names)
          {
		        assert(db != nullptr);
            assert(vm == nullptr);

            std::string query = json_query::build_query(json_query::equals, value, field_names...);

            std::ostringstream jx9;

            jx9 << "$callback = function($rec)" << std::endl
              << "  {" << std::endl
              << "    if( $rec." + query + " )" << std::endl
              << "      { return TRUE; }" << std::endl
              << "    else" << std::endl
              << "      { return FALSE; }" << std::endl
              << "    };" << std::endl
              << "$data = db_fetch_all('" << collection << "', $callback);" << std::endl;

            vm = exec_jx9_vm(db, jx9.str());

            // extract value of $num_records
            unqlite_value* data = unqlite_vm_extract_variable(vm, "data");

            if(data == nullptr)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_UNQLITE_UNDEFINED_VARIABLE << " '$data'";
                throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            return(data);
          }


      }   // unqlite_operations

  }   // transport

#endif //__unqlite_operations_H_
