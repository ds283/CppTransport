//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <string>
#include <sstream>
#include <stdexcept>

#include "transport/manager/instance_manager.h"
#include "transport/models/model.h"
#include "transport/db-xml/db_xml.h"
#include "transport/tasks/task.h"
#include "transport/version.h"
#include "transport/messages_en.h"
#include "transport/exceptions.h"
#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"

#include "dbxml/db.h"
#include "dbxml/dbxml/DbXml.hpp"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


#define __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF   "env"
#define __CPP_TRANSPORT_REPO_CONTAINERS_LEAF    "containers"
#define __CPP_TRANSPORT_REPO_INTOUTPUT_LEAF     "output"
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

#define __CPP_TRANSPORT_VALUE_TRUE              "true"
#define __CPP_TRANSPORT_VALUE_FALSE             "false"


#define __CPP_TRANSPORT_UPDATE_ALIAS            "updatectr"


namespace transport
  {
    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    template <typename number>
    class repository
      {
      public:
        typedef enum { node_storage, document_storage } storage_type;

        class integration_container
          {
          public:
            integration_container(boost::filesystem::path& dir, boost::filesystem::path& sql, unsigned int n)
              : path_to_directory(dir), path_to_sql_container(sql), serial_number(n)
              {
              }

          friend class repository<number>;

          protected:
            const boost::filesystem::path& sql_container_path() { return(this->path_to_sql_container); }

          private:
            boost::filesystem::path path_to_directory;
            boost::filesystem::path path_to_sql_container;
            unsigned int            serial_number;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Open a repository with a specific pathname
        repository(const std::string& path, bool recovery=false);
        //! Create a repository with a specific pathname
        repository(const std::string& path, const repository_creation_key& key, storage_type type=node_storage);

        //! Close a repository, including the corresponding containers and environment
        ~repository();


        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the model database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        void write_package(const initial_conditions<number>& ics, const model<number>* m);

        //! Write a threepf integration task to the integration database.
        //! Delegates write_integration_task() to do the work
        void write_integration(const twopf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_TWOPF_ROOT); }
        //! Write a twopf integration task to the integration database
        //! Delegates write_integration_task() to do the work
        void write_integration(const threepf_task<number>& t, const model<number>* m) { this->write_integration_task(t, m, __CPP_TRANSPORT_NODE_THREEPF_ROOT); }

      protected:
        //! Write a generic task to the integration database, using a supplied node tag
        void write_integration_task(const task<number>& t, const model<number>* m, const std::string& root_node_name);

        // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE

      public:
        //! Query the database for a named task, and reconstruct it if present
        task<number>* query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder);

        //! Extract the XML document for a named package
        std::string extract_package_document(const std::string& name);
        //! Extract the XML document for a named integration
        std::string extract_integration_document(const std::string& name);


        // INTERFACE -- ADD OUTPUT TO TASKS

      public:
        //! Insert a record for new twopf output in the task XML database, and set up paths to a suitable SQL container
        integration_container integration_new_output(twopf_task<number>* tk);
        //! Insert a record for new threepf output in the task XML database, and set up paths to a suitable SQL container
        integration_container integration_new_output(threepf_task<number>* tk);


        // INTERNAL DATA

      private:
        //! BOOST path to repository root directory
        boost::filesystem::path root_path;
        //! BOOST path to repository environment
        boost::filesystem::path env_path;
        //! BOOST path to containers
        boost::filesystem::path containers_path;
        //! BOOST path to model container
        boost::filesystem::path packages_path;
        //! BOOST path to integrations container
        boost::filesystem::path integrations_path;


        // DATABASE ENVIRONMENT

      private:
        //! Berkeley DB XML environment object corresponding to the open repository
        DB_ENV* env;
        //! Berkeley DB XML XmlManager object corresponding to the open repository
        DbXml::XmlManager* mgr;
      };


    // utility functions for manipulating XML documents, but which don't need to be methods for a repository object
    // embedding in an unnamed namespace makes these functions invisible outside this translation unit
    namespace
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

            // runtime-api version
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

            // runtime-api version
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
            if(!(integration.getType() == DbXml::XmlValue::NODE && integration.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            DbXml::XmlValue integration_root = integration.getFirstChild();
            if(!(integration_root.getType() == DbXml::XmlValue::NODE && integration_root.getNodeType() == DbXml::XmlValue::ELEMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

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
            else throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            assert(task != nullptr);

            return(task);
          }


        // insert a new output record with the data block of an integration record
        template <typename number>
        typename repository<number>::integration_container insert_integration_output(DbXml::XmlManager* mgr, const std::string& task_name,
                                                                                     const boost::filesystem::path& root_path,
                                                                                     const boost::filesystem::path& container_path)
          {
            assert(mgr != nullptr);

            DbXml::XmlContainer xml_ctr = mgr->openContainer(container_path.string().c_str());
            DbXml::XmlValue doc = get_integration_by_name(task_name, xml_ctr);

            if(!(doc.getType() == DbXml::XmlValue::NODE && doc.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);
            DbXml::XmlValue doc_root = doc.getFirstChild();

            // find all serial numbers for the output groups associated with this task
            std::string query = dbxml_helper::xquery::value_self(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT, __CPP_TRANSPORT_NODE_INTOUTPUT_GROUP, __CPP_TRANSPORT_NODE_OUTPUT_ID);

            DbXml::XmlQueryContext ctx = mgr->createQueryContext();
            DbXml::XmlQueryExpression expr = mgr->prepare(query, ctx);
            DbXml::XmlResults results = expr.execute(doc_root, ctx);

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

            // construct relative path to output
            std::ostringstream output_leaf;
            output_leaf << __CPP_TRANSPORT_REPO_GROUP_STEM << serial_number;
            boost::filesystem::path output_path = static_cast<boost::filesystem::path>(__CPP_TRANSPORT_REPO_INTOUTPUT_LEAF) / task_name / output_leaf.str();
            boost::filesystem::path sql_path = output_path / __CPP_TRANSPORT_REPO_DATABASE_LEAF;

            // create directories
            boost::filesystem::create_directories(root_path / output_path);

            typename repository<number>::integration_container ctr(output_path, sql_path, serial_number);

            // insert new output group

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

            // to reference this container, we need to add an alias
            xml_ctr.addAlias(output_leaf.str());
            std::string doc_selector = dbxml_helper::xquery::document(output_leaf.str(), task_name);

            std::string update = dbxml_helper::xquery::insert(doc_selector, group_node.str(), dbxml_helper::xquery::as_last, doc_root.getNodeName(), __CPP_TRANSPORT_NODE_INTGRTN_OUTPUT);

            std::cerr << "Update string = " << update << std::endl;

            DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
            DbXml::XmlQueryExpression update_expr = mgr->prepare(update, update_ctx);
            DbXml::XmlResults update_results = update_expr.execute(update_ctx, 0);
          }


        // Update the last-edited time in a package document
        inline void update_package_edit_time(DbXml::XmlManager* mgr, const std::string& name, const boost::filesystem::path& container_path)
          {
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            std::string text_node = boost::posix_time::to_simple_string(now);

            // open container
            DbXml::XmlContainer xml_ctr = mgr->openContainer(container_path.string().c_str());

            // must set an alias in order to refer to this container later via doc()
            xml_ctr.addAlias(__CPP_TRANSPORT_UPDATE_ALIAS);

            // build replacement statemeent
            std::string doc_selector = dbxml_helper::xquery::document(__CPP_TRANSPORT_UPDATE_ALIAS, name);
            std::string update = dbxml_helper::xquery::update(doc_selector, text_node, __CPP_TRANSPORT_NODE_PACKAGE_ROOT, __CPP_TRANSPORT_NODE_PACKAGE_DATA, __CPP_TRANSPORT_NODE_PKGDATA_EDITED);

            DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
            DbXml::XmlQueryExpression update_expr = mgr->prepare(update, update_ctx);
            try
              {
                DbXml::XmlResults update_results = update_expr.execute(update_ctx, 0);
              }
            catch (DbXml::XmlException& xe)
              {
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

            // must set an alias in order to refer to this container later via doc()
            xml_ctr.addAlias(__CPP_TRANSPORT_UPDATE_ALIAS);

            // extract document, in order to find name of root node
            DbXml::XmlValue doc = get_integration_by_name(name, xml_ctr);
            if(!(doc.getType() == DbXml::XmlValue::NODE && doc.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);
            DbXml::XmlValue doc_root = doc.getFirstChild();

            // build replacement statement
            std::string doc_selector = dbxml_helper::xquery::document(__CPP_TRANSPORT_UPDATE_ALIAS, name);
            std::string update = dbxml_helper::xquery::update(doc_selector, text_node, doc_root.getNodeName(), __CPP_TRANSPORT_NODE_INTGRTN_DATA, __CPP_TRANSPORT_NODE_INTDATA_EDITED);

            DbXml::XmlQueryContext update_ctx = mgr->createQueryContext();
            DbXml::XmlQueryExpression update_expr = mgr->prepare(update, update_ctx);
            try
              {
                DbXml::XmlResults update_results = update_expr.execute(update_ctx, 0);
              }
            catch (DbXml::XmlException& xe)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_EDITTIME_FAIL << " (DBXML code=" << xe.getExceptionCode() << ": " << xe.what() << ")";
                throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }


      }   // unnamed namespace


    // Open a named repository
    template <typename number>
    repository<number>::repository(const std::string& path, bool recovery)
      : env(nullptr), mgr(nullptr)
      {
        root_path = path;
        if(!boost::filesystem::is_directory(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ROOT << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        if(!boost::filesystem::is_directory(env_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_ENV << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        if(!boost::filesystem::is_directory(containers_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_CNTR << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        if(!boost::filesystem::is_regular_file(packages_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_PACKAGES << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;
        if(!boost::filesystem::is_regular_file(integrations_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_MISSING_INTGRTNS << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        int dberr;
        if((dberr = ::db_env_create(&env, 0)) > 0)
          {
            if(env != nullptr) env->close(env, 0);
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_ENV << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // set up environment to enable logging, transactional support
        // and locking (so multiple processes can access the repository safely)
        u_int32_t env_flags = DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        if(recovery) env_flags = env_flags | DB_RECOVER | DB_CREATE;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        // we have to allow external access in order for XQuery updates to be processed
        mgr = new DbXml::XmlManager(env, DbXml::DBXML_ADOPT_DBENV | DbXml::DBXML_ALLOW_EXTERNAL_ACCESS);
      }


    // Create a named repository
    template <typename number>
    repository<number>::repository(const std::string& path, const repository_creation_key& key, storage_type type)
      : env(nullptr), mgr(nullptr)
      {
        // check whether root directory already exists
        root_path = path;
        if(boost::filesystem::exists(root_path))
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_ROOT_EXISTS << " '" << path << "'";
            throw std::runtime_error(msg.str());
          }

        env_path = root_path / __CPP_TRANSPORT_REPO_ENVIRONMENT_LEAF;
        containers_path = root_path / __CPP_TRANSPORT_REPO_CONTAINERS_LEAF;
        packages_path = containers_path / __CPP_TRANSPORT_CNTR_PACKAGES_LEAF;
        integrations_path = containers_path / __CPP_TRANSPORT_CNTR_INTEGRATIONS_LEAF;

        // create directories
        boost::filesystem::create_directories(root_path);
        boost::filesystem::create_directories(env_path);
        boost::filesystem::create_directories(containers_path);

        int dberr;
        if((dberr = ::db_env_create(&env, 0)) > 0)
          {
            if(env != nullptr) env->close(env, 0);
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_REPO_FAIL_ENV << " '" << path << "'";
            throw runtime_exception(runtime_exception::REPO_NOT_FOUND, msg.str());
          }

        // set up environment to enable logging, transactional support
        // and locking (so multiple processes can access the repository safely)
        u_int32_t env_flags = DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN;
        env->open(env, env_path.string().c_str(), env_flags, 0);

        // set up XmlManager object
        // we have to allow external access in order for XQuery updates to be processed
        mgr = new DbXml::XmlManager(env, DbXml::DBXML_ADOPT_DBENV | DbXml::DBXML_ALLOW_EXTERNAL_ACCESS);

        // create database containers
        switch(type)
          {
            case node_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::NodeContainer);
            break;

            case document_storage:
              mgr->setDefaultContainerType(DbXml::XmlContainer::WholedocContainer);
            break;

            default:
              assert(false);
          }
        DbXml::XmlContainer packages = this->mgr->createContainer(packages_path.string().c_str());
        DbXml::XmlContainer integrations = this->mgr->createContainer(integrations_path.string().c_str());
      }


    // Destroy a respository object, closing the associated repository
    template <typename number>
    repository<number>::~repository()
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        delete this->mgr;
      }


    // Write a model/initial conditions/parameters combination to the repository
    template <typename number>
    void repository<number>::write_package(const initial_conditions<number>& ics, const model<number>* m)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        try
          {
            // open database container
            DbXml::XmlContainer packages = this->mgr->openContainer(this->packages_path.string().c_str());

            DbXml::XmlUpdateContext ctx = this->mgr->createUpdateContext();

            // begin XML document representing this model
            DbXml::XmlDocument doc = this->mgr->createDocument();
            doc.setName(ics.get_name());

            DbXml::XmlEventWriter& writer = packages.putDocumentAsEventWriter(doc, ctx);
            writer.writeStartDocument(nullptr, nullptr, nullptr);

            // write root node
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ROOT), nullptr, nullptr, 0, false);

            // write model uid
            std::string uid = m->get_identity_string();
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID), nullptr, nullptr, 0, false);
            writer.writeText(DbXml::XmlEventReader::Characters, __CPP_TRANSPORT_DBXML_STRING(uid.c_str()), uid.length());
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_MODELUID), nullptr, nullptr);

            // write data block
            write_package_data_block(writer, m);

            // write initial conditions block
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr, 0, false);
            ics.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ICS), nullptr, nullptr);

            // end root node
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_PACKAGE_ROOT), nullptr, nullptr);

            // finalize XML document
            writer.writeEndDocument();
            writer.close();
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::UNIQUE_ERROR)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MODEL_EXISTS << " '" << ics.get_name() << "'";
                throw std::runtime_error(msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }
          }
      }


    // Write a task to the repository
    template <typename number>
    void repository<number>::write_integration_task(const task<number>& t, const model<number>* m, const std::string& root_node_name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);
        assert(m != nullptr);

        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_NULL_MODEL);

        try
          {
            // open database container
            DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());
            DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());

            // check whether XML document corresponding to our initial_conditions object is in the database
            try
              {
                DbXml::XmlDocument doc = models.getDocument(t.get_ics().get_name());
              }
            catch (DbXml::XmlException& xe)
              {
                if(xe.getExceptionCode() == DbXml::XmlException::DOCUMENT_NOT_FOUND)
                  {
                    this->write_package(t.get_ics(), m);
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
                  }
              }

            // now insert task information into the integrations database
            DbXml::XmlUpdateContext ctx = this->mgr->createUpdateContext();

            // begin XML document representing this integration
            DbXml::XmlDocument doc = this->mgr->createDocument();
            doc.setName(t.get_name());

            DbXml::XmlEventWriter& writer = integrations.putDocumentAsEventWriter(doc, ctx);
            writer.writeStartDocument(nullptr, nullptr, nullptr);

            // write root node
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(root_node_name.c_str()), nullptr, nullptr, 0, false);

            // write data block
            write_integration_data_block(writer, t.get_ics().get_name());

            // write task block
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr, 0, false);
            t.serialize_xml(writer);
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_TASK), nullptr, nullptr);

            // write (empty) output block -- will be populated when integrations are run
            writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(__CPP_TRANSPORT_NODE_INTGRTN_OUTPUT), nullptr, nullptr, 0, true);

            // end root node
            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(root_node_name.c_str()), nullptr, nullptr);

            // finalize XML document
            writer.writeEndDocument();
            writer.close();
          }
        catch (DbXml::XmlException& xe)
          {
            if(xe.getExceptionCode() == DbXml::XmlException::UNIQUE_ERROR)
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INTGRTN_EXISTS << " '" << t.get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_INSERT_ERROR << xe.getExceptionCode() << ": '" << xe.what() << "')";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
              }
          }
      }


    // Query the database for a named task
    template <typename number>
    task<number>* repository<number>::query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // open handles to database containers
        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());
        DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());

        // lookup record for the task
        DbXml::XmlValue integration = get_integration_by_name(name, integrations);
        if(!(integration.getType() == DbXml::XmlValue::NODE && integration.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

        // extract name of record for initial conditions/parameters and lookup the corresponding record
        // note that get_integration_by_name() returns an XmlDocument
        // when this is cast to an XmlValue, the node type is a DOCUMENT_NODE
        // we want the root note for doing queries, so have to descend to the first child
        DbXml::XmlValue integration_root = integration.getFirstChild();
        std::string package_name = get_package_from_integration(this->mgr, integration_root, name);
        DbXml::XmlValue package = get_package_by_name(package_name, models);
        if(!(package.getType() == DbXml::XmlValue::NODE && package.getNodeType() == DbXml::XmlValue::DOCUMENT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PACKAGE);

        // extract uid for model
        DbXml::XmlValue package_root = package.getFirstChild();
        std::string model_uid = get_model_uid_from_package(this->mgr, package_root, package_name);

        // use the supplied finder to recover the model
        // throws an exception if the model cannot be found, which should be caught higher up in the task handler
        m = finder(model_uid);

        // get XML schema describing initial conditions/parameters package
        // this comes from the initial_conditions<number> serialization, and contains an
        // embedded parameters<number> serialization
        DbXml::XmlValue ics_group = get_ics_group(this->mgr, package_root, package_name);

        // build initial_conditions<> object from this schema
        initial_conditions<number> ics = build_ics_object(this->mgr, ics_group, package_name, m);

        // build task<> object from the original task schema
        task<number>* tk = build_task_object(this->mgr, integration, ics, m, name);

        return(tk);
      }


    template <typename number>
    std::string repository<number>::extract_package_document(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        DbXml::XmlContainer models = this->mgr->openContainer(this->packages_path.string().c_str());

        DbXml::XmlDocument document = get_package_by_name(name, models);

        std::string content;
        document.getContent(content);
        return(content);
      }


    template <typename number>
    std::string repository<number>::extract_integration_document(const std::string& name)
      {
        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        DbXml::XmlContainer integrations = this->mgr->openContainer(this->integrations_path.string().c_str());

        DbXml::XmlDocument document = get_integration_by_name(name, integrations);

        std::string content;
        document.getContent(content);
        return(content);
      }


    template <typename number>
    typename repository<number>::integration_container repository<number>::integration_new_output(twopf_task<number>* tk)
      {
        assert(tk != nullptr);

        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // insert a new output record, and return the corresponding integration_container handle
        integration_container ctr = insert_integration_output<number>(this->mgr, tk->get_name(), this->root_path, this->integrations_path);

        update_integration_edit_time(this->mgr, tk->get_name(), this->integrations_path);

        return(ctr);
      }


    template <typename number>
    typename repository<number>::integration_container repository<number>::integration_new_output(threepf_task<number>* tk)
      {
        assert(tk != nullptr);

        assert(this->env != nullptr);
        assert(this->mgr != nullptr);

        // insert a new output record, and return the corresponding integration container handle
        integration_container ctr = insert_integration_output<number>(this->mgr, tk->get_name(), this->root_path, this->integrations_path);

        update_integration_edit_time(this->mgr, tk->get_name(), this->integrations_path);

        std::string content = this->extract_integration_document(tk->get_name());
        std::cerr << "Updated XML document:" << std::endl << content << std::endl << std::endl;

        return(ctr);
      }


  }   // namespace transport



#endif //__repository_H_
