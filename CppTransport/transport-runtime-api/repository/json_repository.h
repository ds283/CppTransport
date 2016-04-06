//
// Created by David Seery on 13/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_JSON_REPOSITORY_H
#define CPPTRANSPORT_JSON_REPOSITORY_H


#include <assert.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/optional.hpp"

#include "json/json.h"


namespace transport
  {

    // 'json_repository_interface' is one interface for getting data in and out of
    // a repository. It can be implemented by a concrete repository class.
    template <typename number>
    class json_repository: public repository<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        json_repository(const boost::filesystem::path path, model_manager<number>& f, repository_mode type,
                        local_environment& ev, error_handler e, warning_handler w, message_handler m,
                        package_finder<number> pf, task_finder<number> tf, derived_product_finder<number> dpf);

        virtual ~json_repository() = default;


        // PULL RECORDS FROM THE REPOSITORY IN JSON FORMAT

      public:

        //! export a package record in JSON format
        virtual std::string export_JSON_package_record(const std::string& name) = 0;

        //! export a task record in JSON format
        virtual std::string export_JSON_task_record(const std::string& name) = 0;

        //! export a derived product record in JSON format
        virtual std::string export_JSON_product_record(const std::string& name) = 0;

        //! export an output group record in JSON format
        virtual std::string export_JSON_content_record(const std::string& name) = 0;


        // REPOSITORY RECORD FACTORIES -- USED TO OBTAIN REPOSITORY RECORD CLASSES FROM OTHER REPRESENTATIONS
        // A json_repository must provide factories based on the JSON representation

      protected:

        //! Create a package record from a JSON value
        virtual std::unique_ptr< package_record<number> > package_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create an integration task record from a JSON value
        virtual std::unique_ptr< integration_task_record<number> > integration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create an output task record from a JSON value
        virtual std::unique_ptr< output_task_record<number> > output_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create a postintegration task record from a JSON value
        virtual std::unique_ptr< postintegration_task_record<number> > postintegration_task_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! create a new derived product record from a JSON value
        virtual std::unique_ptr< derived_product_record<number> > derived_product_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create a new content group for an integration task from a JSON value
        virtual std::unique_ptr< output_group_record<integration_payload> > integration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create a new content group for a postintegration task from a JSON value
        virtual std::unique_ptr< output_group_record<postintegration_payload> > postintegration_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;

        //! Create a new content group for an output task from a JSON value
        virtual std::unique_ptr< output_group_record<output_payload> > output_content_group_record_factory(Json::Value& reader, boost::optional<transaction_manager&> mgr) = 0;


        // SERVICES FOR JSON-BASED REPOSITORIES

      protected:

        //! Write a serialized JSON record to a file.
		    //! Note that this requires a transaction_manager, so commits are not possible without
		    //! opening an explicit transaction
        void commit_JSON_document(transaction_manager& mgr, const boost::filesystem::path& path, const repository_record& record);

        //! Read a serialized JSON record from a file
        Json::Value deserialize_JSON_document(boost::filesystem::path& path);

	    };


    // IMPLEMENTATION


    template <typename number>
    json_repository<number>::json_repository(const boost::filesystem::path path, model_manager<number>& f, repository_mode type,
                                             local_environment& ev, error_handler e, warning_handler w, message_handler m,
                                             package_finder<number> pf, task_finder<number> tf, derived_product_finder<number> dpf)
	    : repository<number>(path, f, type, ev, e, w, m, pf, tf, dpf)
	    {
	    }


    template <typename number>
    void json_repository<number>::commit_JSON_document(transaction_manager& mgr, const boost::filesystem::path& path, const repository_record& record)
	    {
        // serialize the record
        Json::Value root(Json::objectValue);
        record.serialize(root);

        // write out contents to a temporary file
        boost::filesystem::path filename = path.filename();
        boost::filesystem::path parent   = path;
        parent.remove_filename();
        boost::filesystem::path temp_filename = boost::filesystem::path(filename.string() + "-temp");

        boost::filesystem::path abs_record    = this->get_root_path() / path;
        boost::filesystem::path abs_temporary = this->get_root_path() / (parent / temp_filename);

        std::ofstream out;
        out.open(abs_temporary.string().c_str(), std::ios_base::out | std::ios_base::trunc);
        if(!out.is_open() || out.fail())
	        {
            std::stringstream msg;
            msg << CPPTRANSPORT_REPO_COMMIT_FAILURE << " '" << record.get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
	        }

        out << root;
        out.close();

        // if this succeeded, add this record to the transaction journal
		    mgr.journal_deposit(abs_temporary, abs_record);
	    }


    template <typename number>
    Json::Value json_repository<number>::deserialize_JSON_document(boost::filesystem::path& path)
	    {
        boost::filesystem::path abs_record = this->get_root_path() / path;

        Json::Value root;

        if(!boost::filesystem::is_regular_file(abs_record))
	        {
            std::stringstream msg;
            msg << CPPTRANSPORT_REPO_DESERIALIZE_FAILURE << " '" << path << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
	        }

        std::ifstream in;
        in.open(abs_record.string().c_str(), std::ios_base::in);
        if(in.is_open() && !in.fail())
	        {
            in >> root;
	        }
        else
	        {
            std::stringstream msg;
            msg << CPPTRANSPORT_REPO_DESERIALIZE_FAILURE << " '" << path << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
	        }

        return(root);
	    }

  }


#endif //CPPTRANSPORT_JSON_REPOSITORY_H
