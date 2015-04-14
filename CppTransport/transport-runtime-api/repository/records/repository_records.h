//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __repository_records_H_
#define __repository_records_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/repository/records/record_finder.h"

#include "transport-runtime-api/manager/instance_manager.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class if needed
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare derived products if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

#include "boost/timer/timer.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


// JSON node names

#define __CPP_TRANSPORT_NODE_RECORD_NAME                            "name"
#define __CPP_TRANSPORT_NODE_RECORD_TYPE                            "record-type"
#define __CPP_TRANSPORT_NODE_RECORD_PACKAGE                         "package"
#define __CPP_TRANSPORT_NODE_RECORD_INTEGRATION_TASK                "integration-task"
#define __CPP_TRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK            "postintegration-task"
#define __CPP_TRANSPORT_NODE_RECORD_OUTPUT_TASK                     "output-task"
#define __CPP_TRANSPORT_NODE_RECORD_DERIVED_PRODUCT                 "derived-product"
#define __CPP_TRANSPORT_NODE_RECORD_CONTENT                         "content-group"

#define __CPP_TRANSPORT_NODE_METADATA_GROUP                         "metadata"
#define __CPP_TRANSPORT_NODE_METADATA_CREATED                       "created"
#define __CPP_TRANSPORT_NODE_METADATA_EDITED                        "edited"
#define __CPP_TRANSPORT_NODE_METADATA_RUNTIME_API                   "api"

#define __CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS                     "output-groups"

#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME                  "parent-task"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT                  "output-path"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED                     "locked"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES                      "notes"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS                       "tags"

#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE           "database-path"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED             "failed"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP          "workgroup"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS     "failed-serials"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED             "seeded"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP         "seed-group"
#define __CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS         "has-statistics"

#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_DATABASE       "database-path"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED         "failed"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED         "paired"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP   "parent-group"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED         "seeded"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP     "seed-group"
#define __CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS "failed-serials"

#define __CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT                       "contains-products"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF                 "zeta-twopf"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF               "zeta-threepf"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP                "zeta-redbsp"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL                  "fNL_local"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_EQUI                   "fNL_equi"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO                  "fNL_ortho"
#define __CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_DBI                    "fNL_DBI"

#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FAILED                 "failed"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY                  "generated-products"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS            "used-content-groups"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME           "parent-product"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME               "filename"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_CREATED                "creation-time"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_NOTES                  "notes"
#define __CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_TAGS                   "tags"

#define __CPP_TRANSPORT_NODE_TIMINGDATA_GROUP                       "integration-metadata"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME        "total-wallclock-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME              "total-aggregation-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME              "total-integration-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_INT_TIME           "min-mean-integration-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_INT_TIME           "max-mean-integration-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME         "global-min-integration-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME         "global-max-integration-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME            "total-batching-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_BATCH_TIME         "min-mean-batching-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_BATCH_TIME         "max-mean-batching-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME       "global-min-batching-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME       "global-max-batching-time"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS          "configurations-processed"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_NUM_FAILURES                "configurations-failed"
#define __CPP_TRANSPORT_NODE_TIMINGDATA_NUM_REFINED                 "configurations-refined"

#define __CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP                       "output-metadata"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME        "total-wallclock-time"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME               "total-db-time"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME              "total-aggregation-time"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS             "time-cache-hits"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS          "time-cache-unloads"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS            "twopf-cache-hits"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS         "twopf-cache-unloads"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS          "threepf-cache-hits"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS       "threepf-cache-unloads"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS             "data-cache-hits"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS          "data-cache-unloads"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_HITS             "zeta-cache-hits"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_UNLOADS          "zeta-cache-unloads"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS        "time-cache-evictions"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS       "twopf-cache-evictions"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS     "threepf-cache-evictions"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS        "data-cache-evictions"
#define __CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_EVICTIONS        "zeta-cache-evictions"


