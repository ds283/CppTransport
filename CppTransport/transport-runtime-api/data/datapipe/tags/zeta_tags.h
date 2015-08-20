//
// Created by David Seery on 28/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __zeta_tags_H_
#define __zeta_tags_H_


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
    //! zeta two-point function time data tag
    template <typename number>
    class zeta_twopf_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_twopf_time_data_tag(datapipe<number>* p, const twopf_kconfig& k)
	        : data_tag<number>(p),
	          kdata(k)
	        {
	        }

        virtual ~zeta_twopf_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_twopf_time_data_tag<number>* clone() const { return new zeta_twopf_time_data_tag<number>(static_cast<const zeta_twopf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! kserial - controls which k serial number is sampled
        const twopf_kconfig kdata;

	    };


    //! zeta two-point function time data tag
    template <typename number>
    class zeta_threepf_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_threepf_time_data_tag(datapipe<number>* p, const threepf_kconfig& k)
	        : data_tag<number>(p),
	          kdata(k)
	        {
	        }

        virtual ~zeta_threepf_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_threepf_time_data_tag<number>* clone() const { return new zeta_threepf_time_data_tag<number>(static_cast<const zeta_threepf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! kserial - controls which k serial number is sampled
        const threepf_kconfig kdata;

	    };


    //! zeta two-point function time data tag
    template <typename number>
    class zeta_reduced_bispectrum_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_reduced_bispectrum_time_data_tag(datapipe<number>* p, const threepf_kconfig& k)
	        : data_tag<number>(p),
	          kdata(k)
	        {
	        }

        virtual ~zeta_reduced_bispectrum_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_reduced_bispectrum_time_data_tag<number>* clone() const { return new zeta_reduced_bispectrum_time_data_tag<number>(static_cast<const zeta_reduced_bispectrum_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! kserial - controls which k serial number is sampled
        const threepf_kconfig kdata;

	    };


    //! zeta correlation-function kconfig data tag
    template <typename number>
    class zeta_twopf_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_twopf_kconfig_data_tag(datapipe<number>* p, unsigned int ts)
	        : data_tag<number>(p),
	          tserial(ts)
	        {
	        }

        virtual ~zeta_twopf_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_twopf_kconfig_data_tag<number>* clone() const { return new zeta_twopf_kconfig_data_tag<number>(static_cast<const zeta_twopf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

	    };


    //! zeta correlation-function kconfig data tag
    template <typename number>
    class zeta_threepf_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_threepf_kconfig_data_tag(datapipe<number>* p, unsigned int ts)
	        : data_tag<number>(p),
	          tserial(ts)
	        {
	        }

        virtual ~zeta_threepf_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_threepf_kconfig_data_tag<number>* clone() const { return new zeta_threepf_kconfig_data_tag<number>(static_cast<const zeta_threepf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

	    };


    //! zeta correlation-function kconfig data tag
    template <typename number>
    class zeta_reduced_bispectrum_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_reduced_bispectrum_kconfig_data_tag(datapipe<number>* p, unsigned int ts)
	        : data_tag<number>(p),
	          tserial(ts)
	        {
	        }

        virtual ~zeta_reduced_bispectrum_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        zeta_reduced_bispectrum_kconfig_data_tag<number>* clone() const { return new zeta_reduced_bispectrum_kconfig_data_tag<number>(static_cast<const zeta_reduced_bispectrum_kconfig_data_tag&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

	    };


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void zeta_twopf_time_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        // check that we are attached to a content group
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta twopf time sample request, k-configuration " << this->kdata.kserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.zeta_twopf(this->pipe, query, this->kdata.serial, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void zeta_threepf_time_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta threepf time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.zeta_threepf(this->pipe, query, this->kdata.serial, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void zeta_reduced_bispectrum_time_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta reduced bispectrum time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.zeta_redbsp(this->pipe, query, this->kdata.serial, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void zeta_twopf_kconfig_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta twopf kconfig sample request, t-serial " << this->tserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_kslice.zeta_twopf(this->pipe, query, this->tserial, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void zeta_threepf_kconfig_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta threepf kconfig sample request, t-serial " << this->tserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_kslice.zeta_threepf(this->pipe, query, this->tserial, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void zeta_reduced_bispectrum_kconfig_data_tag<number>::pull(std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::postintegration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL zeta reduced bispectrum kconfig sample request, t-serial " << this->tserial;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_kslice.zeta_redbsp(this->pipe, query, this->tserial, sample);
        this->pipe->database_timer.stop();
	    }


    // TAG EQUALITY -- IMPLEMENTATION


    template <typename number>
    bool zeta_twopf_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_twopf_time_data_tag<number>* zeta_tag = dynamic_cast<const zeta_twopf_time_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
	    }


    template <typename number>
    bool zeta_threepf_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_threepf_time_data_tag<number>* zeta_tag = dynamic_cast<const zeta_threepf_time_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
	    }


    template <typename number>
    bool zeta_reduced_bispectrum_time_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_reduced_bispectrum_time_data_tag<number>* zeta_tag = dynamic_cast<const zeta_reduced_bispectrum_time_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->kdata.serial == zeta_tag->kdata.serial);
	    }


    template <typename number>
    bool zeta_twopf_kconfig_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_twopf_kconfig_data_tag<number>* zeta_tag = dynamic_cast<const zeta_twopf_kconfig_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
	    }


    template <typename number>
    bool zeta_threepf_kconfig_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_threepf_kconfig_data_tag<number>* zeta_tag = dynamic_cast<const zeta_threepf_kconfig_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
	    }


    template <typename number>
    bool zeta_reduced_bispectrum_kconfig_data_tag<number>::operator==(const data_tag<number>& obj) const
	    {
        const zeta_reduced_bispectrum_kconfig_data_tag<number>* zeta_tag = dynamic_cast<const zeta_reduced_bispectrum_kconfig_data_tag<number>*>(&obj);

        if(zeta_tag == nullptr) return(false);
        return(this->tserial == zeta_tag->tserial);
	    }

	}   // namespace transport


#endif //__zeta_tags_H_
