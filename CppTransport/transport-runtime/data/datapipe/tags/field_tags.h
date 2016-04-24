//
// Created by David Seery on 28/04/15.
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


#ifndef __field_tags_H_
#define __field_tags_H_


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

    enum class cf_data_type { cf_twopf_re, cf_twopf_im, cf_threepf_momentum, cf_threepf_Nderiv, cf_tensor_twopf };

    //! data group tag -- abstract group used to derive background and field tags
    template <typename number>
    class data_tag
	    {

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
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) = 0;

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::normal) << log_item; }

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
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "background field " << id; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        virtual background_time_data_tag<number>* clone() const override { return new background_time_data_tag<number>(static_cast<const background_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->id*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


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

        cf_time_data_tag(datapipe<number>* p, cf_data_type t, unsigned int i, unsigned int k)
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
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        virtual cf_time_data_tag<number>* clone() const override { return new cf_time_data_tag<number>(static_cast<const cf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kserial*8761 + this->id*2141) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! type
        const cf_data_type type;

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

        cf_kconfig_data_tag(datapipe<number>* p, cf_data_type t, unsigned int i, unsigned int ts)
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
        virtual void pull(derived_data::SQL_query& query, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        virtual cf_kconfig_data_tag<number>* clone() const override { return new cf_kconfig_data_tag<number>(static_cast<const cf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*8761 + this->id*2131) % CPPTRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! type
        const cf_data_type type;

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
            case cf_data_type::cf_twopf_re:
	            msg << "real twopf (time series)";
              break;

            case cf_data_type::cf_twopf_im:
	            msg << "imaginary twopf (time series";
              break;

            case cf_data_type::cf_threepf_momentum:
	            msg << "threepf-momentum (time series)";
              break;

            case cf_data_type::cf_threepf_Nderiv:
              msg << "threepf-Nderiv (time series)";
              break;

            case cf_data_type::cf_tensor_twopf:
	            msg << "tensor twopf (time series)";
              break;
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
            case cf_data_type::cf_twopf_re:
	            msg << "real twopf (kconfig series)";
              break;

            case cf_data_type::cf_twopf_im:
	            msg << "imaginary twopf (kconfig series";
              break;

            case cf_data_type::cf_threepf_momentum:
	            msg << "threepf-momentum (kconfig series)";
              break;

            case cf_data_type::cf_threepf_Nderiv:
              msg << "threepf-Nderiv (kconfig series)";
              break;

            case cf_data_type::cf_tensor_twopf:
	            msg << "tensor twopf (kconfig series)";
              break;
	        }

        msg << ", element = " << this->id << ", tserial = " << this->tserial;
        return(msg.str());
	    }


    // TAG PULL -- IMPLEMENTATION


    template <typename number>
    void background_time_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL background time sample request for element " << this->id;
#endif

        timing_instrument timer(this->pipe->database_timer);
        this->pipe->data_mgr.pull_background_time_sample(this->pipe, this->id, query, sample);
	    }


    template <typename number>
    void cf_time_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        timing_instrument timer(this->pipe->database_timer);
        switch(this->type)
          {
            case cf_data_type::cf_twopf_re:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL twopf time sample request, type = real, for element " << this->id << ", k-configuration " << this->kserial;
#endif
              this->pipe->data_mgr.pull_twopf_time_sample(this->pipe, this->id, query, this->kserial, sample, twopf_type::real);
              break;

            case cf_data_type::cf_twopf_im:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL twopf time sample request, type = imaginary, for element " << this->id << ", k-configuration " << this->kserial;
#endif
              this->pipe->data_mgr.pull_twopf_time_sample(this->pipe, this->id, query, this->kserial, sample, twopf_type::imag);
              break;

            case cf_data_type::cf_threepf_momentum:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL threepf-momentum time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif
              this->pipe->data_mgr.pull_threepf_time_sample(this->pipe, this->id, query, this->kserial, sample, threepf_type::momentum);
              break;

            case cf_data_type::cf_threepf_Nderiv:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL threepf-Nderiv time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif
              this->pipe->data_mgr.pull_threepf_time_sample(this->pipe, this->id, query, this->kserial, sample, threepf_type::Nderiv);
            break;

            case cf_data_type::cf_tensor_twopf:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL tensor twopf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif
              this->pipe->data_mgr.pull_tensor_twopf_time_sample(this->pipe, this->id, query, this->kserial, sample);
              break;
          }
	    }


    template <typename number>
    void cf_kconfig_data_tag<number>::pull(derived_data::SQL_query& query, std::vector<number>& sample)
	    {
        // check that we are attached to an integration content group
        assert(this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached));
        if(!this->pipe->validate_attached(datapipe<number>::attachment_type::integration_attached)) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        timing_instrument timer(this->pipe->database_timer);
        switch(this->type)
          {
            case cf_data_type::cf_twopf_re:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL twopf kconfig sample request, type = real, for element " << this->id << ", t-serial " << this->tserial;
#endif
              this->pipe->data_mgr.pull_twopf_kconfig_sample(this->pipe, this->id, query, this->tserial, sample, twopf_type::real);
              break;

            case cf_data_type::cf_twopf_im:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL twopf kconfig sample request, type = imaginary, for element " << this->id << ", t-serial " << this->tserial;
#endif
              this->pipe->data_mgr.pull_twopf_kconfig_sample(this->pipe, this->id, query, this->tserial, sample, twopf_type::imag);
              break;

            case cf_data_type::cf_threepf_momentum:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL threepf-momentum kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
              this->pipe->data_mgr.pull_threepf_kconfig_sample(this->pipe, this->id, query, this->tserial, sample, threepf_type::momentum);
              break;

            case cf_data_type::cf_threepf_Nderiv:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL threepf-Nderiv kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
              this->pipe->data_mgr.pull_threepf_kconfig_sample(this->pipe, this->id, query, this->tserial, sample, threepf_type::Nderiv);
              break;

            case cf_data_type::cf_tensor_twopf:
#ifdef CPPTRANSPORT_DEBUG_DATAPIPE
              BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::log_severity_level::datapipe_pull) << "** PULL tensor twopf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
              this->pipe->data_mgr.pull_tensor_twopf_kconfig_sample(this->pipe, this->id, query, this->tserial, sample);
              break;
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
