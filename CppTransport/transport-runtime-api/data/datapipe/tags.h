//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __tags_H_
#define __tags_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

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
        void pull(const std::vector<unsigned int>& sns, std::vector<double>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::normal) << log_item; }

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
        void pull(const std::vector<unsigned int>& sns, std::vector<twopf_configuration>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::normal) << log_item; }

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
        void pull(const std::vector<unsigned int>& sns, std::vector<threepf_configuration>& data);

        //! emit a log item for this tag
        void log(const std::string& log_item) const { BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::normal) << log_item; }

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
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) = 0;

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
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "background field " << id; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new background_time_data_tag<number>(static_cast<const background_time_data_tag<number>&>(*this)); }


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
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new cf_time_data_tag<number>(static_cast<const cf_time_data_tag<number>&>(*this)); }


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
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override;


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new cf_kconfig_data_tag<number>(static_cast<const cf_kconfig_data_tag<number>&>(*this)); }


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


    //! zeta two-point function time data tag
		template <typename number>
    class zeta_twopf_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_twopf_time_data_tag(datapipe<number>* p, const twopf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          kdata(k),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_twopf_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_twopf_time_data_tag<number>(static_cast<const zeta_twopf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! kserial - controls which k serial number is sampled
        const twopf_configuration kdata;

        //! compute delegate
        derived_data::zeta_timeseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    //! zeta two-point function time data tag
		template <typename number>
    class zeta_threepf_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_threepf_time_data_tag(datapipe<number>* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          kdata(k),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_threepf_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_threepf_time_data_tag<number>(static_cast<const zeta_threepf_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! kserial - controls which k serial number is sampled
        const threepf_configuration kdata;

        //! compute delegate
        derived_data::zeta_timeseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    //! zeta two-point function time data tag
		template <typename number>
    class zeta_reduced_bispectrum_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_reduced_bispectrum_time_data_tag(datapipe<number>* p, const threepf_configuration& k, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          kdata(k),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_reduced_bispectrum_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, kserial =  " << kdata.serial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_reduced_bispectrum_time_data_tag<number>(static_cast<const zeta_reduced_bispectrum_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->kdata.serial*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! kserial - controls which k serial number is sampled
        const threepf_configuration kdata;

        //! compute delegate
        derived_data::zeta_timeseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    //! zeta correlation-function kconfig data tag
		template <typename number>
    class zeta_twopf_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_twopf_kconfig_data_tag(datapipe<number>* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          tserial(ts),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_twopf_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta two-point function, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_twopf_kconfig_data_tag<number>(static_cast<const zeta_twopf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

        //! compute delegate
        derived_data::zeta_kseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    //! zeta correlation-function kconfig data tag
		template <typename number>
    class zeta_threepf_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_threepf_kconfig_data_tag(datapipe<number>* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          tserial(ts),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_threepf_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta three-point function, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_threepf_kconfig_data_tag<number>(static_cast<const zeta_threepf_kconfig_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

        //! compute delegate
        derived_data::zeta_kseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    //! zeta correlation-function kconfig data tag
		template <typename number>
    class zeta_reduced_bispectrum_kconfig_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_reduced_bispectrum_kconfig_data_tag(datapipe<number>* p, unsigned int ts, integration_task<number>* t, unsigned int N, bool c)
	        : data_tag<number>(p),
	          tserial(ts),
	          tk(t),
	          N_fields(N),
	          cached(c)
	        {
	        }

        virtual ~zeta_reduced_bispectrum_kconfig_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "zeta reduced bispectrum, tserial =  " << tserial; return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new zeta_reduced_bispectrum_kconfig_data_tag<number>(static_cast<const zeta_reduced_bispectrum_kconfig_data_tag&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((this->tserial*2131) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! tserial - controls which t serial number is sampled
        const unsigned int tserial;

        //! compute delegate
        derived_data::zeta_kseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

	    };


    // fNL time-series tag
		template <typename number>
    class fNL_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        fNL_time_data_tag(datapipe<number>* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
	        : data_tag<number>(p),
	          tk(t),
	          N_fields(N),
	          type(ty),
	          cached(c)
	        {
	        }

        virtual ~fNL_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "fNL, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new fNL_time_data_tag<number>(static_cast<const fNL_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! cached version available?
        bool cached;

        //! compute delegate
        derived_data::fNL_timeseries_compute<number> computer;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

        //! template type
        typename derived_data::template_type type;

	    };


    // bispectrum.template time-series tag
		template <typename number>
    class BT_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        BT_time_data_tag(datapipe<number>* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
	        : data_tag<number>(p),
	          tk(t),
	          N_fields(N),
	          type(ty),
	          cached(c),
	          restrict_triangles(false)
	        {
	        }

        BT_time_data_tag(datapipe<number>* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty,
                         const std::vector<unsigned int>& kc, bool c)
	        : data_tag<number>(p),
	          tk(t),
	          N_fields(N),
	          type(ty),
	          cached(c),
	          restrict_triangles(true),
	          kconfig_sns(kc)
	        {
	        }

        virtual ~BT_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "bispectrum.template, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new BT_time_data_tag<number>(static_cast<const BT_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! compute delegate
        derived_data::fNL_timeseries_compute<number> computer;

        //! cached version available?
        bool cached;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

        //! template type
        typename derived_data::template_type type;

        //! restrict integration to supplied set of triangles?
        bool restrict_triangles;

        //! set of kconfig serial numbers to restrict to, if used
        std::vector<unsigned int> kconfig_sns;

	    };


    // template.template time-series tag
		template <typename number>
    class TT_time_data_tag: public data_tag<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        TT_time_data_tag(datapipe<number>* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty, bool c)
	        : data_tag<number>(p),
	          tk(t),
	          N_fields(N),
	          type(ty),
	          cached(c),
	          restrict_triangles(false)
	        {
	        }

        TT_time_data_tag(datapipe<number>* p, integration_task<number>* t, unsigned int N, derived_data::template_type ty,
                         const std::vector<unsigned int>& kc, bool c)
	        : data_tag<number>(p),
	          tk(t),
	          N_fields(N),
	          type(ty),
	          cached(c),
	          restrict_triangles(true),
	          kconfig_sns(kc)
	        {
	        }

        virtual ~TT_time_data_tag() = default;


        // INTERFACE

      public:

        //! check for tag equality
        virtual bool operator==(const data_tag<number>& obj) const override;

        //! pull data corresponding to this tag
        virtual void pull(const std::vector<unsigned int>& sns, std::vector<number>& data) override;

        //! identify this tag
        virtual std::string name() const override { std::ostringstream msg; msg << "template.template, template =  " << template_name(this->type); return(msg.str()); }


        // CLONE

      public:

        //! copy this object
        data_tag<number>* clone() const { return new TT_time_data_tag<number>(static_cast<const TT_time_data_tag<number>&>(*this)); }


        // HASH

      public:

        //! hash
        virtual unsigned int hash() const override { return((static_cast<unsigned int>(this->type)*2141) % __CPP_TRANSPORT_LINECACHE_HASH_TABLE_SIZE); }


        // INTERNAL DATA

      protected:

        //! compute delegate
        derived_data::fNL_timeseries_compute<number> computer;

        //! cached version available?
        bool cached;

        //! pointer to task
        integration_task<number>* tk;

        //! number of fields
        unsigned int N_fields;

        //! template type
        typename derived_data::template_type type;

        //! restrict integration to supplied set of triangles?
        bool restrict_triangles;

        //! set of kconfig serial numbers to restrict to, if used
        std::vector<unsigned int> kconfig_sns;

	    };


    template <typename number>
    void time_config_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<double>& data)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL time sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.time(this->pipe, sns, data);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void twopf_kconfig_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<twopf_configuration>& data)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL 2pf k-configuration sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.twopf(this->pipe, sns, data);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void threepf_kconfig_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<threepf_configuration>& data)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL 3pf k-configuration sample request";
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_config.threepf(this->pipe, sns, data);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void background_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL background time sample request for element " << this->id;
#endif

        this->pipe->database_timer.resume();
        this->pipe->pull_timeslice.background(this->pipe, this->id, sns, sample);
        this->pipe->database_timer.stop();
	    }


    template <typename number>
    void cf_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == data_tag<number>::cf_twopf_re)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf time sample request, type = real, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, sns, this->kserial, sample, datapipe<number>::twopf_real);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_twopf_im)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf time sample request, type = imaginary, for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.twopf(this->pipe, this->id, sns, this->kserial, sample, datapipe<number>::twopf_imag);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_threepf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
				    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL threepf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.threepf(this->pipe, this->id, sns, this->kserial, sample);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_tensor_twopf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL tensor twopf time sample request for element " << this->id << ", k-configuration " << this->kserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.tensor_twopf(this->pipe, this->id, sns, this->kserial, sample);
            this->pipe->database_timer.stop();
	        }
        else
	        {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
	        }
	    }


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
    void cf_kconfig_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        if(this->type == data_tag<number>::cf_twopf_re)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = real, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, sns, this->tserial, sample, datapipe<number>::twopf_real);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_twopf_im)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL twopf kconfig sample request, type = imaginary, for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.twopf(this->pipe, this->id, sns, this->tserial, sample, datapipe<number>::twopf_imag);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_threepf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL threepf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif

            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.threepf(this->pipe, this->id, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else if(this->type == data_tag<number>::cf_tensor_twopf)
	        {
#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
            BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL tensor twopf kconfig sample request for element " << this->id << ", t-serial " << this->tserial;
#endif
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.tensor_twopf(this->pipe, this->id, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else
	        {
            assert(false);
            throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_UNKNOWN_CF_TYPE);
	        }
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


    template <typename number>
    void zeta_twopf_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta twopf time sample request, k-configuration " << this->kdata.kserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.zeta_twopf(this->pipe, sns, this->kdata.serial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
            this->computer.twopf(handle, sample, this->kdata);
	        }
	    }


    template <typename number>
    void zeta_threepf_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta threepf time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.zeta_threepf(this->pipe, sns, this->kdata.serial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
            this->computer.threepf(handle, sample, this->kdata);
	        }
	    }


    template <typename number>
    void zeta_reduced_bispectrum_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta reduced bispectrum time sample request, type = real, k-configuration " << this->kdata.kserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.zeta_redbsp(this->pipe, sns, this->kdata.serial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields);
            this->computer.reduced_bispectrum(handle, sample, this->kdata);
	        }
	    }


    template <typename number>
    void zeta_twopf_kconfig_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta twopf kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_twopf(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- not efficient! but is there a better way?
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.twopf(handle, sample, 0);
	        }
	    }


    template <typename number>
    void zeta_threepf_kconfig_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta threepf kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_threepf(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- as above, not efficient
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.threepf(handle, sample, 0);
	        }
	    }


    template <typename number>
    void zeta_reduced_bispectrum_kconfig_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL zeta reduced bispectrum kconfig sample request, t-serial " << this->tserial;