namespace transport
	{


    // REPOSITORY METADATA RECORD


    //! Encapsulates metadata for a record stored in the repository
    class record_metadata: public serializable
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a metadata record
        record_metadata();

        //! deserialization constructor
        record_metadata(Json::Value& reader);

        virtual ~record_metadata() = default;


        // GET AND SET METADATA

      public:

        //! get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->creation_time); }

        //! set creation time (to be used with care)
        void set_creation_time(const boost::posix_time::ptime& t) { this->creation_time = t; }

        //! get last-edit time
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->last_edit_time); }

        //! reset last-edit time to now
        void update_last_edit_time() { this->last_edit_time = boost::posix_time::second_clock::universal_time(); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->runtime_api); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! creation time
        boost::posix_time::ptime creation_time;

        //! last-edited time
        boost::posix_time::ptime last_edit_time;

        //! version of runtime API used to create this record
        unsigned int runtime_api;

	    };


    // GENERIC REPOSITORY RECORD


    //! Encapsulates common details shared by all repository records
    class repository_record: public serializable
	    {

      public:

        typedef std::function<void(repository_record&)> commit_handler;

        class handler_package
	        {
          public:
            commit_handler commit;
	        };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a repository record with a default name (taken from its creation time)
        repository_record(handler_package& pkg);

        //! construct a repository record with a supplied name
        repository_record(const std::string& nm, repository_record::handler_package& pkg);

        //! deserialization constructor
        repository_record(Json::Value& reader, repository_record::handler_package& pkg);

        virtual ~repository_record() = default;


        // NAME HANDLING

      public:

        //! Get record name
        const std::string get_name() const { return(this->name); }

        //! Set record time from creation-time metadata
        void set_name(const std::string& n) { this->name = n; }


        // GET AND SET METADATA -- delegated to the metadata container

      public:

        //! get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->metadata.get_creation_time()); }

        //! set creation time (use with care)
        void set_creation_time(const boost::posix_time::ptime& t) { this->metadata.set_creation_time(t); }

        //! get last-edit time
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->metadata.get_last_edit_time()); }

        //! reset last-edit time to now
        void update_last_edit_time() { this->metadata.update_last_edit_time(); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->metadata.get_runtime_API_version()); }


        // COMMIT TO DATABASE

      public:

        //! Commit this record to the database
        void commit() { this->handlers.commit(*this); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const = 0;


        // INTERNAL DATA

      protected:

        //! Name
        std::string name;

        //! Metadata record
        record_metadata metadata;

        //! Handler package
        handler_package handlers;

	    };


    // PACKAGE RECORD


    //! Encapsulates metadata for a package record stored in the repository
		template <typename number>
    class package_record: public repository_record
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a package record
        package_record(const initial_conditions<number>& i, repository_record::handler_package& pkg);

        //! deserialization constructor
        package_record(Json::Value& reader, typename instance_manager<number>::model_finder& f, repository_record::handler_package& pkg);

        virtual ~package_record() = default;


        // GET CONTENTS

      public:

        //! Get initial conditionss
        const initial_conditions<number>& get_ics() const { return(this->ics); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new package_record(static_cast<const package_record&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Initial conditions data associated with this package
        initial_conditions<number> ics;

	    };


    // GENERIC TASK RECORD


		template <typename number>
    class task_record: public repository_record
	    {

      public:

        typedef enum { integration, postintegration, output } type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a task record
        task_record(const std::string& nm, repository_record::handler_package& pkg);

        //! deserialization constructor
        task_record(Json::Value& f, repository_record::handler_package& pkg);

        virtual ~task_record() = default;


		    // GET CONTENTS

      public:

		    //! get task object
		    virtual task<number>* get_abstract_task() const = 0;


        // ADMINISTRATION

      public:

        //! get record type
        virtual type get_type() const = 0;

        //! Add content
        void add_new_output_group(const std::string& name) { this->content_groups.push_back(name); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of content groups associated with this task
        std::list<std::string> content_groups;

	    };


    // INTEGRATION TASK RECORD


		template <typename number>
    class integration_task_record: public task_record<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct an integration task record
        integration_task_record(const integration_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        integration_task_record(const integration_task_record& obj);

        //! deserialization constructor
        integration_task_record(Json::Value& reader, typename repository_finder<number>::package_finder& f, repository_record::handler_package& pkg);

        virtual ~integration_task_record();


        // GET CONTENTS

      public:

        //! Get task
        integration_task<number>* get_task() const { return(this->tk); }

		    //! Get abstract task
		    virtual task<number>* get_abstract_task() const override { return(this->tk); }


        // ADMINISTRATION

      public:

        //! Get type
        virtual typename task_record<number>::type get_type() const override { return(task_record<number>::integration); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new integration_task_record(static_cast<const integration_task_record&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        integration_task<number>* tk;

	    };


    // POSTINTEGRATION TASK RECORD


		template <typename number>
    class postintegration_task_record: public task_record<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a postintegration_task_record
        postintegration_task_record(const postintegration_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        postintegration_task_record(const postintegration_task_record& obj);

        //! deserialization constructor
        postintegration_task_record(Json::Value& reader, typename repository_finder<number>::task_finder& f, repository_record::handler_package& pkg);

        virtual ~postintegration_task_record();


        // GET CONTENTS

      public:

        //! Get task
        postintegration_task<number>* get_task() const { return(this->tk); }

        //! Get abstract task
        virtual task<number>* get_abstract_task() const override { return(this->tk); }


        // ADMINISTRATION

      public:

        //! Get type
        virtual typename task_record<number>::type get_type() const override { return(task_record<number>::postintegration); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new postintegration_task_record(static_cast<const postintegration_task_record&>(*this)); }


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        postintegration_task<number>* tk;

	    };


    // DERIVED PRODUCT RECORD


		template <typename number>
    class derived_product_record: public repository_record
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a derived product record
        derived_product_record(const derived_data::derived_product<number>& prod, repository_record::handler_package& pkg);

        //! Override copy constructor to perform a deep copy
        derived_product_record(const derived_product_record& obj);

        //! deserialization constructor
        derived_product_record(Json::Value& reader, typename repository_finder<number>::task_finder& f, repository_record::handler_package& pkg);

        virtual ~derived_product_record();


        // GET CONTENTS

      public:

        //! Get product
        derived_data::derived_product<number>* get_product() const { return(this->product); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new derived_product_record(static_cast<const derived_product_record&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Derived product associated with this record
        derived_data::derived_product<number>* product;

	    };


    // OUTPUT TASK RECORD


		template <typename number>
    class output_task_record: public task_record<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct an output task record
        output_task_record(const output_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        output_task_record(const output_task_record& obj);

        //! deserialization constructor
        output_task_record(Json::Value& reader, typename repository_finder<number>::derived_product_finder& f, repository_record::handler_package& pkg);

        virtual ~output_task_record();


        // GET CONTENTS

      public:

        //! Get task
        output_task<number>* get_task() const { return(this->tk); }

        //! Get abstract task
        virtual task<number>* get_abstract_task() const override { return(this->tk); }


        // ADMINISTRATION

      public:

        //! Get type
        virtual typename task_record<number>::type get_type() const override { return(task_record<number>::output); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new output_task_record(static_cast<const output_task_record&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        output_task<number>* tk;

	    };


    // OUTPUT GROUP RECORDS


    // OUTPUT METADATA

    //! Timing metadata for an integration
    class integration_metadata : public serializable
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! null constructor - set all fields to zero
        integration_metadata(void)
	        : total_wallclock_time(0),
	          total_aggregation_time(0),
	          total_integration_time(0),
	          min_mean_integration_time(0),
	          max_mean_integration_time(0),
	          global_min_integration_time(0),
	          global_max_integration_time(0),
	          total_batching_time(0),
	          min_mean_batching_time(0),
	          max_mean_batching_time(0),
	          global_min_batching_time(0),
	          global_max_batching_time(0),
	          total_configurations(0),
            total_failures(0),
            total_refinements(0)
	        {
	        }

        //! value constructor - ensure all fields get set at once
        integration_metadata(boost::timer::nanosecond_type wc, boost::timer::nanosecond_type ag,
                             boost::timer::nanosecond_type it, boost::timer::nanosecond_type min_m_it, boost::timer::nanosecond_type max_m_it,
                             boost::timer::nanosecond_type min_it, boost::timer::nanosecond_type max_it,
                             boost::timer::nanosecond_type bt, boost::timer::nanosecond_type min_m_bt, boost::timer::nanosecond_type max_m_bt,
                             boost::timer::nanosecond_type min_bt, boost::timer::nanosecond_type max_bt,
                             unsigned int num_processed, unsigned int num_failed, unsigned int num_refined,
                             const std::list<unsigned int>& failures, bool f)
	        : total_wallclock_time(wc),
	          total_aggregation_time(ag),
	          total_integration_time(it),
	          min_mean_integration_time(min_m_it),
	          max_mean_integration_time(max_m_it),
	          global_min_integration_time(min_it),
	          global_max_integration_time(max_it),
	          total_batching_time(bt),
	          min_mean_batching_time(min_m_bt),
	          max_mean_batching_time(max_m_bt),
	          global_min_batching_time(min_bt),
	          global_max_batching_time(max_bt),
	          total_configurations(num_processed),
            total_failures(num_failed),
            total_refinements(num_refined)
	        {
	        }

        //! deserialization constructor
        integration_metadata(Json::Value& reader);


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        void serialize(Json::Value& writer) const override;


        // DATA

      public:

        //! total wallclock time (measured on master process)
        boost::timer::nanosecond_type total_wallclock_time;

        //! total aggregation time (measured on master process)
        boost::timer::nanosecond_type total_aggregation_time;

        //! total integration time (measured by aggregating times reported by workers)
        boost::timer::nanosecond_type total_integration_time;

        //! minimum mean integration time (taken over all workers)
        boost::timer::nanosecond_type min_mean_integration_time;

        //! maxmimum mean integration time (taken over all workers)
        boost::timer::nanosecond_type max_mean_integration_time;

        //! global minimum integration time (per configuration)
        boost::timer::nanosecond_type global_min_integration_time;

        // global maximum integration time (per configuration)
        boost::timer::nanosecond_type global_max_integration_time;

        // total batching time (measured by aggregating times reported by workers)
        boost::timer::nanosecond_type total_batching_time;

        //! minimum mean batching time (taken over all workers)
        boost::timer::nanosecond_type min_mean_batching_time;

        // maximum mean batching time (taken over all workers)
        boost::timer::nanosecond_type max_mean_batching_time;

        // global minimum batching time (per configuration)
        boost::timer::nanosecond_type global_min_batching_time;

        // global maximum batching time (per configuration)
        boost::timer::nanosecond_type global_max_batching_time;

        // total number of configurations processed
        unsigned int total_configurations;

        //! total number of failures reported
        unsigned int total_failures;

        //! total number of mesh refinements reported
        unsigned int total_refinements;

	    };


    //! Timing metadata for an output task
    class output_metadata : public serializable
	    {
      public:

        //! null constructor - set all fields to zero
        output_metadata(void)
	        : work_time(0), db_time(0), aggregation_time(0),
	          time_config_hits(0), time_config_unloads(0),
	          twopf_kconfig_hits(0), twopf_kconfig_unloads(0),
	          threepf_kconfig_hits(0), threepf_kconfig_unloads(0),
	          data_hits(0), data_unloads(0),
	          zeta_hits(0), zeta_unloads(0),
	          time_config_evictions(0), twopf_kconfig_evictions(0),
	          threepf_kconfig_evictions(0), data_evictions(0),
	          zeta_evictions(0)
	        {
	        }

        //! value constructor - ensure all fields get set at once
        output_metadata(boost::timer::nanosecond_type wt, boost::timer::nanosecond_type dt, boost::timer::nanosecond_type ag,
                        unsigned int tc_h, unsigned int tc_u,
                        unsigned int tw_k_h, unsigned int tw_k_u,
                        unsigned int th_k_h, unsigned int th_k_u,
                        unsigned int dc_h, unsigned int dc_u,
                        unsigned int zc_h, unsigned int zc_u,
                        boost::timer::nanosecond_type tc_e, boost::timer::nanosecond_type tw_e,
                        boost::timer::nanosecond_type th_e, boost::timer::nanosecond_type d_e,
                        boost::timer::nanosecond_type zc_e)
	        : work_time(wt), db_time(dt), aggregation_time(ag),
	          time_config_hits(tc_h), time_config_unloads(tc_u),
	          twopf_kconfig_hits(tw_k_h), twopf_kconfig_unloads(tw_k_u),
	          threepf_kconfig_hits(th_k_h), threepf_kconfig_unloads(th_k_u),
	          data_hits(dc_h), data_unloads(dc_u),
	          zeta_hits(zc_h), zeta_unloads(zc_u),
	          time_config_evictions(tc_e), twopf_kconfig_evictions(tw_e),
	          threepf_kconfig_evictions(th_e), data_evictions(d_e),
	          zeta_evictions(zc_e)
	        {
	        }

        //! deserialization constructor
        output_metadata(Json::Value& reader);


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        void serialize(Json::Value& writer) const override;


        // DATA

      public:

        //! total time spent working
        boost::timer::nanosecond_type work_time;

        //! total time spent querying the database
        boost::timer::nanosecond_type db_time;

        //! total time spend aggregating
        boost::timer::nanosecond_type aggregation_time;

        //! total number of time-configuration cache hits
        unsigned int time_config_hits;

        //! total number of twopf k-configuration cache hits
        unsigned int twopf_kconfig_hits;

        //! total number of threepf k-configuration cache hits
        unsigned int threepf_kconfig_hits;

        //! total number of data cache hits
        unsigned int data_hits;

        //! total number of zeta cache hits
        unsigned int zeta_hits;

        //! total number of time-configuration cache unloads
        unsigned int time_config_unloads;

        //! total number of twopf k-configuration cache unloads
        unsigned int twopf_kconfig_unloads;

        //! total number of threepf k-configuration cache unloads
        unsigned int threepf_kconfig_unloads;

        // total number of data cache unloads
        unsigned int data_unloads;

        // total number of zeta cache unloads
        unsigned int zeta_unloads;

        // total time spent doing time-config cache evictions
        boost::timer::nanosecond_type time_config_evictions;

        //! total time spent doing twopf k-config cache evictions
        boost::timer::nanosecond_type twopf_kconfig_evictions;

        //! total time spent doing threepf k-config cache evictions
        boost::timer::nanosecond_type threepf_kconfig_evictions;

        //! total time spent doing data cache evictions
        boost::timer::nanosecond_type data_evictions;

        //! total time spent doing zeta cache evictions
        boost::timer::nanosecond_type zeta_evictions;

	    };


    // OUTPUT DESCRIPTORS

    //! Derived product descriptor. Used to enumerate the content associated with
    //! a output task output group.
    class derived_content: public serializable
	    {

      public:

        //! Create a derived_product descriptor
        derived_content(const std::string& prod, const std::string& fnam, const boost::posix_time::ptime& now,
                        const std::list<std::string>& gp, const std::list<std::string>& nt, const std::list<std::string>& tg)
	        : parent_product(prod),
            filename(fnam),
            created(now),
            content_groups(gp),
            notes(nt),
            tags(tg)
	        {
	        }

        //! Deserialization constructor
        derived_content(Json::Value& reader);

        //! Destroy a derived_product descriptor
        ~derived_content() = default;


        // INTERFACE

      public:

        //! Get product name
        const std::string& get_parent_product() const { return (this->parent_product); }

        //! Get product pathname
        const boost::filesystem::path& get_filename() const { return(this->filename); }

        //! Get notes
        const std::list<std::string>& get_notes() const { return(this->notes); }

        //! Add note
        void add_note(const std::string& note) { this->notes.push_back(note); }

        //! Get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->created); }

        //! Get content groups
        const std::list<std::string>& get_content_groups() const { return(this->content_groups); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Name of parent derived product
        std::string parent_product;

        //! Path to output
        boost::filesystem::path filename;

        //! Creation time
        boost::posix_time::ptime created;

        //! Notes
        std::list<std::string> notes;

        //! Tags
        std::list<std::string> tags;

        //! content groups used to create
        std::list<std::string> content_groups;

	    };


    // PAYLOADS FOR OUTPUT GROUPS
    class precomputed_products: public serializable
	    {

      public:

        //! Create a precomputed products record
        precomputed_products()
	        : zeta_twopf(false),
	          zeta_threepf(false),
	          zeta_redbsp(false),
	          fNL_local(false),
	          fNL_equi(false),
	          fNL_ortho(false),
	          fNL_DBI(false)
	        {
	        }

        //! Deserialization constructor
        precomputed_products(Json::Value& reader);

        //! Destroy a precomputed products record
        ~precomputed_products() = default;


        // GET AND SET PROPERTIES

      public:

        //! Get precomputed zeta_twopf availability
        bool get_zeta_twopf() const { return(this->zeta_twopf); }
        //! Add zeta_twopf availability flag
        void add_zeta_twopf() { this->zeta_twopf = true; }

        //! Get precomputed zeta_threepf availability
        bool get_zeta_threepf() const { return(this->zeta_threepf); }
        //! Add zeta_threepf availability flag
        void add_zeta_threepf() { this->zeta_threepf = true; }

        //! Get precomputed zeta reduced bispectrum availability
        bool get_zeta_redbsp() const { return(this->zeta_redbsp); }
        //! Add zeta_threepf availability flag
        void add_zeta_redbsp() { this->zeta_redbsp = true; }

        //! Get precomputed fNL_local availability
        bool get_fNL_local() const { return(this->fNL_local); }
        //! Add fNL_local availability flag
        void add_fNL_local() { this->fNL_local = true; }

        //! Get precomputed fNL_equi availability
        bool get_fNL_equi() const { return(this->fNL_equi); }
        //! Add fNL_equi availability flag
        void add_fNL_equi() { this->fNL_equi = true; }

        //! Get precomputed fNL_ortho availability
        bool get_fNL_ortho() const { return(this->fNL_ortho); }
        //! Add fNL_ortho availability flag
        void add_fNL_ortho() { this->fNL_ortho = true; }

        //! Get precomputed fNL_DBI availability
        bool get_fNL_DBI() const { return(this->fNL_DBI); }
        //! Add fNL_DBI availability flag
        void add_fNL_DBI() { this->fNL_DBI = true; }


        // WRITE TO A STREAM

      public:

        void write(std::ostream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! group has pre-computed zeta twopf data?
        bool zeta_twopf;

        //! group has pre-computed zeta threepf data?
        bool zeta_threepf;

        //! group had pre-computed zeta reduced bispectrum data?
        bool zeta_redbsp;

        //! group has pre-computed fNL_local data?
        bool fNL_local;

        //! group has pre-computed fNL_equi data?
        bool fNL_equi;

        //! group has pre-computed fNL_ortho data?
        bool fNL_ortho;

        //! group has pre-computed fNL_DBI data?
        bool fNL_DBI;

	    };


    //! Integration payload
    class integration_payload: public serializable
	    {

      public:

        //! Create a payload
        integration_payload()
	        : metadata(),
            fail(false),
            workgroup_number(0),
            seeded(false),
            statistics(false)
	        {
	        }

        //! Deserialization constructor
        integration_payload(Json::Value& reader);

        //! Destroy a payload
        ~integration_payload() = default;


        // GET AND SET RECORD (META)DATA

      public:

        //! Get path of data container
        const boost::filesystem::path& get_container_path() const { return(this->container); }

        //! Set path of data container
        void set_container_path(const boost::filesystem::path& pt) { this->container = pt; }

        //! Get metadata
        const integration_metadata& get_metadata() const { return(this->metadata); }

        //! Set metadata
        void set_metadata(const integration_metadata& data) { this->metadata = data; }

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of failed serial numbers
        const std::list<unsigned int> get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(const std::list<unsigned int> f) { this->failed_serials = f; }

        //! Get workgroup number
        unsigned int get_workgroup_number() const { return(this->workgroup_number); }

        //! Set workgroup number
        void set_workgroup_number(unsigned int w) { this->workgroup_number = w; }

        //! Set seed
        void set_seed(const std::string& s) { this->seeded = true; this->seed_group = s; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seed group
        const std::string& get_seed_group() const { return(this->seed_group); }

        //! Set statistics flag
        void set_statistics(bool g) { this->statistics = g; }

        //! Get statistics flag
        bool has_statistics() const { return(this->statistics); }


        // GET AND SET PROPERTIES

      public:


        // WRITE TO A STREAM

      public:

        void write(std::ostream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Path to data container
        boost::filesystem::path container;

        //! Metadata
        integration_metadata metadata;

        //! mark this group as failed?
        bool fail;

        //! serial numbers reported failed
        std::list< unsigned int > failed_serials;

        //! workgroup number associated with this integration
        unsigned int workgroup_number;

        //! was this integration seeded?
        bool seeded;

        //! if this integration was seeded, parent output group
        std::string seed_group;

        //! does this group have per-configuration statistics?
        bool statistics;

	    };


    //! Postintegration payload
    class postintegration_payload: public serializable
	    {

      public:

        //! Create a payload
        postintegration_payload()
	        : metadata(),
	          precomputed(),
            paired(false),
            seeded(false)
	        {
	        }

        //! Deserialization constructor
        postintegration_payload(Json::Value& reader);

        //! Destroy a payload
        ~postintegration_payload() = default;


        // GET AND SET RECORD (META)DATA

      public:

        //! Get path of data container
        const boost::filesystem::path& get_container_path() const { return(this->container); }
        //! Set path of data container
        void set_container_path(const boost::filesystem::path& pt) { this->container = pt; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }
        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }


        // GET AND SET PROPERTIES

      public:

        //! Get precomputed products record
        precomputed_products& get_precomputed_products() { return(this->precomputed); }

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of failed serial numbers
        const std::list<unsigned int> get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(const std::list<unsigned int> f) { this->failed_serials = f; }

        //! Set pair
        void set_pair(bool g) { this->paired = g; }

        //! Query paired status
        bool is_paired() const { return(this->paired); }

        //! Set parent content group
        void set_parent_group(const std::string& p) { this->parent_group = p; }

        //! Query paired gorup
        const std::string& get_parent_group() const { return(this->parent_group); }

        //! Set seed
        void set_seed(const std::string& s) { this->seeded = true; this->seed_group = s; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seed group
        const std::string& get_seed_group() const { return(this->seed_group); }


        // WRITE TO A STREAM

      public:

        void write(std::ostream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Path to data container
        boost::filesystem::path container;

        //! Metadata
        output_metadata metadata;

        //! Precomputed products
        precomputed_products precomputed;

        //! mark this group as failed?
        bool fail;

        //! serial numbers reported failed
        std::list< unsigned int > failed_serials;

        //! Paired to an integration output group?
        bool paired;

        //! Paired output group name, if used
        std::string parent_group;

        //! was this postintegration seeded?
        bool seeded;

        //! if this postintegration was seeded, parent output group
        std::string seed_group;

	    };


    //! Derived product payload
    class output_payload: public serializable
	    {

      public:

        //! Create a payload
        output_payload()
	        : metadata(),
            fail(false)
	        {
	        }

        //! Deserialization constructor
        output_payload(Json::Value& reader);

        //! Destroy a payload
        ~output_payload() = default;


        // ADMIN

      public:

        //! Add an output
        void add_derived_content(const derived_content& prod) { this->content.push_back(prod); }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }
        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }


        // GET/SET PROPERTIES

      public:

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of used content groups
        const std::list<std::string>& get_content_groups() const { return(this->used_groups); }

        //! Set list of used content groups
        void set_content_groups(const std::list<std::string>& list) { this->used_groups = list; }

        // WRITE TO A STREAM

      public:

        void write(std::ostream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of derived outputs
        std::list<derived_content> content;

        //! Metadata
        output_metadata metadata;

        //! failed flag
        bool fail;

        //! list of content groups used to produce this output
        std::list<std::string> used_groups;

	    };


    //! Output group descriptor. Used to enumerate the output groups available for a particular task
    template <typename Payload>
    class output_group_record: public repository_record
	    {

      public:

        class paths_group
	        {
          public:
            boost::filesystem::path root;
            boost::filesystem::path output;
	        };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create an output_group_record descriptor
        output_group_record(const std::string& tn, const paths_group& p,
                            bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg,
                            repository_record::handler_package& pkg);

        //! Deserialization constructor
        output_group_record(Json::Value& reader, const boost::filesystem::path& root, repository_record::handler_package& pkg);

        //! Destroy an output_group_record descriptor
        ~output_group_record() = default;


        // GET AND SET METADATA

      public:

        //! Get task name
        const std::string& get_task_name() const { return(this->task); }

        //! Get locked flag
        bool get_lock_status() const { return (this->locked); }

        //! Set locked flag
        void set_lock_status(bool g) { this->locked = g; }

        //! Get notes
        const std::list<std::string>& get_notes() const { return (this->notes); }

        //! Add note
        void push_note(const std::string& note) { this->notes.push_back(note); }

        //! Get tags
        const std::list<std::string>& get_tags() const { return (this->tags); }

        //! Add tag
        void push_tag(const std::string& tag) { this->tags.push_back(tag); }

        //! Check whether we match a set of tags
        bool check_tags(std::list<std::string> match_tags) const;


        // ABSOLUTE PATHS

      public:

        //! Get path to repository root
        const boost::filesystem::path& get_abs_repo_path() const { return(this->paths.root); }

        //! Get path to output root (typically a subdir of the repository root)
        const boost::filesystem::path get_abs_output_path() const { return(this->paths.root/this->paths.output); }


        // PAYLOAD

      public:

        //! Get payload
        Payload& get_payload() { return(this->payload); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const override { return new output_group_record<Payload>(static_cast<const output_group_record<Payload>&>(*this)); };


        // WRITE TO A STREAM

      public:

        //! Write self to output stream
        void write(std::ostream& out) const;


        // INTERNAL DATA

      private:

        // PAYLOAD

        //! Payload
        Payload payload;


        // METADATA

        //! Parent task associated with this output.
        std::string task;

        //! Flag indicating whether or not this output group is locked
        bool locked;

        //! Array of strings representing notes attached to this group
        std::list<std::string> notes;

        //! Array of strings representing metadata tags
        std::list<std::string> tags;


        // PATHS

        //! Paths associated with this output group
        paths_group paths;

	    };


    // METADATA METHODS


    // REPOSITORY RECORD METADATA


    record_metadata::record_metadata()
	    : creation_time(boost::posix_time::second_clock::universal_time()),
	      last_edit_time(boost::posix_time::second_clock::universal_time()),    // don't initialize from creation_time; order of initialization depends on order of *declaration* in class, and that might change
	      runtime_api(__CPP_TRANSPORT_RUNTIME_API_VERSION)
	    {
	    }


    record_metadata::record_metadata(Json::Value& reader)
	    {
        std::string ctime_str = reader[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_CREATED].asString();
        this->creation_time = boost::posix_time::from_iso_string(ctime_str);

        std::string etime_str = reader[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_EDITED].asString();
        this->last_edit_time = boost::posix_time::from_iso_string(etime_str);

        this->runtime_api = reader[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_RUNTIME_API].asUInt();
	    }


    void record_metadata::serialize(Json::Value& writer) const
	    {
        Json::Value metadata(Json::objectValue);

        writer[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_CREATED] = boost::posix_time::to_iso_string(this->creation_time);
        writer[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_EDITED] = boost::posix_time::to_iso_string(this->last_edit_time);
        writer[__CPP_TRANSPORT_NODE_METADATA_GROUP][__CPP_TRANSPORT_NODE_METADATA_RUNTIME_API] = this->runtime_api;
	    }


    // GENERIC REPOSITORY RECORD


    repository_record::repository_record(repository_record::handler_package& pkg)
	    : metadata(),
	      handlers(pkg)
	    {
        name = boost::posix_time::to_iso_string(metadata.get_creation_time());
	    }


    repository_record::repository_record(const std::string& nm, repository_record::handler_package& pkg)
	    : name(nm),
	      metadata(),
	      handlers(pkg)
	    {
	    }


    repository_record::repository_record(Json::Value& reader, repository_record::handler_package& pkg)
	    : metadata(reader),
	      handlers(pkg)
	    {
		    name = reader[__CPP_TRANSPORT_NODE_RECORD_NAME].asString();
	    }


    void repository_record::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_NAME] = this->name;
        this->metadata.serialize(writer);
	    }


    // PACKAGE RECORD


    template <typename number>
    package_record<number>::package_record(const initial_conditions<number>& i, repository_record::handler_package& pkg)
	    : repository_record(i.get_name(), pkg),
	      ics(i)
	    {
	    }


    template <typename number>
    package_record<number>::package_record(Json::Value& reader, typename instance_manager<number>::model_finder& f,
                                           repository_record::handler_package& pkg)
	    : repository_record(reader, pkg),
	      ics(this->name, reader, f)        // name gets deserialized by repository_record, so is safe to use here
	    {
	    }


    template <typename number>
    void package_record<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_PACKAGE);
        this->ics.serialize(writer);
        this->repository_record::serialize(writer);
	    }


    // GENERIC TASK RECORD


		template <typename number>
    task_record<number>::task_record(const std::string& name, repository_record::handler_package& pkg)
	    : repository_record(name, pkg)
	    {
	    }


		template <typename number>
    task_record<number>::task_record(Json::Value& reader, repository_record::handler_package& pkg)
	    : repository_record(reader, pkg)
	    {
		    Json::Value& group_list = reader[__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS];
				assert(group_list.isArray());

        for(Json::Value::iterator t = group_list.begin(); t != group_list.end(); t++)
	        {
            std::string name = t->asString();
            this->content_groups.push_back(name);
	        }
	    }


		template <typename number>
    void task_record<number>::serialize(Json::Value& writer) const
	    {
		    Json::Value group_list(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = this->content_groups.begin(); t != this->content_groups.end(); t++)
	        {
            Json::Value group_element = *t;
		        group_list.append(group_element);
	        }
        writer[__CPP_TRANSPORT_NODE_TASK_OUTPUT_GROUPS] = group_list;

        this->repository_record::serialize(writer);
	    }


    // INTEGRATION TASK RECORD


    template <typename number>
    integration_task_record<number>::integration_task_record(const integration_task<number>& t, repository_record::handler_package& pkg)
	    : task_record<number>(t.get_name(), pkg),
	      tk(dynamic_cast<integration_task<number>*>(t.clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    integration_task_record<number>::integration_task_record(const integration_task_record<number>& obj)
	    : task_record<number>(obj),
	      tk(dynamic_cast<integration_task<number>*>(obj.tk->clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    integration_task_record<number>::integration_task_record(Json::Value& reader, typename repository_finder<number>::package_finder& f, repository_record::handler_package& pkg)
	    : task_record<number>(reader, pkg),
	      tk(integration_task_helper::deserialize<number>(this->name, reader, f))
	    {
        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
	    }


    template <typename number>
    integration_task_record<number>::~integration_task_record()
	    {
        delete this->tk;
	    }


    template <typename number>
    void integration_task_record<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_INTEGRATION_TASK);
        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
	    }


    // POSTINTEGRATION TASK RECORD


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const postintegration_task<number>& t, repository_record::handler_package& pkg)
	    : task_record<number>(t.get_name(), pkg),
	      tk(dynamic_cast<postintegration_task<number>*>(t.clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const postintegration_task_record<number>& obj)
	    : task_record<number>(obj),
	      tk(dynamic_cast<postintegration_task<number>*>(obj.tk->clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(Json::Value& reader, typename repository_finder<number>::task_finder& f, repository_record::handler_package& pkg)
	    : task_record<number>(reader, pkg),
	      tk(postintegration_task_helper::deserialize<number>(this->name, reader, f))
	    {
        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
	    }


    template <typename number>
    postintegration_task_record<number>::~postintegration_task_record()
	    {
        delete this->tk;
	    }


    template <typename number>
    void postintegration_task_record<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK);
        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
	    }


    // OUTPUT TASK RECORD


    template <typename number>
    output_task_record<number>::output_task_record(const output_task<number>& t, repository_record::handler_package& pkg)
	    : task_record<number>(t.get_name(), pkg),
	      tk(dynamic_cast<output_task<number>*>(t.clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    output_task_record<number>::output_task_record(const output_task_record<number>& obj)
	    : task_record<number>(obj),
	      tk(dynamic_cast<output_task<number>*>(obj.tk->clone()))
	    {
        assert(tk != nullptr);
	    }


    template <typename number>
    output_task_record<number>::output_task_record(Json::Value& reader, typename repository_finder<number>::derived_product_finder& f, repository_record::handler_package& pkg)
	    : task_record<number>(reader, pkg),
	      tk(output_task_helper::deserialize<number>(this->name, reader, f))
	    {
        assert(tk != nullptr);

        if(tk == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
	    }


    template <typename number>
    output_task_record<number>::~output_task_record()
	    {
        delete this->tk;
	    }


    template <typename number>
    void output_task_record<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_OUTPUT_TASK);
        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
	    }


    // DERIVED PRODUCT RECORD


    template <typename number>
    derived_product_record<number>::derived_product_record(const derived_data::derived_product<number>& prod, repository_record::handler_package& pkg)
	    : repository_record(prod.get_name(), pkg),
	      product(prod.clone())
	    {
        assert(product != nullptr);
	    }


    template <typename number>
    derived_product_record<number>::derived_product_record(const derived_product_record<number>& obj)
	    : repository_record(obj),
	      product(obj.product->clone())
	    {
        assert(product != nullptr);
	    }


    template <typename number>
    derived_product_record<number>::derived_product_record(Json::Value& reader, typename repository_finder<number>::task_finder& f, repository_record::handler_package& pkg)
	    : repository_record(reader, pkg),
	      product(derived_data::derived_product_helper::deserialize<number>(this->name, reader, f))
	    {
        assert(product != nullptr);

        if(product == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_REPO_PRODUCT_DESERIALIZE_FAIL);
	    }


    template <typename number>
    derived_product_record<number>::~derived_product_record()
	    {
        delete this->product;
	    }


    template <typename number>
    void derived_product_record<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_DERIVED_PRODUCT);
        this->product->serialize(writer);
        this->repository_record::serialize(writer);
	    }


    // OUTPUT METADATA


    output_metadata::output_metadata(Json::Value& reader)
	    {
        work_time                 = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME].asLargestInt();
        db_time                   = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME].asLargestInt();
        aggregation_time          = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME].asLargestInt();
        time_config_hits          = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS].asUInt();
        time_config_unloads       = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS].asUInt();
        twopf_kconfig_hits        = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS].asUInt();
        twopf_kconfig_unloads     = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS].asUInt();
        threepf_kconfig_hits      = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS].asUInt();
        threepf_kconfig_unloads   = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS].asUInt();
        data_hits                 = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS].asUInt();
        data_unloads              = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS].asUInt();
        zeta_hits                 = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_HITS].asUInt();
        zeta_unloads              = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_UNLOADS].asUInt();
        time_config_evictions     = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS].asLargestInt();
        twopf_kconfig_evictions   = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS].asLargestInt();
        threepf_kconfig_evictions = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS].asLargestInt();
        data_evictions            = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS].asLargestInt();
        zeta_evictions            = reader[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_EVICTIONS].asLargestInt();
	    }


    void output_metadata::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_WALLCLOCK_TIME]    = static_cast<Json::LargestInt>(this->work_time);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_DB_TIME]           = static_cast<Json::LargestInt>(this->db_time);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TOTAL_AGG_TIME]          = static_cast<Json::LargestInt>(aggregation_time);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_HITS]         = this->time_config_hits;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_UNLOADS]      = this->time_config_unloads;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_HITS]        = this->twopf_kconfig_hits;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_UNLOADS]     = this->twopf_kconfig_unloads;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_HITS]      = this->threepf_kconfig_hits;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_UNLOADS]   = this->threepf_kconfig_unloads;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_HITS]         = this->data_hits;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_UNLOADS]      = this->data_unloads;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_HITS]         = this->zeta_hits;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_UNLOADS]      = this->zeta_unloads;
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TIME_CACHE_EVICTIONS]    = static_cast<Json::LargestInt>(this->time_config_evictions);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_TWOPF_CACHE_EVICTIONS]   = static_cast<Json::LargestInt>(this->twopf_kconfig_evictions);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_THREEPF_CACHE_EVICTIONS] = static_cast<Json::LargestInt>(this->threepf_kconfig_evictions);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_DATA_CACHE_EVICTIONS]    = static_cast<Json::LargestInt>(this->data_evictions);
        writer[__CPP_TRANSPORT_NODE_OUTPUTDATA_GROUP][__CPP_TRANSPORT_NODE_OUTPUTDATA_ZETA_CACHE_EVICTIONS]    = static_cast<Json::LargestInt>(this->zeta_evictions);
	    }


    // INTEGRATION METADATA


    integration_metadata::integration_metadata(Json::Value& reader)
	    {
        total_wallclock_time        = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME].asLargestInt();
        total_aggregation_time      = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME].asLargestInt();
        total_integration_time      = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME].asLargestInt();
        min_mean_integration_time   = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_INT_TIME].asLargestInt();
        max_mean_integration_time   = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_INT_TIME].asLargestInt();
        global_min_integration_time = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME].asLargestInt();
        global_max_integration_time = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME].asLargestInt();
        total_batching_time         = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME].asLargestInt();
        min_mean_batching_time      = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_BATCH_TIME].asLargestInt();
        max_mean_batching_time      = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_BATCH_TIME].asLargestInt();
        global_min_batching_time    = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME].asLargestInt();
        global_max_batching_time    = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME].asLargestInt();
        total_configurations        = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS].asUInt();
        total_failures              = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_FAILURES].asUInt();
        total_refinements           = reader[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_REFINED].asUInt();
	    }


    void integration_metadata::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_WALLCLOCK_TIME]  = static_cast<Json::LargestInt>(this->total_wallclock_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_AGG_TIME]        = static_cast<Json::LargestInt>(this->total_aggregation_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_INT_TIME]        = static_cast<Json::LargestInt>(this->total_integration_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_INT_TIME]     = static_cast<Json::LargestInt>(this->min_mean_integration_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_INT_TIME]     = static_cast<Json::LargestInt>(this->max_mean_integration_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_INT_TIME]   = static_cast<Json::LargestInt>(this->global_min_integration_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_INT_TIME]   = static_cast<Json::LargestInt>(this->global_max_integration_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_TOTAL_BATCH_TIME]      = static_cast<Json::LargestInt>(this->total_batching_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MIN_MEAN_BATCH_TIME]   = static_cast<Json::LargestInt>(this->min_mean_batching_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_MAX_MEAN_BATCH_TIME]   = static_cast<Json::LargestInt>(this->max_mean_batching_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MIN_BATCH_TIME] = static_cast<Json::LargestInt>(this->global_min_batching_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_GLOBAL_MAX_BATCH_TIME] = static_cast<Json::LargestInt>(this->global_max_batching_time);
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_CONFIGURATIONS]    = this->total_configurations;
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_FAILURES]          = this->total_failures;
        writer[__CPP_TRANSPORT_NODE_TIMINGDATA_GROUP][__CPP_TRANSPORT_NODE_TIMINGDATA_NUM_REFINED]           = this->total_refinements;
	    }


    // OUTPUT_GROUP METHODS


    template <typename Payload>
    output_group_record<Payload>::output_group_record(const std::string& tn, const paths_group& p,
                                                      bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg,
                                                      repository_record::handler_package& pkg)
	    : repository_record(pkg),
	      task(tn),
	      paths(p),
	      locked(lock), notes(nt), tags(tg),
	      payload()
	    {
	    }


    template <typename Payload>
    void output_group_record<Payload>::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_OUTPUT_GROUP;
        if(this->locked) out << ", " << __CPP_TRANSPORT_OUTPUT_GROUP_LOCKED;
        out << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_REPO_ROOT << " = " << this->paths.root << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_DATA_ROOT << " = " << this->paths.output << std::endl;

        unsigned int count = 0;

        for(std::list<std::string>::const_iterator t = this->notes.begin(); t != this->notes.end(); t++)
	        {
            out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_NOTE << " " << count << std::endl;
            out << "    " << *t << std::endl;
            count++;
	        }

        count = 0;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_TAGS << ": ";
        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); t++)
	        {
            if(count > 0) out << ", ";
            out << *t;
            count++;
	        }
        out << std::endl;

        this->payload.write(out);

        out << std::endl;
	    }


    template <typename Payload>
    bool output_group_record<Payload>::check_tags(std::list<std::string> match_tags) const
	    {
        // remove all this group's tags from the matching set.
        // If any remain after this process, then the match set isn't a subset of the group's tags.
        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); t++)
	        {
            match_tags.remove(*t);
	        }

        return (!match_tags.empty());
	    }


    template <typename Payload>
    output_group_record<Payload>::output_group_record(Json::Value& reader, const boost::filesystem::path& root,
                                                      repository_record::handler_package& pkg)
	    : repository_record(reader, pkg),
	      payload(reader)
	    {
        paths.root = root;

        task         = reader[__CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME].asString();
        paths.output = reader[__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT].asString();
        locked       = reader[__CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED].asBool();

        Json::Value note_list = reader[__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES];
				assert(note_list.isArray());

        for(Json::Value::iterator t = note_list.begin(); t != note_list.end(); t++)
	        {
            notes.push_back(t->asString());
	        }

        Json::Value tag_list = reader[__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS];
		    assert(tag_list.isArray());

        for(Json::Value::iterator t = tag_list.begin(); t != tag_list.end(); t++)
	        {
            tags.push_back(t->asString());
	        }
	    }


    template <typename Payload>
    void output_group_record<Payload>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RECORD_TYPE] = std::string(__CPP_TRANSPORT_NODE_RECORD_CONTENT);

        writer[__CPP_TRANSPORT_NODE_OUTPUTGROUP_TASK_NAME] = this->task;
        writer[__CPP_TRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT] = this->paths.output.string();
        writer[__CPP_TRANSPORT_NODE_OUTPUTGROUP_LOCKED]    = this->locked;

        Json::Value note_list(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = this->notes.begin(); t != this->notes.end(); t++)
	        {
            Json::Value note_element = *t;
		        note_list.append(note_element);
	        }
        writer[__CPP_TRANSPORT_NODE_OUTPUTGROUP_NOTES] = note_list;

        Json::Value tag_list(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = tags.begin(); t != tags.end(); t++)
	        {
            Json::Value tag_element = *t;
		        tag_list.append(tag_element);
	        }
        writer[__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS] = tag_list;

        this->payload.serialize(writer);

        this->repository_record::serialize(writer);
	    }


    // output an output_group_record descriptor to a standard stream
    // notice obscure syntax to declare a templated member of an explicitly named namespace
    template <typename Payload>
    std::ostream& operator<<(std::ostream& out, const output_group_record<Payload>& group)
	    {
        group.write(out);
        return (out);
	    }


    namespace
	    {

        namespace output_group_helper
	        {

            // used for sorting a list of output_groups into decreasing chronological order
            template <typename Payload>
            bool comparator(const std::shared_ptr< output_group_record<Payload> >& A,
                            const std::shared_ptr< output_group_record<Payload> >& B)
	            {
                return (A->get_creation_time() > B->get_creation_time());
	            }

	        }   // namespace output_group_helper

	    }   // unnamed namespace


    precomputed_products::precomputed_products(Json::Value& reader)
	    {
        zeta_twopf   = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF].asBool();
        zeta_threepf = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF].asBool();
        zeta_redbsp  = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP].asBool();
        fNL_local    = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL].asBool();
        fNL_equi     = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_EQUI].asBool();
        fNL_ortho    = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO].asBool();
        fNL_DBI      = reader[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_DBI].asBool();
	    }


    void precomputed_products::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF]   = this->zeta_twopf;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF] = this->zeta_threepf;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP]  = this->zeta_redbsp;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL]    = this->fNL_local;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_EQUI]     = this->fNL_equi;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO]    = this->fNL_ortho;
        writer[__CPP_TRANSPORT_NODE_PRECOMPUTED_ROOT][__CPP_TRANSPORT_NODE_PRECOMPUTED_FNL_DBI]      = this->fNL_DBI;
	    }


    void precomputed_products::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_PAYLOAD_HAS_ZETA_TWO << ": " << (this->zeta_twopf ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_ZETA_THREE << ": " << (this->zeta_threepf ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_ZETA_REDBSP << ": " << (this->zeta_redbsp ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_FNL_LOCAL << ": " << (this->fNL_local ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_FNL_EQUI << ": " << (this->fNL_equi ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_FNL_ORTHO << ": " << (this->fNL_ortho ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
        out << __CPP_TRANSPORT_PAYLOAD_HAS_FNL_DBI << ": " << (this->fNL_DBI ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << std::endl;
	    }


    integration_payload::integration_payload(Json::Value& reader)
	    : metadata(reader)
	    {
        container        = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE].asString();
        fail             = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED].asBool();
        workgroup_number = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP].asUInt();
        seeded           = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED].asBool();
        seed_group       = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP].asString();
        statistics       = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS].asBool();

        Json::Value failure_array = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS];
        assert(failure_array.isArray());
        failed_serials.clear();
        for(Json::Value::iterator t = failure_array.begin(); t != failure_array.end(); t++)
          {
            failed_serials.push_back(t->asUInt());
          }
	    }


    void integration_payload::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE]   = this->container.string();
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED]     = this->fail;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP]  = this->workgroup_number;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED]     = this->seeded;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP] = this->seed_group;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS] = this->statistics;

        Json::Value failure_array(Json::arrayValue);
        for(std::list<unsigned int>::const_iterator t = this->failed_serials.begin(); t != this->failed_serials.end(); t++)
          {
            Json::Value element = *t;
            failure_array.append(element);
          }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS] = failure_array;

        this->metadata.serialize(writer);
	    }


    void integration_payload::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_PAYLOAD_INTEGRATION_DATA << " = " << this->container << std::endl;
	    }


    postintegration_payload::postintegration_payload(Json::Value& reader)
	    : metadata(reader),
	      precomputed(reader)
	    {
        container    = reader[__CPP_TRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE].asString();
        fail         = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED].asBool();
        paired       = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED].asBool();
        parent_group = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP].asString();
        seeded       = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED].asBool();
        seed_group   = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP].asString();

        Json::Value failure_array = reader[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS];
        assert(failure_array.isArray());
        failed_serials.clear();
        for(Json::Value::iterator t = failure_array.begin(); t != failure_array.end(); t++)
          {
            failed_serials.push_back(t->asUInt());
          }
	    }


    void postintegration_payload::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_DATABASE]     = this->container.string();
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED]       = this->fail;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED]       = this->paired;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP] = this->parent_group;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED]       = this->seeded;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP]   = this->seed_group;

        Json::Value failure_array(Json::arrayValue);
        for(std::list<unsigned int>::const_iterator t = this->failed_serials.begin(); t != this->failed_serials.end(); t++)
          {
            Json::Value element = *t;
            failure_array.append(element);
          }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS] = failure_array;

        this->metadata.serialize(writer);
        this->precomputed.serialize(writer);
	    }


    void postintegration_payload::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_PAYLOAD_INTEGRATION_DATA << " = " << this->container << std::endl;

        this->precomputed.write(out);
	    }


    output_payload::output_payload(Json::Value& reader)
	    : metadata(reader)
	    {
        fail = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FAILED].asBool();

        Json::Value& content_array = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY];
		    assert(content_array.isArray());

        for(Json::Value::iterator t = content_array.begin(); t != content_array.end(); t++)
	        {
            this->content.push_back( derived_content(*t) );
	        }
	    }


    void output_payload::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FAILED] = this->fail;

        Json::Value content_array(Json::arrayValue);

        for(std::list<derived_content>::const_iterator t = this->content.begin(); t != this->content.end(); t++)
	        {
            Json::Value element(Json::objectValue);
            (*t).serialize(element);
		        content_array.append(element);
	        }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY] = content_array;

        this->metadata.serialize(writer);
	    }


    void output_payload::write(std::ostream& out) const
	    {
        for(std::list<derived_content>::const_iterator t = this->content.begin(); t != this->content.end(); t++)
	        {
            out << __CPP_TRANSPORT_PAYLOAD_OUTPUT_CONTENT_PRODUCT << " = " << (*t).get_parent_product() << ", "
	            << __CPP_TRANSPORT_PAYLOAD_OUTPUT_CONTENT_PATH    << " = " << (*t).get_filename() << std::endl;
	        }
	    }


    derived_content::derived_content(Json::Value& reader)
	    {
        parent_product = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME].asString();
        filename = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME].asString();

        std::string ctime_string = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_CREATED].asString();
        created = boost::posix_time::from_iso_string(ctime_string);

        Json::Value& content_groups_array = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS];
        assert(content_groups_array.isArray());

        for(Json::Value::iterator t = content_groups_array.begin(); t != content_groups_array.end(); t++)
          {
            this->content_groups.push_back(t->asString());
          }

        Json::Value note_list = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_NOTES];
		    assert(note_list.isArray());

        for(Json::Value::iterator t = note_list.begin(); t != note_list.end(); t++)
	        {
            notes.push_back(t->asString());
	        }

        Json::Value tag_list = reader[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_TAGS];
		    assert(tag_list.isArray());

        for(Json::Value::iterator t = tag_list.begin(); t != tag_list.end(); t++)
	        {
            tags.push_back(t->asString());
	        }
	    }


    void derived_content::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME] = this->parent_product;
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME]     = this->filename.string();
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_CREATED]      = boost::posix_time::to_iso_string(this->created);

        Json::Value content_groups_array(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = this->content_groups.begin(); t != this->content_groups.end(); t++)
          {
            Json::Value element = *t;
            content_groups_array.append(element);
          }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS] = content_groups_array;

        Json::Value note_list(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = this->notes.begin(); t != this->notes.end(); t++)
	        {
            Json::Value note_element = *t;
		        note_list.append(note_element);
	        }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_NOTES] = note_list;

        Json::Value tag_list(Json::arrayValue);

        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); t++)
	        {
            Json::Value tag_element = *t;
		        tag_list.append(tag_element);
	        }
        writer[__CPP_TRANSPORT_NODE_PAYLOAD_CONTENT_TAGS] = tag_list;
	    }

	}   // namespace transport


#endif //__repository_records_H_
