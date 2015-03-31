//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/manager/instance_manager.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare derived_product if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

#include "transport-runtime-api/repository/records/repository_records.h"

#include "transport-runtime-api/repository/writers/writers.h"

#include "boost/filesystem/operations.hpp"


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

        //! Error-reporting callback object
        typedef std::function<void(const std::string&)> error_callback;

        //! Warning callback object
        typedef std::function<void(const std::string&)> warning_callback;

        //! Message callback object
        typedef std::function<void(const std::string&)> message_callback;


        // LOGGING SERVICES

      public:

        //! Read-only/Read-write access to the repository
        typedef enum { readonly, readwrite } access_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a repository object
        repository(const std::string& path, access_type mode,
                   typename repository<number>::error_callback e,
                   typename repository<number>::warning_callback w,
                   typename repository<number>::message_callback m,
                   typename repository_finder<number>::package_finder pf,
                   typename repository_finder<number>::task_finder tf,
                   typename repository_finder<number>::derived_product_finder dpf);


        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository() = default;


        // ADMINISTRATION

      public:

		    //! Set model_finder object
		    void set_model_finder(const typename instance_manager<number>::model_finder& f);

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() const { return (this->root_path); };

        //! Get access mode
        const access_type& get_access_mode() const { return (this->access_mode); }


		    // CREATE RECORDS

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void commit_package(const initial_conditions<number>& ics) = 0;

        //! Write an integration task to the database.
        virtual void commit_task(const integration_task<number>& tk) = 0;

        //! Write an output task to the database
        virtual void commit_task(const output_task<number>& tk) = 0;

        //! Write a postintegration task to the database
        virtual void commit_task(const postintegration_task<number>& tk) = 0;

        //! Write a derived product specification
        virtual void commit_derived_product(const derived_data::derived_product<number>& d) = 0;


		    // READ RECORDS FROM THE DATABASE

      public:

		    //! Read a package record from the database
		    virtual package_record<number>* query_package(const std::string& name) = 0;

		    //! Read a task record from the database
		    virtual task_record<number>* query_task(const std::string& name) = 0;

		    //! Read a derived product specification from the database
		    virtual derived_product_record<number>* query_derived_product(const std::string& name) = 0;

        //! Enumerate the output groups available from a named integration task
        virtual std::list< std::shared_ptr< output_group_record<integration_payload> > > enumerate_integration_task_content(const std::string& name) = 0;

		    //! Enumerate the output groups available from a named output task
		    virtual std::list< std::shared_ptr< output_group_record<output_payload> > > enumerate_output_task_content(const std::string& name) = 0;
		    

        // ADD CONTENT ASSOCIATED WITH A TASK

      public:

		    //! Generate a writer object for new integration output
		    virtual std::shared_ptr< integration_writer<number> > new_integration_task_content(integration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) = 0;

		    //! Generate a writer object for new derived-content output
		    virtual std::shared_ptr< derived_content_writer<number> > new_output_task_content(output_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) = 0;

        //! Generate a writer object for new postintegration output
        virtual std::shared_ptr< postintegration_writer<number> > new_postintegration_task_content(postintegration_task_record<number>* rec, const std::list<std::string>& tags, unsigned int worker) = 0;


        // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS

      public:

        //! Find an output group for an integration task
        virtual std::shared_ptr< output_group_record<integration_payload> > find_integration_task_output(const std::string& name, const std::list<std::string>& tags) = 0;


        // PRIVATE DATA

      protected:

        //! Access mode
        const access_type access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;


		    // ERROR, WARNING, MESSAGE HANDLERS

		    //! error handler
		    error_callback error;

		    //! warning handler
		    warning_callback warning;

		    //! message handler
		    message_callback message;


		    // FINDER SERVICES

        //! Cached model-finder supplied by instance manager
        typename instance_manager<number>::model_finder model_finder;

        //! Cached package-finder instance
        typename repository_finder<number>::package_finder pkg_finder;

        //! Cached task-finder instance
        typename repository_finder<number>::task_finder tk_finder;

        //! Cached derived-product-finder instance
        typename repository_finder<number>::derived_product_finder dprod_finder;

	    };


    // ADMINISTRATION


    template <typename number>
    repository<number>::repository(const std::string& path, typename repository<number>::access_type mode,
                                   typename repository<number>::error_callback e,
                                   typename repository<number>::warning_callback w,
                                   typename repository<number>::message_callback m,
                                   typename repository_finder<number>::package_finder pf,
                                   typename repository_finder<number>::task_finder tf,
                                   typename repository_finder<number>::derived_product_finder dpf)
	    : root_path(path),
	      access_mode(mode),
	      error(e),
	      warning(w),
	      message(m),
	      pkg_finder(pf),
	      tk_finder(tf),
	      dprod_finder(dpf)
	    {
	    }


    template <typename number>
    void repository<number>::set_model_finder(const typename instance_manager<number>::model_finder& f)
	    {
        this->model_finder = f;
	    }


	}   // namespace transport



#endif //__repository_H_
