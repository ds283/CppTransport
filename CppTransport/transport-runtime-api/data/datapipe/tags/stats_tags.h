//
// Created by David Seery on 28/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __stats_tags_H_
#define __stats_tags_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"

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

    // k-configuration statistics tag
    template <typename number>
    class k_statistics_tag
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        k_statistics_tag(datapipe<number>* p)
	        : pipe(p)
	        {
	        }

        ~k_statistics_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        bool operator==(const k_statistics_tag<number>& obj) const { return(true); }    // nothing to check

        //! pull data corresponding to this tag
        void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<kconfiguration_statistics>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::normal) << log_item; }

        //! identify this tag
        std::string name() const { return(std::string("k-configuration statistics")); }


        // CLONE

      public:

        //! copy this object
        k_statistics_tag<number>* clone() const { return new k_statistics_tag<number>(static_cast<const k_statistics_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        unsigned int hash() const { return(0); }


        // INTERNAL DATA

      protected:

        //! parent datapipe
        datapipe<number>* pipe;

	    };


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void k_statistics_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<kconfiguration_statistics>& data)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::integration_attached)) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        std::shared_ptr< output_group_record<integration_payload> > record = pipe->get_attached_integration_record();
        if(!record) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(record->get_payload().has_statistics())
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_statistics.k_statistics(this->pipe, query, data);
            this->pipe->database_timer.stop();
	        }
        else
	        {
            data.clear();
	        }
	    }


    // TAG EQUALITY -- IMPLEMENTATION

	}   // namespace transport


#endif //__stats_tags_H_
