//
// Created by David Seery on 28/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __fNL_tags_H_
#define __fNL_tags_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"

#include "transport-runtime-api/instruments/timing_instrument.h"

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

    // fNL time-series tag
    template <typename number>
    class fNL_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        fNL_time_data_tag(datapipe<number>* p, derived_data::template_type ty)
	        : data_tag<number>(p),
	          type(ty)
	        {
	        }

        virtual ~fNL_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "fNL, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        virtual fNL_time_data_tag<number>* clone() const override { return new fNL_time_data_tag<number>(static_cast<const fNL_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! template type
        typename derived_data::template_type type;

	    };


    // bispectrum.template time-series tag
    template <typename number>
    class BT_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        BT_time_data_tag(datapipe<number>* p, derived_data::template_type ty)
	        : data_tag<number>(p),
	          type(ty)
	        {
	        }

        virtual ~BT_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "bispectrum.template, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        BT_time_data_tag<number>* clone() const { return new BT_time_data_tag<number>(static_cast<const BT_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! template type
        typename derived_data::template_type type;

	    };


    // template.template time-series tag
    template <typename number>
    class TT_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        TT_time_data_tag(datapipe<number>* p, derived_data::template_type ty)
	        : data_tag<number>(p),
	          type(ty)
	        {
	        }

        virtual ~TT_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "template.template, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        TT_time_data_tag<number>* clone() const { return new TT_time_data_tag<number>(static_cast<const TT_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! template type
        typename derived_data::template_type type;

	    };


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void fNL_time_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL fNL sample request, template = " << template_type(this->type);
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->pull_timeslice.fNL(this->pipe, query, sample, this->type);
	    }


    template <typename number>
    void BT_time_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL bispectrum.template sample request, template = " << template_type(this->type);
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->pull_timeslice.BT(this->pipe, query, sample, this->type);
	    }


    template <typename number>
    void TT_time_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL template.template sample request, template = " << template_type(this->type);
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->pull_timeslice.BT(this->pipe, query, sample, this->type);
	    }


    // TAG EQUALITY -- IMPLEMENTATION


    template <typename number>
    bool fNL_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const fNL_time_data_tag<number>* fNL_tag = dynamic_cast<const fNL_time_data_tag<number>*>(&obj);

        if(fNL_tag == nullptr) return(false);
        return(this->type == fNL_tag->type);
	    }


    template <typename number>
    bool BT_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const BT_time_data_tag<number>* BT_tag = dynamic_cast<const BT_time_data_tag<number>*>(&obj);

        if(BT_tag == nullptr) return(false);
        return(this->type == BT_tag->type);
	    }


    template <typename number>
    bool TT_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const TT_time_data_tag<number>* TT_tag = dynamic_cast<const TT_time_data_tag<number>*>(&obj);

        if(TT_tag == nullptr) return(false);
        return(this->type == TT_tag->type);
	    }

	}   // namespace transport


#endif //__fNL_tags_H_
