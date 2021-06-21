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


#ifndef CPPTRANSPORT_INTEGRATION_WRITER_H
#define CPPTRANSPORT_INTEGRATION_WRITER_H


#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <set>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/exceptions.h"
#include "transport-runtime/localizations/messages_en.h"

#include "transport-runtime/repository/writers/aggregation_profiler.h"
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


    // mini 'traits' function for converting integration data type to a textual name
    template <typename number>
    std::string data_type_name();

    // specialize for the basic data types; if more complex types are used,
    // separate specializations will need to be provided
    template <>
    std::string data_type_name<float>() { return std::string{"float"}; }

    template <>
    std::string data_type_name<double>() { return std::string{"double"}; }

    template <>
    std::string data_type_name<long double>() { return std::string{"long double"}; }

    

    // forward-declare integration writer
    template <typename number> class integration_writer;

    //! Commit object
    template <typename number>
    class integration_writer_commit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        integration_writer_commit() = default;

        //! destructor is default
        virtual ~integration_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        virtual void operator()(integration_writer<number>& writer) = 0;

      };



    //! Abort object
    template <typename number>
    class integration_writer_abort
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        integration_writer_abort() = default;

        //! destructor is default
        virtual ~integration_writer_abort() = default;


        // INTERFACE

      public:

        //! abort
        virtual void operator()(integration_writer<number>& writer) = 0;

      };


    //! Aggregation object
    template <typename number>
    class integration_writer_aggregate
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        integration_writer_aggregate() = default;

        //! destructor is default
        virtual ~integration_writer_aggregate() = default;


        // INTERFACE

      public:

        //! aggregate
        virtual bool operator()(integration_writer<number>& writer, const boost::filesystem::path& product) = 0;

      };


    //! Integrity-check object
    template <typename number>
    class integration_writer_integrity
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        integration_writer_integrity() = default;

        //! destructor is default
        virtual ~integration_writer_integrity() = default;


        // INTERFACE

      public:

        //! check integrity
        virtual void operator()(integration_writer<number>& writer, integration_task<number>& task) = 0;

      };


    //! Finalize object
    template <typename number>
    class integration_writer_finalize
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        integration_writer_finalize() = default;

        //! destructor is default
        virtual ~integration_writer_finalize() = default;


        // INTERFACE

      public:

        //! check integrity
        virtual void operator()(integration_writer<number>& writer) = 0;

      };


    // WRITER FOR INTEGRATION OUTPUT


    //! Integration writer: used to commit integration output to the database
		template <typename number>
    class integration_writer: public generic_writer
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct an integration writer object.
        //! After creation it is not yet associated with anything in the data_manager backend; that must be done later
        //! by the task_manager, which can depute a data_manager object of its choice to do the work.
        integration_writer(const std::string& n, integration_task_record<number>& rec,
                           std::unique_ptr< integration_writer_commit<number> > c,
                           std::unique_ptr< integration_writer_abort<number> > a,
                           const typename generic_writer::metadata_group& m, const typename generic_writer::paths_group& p,
                           unsigned int w, unsigned int wg);

        //! disallow copying to ensure consistency of RAII idiom (writers abort when they go out of scope unless explicitly committed)
        integration_writer(const integration_writer<number>& obj) = delete;

        //! Destroy an integration container object
        virtual ~integration_writer();


        // AGGREGATION

      public:

        //! Set aggregator
        void set_aggregation_handler(std::unique_ptr< integration_writer_aggregate<number> > c) { this->aggregate_h = std::move(c); }

        //! Aggregate a product
        bool aggregate(const boost::filesystem::path& product);


        // DATABASE FUNCTIONS

      public:

        //! Commit contents of this integration_writer to the database
        void commit() { (*this->commit_h)(*this); this->committed = true; }

        //! Set integrity check callback
        void set_integrity_check_handler(std::unique_ptr< integration_writer_integrity<number> > c) { this->integrity_h = std::move(c); }

        //! Check integrity
        //! Normally does not need to be invoked manually, because it is called by data_manager<> in close_writer()
        void check_integrity() { if(this->integrity_h && this->task) (*this->integrity_h)(*this, *this->task); }

        //! Set finalization callback
        void set_finalize_handler(std::unique_ptr< integration_writer_finalize<number> > f) { this->finalize_h = std::move(f); }

        //! Perform finalization
        //! Normally does not need to be invoked manually, because it is called by data_manager<> in close_writer()
        void finalize() { if(this->finalize_h) (*this->finalize_h)(*this); }


        // STATISTICS AND OTHER AUXILIARY INFORMATION

      public:

        //! Are we collecting per-configuration statistics?
        bool is_collecting_statistics() const { return(this->collect_statistics); }

        //! Set collecting-statistics mode flag
        void set_collecting_statistics(bool g) { this->collect_statistics = g; }

		    //! Are we collecting initial conditions data?
				bool is_collecting_initial_conditions() const { return(this->collect_initial_conditions); }

        //! Set initial-conditions collection mode flag
        void set_collecting_initial_conditions(bool g) { this->collect_initial_conditions = g; }

        //! Are we collecting spectral data?
        bool is_collecting_spectral_data() const { return(this->collect_spectral_data); }

        //! Set spectral data collection flag
        [[deprecated("This function should not be used, because it can override the collection behaviour specified in the task. Use could lead to an inconsistent repository.")]]
        void set_collecting_spectral_data(bool g) { this->collect_spectral_data = g; }


        // METADATA

      public:

        //! get workgroup number
        unsigned int get_workgroup_number() const { return(this->workgroup_number); }

        //! set workgroup number (used if seeding this writer by a previous integration)
        void set_workgroup_number(unsigned int wg) { this->workgroup_number = wg; }

        //! Return task
        const std::string& get_task_name() const { return(this->task->get_name()); }

        //! Return task
        template <typename TaskType>
        const TaskType& get_task() const { return dynamic_cast<TaskType&>(*this->task); }

        //! Return writer type
        integration_task_type get_type() const { return(this->type); }

        //! Set metadata
        void set_metadata(const integration_metadata& data) { this->metadata = data; }

        //! Get metadata
        const integration_metadata& get_metadata() const { return(this->metadata); }

        //! Set seed
        void set_seed(const std::string& g) { this->seeded = true; this->seed_group = g; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seeded group name
        const std::string& get_seed_group() const { return(this->seed_group); }

        //! Get integration data type
        const std::string& get_data_type() const { return(this->data_type); }


        // INTEGRITY CHECK

      public:

        //! Add list of serial numbers which the backend advises have failed (not all backends may support this)
        void merge_failure_list(const serial_number_list& failed)
          {
            this->failed_serials.insert(failed.begin(), failed.end());
            this->set_fail(!this->failed_serials.empty());
          }

        //! Get list of serial numbers which the backend advises have failed
        const serial_number_list& get_failed_serials() const { return(this->failed_serials); }

        //! get list of missing k-configuration serials
        const serial_number_list& get_missing_serials() const { return(this->missing_serials); }

		    //! set list of missing k-configuration serials
        template <typename Database>
        void set_missing_serials(const serial_number_list& s, const Database& db)
          {
            this->missing_serials = s;

            // update metadata
            this->metadata.total_configurations = db.size() - s.size();
            this->metadata.total_failures = s.size();
          }


        // PROFILING SUPPORT

      public:

        //! get aggregation profiler
        aggregation_profiler& get_aggregation_profiler() { return(this->agg_profile); }


        // INTERNAL DATA

      private:

        // REPOSITORY CALLBACK FUNCTIONS

        //! Commit handler
        std::unique_ptr< integration_writer_commit<number> > commit_h;

        //! Abort handler
        std::unique_ptr< integration_writer_abort<number> > abort_h;


        // DATA MANAGER CALLBACK FUNCTIONS

        //! Aggregate callback
        std::unique_ptr< integration_writer_aggregate<number> > aggregate_h;

        //! Integrity check callback
        std::unique_ptr< integration_writer_integrity<number> > integrity_h;

        //! Finalize callback
        std::unique_ptr< integration_writer_finalize<number> > finalize_h;


        // METADATA

        //! workgroup number
        unsigned int workgroup_number;

        //! copy of task associated with this integration writer; needed for interrogration of task properties
        std::unique_ptr< integration_task<number> > task;

        //! type of task
        integration_task_type type;

        //! metadata for this integration
        integration_metadata metadata;

        //! was this writer seeded?
        bool seeded;

        //! name of seed group, if so
        std::string seed_group;

        //! name of data type used during integration
        const std::string data_type;


        // FAILURE STATUS

        //! List of failed serial numbers reported by backend (not all backends may support this)
        serial_number_list failed_serials;


        // INTEGRITY STATUS

        //! List of missing serial numbers
        //! (this isn't the same as the list of failed serials reported by the backend; we compute this by testing the
        //! integrity of the database directly and cross-check with failures reported by the backend)
        serial_number_list missing_serials;


        // COLLECTION INFORMATION
        // enable/disable configurable collection behaviour

        //! are we collecting per-configuration statistics?
        bool collect_statistics;

		    //! are we collecting initial conditions data?
		    bool collect_initial_conditions;

		    //! are we collecting spectral data?
        bool collect_spectral_data;

        // PROFILING SUPPORT

        //! profile gadget
        aggregation_profiler agg_profile;

	    };


    // INTEGRATION_WRITER METHODS


    template <typename number>
    integration_writer<number>::integration_writer(const std::string& n, integration_task_record<number>& rec,
                                                   std::unique_ptr< integration_writer_commit<number> > c,
                                                   std::unique_ptr< integration_writer_abort<number> > a,
                                                   const generic_writer::metadata_group& m, const generic_writer::paths_group& p,
                                                   unsigned int w, unsigned int wg)
	    : generic_writer(n, m, p, w),
        workgroup_number(wg),
        seeded(false),
        commit_h(std::move(c)),
        abort_h(std::move(a)),
	      aggregate_h(nullptr),
        integrity_h(nullptr),
        task(dynamic_cast< integration_task<number>* >(rec.get_task()->clone())),
        type(rec.get_task_type()),
        collect_initial_conditions{false},  // defaults to false, but can be overwritten in constructor body
	      collect_statistics(rec.get_task()->get_model()->supports_per_configuration_statistics()),
	      collect_spectral_data{false},       // defaults to false, but can be overwritten in constructor body
	      metadata(),
        data_type(data_type_name<number>()),
        agg_profile(n)
	    {
	      // test whether supplied task is a 2pf integration task
	      auto* tk_as_twopf_db = dynamic_cast< twopf_db_task<number>* >(rec.get_task());

	      // if supplied integration task resolves to a 2pf integration task (or something that contains a 2pf
	      // integration task, such as a 3pf integration task), then inherit collecting status from the task
	      if(tk_as_twopf_db != nullptr)
		      {
			      collect_initial_conditions = tk_as_twopf_db->get_collect_initial_conditions();
			      collect_spectral_data = tk_as_twopf_db->get_collect_spectral_data();
		      }
	    }


		template <typename number>
    integration_writer<number>::~integration_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) (*this->abort_h)(*this);
	    }


		template <typename number>
    bool integration_writer<number>::aggregate(const boost::filesystem::path& product)
	    {
        if(!this->aggregate_h)
	        {
            assert(false);
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET);
	        }

        return (*this->aggregate_h)(*this, product);
	    }


	}   // namespace transport


#endif //CPPTRANSPORT_INTEGRATION_WRITER_H
