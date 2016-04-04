//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DERIVED_CONTENT_WRITER_H
#define CPPTRANSPORT_DERIVED_CONTENT_WRITER_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/repository/records/repository_records_decl.h"
#include "transport-runtime-api/repository/writers/generic_writer.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
	{

    // forward-declare derived content writer
    template <typename number> class derived_content_writer;

    //! Commit object
    template <typename number>
    class derived_content_writer_commit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        derived_content_writer_commit() = default;

        //! destructor is default
        virtual ~derived_content_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        virtual void operator()(derived_content_writer<number>& writer) = 0;

      };



    //! Abort object
    template <typename number>
    class derived_content_writer_abort
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        derived_content_writer_abort() = default;

        //! destructor is default
        virtual ~derived_content_writer_abort() = default;


        // INTERFACE

      public:

        //! abort
        virtual void operator()(derived_content_writer<number>& writer) = 0;

      };


    //! Aggregation object
    template <typename number>
    class derived_content_writer_aggregate
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        derived_content_writer_aggregate() = default;

        //! destructor is default
        virtual ~derived_content_writer_aggregate() = default;


        // INTERFACE

      public:

        //! aggregate
        virtual bool operator()(derived_content_writer<number>& writer, const std::string& product, const std::list<std::string>& used_groups) = 0;

      };


    // WRITER FOR DERIVED CONTENT OUTPUT

    //! Derived content writer: used to commit derived products to the database
		template <typename number>
    class derived_content_writer: public generic_writer
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a derived-content writer object
        derived_content_writer(const std::string& n, output_task_record<number>& rec,
                               std::unique_ptr< derived_content_writer_commit<number> > c,
                               std::unique_ptr< derived_content_writer_abort<number> > a,
                               const typename generic_writer::metadata_group& m, const typename generic_writer::paths_group& p,
                               unsigned int w);

        //! disallow copying to ensure consistency of RAII idiom
        derived_content_writer(const derived_content_writer<number>& obj) = delete;

        //! Destroy a derived-content writer object
        virtual ~derived_content_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(std::unique_ptr< derived_content_writer_aggregate<number> > c) { this->aggregate_h = std::move(c); }

        //! Aggregate a product
        bool aggregate(const std::string& product, const std::list<std::string>& used_groups);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { (*this->commit_h)(*this); this->committed = true; }

        //! Lookup derived product in task
        boost::optional< derived_data::derived_product<number>& > lookup_derived_product(const std::string& name) { return(this->task->lookup_derived_product(name)); }


        // ADMINISTRATION

      public:

        //! set_data_manager_handle is not used in this writer
        template <typename data_manager_type>
        void set_data_manager_handle(data_manager_type data) = delete;

        //! get_data_manager_handle is not used in this writer
        template <typename data_manager_type>
        void get_data_manager_handle(data_manager_type* data) = delete;


        // CONTENT MANAGEMENT

      public:

        //! Push new item of derived content to the writer
        void push_content(derived_data::derived_product<number>& product, const std::list<std::string>& used_groups);

        //! Get content
        const std::list<derived_content>& get_content() const { return(this->content); }


        // PATHS

      public:

        //! get_abs_container_path() is not used in this writer
        boost::filesystem::path get_abs_container_path() const = delete;

        //! get_relative_container_path() is not used in this writer
        boost::filesystem::path get_relative_container_path() const = delete;


        // METADATA

      public:

        //! Return task name
        const std::string& get_task_name() const { return(this->task->get_name()); }

        //! Return task
        template <typename TaskType>
        const TaskType& get_task() const { return dynamic_cast<TaskType&>(*this->task); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Merge list of failed serials reported by backend (not all backends may support this)
        void merge_failure_list(const std::list<unsigned int>& failed) { throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_FAILURE_UNSUPPORTED); }

        //! Set content groups
        void set_content_groups(const std::list<std::string>& l) { this->content_groups = l; }

        //! Get content groups
        const std::list<std::string>& get_content_groups() const { return(this->content_groups); }


        // INTERNAL DATA

      private:

        // REPOSITORY CALLBACK FUNCTIONS

        //! Commit handler
        std::unique_ptr< derived_content_writer_commit<number> > commit_h;

        //! Abort handler
        std::unique_ptr< derived_content_writer_abort<number> > abort_h;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        std::unique_ptr< derived_content_writer_aggregate<number> > aggregate_h;


        // CONTENT

        //! List of derived content produced using this writer
        std::list<derived_content> content;


        // METADATA

        //! copy of task associated with this derived_content_writer; needed to interrogate for task properties
        std::unique_ptr< output_task<number> > task;

        //! metadata for this output task
        output_metadata metadata;

        //! global list of content groups used in this task
        std::list<std::string> content_groups;

	    };


    // DERIVED CONTENT WRITER METHODS


    template <typename number>
    derived_content_writer<number>::derived_content_writer(const std::string& n, output_task_record<number>& rec,
                                                           std::unique_ptr< derived_content_writer_commit<number> > c,
                                                           std::unique_ptr< derived_content_writer_abort<number> > a,
                                                           const generic_writer::metadata_group& m, const generic_writer::paths_group& p,
                                                           unsigned int w)
	    : generic_writer(n, m, p, w),
        commit_h(std::move(c)),
        abort_h(std::move(a)),
	      aggregate_h(nullptr),
        task(rec.get_task()->clone()),
	      metadata()
	    {
	    }


		template <typename number>
    derived_content_writer<number>::~derived_content_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) (*this->abort_h)(*this);
	    }


    template <typename number>
    bool derived_content_writer<number>::aggregate(const std::string& product, const std::list<std::string>& used_groups)
	    {
        if(!this->aggregate_h)
	        {
            assert(false);
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return (*this->aggregate_h)(*this, product, used_groups);
	    }


		template <typename number>
    void derived_content_writer<number>::push_content(derived_data::derived_product<number>& product, const std::list<std::string>& used_groups)
	    {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        std::list<std::string> notes;
        std::list<std::string> tags;

        this->content.emplace_back(product.get_name(), product.get_filename().string(), now, used_groups, notes, tags);
	    }

	}

#endif //CPPTRANSPORT_DERIVED_CONTENT_WRITER_H
