//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __postintegration_writer_H_
#define __postintegration_writer_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

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

    // WRITER FOR POSTINTEGRATION OUTPUT

    //! Postintegration writer: used to commit postprocessing of integration output to the database
		template <typename number>
    class postintegration_writer: public generic_writer
	    {

      public:

        //! Define a commit callback object. Used to commit data products to the repository
        typedef std::function<void(postintegration_writer<number>&)> commit_callback;

        //! Define an abort callback object. Used to abort storage of data products
        typedef std::function<void(postintegration_writer<number>&)> abort_callback;

        //! Define an aggregation callback object. Used to aggregate results from worker processes
        typedef std::function<bool(postintegration_writer<number>&, const std::string&)> aggregate_callback;

        //! Define an integrity check callback object.
        typedef std::function<void(postintegration_writer<number>&, postintegration_task<number>*)> integrity_callback;

        class callback_group
	        {
          public:
            commit_callback commit;
            abort_callback  abort;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a postintegration writer object.
        //! After creation it must be initialized by a suitable data_manager
        postintegration_writer(const std::string& n, postintegration_task_record<number>* rec, const callback_group& c,
                               const typename generic_writer::metadata_group& m, const typename generic_writer::paths_group& p,
                               unsigned int w);

        //! disallow copying to ensure consistency of RAII idiom (writers abort when they go out of scope unless explicitly committed)
        postintegration_writer(const postintegration_writer<number>& obj) = delete;

        //! Destroy a postintegration writer object
        virtual ~postintegration_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(aggregate_callback c) { this->aggregator = c; }

        //! Aggregate a product
        bool aggregate(const std::string& product);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { this->callbacks.commit(*this); this->committed = true; }

        //! Set integrity check callback
        void set_integrity_check_handler(integrity_callback c) { this->integrity_checker = c; }

        //! Check integrity
        void check_integrity(postintegration_task<number>* tk) { if(this->integrity_checker) this->integrity_checker(*this, tk); }


        // PAIRING

      public:

        //! pair with a named integration output group; if paired, then the parent group is the partner
        void set_pair(bool g) { this->paired = g; }

        //! query pairing status
        bool is_paired() const { return(this->paired); }

        //! set parent group
        void set_parent_group(const std::string& p) { this->parent_group = p; }

        //! query parent group
        const std::string& get_parent_group() const { return(this->parent_group); }


        // METADATA

      public:

        //! Return task
        postintegration_task_record<number>* get_record() const { return(this->parent_record); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Merge list of failed serials reported by backend or paired integrator (not all backends may support this)
        void merge_failure_list(const std::list<unsigned int>& failed) { std::list<unsigned int> temp = failed; this->set_fail(true); temp.sort(); this->failed_serials.merge(temp); }

        //! Set seed
        void set_seed(const std::string& g) { this->seeded = true; this->seed_group = g; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seeded group name
        const std::string& get_seed_group() const { return(this->seed_group); }


        // INTEGRITY CHECK

      public:

        //! get list of missing k-configuration serials
        const std::list<unsigned int>& get_missing_serials() const { return(this->missing_serials); }

        //! set list of missing k-configuration serials
        void set_missing_serials(const std::list<unsigned int>& s) { this->missing_serials = s; this->missing_serials.sort(); }


		    // CONTENT

      public:

		    //! Return contents
		    precomputed_products& get_products() { return(this->contents); }


        // INTERNAL DATA

      private:

        // REPOSITORY CALLBACK FUNCTIONS

        //! Repository callbacks
        callback_group callbacks;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        aggregate_callback aggregator;

        //! Integrity check callback
        integrity_callback integrity_checker;


        // METADATA

        //! task associated with this integration writer
        postintegration_task_record<number>* parent_record;

        //! output metadata for this task
        output_metadata metadata;

        //! was this writer seeded?
        bool seeded;

        //! name of seed group, if so
        std::string seed_group;


        // PARENT CONTENT

        //! is this a paired postintegration?
        bool paired;

        //! name of parent integration group
        std::string parent_group;


        // FAILURE STATUS

        //! List of failed serial numbers
        std::list<unsigned int> failed_serials;


        // INTEGRITY STATUS

        //! List of missing serial numbers
        //! (this isn't the same as the list of failed serials reported by the backend; we compute this by testing the
        //! integrity of the database directly and cross-check with failures reported by the backend)
        std::list<unsigned int> missing_serials;


		    // CONTENT TAGS

		    //! record products associated with this writer
		    precomputed_products contents;

	    };


    // POSTINTEGRATION WRITER METHODS


    template <typename number>
    postintegration_writer<number>::postintegration_writer(const std::string& n, postintegration_task_record<number>* rec,
                                                           const typename postintegration_writer<number>::callback_group& c,
                                                           const generic_writer::metadata_group& m, const generic_writer::paths_group& p, unsigned int w)
	    : generic_writer(n, m, p, w),
        paired(false),
        seeded(false),
	      callbacks(c),
	      aggregator(nullptr),
        integrity_checker(nullptr),
	      parent_record(dynamic_cast< postintegration_task_record<number>* >(rec->clone())),
	      metadata()
	    {
        assert(this->parent_record != nullptr);
	    }


		template <typename number>
    postintegration_writer<number>::~postintegration_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) this->callbacks.abort(*this);

        delete this->parent_record;
	    }


    template <typename number>
    bool postintegration_writer<number>::aggregate(const std::string& product)
	    {
        if(!this->aggregator)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return this->aggregator(*this, product);
	    }

	}


#endif //__postintegration_writer_H_
