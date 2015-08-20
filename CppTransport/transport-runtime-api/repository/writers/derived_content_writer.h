//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __derived_content_writer_H_
#define __derived_content_writer_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/repository/records/repository_records.h"
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

    // WRITER FOR DERIVED CONTENT OUTPUT

    //! Derived content writer: used to commit derived products to the database
		template <typename number>
    class derived_content_writer: public generic_writer
	    {

      public:

        //! Define a commit callback object. Used to commit data products to the repository
        typedef std::function<void(derived_content_writer<number>&)> commit_callback;

        //! Define an abort callback object. Used to abort storage of data products
        typedef std::function<void(derived_content_writer<number>&)> abort_callback;

        //! Define an aggregation callback object. Used to aggregate results from worker processes
        typedef std::function<bool(derived_content_writer<number>&, const std::string&, const std::list<std::string>&)> aggregate_callback;

        class callback_group
	        {
          public:
            commit_callback commit;
            abort_callback  abort;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a derived-content writer object
        derived_content_writer(const std::string& n, output_task_record<number>* rec, const callback_group& c,
                               const typename generic_writer::metadata_group& m, const typename generic_writer::paths_group& p,
                               unsigned int w);

        //! disallow copying to ensure consistency of RAII idiom
        derived_content_writer(const derived_content_writer<number>& obj) = delete;

        //! Destroy a derived-content writer object
        virtual ~derived_content_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(aggregate_callback c) { this->aggregator = c; }

        //! Aggregate a product
        bool aggregate(const std::string& product, const std::list<std::string>& used_groups);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { this->callbacks.commit(*this); this->committed = true; }


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

        //! Return task
        output_task_record<number>* get_record() const { return(this->parent_record); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Merge list of failed serials reported by backend (not all backends may support this)
        void merge_failure_list(const std::list<unsigned int>& failed) { throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_FAILURE_UNSUPPORTED); }


        // INTERNAL DATA

      private:

        // REPOSITORY CALLBACK FUNCTIONS

        //! Repository callbacks
        callback_group callbacks;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        aggregate_callback aggregator;


        // CONTENT

        //! List of derived content produced using this writer
        std::list<derived_content> content;


        // METADATA

        //! task associated with this derived_content_writer
        output_task_record<number>* parent_record;

        //! metadata for this output task
        output_metadata metadata;

	    };


    // DERIVED CONTENT WRITER METHODS


    template <typename number>
    derived_content_writer<number>::derived_content_writer(const std::string& n, output_task_record<number>* rec,
                                                           const typename derived_content_writer<number>::callback_group& c,
                                                           const generic_writer::metadata_group& m, const generic_writer::paths_group& p,
                                                           unsigned int w)
	    : generic_writer(n, m, p, w),
	      callbacks(c),
	      aggregator(nullptr),
	      parent_record(dynamic_cast< output_task_record<number>* >(rec->clone())),
	      metadata()
	    {
        assert(this->parent_record != nullptr);
	    }


		template <typename number>
    derived_content_writer<number>::~derived_content_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) this->callbacks.abort(*this);

        delete this->parent_record;
	    }


    template <typename number>
    bool derived_content_writer<number>::aggregate(const std::string& product, const std::list<std::string>& used_groups)
	    {
        if(!this->aggregator)
	        {
            assert(false);
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return this->aggregator(*this, product, used_groups);
	    }


		template <typename number>
    void derived_content_writer<number>::push_content(derived_data::derived_product<number>& product, const std::list<std::string>& used_groups)
	    {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        std::list<std::string> notes;
        std::list<std::string> tags;

        derived_content data(product.get_name(), product.get_filename().string(), now, used_groups, notes, tags);

        content.push_back(data);
	    }

	}

#endif //__derived_content_writer_H_
