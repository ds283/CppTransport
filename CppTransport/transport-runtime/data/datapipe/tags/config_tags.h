//
// Created by David Seery on 28/04/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef __config_tags_H_
#define __config_tags_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"

#include "transport-runtime/instruments/timing_instrument.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
	{

    //! time configuration data group tag
    template <typename number>
    class time_config_tag
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        time_config_tag(datapipe<number>* p)
	        : pipe(p)
	        {
	        }

        ~time_config_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        bool operator==(const time_config_tag<number>& obj) const { return(true); }   // nothing to check

        //! pull data corresponding to this tag
        void pull(derived_data::SQL_time_config_query& query, std::vector<time_config>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::normal) << log_item; }

        //! identify this tag
        std::string name() const { return(std::string("time config")); }


        // CLONE

      public:

        //! copy this object
        time_config_tag<number>* clone() const { return new time_config_tag<number>(static_cast<const time_config_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        unsigned int hash() const { return(0); }


        // INTERNAL DATA

      protected:

        //! parent datapipe
        datapipe<number>* pipe;

	    };


    //! twopf k-configuration data group tag
    template <typename number>
    class twopf_kconfig_tag
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        twopf_kconfig_tag(datapipe<number>* p)
	        : pipe(p)
	        {
	        }

        ~twopf_kconfig_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        bool operator==(const twopf_kconfig_tag<number>& obj) const { return(true); }     // nothing to check

        //! pull data corresponding to this tag
        void pull(derived_data::SQL_twopf_kconfig_query& query, std::vector<twopf_kconfig>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::normal) << log_item; }

        //! identify this tag
        std::string name() const { return(std::string("twopf k-config")); }


        // CLONE

      public:

        //! copy this object
        twopf_kconfig_tag<number>* clone() const { return new twopf_kconfig_tag<number>(static_cast<const twopf_kconfig_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        unsigned int hash() const { return(0); }


        // INTERNAL DATA

      protected:

        //! parent pipe
        datapipe<number>* pipe;

	    };


    //! threepf k-configuration data group tag
    template <typename number>
    class threepf_kconfig_tag
	    {

        // CONTRUCTOR, DESTRUCTOR

      public:

        threepf_kconfig_tag(datapipe<number>* p)
	        : pipe(p)
	        {
	        }

        ~threepf_kconfig_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        bool operator==(const threepf_kconfig_tag<number>& obj) const { return(true); }     // nothing to check

        //! pull data corresponding to this tag
        void pull(derived_data::SQL_threepf_kconfig_query& query, std::vector<threepf_kconfig>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::normal) << log_item; }

        //! identify this tag
        std::string name() const { return(std::string("threepf k-config")); }


        // CLONE

      public:

        //! copy this object
        threepf_kconfig_tag<number>* clone() const { return new threepf_kconfig_tag<number>(static_cast<const threepf_kconfig_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        unsigned int hash() const { return(0); }


        // INTERNAL DATA

      protected:

        //! sampler function
        datapipe<number>* pipe;

	    };


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void time_config_tag<number>::pull(derived_data::SQL_time_config_query& query, std::vector<time_config>& data)
	    {
        // check we are attached to a content group
        // time configuration data can be extracted from any container, so it doesn't matter what sort of group
        // we are attached to
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL time sample request";
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->data_mgr.pull_time_config(this->pipe, query, data);
	    }


    template <typename number>
    void twopf_kconfig_tag<number>::pull(derived_data::SQL_twopf_kconfig_query& query, std::vector<twopf_kconfig>& data)
	    {
        // TODO: should work out whether this content group can serve twopf configuration data?
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL 2pf k-configuration sample request";
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->data_mgr.pull_kconfig_twopf(this->pipe, query, data);
	    }


    template <typename number>
    void threepf_kconfig_tag<number>::pull(derived_data::SQL_threepf_kconfig_query& query, std::vector<threepf_kconfig>& data)
	    {
        // TODO: should work out whether this content group can serve threepf configuration data?
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL 3pf k-configuration sample request";
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->data_mgr.pull_kconfig_threepf(this->pipe, query, data);
	    }


    // TAG EQUALITY -- IMPLEMENTATION


	}   // namespace transport


#endif //__config_tags_H_
