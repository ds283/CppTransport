//
// Created by David Seery on 28/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __field_tags_H_
#define __field_tags_H_


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

    //! data group tag -- abstract group used to derive background and field tags
    template <typename number>
    class data_tag
	    {

      public:

        typedef enum { cf_twopf_re, cf_twopf_im, cf_threepf, cf_tensor_twopf } cf_data_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        data_tag(datapipe<number>* p)
	        : pipe(p)
	        {
            assert(pipe != nullptr);
	        }

        virtual ~data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const = 0;

        //! virtual function to pull a cache line
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) = 0;

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::normal) << log_item; }

        //! virtual function to identify this tag
        virtual std::string name() const = 0;

        // CLONE

      public:

        //! copy this object
        virtual data_tag<number>* clone() const = 0;


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const = 0;


        // INTERNAL DATA

      protected:

        //! parent datapipe
        datapipe<number>* pipe;

	    };


    //! background time data group tag
    template <typename number>
    class background_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        background_time_data_tag(datapipe<number>* p, unsigned int i)
	        : data_tag<number>(p),
	          id(i)
	        {
	        }

        virtual ~background_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "background field " << id; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        background_time_data_tag<number>* clone() const { return new background_time_data_tag<number>(static_cast<const background_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->id*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! data id - controls which background field is sampled
        const unsigned int id;

	    };


    //! field correlation-function time data group tag
    template <typename number>
    class cf_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        cf_time_data_tag(datapipe<number>* p, typename data_tag<number>::cf_data_type t, unsigned int i, unsigned int k)
	        : data_tag<number>(p),
	          type(t),
	          id(i),
	          kserial(k)
	        {
	        }

        virtual ~cf_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        cf_time_data_tag<number>* clone() const { return new cf_time_data_tag<number>(static_cast<const cf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kserial*8761 + this->id*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! type
        const typename data_tag<number>::cf_data_type type;

        //! data id - controls which element id is sampled
        const unsigned int id;

        //! kserial - controls which k serial number is sampled
        const unsigned int kserial;

	    };


    //! field correlation-function kconfig data group tag
    template <typename number>
    class cf_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        cf_kconfig_data_tag(datapipe<number>* p, typename data_tag<number>::cf_data_type t, unsigned int i, unsigned int ts)
	        : data_tag<number>(p),
	          type(t),
	          id(i),
	          tserial(ts)
	        {
	        }

        virtual ~cf_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        cf_kconfig_data_tag<number>* clone() const { return new cf_kconfig_data_tag<number>(static_cast<const cf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*8761 + this->id*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! type
        const typename data_tag<number>::cf_data_type type;

        //! data id - controls which element id is samples
        const unsigned int id;

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

	    };


    // TAG NAMES -- IMPLEMENTATION


    template <typename number>
    std::string cf_time_data_tag<number>::name() const
	    {
        std::ostringstream msg;

        switch(this->type)
	        {
            case data_tag<number>::cf_twopf_re:
	            msg << "real twopf (time series)";
            break;

            case data_tag<number>::cf_twopf_im:
	            msg << "imaginary twopf (time series";
            break;

            case data_tag<number>::cf_threepf:
	            msg << "threepf (time series)";
            break;

            case data_tag<number>::cf_tensor_twopf:
	            msg << "tensor twopf (time series)";
            break;

            default:
	            msg << "unknown";
	        }

        msg << ", element = " << this->id << ", kserial = " << this->kserial;
        return(msg.str());
	    }


    template <typename number>
    std::string cf_kconfig_data_tag<number>::name() const
	    {
        std::ostringstream msg;

        switch(this->type)
	        {
            case data_tag<number>::cf_twopf_re:
	            msg << "real twopf (kconfig series)";
            break;

            case data_tag<number>::cf_twopf_im:
	            msg << "imaginary twopf (kconfig series";
            break;

            case data_tag<number>::cf_threepf:
	            msg << "threepf (kconfig series)";
            break;

            case data_tag<number>::cf_tensor_twopf:
	            msg << "tensor twopf (kconfig series)";
            break;

            default:
	            msg << "unknown";
	        }

        msg << ", element = " << this->id << ", tserial = " << this->tserial;
        return(msg.str());
	    }


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void background_time_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::integration_attached)) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL background time sample request for element " << this->id;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.background(this->pipe, this->id, query, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void cf_time_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::integration_attached)) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == data_tag<number>::cf_twopf_re)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf time sample request, type = real, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, query, this->kserial, sample, datapipe<number>::twopf_real);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_twopf_im)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf time sample request, type = imaginary, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, query, this->kserial, sample, datapipe<number>::twopf_imag);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_threepf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL threepf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.threepf(this->pipe, this->id, query, this->kserial, sample);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_tensor_twopf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL tensor twopf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.tensor_twopf(this->pipe, this->id, query, this->kserial, sample);
            this->pipe->database_timer.stop();
	        }
        else
	        {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
	        }
	    }


    template <typename number>
    void cf_kconfig_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::integration_attached)) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == data_tag<number>::cf_twopf_re)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = real, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, query, this->tserial, sample, datapipe<number>::twopf_real);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_twopf_im)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = imaginary, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, query, this->tserial, sample, datapipe<number>::twopf_imag);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_threepf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL threepf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.threepf(this->pipe, this->id, query, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_tensor_twopf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL tensor twopf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.tensor_twopf(this->pipe, this->id, query, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else
	        {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
	        }
	    }


    // TAG EQUALITY -- IMPLEMENTATION


    template <typename number>
    bool background_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const background_time_data_tag<number>* bg_tag = dynamic_cast<const background_time_data_tag<number>*>(&obj);

        if(bg_tag == nullptr) return(false);
        return(this->id == bg_tag->id);
	    }


    template <typename number>
    bool cf_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const cf_time_data_tag<number>* cf_tag = dynamic_cast<const cf_time_data_tag<number>*>(&obj);

        if(cf_tag == nullptr) return(false);
        return(this->type == cf_tag->type && this->id == cf_tag->id && this->kserial == cf_tag->kserial);
	    }


    template <typename number>
    bool cf_kconfig_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const cf_kconfig_data_tag<number>* cf_tag = dynamic_cast<const cf_kconfig_data_tag<number>*>(&obj);

        if(cf_tag == nullptr) return(false);
        return(this->type == cf_tag->type && this->id == cf_tag->id && this->tserial == cf_tag->tserial);
	    }

	}   // namespace transport


#endif //__field_tags_H_
