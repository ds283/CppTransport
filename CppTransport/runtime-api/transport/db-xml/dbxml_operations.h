//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __db_xml_repository_H_
#define __db_xml_repository_H_


#include "transport/models/model.h"

#include "transport/db-xml/db_xml.h"

#include "dbxml/db.h"
#include "dbxml/dbxml/DbXml.hpp"

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
#define __CPP_TRANSPORT_CNTR_PACKAGES_LEAF      "packages.dbxml"
#define __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF  "integrations.dbxml"

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
    namespace dbxml_operations
      {

        // Write the data block for a package document
        template <typename number>
        void write_package_data_block(DbXml::XmlEventWriter& writer, const model<number>* m)
          {
            std::string text_node;

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_DATA), nullptr, nullptr, 0, false);

            // model name
            text_node = m->get_name();
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_NAME), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_NAME), nullptr, nullptr);

            // model author
            text_node = m->get_author();
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_AUTHOR), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_AUTHOR), nullptr, nullptr);

            // model tag
            text_node = m->get_tag();
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_TAG), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_TAG), nullptr, nullptr);

            // creation time
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            text_node = boost::posix_time::to_simple_string(now);
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_CREATED), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_CREATED), nullptr, nullptr);

            // last edited time -- here, the same as the creation time
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_EDITED), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_EDITED), nullptr, nullptr);

            // runtime-api version used to create the task
            text_node = boost::lexical_cast<std::string>(__CPP_TRANSPORT_RUNTIME_API_VERSION);
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_RUNTIMEAPI), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PKGDATA_RUNTIMEAPI), nullptr, nullptr);

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_DATA), nullptr, nullptr);
          }


        // Write the data block for an integration document
        inline void write_integration_data_block(DbXml::XmlEventWriter& writer, const std::string& package_name)
          {
            std::string text_node;

            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_DATA), nullptr, nullptr, 0, false);

            // parent package name
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_PACKAGE), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(package_name.c_str()), package_name.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_PACKAGE), nullptr, nullptr);

            // creation time
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            text_node = boost::posix_time::to_simple_string(now);
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_CREATED), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_CREATED), nullptr, nullptr);

            // last edited time -- here, the same as the creation time
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_EDITED), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_EDITED), nullptr, nullptr);

            // runtime-api version used to create the integration
            text_node = boost::lexical_cast<std::string>(__CPP_TRANSPORT_RUNTIME_API_VERSION);
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_RUNTIMEAPI), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(text_node.c_str()), text_node.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTDATA_RUNTIMEAPI), nullptr, nullptr);

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_DATA), nullptr, nullptr);
          }


        // Use an explicit name to get the XML document for an integration task
        inline DbXml::XmlDocument get_integration_by_name(const std::string& name, DbXml::XmlContainer& ctr)
          {
            DbXml::XmlDocument integration_document;
            try
              {
                // find XML document corresponding to our integration name; will throw a DOCUMENT_NOT_FOUND EXCEPTION if it does not exist
                integration_document = ctr.getDocument(name);
              }
            catch (DbXml::XmlException& xe)
              {
                if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
                  {
                    throw runtime_exception(runtime_exception::TASK_NOT_FOUND, name);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_QUERY_ERROR << xe.getExceptionCode() << ": " << xe.what() << "')";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
                  }
              }

            return(integration_document);
          }


        // Use an explicit name to get the XML document for a package
        inline DbXml::XmlDocument get_package_by_name(const std::string& name, DbXml::XmlContainer& ctr)
          {
            DbXml::XmlDocument package_document;
            try
              {
                // find XML document corresponding to our model name; will throw a DOCUMENT_NOT_FOUND EXCEPTION if it does not exist
                package_document = ctr.getDocument(name);
              }
            catch (DbXml::XmlException& xe)
              {
                if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
                  {
                    throw runtime_exception(runtime_exception::MODEL_NOT_FOUND, name);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_QUERY_ERROR << xe.getExceptionCode() << ": " << xe.what() << "')";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
                  }
              }

            return(package_document);
          }


        // Build an initial_conditions<> object from an ics XML block
        template <typename number>
        initial_conditions<number> build_ics_object(DbXml::XmlManager* mgr, DbXml::XmlValue& ics_group,
                                                    const std::string& package_name, model<number>* model)
          {
            assert(mgr != nullptr);

            // obtain parameter and initial-conditions validators from this model
            typename parameters<number>::params_validator p_validator = model->params_validator_factory();
            typename initial_conditions<number>::ics_validator ics_validator = model->ics_validator_factory();

            // search for the embedded parameters<number> serialization and construct a parameters<number> object from it
            number Mp;
            std::vector<number> params;
            std::vector<std::string> param_names;
            parameters_dbxml::extract(mgr, ics_group, Mp, params, param_names, model->get_param_names());

            parameters<number> parameters(Mp, params, param_names, p_validator);

            // now construct an initial_conditions<number> object
            double Nstar;
            std::vector<number> coords;
            std::vector<std::string> coord_names;
            ics_dbxml::extract(mgr, ics_group, Nstar, coords, coord_names, model->get_state_names());

            initial_conditions<number> ics = initial_conditions<number>(package_name, parameters, coords, coord_names, Nstar, ics_validator);

            return(ics);
          }


        // Extract package name from an integration block
        inline std::string get_package_from_integration(DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& name)
          {
            assert(mgr != nullptr);

            // run a query to find the XML record for the corresponding initial conditions/parameter package
            std::string query = dbxml_helper::xquery::value_self(__CPP_TRANSPORT_NODE_INTGRTN_DATA, __CPP_TRANSPORT_NODE_INTDATA_PACKAGE);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value,
                                                                     std::string(__CPP_TRANSPORT_BADLY_FORMED_INTGRTN) + " '" + name + "'");

            return(node.asString());
          }


        // Extract uid name from a package block
        inline std::string get_model_uid_from_package(DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& name)
          {
            assert(mgr != nullptr);

            std::string query = dbxml_helper::xquery::value_self(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value,
                                                                     std::string(__CPP_TRANSPORT_BADLY_FORMED_PACKAGE) + " '" + name + "'");

            return(node.asString());
          }


        // Extract an ics group from a package block
        inline DbXml::XmlValue get_ics_group(DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& name)
          {
            assert(mgr != nullptr);

            std::string query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_PACKAGE_ICS);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value,
                                                                     std::string(__CPP_TRANSPORT_BADLY_FORMED_PACKAGE) + " '" + name + "'");

            return(node);
          }


        // Build a task<> object from an task XML schema
        template <typename number>
        task<number>* build_task_object(DbXml::XmlManager* mgr, DbXml::XmlValue& integration, const initial_conditions<number>& ics,
                                        model<number>* model, const std::string& task_name)
          {
            assert(mgr != nullptr);

            task<number>* task = nullptr;

            // first job is to decide what sort of integration we have - a twopf or a threepf?
            if(!(integration.getType() == DbXml::XmlValue::NODE && integration.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            DbXml::XmlValue integration_root = integration.getFirstChild();
            if(!(integration_root.getType() == DbXml::XmlValue::NODE && integration_root.getNodeType() == DbXml::XmlValue::ELEMENT_NODE)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            // now pull out the integration-task schema
            std::string query_task = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_INTGRTN_TASK);

            DbXml::XmlValue task_node = dbxml_helper::extract_single_node(query_task, mgr, integration_root, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            // the name of this node should tell us what we are dealing with
            if(integration_root.getNodeName() == __CPP_TRANSPORT_NODE_TWOPF_ROOT)
              {
                task = task_dbxml::extract_twopf_task(mgr, task_node, task_name, ics, model->kconfig_kstar_factory());
              }
            else if(integration_root.getNodeName() == __CPP_TRANSPORT_NODE_THREEPF_ROOT)
              {
                task = task_dbxml::extract_threepf_task(mgr, task_node, task_name, ics, model->kconfig_kstar_factory());
              }
            else throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            assert(task != nullptr);

            return(task);
          }


        // insert a new output record with the data block of an integration record
        template <typename number>
        typename repository<number>::integration_container insert_integration_output(DbXml::XmlManager* mgr, const std::string& task_name,
                                                                                     const boost::filesystem::path& root_path,
                                                                                     const boost::filesystem::path& container_path,
                                                                                     const std::string& backend, unsigned int worker)
          {
            assert(mgr != nullptr);

            DbXml::XmlContainerConfig cfg;
            cfg.setTransactional(true);
            DbXml::XmlContainer xml_ctr = mgr->openContainer(container_path.string().c_str(), cfg);

            DbXml::XmlTransaction txn = mgr->createTransaction();
            try
              {
                DbXml::XmlValue doc = get_integration_by_name(task_name, xml_ctr);

                if(!(doc.getType() == DbXml::XmlValue::NODE && doc.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);
                DbXml::XmlValue doc_root = doc.getFirstChild();

                // find all serial numbers for the output groups associated with this task
                std::string query = dbxml_helper::xquery::value_self(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT, __CPP_TRANSPORT_NODE_INTOUTPUT_GROUP, __CPP_TRANSPORT_NODE_OUTPUT_ID);

                DbXml::XmlQueryContext ctx = mgr->createQueryContext();
                DbXml::XmlQueryExpression expr = mgr->prepare(txn, query, ctx);
                DbXml::XmlResults results = expr.execute(txn, doc_root, ctx);

                // allocate a serial number

                unsigned int output_groups = results.size();  // first available identifier should fall in the range <= output_groups+1
                std::vector<unsigned int> used_serial_numbers;
                DbXml::XmlValue serial_node;
                while(results.next(serial_node))
                  {
                    used_serial_numbers.push_back(boost::lexical_cast<unsigned int>(serial_node.asString()));
                  }

                unsigned int serial_number;
                for(serial_number = 0; (serial_number <= output_groups) && (std::find(used_serial_numbers.begin(), used_serial_numbers.end(), serial_number) != used_serial_numbers.end()); serial_number++)
                  ;

                if(serial_number > output_groups) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NO_SERIALNO);

                // construct paths for the various output files and directories
                std::ostringstream output_leaf;
                output_leaf << __CPP_TRANSPORT_REPO_GROUP_STEM << serial_number;
                boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_INTOUTPUT_LEAF) / task_name / output_leaf.str();
                boost::filesystem::path sql_path    = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;
                boost::filesystem::path log_path    = output_path / __CPP_TRANSPORT_REPO_LOGDIR_LEAF;
                boost::filesystem::path task_path   = output_path / __CPP_TRANSPORT_REPO_TASKFILE_LEAF;
                boost::filesystem::path temp_path   = output_path / __CPP_TRANSPORT_REPO_TEMPDIR_LEAF;

                // insert new output group in the repository database

                // generate XML for the group
                // there seems no way to do this natively with the DBXML API ...
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                std::string now_string = boost::posix_time::to_simple_string(now);

                std::ostringstream group_node;
                group_node
                  << "<"  << __CPP_TRANSPORT_NODE_INTOUTPUT_GROUP << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_ID << ">"
                  << serial_number
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_ID << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_BACKEND << ">"
                  << backend
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_BACKEND << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_PATH << ">"
                  << output_path.string()
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_PATH << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_DATABASE << ">"
                  << sql_path.string()
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_DATABASE << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_CREATED << ">"
                  << now_string
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_CREATED << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_LOCKED << ">"
                  << __CPP_TRANSPORT_VALUE_FALSE
                  << "</" << __CPP_TRANSPORT_NODE_OUTPUT_LOCKED << ">"
                  << "<"  << __CPP_TRANSPORT_NODE_OUTPUT_NOTES << "/>"
                  << "</" << __CPP_TRANSPORT_NODE_INTOUTPUT_GROUP << ">";

                // to reference this container, we need to add an alias -- this is a feature
                // of Berkeley DB XML
                // see https://community.oracle.com/message/2191426
                // and http://download.oracle.com/otndocs/products/berkeleydb/html/xml/2.3.10.html
                // especially 2.3.8 change #13881 ('changes that require modification' no. 3)
                // and #14234. #14234 suggests that we could use //// to identify an absolute UNIX
                // path, but the community.oracle exchange above suggests that aliases are
                // the preferred solution
                xml_ctr.addAlias(output_leaf.str());
                std::string doc_selector = dbxml_helper::xquery::document(output_leaf.str(), task_name);

                std::string update = dbxml_helper::xquery::insert(doc_selector, group_node.str(), dbxml_helper::xquery::as_last, doc_root.getNodeName(), __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);
                DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
                DbXml::XmlQueryExpression update_expr = mgr->prepare(txn, update, update_ctx);
                DbXml::XmlResults update_results = update_expr.execute(txn, update_ctx, 0);

                txn.commit();

                // create directories
                boost::filesystem::create_directories(root_path / output_path);
                boost::filesystem::create_directories(root_path / log_path);
                boost::filesystem::create_directories(root_path / temp_path);

                return typename repository<number>::integration_container(root_path/output_path, root_path/sql_path, root_path/log_path, root_path/task_path, root_path/temp_path, serial_number, worker);
              }
            catch (DbXml::XmlException& xe)
              {
                txn.abort();

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_FAIL << " (DBXML code=" << xe.getExceptionCode() << ": " << xe.what() << ")";
                throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }


        // Update the last-edited time in a package document
        inline void update_package_edit_time(DbXml::XmlManager* mgr, const std::string& name, const boost::filesystem::path& container_path)
          {
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            std::string text_node = boost::posix_time::to_simple_string(now);

            // open container
            DbXml::XmlContainer xml_ctr = mgr->openContainer(container_path.string().c_str());

            DbXml::XmlTransaction txn = mgr->createTransaction();
            try
              {
                // must set an alias in order to refer to this container later via doc()
                xml_ctr.addAlias(__CPP_TRANSPORT_UPDATE_ALIAS);

                // build replacement statemeent
                std::string doc_selector = dbxml_helper::xquery::document(__CPP_TRANSPORT_UPDATE_ALIAS, name);
                std::string update = dbxml_helper::xquery::update(doc_selector, text_node, __CPP_TRANSPORT_NODE_PACKAGE_ROOT, __CPP_TRANSPORT_NODE_PACKAGE_DATA, __CPP_TRANSPORT_NODE_PKGDATA_EDITED);

                DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
                DbXml::XmlQueryExpression update_expr = mgr->prepare(txn, update, update_ctx);

                DbXml::XmlResults update_results = update_expr.execute(txn, update_ctx, 0);
                txn.commit();
              }
            catch (DbXml::XmlException& xe)
              {
                // rollback the transaction
                txn.abort();

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_EDITTIME_FAIL << " (DBXML code=" << xe.getExceptionCode() << ": " << xe.what() << ")";
                throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }


        // Update the last-edited time in an integration document
        inline void update_integration_edit_time(DbXml::XmlManager* mgr, const std::string& name, const boost::filesystem::path& container_path)
          {
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            std::string text_node = boost::posix_time::to_simple_string(now);

            // open container
            DbXml::XmlContainer xml_ctr = mgr->openContainer(container_path.string().c_str());

            DbXml::XmlTransaction txn = mgr->createTransaction();
            try
              {
                // must set an alias in order to refer to this container later via doc()
                xml_ctr.addAlias(__CPP_TRANSPORT_UPDATE_ALIAS);

                // extract document, in order to find name of root node
                DbXml::XmlValue doc = get_integration_by_name(name, xml_ctr);
                if(!(doc.getType() == DbXml::XmlValue::NODE && doc.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);
                DbXml::XmlValue doc_root = doc.getFirstChild();

                // build replacement statement
                std::string doc_selector = dbxml_helper::xquery::document(__CPP_TRANSPORT_UPDATE_ALIAS, name);
                std::string update = dbxml_helper::xquery::update(doc_selector, text_node, doc_root.getNodeName(), __CPP_TRANSPORT_NODE_INTGRTN_DATA, __CPP_TRANSPORT_NODE_INTDATA_EDITED);

                DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
                DbXml::XmlQueryExpression update_expr = mgr->prepare(txn, update, update_ctx);

                DbXml::XmlResults update_results = update_expr.execute(txn, update_ctx, 0);
                txn.commit();
              }
            catch (DbXml::XmlException& xe)
              {
                txn.abort();

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_EDITTIME_FAIL << " (DBXML code=" << xe.getExceptionCode() << ": " << xe.what() << ")";
                throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }


      }   // namespace dbxml_operations

  }   // namespace transport


#endif //__db_xml_repository_H_
