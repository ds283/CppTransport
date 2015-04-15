//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_list_task_H_
#define __twopf_list_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"


#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE         "twopf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN      "n"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG      "bg"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K       "k"


namespace transport
	{

    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a flat vector of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public integration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a twopf-list-task object
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                        typename integration_task<number>::time_config_storage_policy p);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE

      public:

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<twopf_kconfig>& get_twopf_kconfig_list() const { return(this->twopf_config_list); }


        // SERVICES FOR DERIVED CLASSES

      protected:

        //! Add a wavenumber to the list. The wavenumber should be conventionally normalized.
        //! Returns the serial number of the new configuration.
        unsigned int push_twopf_klist(double k, bool store=false);

        //! Convert a conventionally-normalized wavenumber to a comoving wavenumber
        double comoving_normalize(double k) const;

        //! Search for a twopf kconfiguration by conventionally-normalized wavenumber.
        bool find_comoving_k(double k, unsigned int& serial) const;

        //! Look up a twopf k-configuration by serial number
        const twopf_kconfig& lookup_twopf_kconfig(unsigned int serial);


        // FAST-FORWARD INTEGRATION

      public:

        //! get largest k-mode included in the integration
        virtual double get_kmax() const override { return(this->kmax); }

        //! get smallest k-mode included in the integration
        virtual double get_kmin() const override { return(this->kmin); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      private:

        //! Normalization constant for comoving ks
        double comoving_normalization;

        //! List of twopf k-configurations associated with this task
        std::vector<twopf_kconfig> twopf_config_list;

        //! Maximum wavenumber
        double kmax;

        //! Minimum wavenumber
        double kmin;

        //! current serial number
        unsigned int serial;

	    };


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                             typename integration_task<number>::time_config_storage_policy p)
	    : integration_task<number>(nm, i, t, p),
	      kmax(-DBL_MAX),
	      kmin(DBL_MAX),
	      serial(0)
	    {
        // we can use 'this' here, because the integration-task components are guaranteed to be initialized
        // by the time the body of the constructor is executed
        model<number>* m = i.get_model();

        comoving_normalization = m->compute_kstar(this);
	    }


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : integration_task<number>(nm, reader, i),
	      kmax(-DBL_MAX),
	      kmin(DBL_MAX),
	      serial(0)
	    {
        // deserialize comoving normalization constant
        comoving_normalization = reader[__CPP_TRANSPORT_NODE_KSTAR].asDouble();

        // deserialize list of twopf k-configurations
        Json::Value& config_list = reader[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE];
        assert(config_list.isArray());

        twopf_config_list.clear();
        twopf_config_list.reserve(config_list.size());

        for(Json::Value::iterator t = config_list.begin(); t != config_list.end(); t++)
	        {
            twopf_kconfig c;
            c.serial         = (*t)[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN].asUInt();
            c.k_conventional = (*t)[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K].asDouble();

            assert(c.k_conventional > 0.0);
            c.k_comoving = c.k_conventional*comoving_normalization;
            if(c.k_conventional > this->kmax) this->kmax = c.k_conventional;
            if(c.k_conventional < this->kmin) this->kmin = c.k_conventional;

            c.store_background = (*t)[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG].asBool();

            twopf_config_list.push_back(c);
            serial++;
	        }

        // sort twopf_config_list into ascending serial-number order
        // this isn't absolutely required, because nothing in the integration step depends on
        // the twopf_kconfig list being in serial-number order.
        struct KConfigSorter
	        {
            bool operator() (const twopf_kconfig& a, const twopf_kconfig& b) { return(a.serial < b.serial); }
	        };

        std::sort(twopf_config_list.begin(), twopf_config_list.end(), KConfigSorter());
	    }


    template <typename number>
    void twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        // serialize comoving normalization constant
        writer[__CPP_TRANSPORT_NODE_KSTAR] = this->comoving_normalization;

        // serialize list of twopf kconfigurations
        // note we store only k_conventional to save space
        // k_comoving can be reconstructed on deserialization
        Json::Value config_list(Json::arrayValue);
        for(std::vector<twopf_kconfig>::const_iterator t = this->twopf_config_list.begin(); t != this->twopf_config_list.end(); t++)
	        {
            Json::Value config_element(Json::objectValue);
            config_element[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN] = t->serial;
            config_element[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K]  = t->k_conventional;
            config_element[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG] = t->store_background;
            config_list.append(config_element);
	        }
        writer[__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE] = config_list;

        this->integration_task<number>::serialize(writer);
	    }


    template <typename number>
    unsigned int twopf_list_task<number>::push_twopf_klist(double k, bool store)
	    {
        assert(k > 0.0);

        twopf_kconfig c;

        c.serial         = this->serial++;
        c.k_comoving     = k*this->comoving_normalization;
        c.k_conventional = k;

        c.store_background = store;

        this->twopf_config_list.push_back(c);
        if(k > this->kmax) this->kmax = k;
        if(k < this->kmin) this->kmin = k;

        return(c.serial);
	    }


    template <typename number>
    double twopf_list_task<number>::comoving_normalize(double k) const
	    {
        return(k*this->comoving_normalization);
	    }


    template <typename number>
    bool twopf_list_task<number>::find_comoving_k(double k, unsigned int& serial) const
	    {
        bool rval = false;
        for(std::vector<twopf_kconfig>::const_iterator t = this->twopf_config_list.begin(); !rval && t != this->twopf_config_list.end(); t++)
	        {
            if(fabs(t->k_conventional - k) < __CPP_TRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION)
	            {
                rval = true;
                serial = t->serial;
	            }
	        }

        return(rval);
	    }


    template <typename number>
    const twopf_kconfig& twopf_list_task<number>::lookup_twopf_kconfig(unsigned int serial)
	    {
        std::vector<twopf_kconfig>::const_iterator t;
        for(t = this->twopf_config_list.begin(); t != this->twopf_config_list.end(); t++)
	        {
            if(t->serial == serial) break;
	        }

        if(t == this->twopf_config_list.end()) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TASK_TWOPF_CONFIG_SN_TOO_BIG);

        return(*t);
	    }

	}   // namespace transport


#endif //__twopf_list_task_H_