#endif

        if(this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_kslice.zeta_redbsp(this->pipe, sns, this->tserial, sample);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // pull information on this time-value from the database -- as above, not efficient
            std::vector<unsigned int> time_sn;
            time_sn.push_back(this->tserial);

            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(time_sn);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::zeta_kseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_sn, time_values, this->N_fields);
            this->computer.reduced_bispectrum(handle, sample, 0);
	        }
	    }


    template <typename number>
    void fNL_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL fNL sample request, template = " << template_type(this->type);
#endif

        if(this->cached)  // extract from data container
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.fNL(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
            this->computer.fNL(handle, sample);
	        }
	    }


    template <typename number>
    void BT_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL bispectrum.template sample request, template = " << template_type(this->type);
#endif

        if(!this->restrict_triangles && this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.BT(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            if(this->restrict_triangles)
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type, this->kconfig_sns);
                this->computer.BT(handle, sample);
	            }
            else
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
                this->computer.BT(handle, sample);
	            }
	        }
	    }


    template <typename number>
    void TT_time_data_tag<number>::pull(const std::vector<unsigned int>& sns, std::vector<number>& sample)
	    {
        assert(this->pipe->validate_attached());
        if(!this->pipe->validate_attached()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

#ifdef __CPP_TRANSPORT_DEBUG_DATAPIPE
		    BOOST_LOG_SEV(this->pipe->get_log(), datapipe<number>::datapipe_pull) << "** PULL template.template sample request, template = " << template_type(this->type);
#endif

        if(!this->restrict_triangles && this->cached)  // extract from data contanier
	        {
            this->pipe->database_timer.resume();
            this->pipe->pull_timeslice.BT(this->pipe, sns, sample, this->type);
            this->pipe->database_timer.stop();
	        }
        else              // have to compute values on the fly
	        {
            // look up time values corresponding to these serial numbers
            typename datapipe<number>::time_config_handle& tc_handle = this->pipe->new_time_config_handle(sns);
            time_config_tag<number> tc_tag = this->pipe->new_time_config_tag();

            const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

            // set up handle for compute delegate
            if(this->restrict_triangles)
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type, this->kconfig_sns);
                this->computer.TT(handle, sample);
	            }
            else
	            {
                std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->computer.make_handle(*(this->pipe), this->tk, sns, time_values, this->N_fields, this->type);
                this->computer.TT(handle, sample);
	            }
	        }
	    }


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


#endif //__tags_H_
