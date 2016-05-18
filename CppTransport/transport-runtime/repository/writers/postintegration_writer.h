//
// Created by David Seery on 25/03/15.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_POSTINTEGRATION_WRITER_H
#define CPPTRANSPORT_POSTINTEGRATION_WRITER_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <set>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/localizations/messages_en.h"

#include "transport-runtime/derived-products/derived-content/correlation-functions/template_types.h"

#include "transport-runtime/repository/records/repository_records_decl.h"
#include "transport-runtime/repository/writers/generic_writer.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
	{

    // forward-declare postintegration writer
    template <typename number> class postintegration_writer;

    //! Commit object
    template <typename number>
    class postintegration_writer_commit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        postintegration_writer_commit() = default;

        //! destructor is default
        virtual ~postintegration_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        virtual void operator()(postintegration_writer<number>& writer) = 0;

      };



    //! Abort object
    template <typename number>
    class postintegration_writer_abort
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        postintegration_writer_abort() = default;

        //! destructor is default
        virtual ~postintegration_writer_abort() = default;


        // INTERFACE

      public:

        //! abort
        virtual void operator()(postintegration_writer<number>& writer) = 0;

      };


    //! Aggregation object
    template <typename number>
    class postintegration_writer_aggregate
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        postintegration_writer_aggregate() = default;

        //! destructor is default
        virtual ~postintegration_writer_aggregate() = default;


        // INTERFACE

      public:

        //! aggregate
        virtual bool operator()(postintegration_writer<number>& writer, const std::string& product) = 0;

      };


    //! Integrity-check object
    template <typename number>
    class postintegration_writer_integrity
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        postintegration_writer_integrity() = default;

        //! destructor is default
        virtual ~postintegration_writer_integrity() = default;


        // INTERFACE

      public:

        //! check integrity
        virtual void operator()(postintegration_writer<number>& writer, postintegration_task<number>& task) = 0;

      };


    //! Finalize object
    template <typename number>
    class postintegration_writer_finalize
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        postintegration_writer_finalize() = default;

        //! destructor is default
        virtual ~postintegration_writer_finalize() = default;


        // INTERFACE

      public:

        //! check integrity
        virtual void operator()(postintegration_writer<number>& writer) = 0;

      };


    // WRITER FOR POSTINTEGRATION OUTPUT

    //! Postintegration writer: used to commit postprocessing of integration output to the database
		template <typename number>
    class postintegration_writer: public generic_writer
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a postintegration writer object.
        //! After creation it must be initialized by a suitable data_manager
        postintegration_writer(const std::string& n, postintegration_task_record<number>& rec,
                               std::unique_ptr< postintegration_writer_commit<number> > c,
                               std::unique_ptr< postintegration_writer_abort<number> > a,
                               const typename generic_writer::metadata_group& m, const typename generic_writer::paths_group& p,
                               unsigned int w);

        //! disallow copying to ensure consistency of RAII idiom (writers abort when they go out of scope unless explicitly committed)
        postintegration_writer(const postintegration_writer<number>& obj) = delete;

        //! Destroy a postintegration writer object
        virtual ~postintegration_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(std::unique_ptr< postintegration_writer_aggregate<number> > c) { this->aggregate_h = std::move(c); }

        //! Aggregate a product
        bool aggregate(const std::string& product);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { (*this->commit_h)(*this); this->committed = true; }

        //! Set integrity check callback
        void set_integrity_check_handler(std::unique_ptr< postintegration_writer_integrity<number> > c) { this->integrity_h = std::move(c); }

        //! Check integrity
        //! Normally does not need to be invoked manually, because it is called by data_manager<> in close_writer()
        void check_integrity() { if(this->integrity_h && this->task) (*this->integrity_h)(*this, *this->task); }

        //! Set finalization callback
        void set_finalize_handler(std::unique_ptr< postintegration_writer_finalize<number> > f) { this->finalize_h = std::move(f); }

        //! Perform finalization
        //! Normally does not need to be invoked manually, because it is called by data_manager<> in close_writer()
        void finalize() { if(this->finalize_h) (*this->finalize_h)(*this); }


        // PAIRING

      public:

        //! pair with a named integration content group; if paired, then the parent group is the partner
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
        const std::string& get_task_name() const { return(this->task->get_name()); }

        //! Return task
        template <typename TaskType>
        const TaskType& get_task() const { return dynamic_cast<TaskType&>(*this->task); }

        //! Return task type
        postintegration_task_type get_type() const { return(this->type); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Set seed
        void set_seed(const std::string& g) { this->seeded = true; this->seed_group = g; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seeded group name
        const std::string& get_seed_group() const { return(this->seed_group); }


        // INTEGRITY CHECK

      public:

        //! Add list of serial numbers which the backend (or a paired integrator) advises have failed (not all backends may support this)
        void merge_failure_list(const std::set<unsigned int>& failed)
          {
            this->failed_serials.insert(failed.begin(), failed.end());
            this->set_fail(!this->failed_serials.empty());
          }

        //! Get list of serial numbers which the backend (or a paired integrator) advises have failed
        const std::set<unsigned int>& get_failed_serials() const { return(this->failed_serials); }

        //! get list of missing k-configuration serials
        const std::set<unsigned int>& get_missing_serials() const { return(this->missing_serials); }

        //! set list of missing k-configuration serials
        template <typename Database>
        void set_missing_serials(const std::set<unsigned int>& s, const Database& db)
          {
            this->missing_serials = s;
          }


		    // CONTENT

      public:

		    //! Return contents
		    precomputed_products& get_products() { return(this->contents); }


        // INTERNAL DATA

      private:

        // REPOSITORY CALLBACK FUNCTIONS

        //! Commit handler
        std::unique_ptr< postintegration_writer_commit<number> > commit_h;

        //! Abort handler
        std::unique_ptr< postintegration_writer_abort<number> > abort_h;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        std::unique_ptr< postintegration_writer_aggregate<number> > aggregate_h;

        //! Integrity check callback
        std::unique_ptr< postintegration_writer_integrity<number> > integrity_h;

        //! Finalize callback
        std::unique_ptr< postintegration_writer_finalize<number> > finalize_h;


        // METADATA

        //! copy of task associated with this integration writer; needed for interrogation of task properties
        std::unique_ptr< postintegration_task<number> > task;

        //! type of task
        postintegration_task_type type;

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
        std::set<unsigned int> failed_serials;


        // INTEGRITY STATUS

        //! List of missing serial numbers
        //! (this isn't the same as the list of failed serials reported by the backend; we compute this by testing the
        //! integrity of the database directly and cross-check with failures reported by the backend)
        std::set<unsigned int> missing_serials;


		    // CONTENT TAGS

		    //! record products associated with this writer
		    precomputed_products contents;

	    };


    // POSTINTEGRATION WRITER METHODS


    template <typename number>
    postintegration_writer<number>::postintegration_writer(const std::string& n, postintegration_task_record<number>& rec,
                                                           std::unique_ptr< postintegration_writer_commit<number> > c,
                                                           std::unique_ptr< postintegration_writer_abort<number> > a,
                                                           const generic_writer::metadata_group& m, const generic_writer::paths_group& p, unsigned int w)
	    : generic_writer(n, m, p, w),
        paired(false),
        seeded(false),
        commit_h(std::move(c)),
        abort_h(std::move(a)),
	      aggregate_h(nullptr),
        integrity_h(nullptr),
        task(dynamic_cast< postintegration_task<number>* >(rec.get_task()->clone())),
        type(rec.get_task_type()),
	      metadata()
	    {
	    }


		template <typename number>
    postintegration_writer<number>::~postintegration_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) (*this->abort_h)(*this);
	    }


    template <typename number>
    bool postintegration_writer<number>::aggregate(const std::string& product)
	    {
        if(!this->aggregate_h)
	        {
            assert(false);
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return (*this->aggregate_h)(*this, product);
	    }

	}


#endif //CPPTRANSPORT_POSTINTEGRATION_WRITER_H
