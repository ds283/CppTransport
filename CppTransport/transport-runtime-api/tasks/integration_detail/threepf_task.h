//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __threepf_task_H_
#define __threepf_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"
#include "transport-runtime-api/tasks/integration_detail/twopf_list_task.h"


#define __CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE            "integrable"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE       "threepf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN    "n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN "k1n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN "k2n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN "k3n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG    "bg"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1  "tpf1"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2  "tpf2"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3  "tpf3"

#define __CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING         "k-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING        "kt-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING     "alpha-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING      "beta-spacing"


namespace transport
	{

    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
	    {

        // THREEPF CONFIGURATION STORAGE POLICIES

      public:

        //! defines a 'threepf-configuration storage policy' data object, passed to a policy specification
        //! for the purpose of deciding whether a threepf-kconfiguration will be kept
        class threepf_kconfig_storage_policy_data
	        {

          public:

            threepf_kconfig_storage_policy_data(double k, double a, double b, unsigned int s)
	            : k_t(k),
	              alpha(a),
	              beta(b),
	              serial(s)
	            {
                k1 = (k_t/4.0)*(1.0 + alpha + beta);
                k2 = (k_t/4.0)*(1.0 - alpha + beta);
                k3 = (k_t/2.0)*(1.0 - beta);
	            }

          public:

            double k_t;
            double alpha;
            double beta;
            double k1;
            double k2;
            double k3;
            unsigned int serial;
	        };


        //! defines a 'threepf-kconfiguration storage policy' object which determines which threepf-kconfigurations
        //! are retained in the database
        typedef std::function<bool(threepf_kconfig_storage_policy_data&)> threepf_kconfig_storage_policy;

      protected:

        //! default threepf kconfig storage policy - store everything
        class default_threepf_kconfig_storage_policy
	        {
          public:
            bool operator() (threepf_kconfig_storage_policy_data&) { return(true); }
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a threepf-task
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     typename integration_task<number>::time_config_storage_policy p);

        //! deserialization constructor
        threepf_task(const std::string& n, Json::Value& reader, const initial_conditions<number>& i);

        //! Destroy a three-point function task
        virtual ~threepf_task() = default;


        // INTERFACE

      public:

        //! Get list of k-configurations at which this task will sample the threepf
        const std::vector<threepf_kconfig>& get_threepf_kconfig_list() const { return(this->threepf_config_list); }

        //! Determine whether this task is integrable
        bool is_integrable() const { return(this->integrable); }

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const = 0;

        //! get measure at a particular k-configuration
        virtual number measure(const threepf_kconfig& config) const = 0;


        // SERIALIZATION -- implements a 'serialiazble' interface

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of k-configurations associated with this task
        std::vector<threepf_kconfig> threepf_config_list;

        //! next serial number to be assigned to a k-configuration
        unsigned int serial;

        //! Is this threepf task integrable? ie., have we dropped any configurations, and is the spacing linear?
        bool integrable;

	    };


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       typename integration_task<number>::time_config_storage_policy p)
	    : twopf_list_task<number>(nm, i, t, p),
	      serial(0), integrable(true)
	    {
	    }


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : twopf_list_task<number>(nm, reader, i),
	      serial(0)
	    {
        //! deserialize integrable status
        integrable = reader[__CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE].asBool();

        //! deserialize array of k-configurations
        Json::Value& config_list = reader[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE];
        assert(config_list.isArray());

        threepf_config_list.clear();
        threepf_config_list.reserve(config_list.size());

        for(Json::Value::iterator t = config_list.begin(); t != config_list.end(); t++)
	        {
            threepf_kconfig c;
            c.serial           = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN].asUInt();
            c.index[0]         = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN].asUInt();
            c.index[1]         = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN].asUInt();
            c.index[2]         = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN].asUInt();
            c.store_background = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG].asBool();
            c.store_twopf_k1   = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1].asBool();
            c.store_twopf_k2   = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2].asBool();
            c.store_twopf_k3   = (*t)[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3].asBool();

            const twopf_kconfig& k1 = this->lookup_twopf_kconfig(c.index[0]);
            const twopf_kconfig& k2 = this->lookup_twopf_kconfig(c.index[1]);
            const twopf_kconfig& k3 = this->lookup_twopf_kconfig(c.index[2]);

            c.k1_conventional = k1.k_conventional;
            c.k1_comoving     = k1.k_comoving;
            c.k2_conventional = k2.k_conventional;
            c.k2_comoving     = k2.k_comoving;
            c.k3_conventional = k3.k_conventional;
            c.k3_comoving     = k3.k_comoving;

            c.k_t_conventional = k1.k_conventional + k2.k_conventional + k3.k_conventional;
            c.k_t_comoving     = k1.k_comoving + k2.k_comoving + k3.k_comoving;

            c.beta  = 1.0 - 2.0 * k3.k_comoving/(c.k_t_comoving);
            c.alpha = 4.0*k2.k_comoving/(c.k_t_comoving) - 1.0 - c.beta;

            threepf_config_list.push_back(c);
            serial++;
	        }

        // sort threepf_config_list into ascending serial-number order
        // this isn't absolutely required, because nothing in the integration step depends on it
        struct KConfigSorter
	        {
            bool operator() (const threepf_kconfig& a, const threepf_kconfig& b) { return(a.serial < b.serial); }
	        };

        std::sort(threepf_config_list.begin(), threepf_config_list.end(), KConfigSorter());
	    }


    template <typename number>
    void threepf_task<number>::serialize(Json::Value& writer) const
	    {
        // serialize integrable status
        writer[__CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE] = this->integrable;

        // serialize array of k-configuration
        Json::Value config_list(Json::arrayValue);
        for(std::vector<threepf_kconfig>::const_iterator t = this->threepf_config_list.begin(); t != this->threepf_config_list.end(); t++)
	        {
            Json::Value config_element(Json::objectValue);
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN]    = t->serial;
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN] = t->index[0];
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN] = t->index[1];
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN] = t->index[2];
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG]    = t->store_background;
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1]  = t->store_twopf_k1;
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2]  = t->store_twopf_k2;
            config_element[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3]  = t->store_twopf_k3;
            config_list.append(config_element);
	        }
        writer[__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE] = config_list;

        this->twopf_list_task<number>::serialize(writer);
	    }


    template <typename number>
    class threepf_cubic_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks,
        //! with specified policies
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                           const range<double>& t, const range<double>& ks,
                           typename integration_task<number>::time_config_storage_policy tp,
                           typename threepf_task<number>::threepf_kconfig_storage_policy kp);

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks,
        //! with default policies
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                           const range<double>& ks)
	        : threepf_cubic_task(nm, i, t, ks,
	                             typename integration_task<number>::default_time_config_storage_policy(),
	                             typename threepf_task<number>::default_threepf_kconfig_storage_policy())
	        {
	        }

        //! Deserialization constructor
        threepf_cubic_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->spacing*this->spacing*this->spacing); }

        //! get measure; here, just dk1 dk2 dk3 so there is nothing to do
        virtual number measure(const threepf_kconfig& config) const override { return(1.0); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_cubic_task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        double spacing;

	    };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                                                   const range<double>& t, const range<double>& ks,
                                                   typename integration_task<number>::time_config_storage_policy tp,
                                                   typename threepf_task<number>::threepf_kconfig_storage_policy kp)
	    : threepf_task<number>(nm, i, t, tp)
	    {
        bool stored_background = false;

        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        for(unsigned int j = 0; j < ks.size(); j++)
	        {
            for(unsigned int k = 0; k <= j; k++)
	            {
                for(unsigned int l = 0; l <= k; l++)
	                {
                    auto maxij  = (ks[j] > ks[k] ? ks[j] : ks[k]);
                    auto maxijk = (maxij > ks[l] ? maxij : ks[l]);

                    if(ks[j] + ks[k] + ks[l] >= 2.0 * maxijk)   // impose the triangle conditions
	                    {
                        threepf_kconfig kconfig;

                        kconfig.k1_conventional = ks[j];
                        kconfig.k1_comoving     = this->comoving_normalize(ks[j]);
                        kconfig.k2_conventional = ks[k];
                        kconfig.k2_comoving     = this->comoving_normalize(ks[k]);
                        kconfig.k3_conventional = ks[l];
                        kconfig.k3_comoving     = this->comoving_normalize(ks[l]);

                        kconfig.k_t_conventional = ks[j] + ks[k] + ks[l];
                        kconfig.k_t_comoving     = this->comoving_normalize(kconfig.k_t_conventional);
                        kconfig.beta             = 1.0 - 2.0 * ks[l] / (ks[j] + ks[k] + ks[l]);
                        kconfig.alpha            = 4.0 * ks[j] / (ks[j] + ks[k] + ks[l]) - 1.0 - kconfig.beta;


                        kconfig.serial = this->serial++;

                        typename threepf_task<number>::threepf_kconfig_storage_policy_data data(kconfig.k_t_conventional, kconfig.alpha, kconfig.beta, kconfig.serial);
                        if(kp(data))
	                        {
                            // check whether any of these k-wavenumbers have been stored before
                            bool stored;
                            unsigned int sn;

                            stored = this->find_comoving_k(ks[j], sn);
                            if(stored) { kconfig.index[0] = sn; kconfig.store_twopf_k1 = false; }
                            else { kconfig.index[0] = this->push_twopf_klist(ks[j]); kconfig.store_twopf_k1 = true; }

                            stored = this->find_comoving_k(ks[k], sn);
                            if(stored) { kconfig.index[1] = sn; kconfig.store_twopf_k2 = false; }
                            else { kconfig.index[1] = this->push_twopf_klist(ks[k]); kconfig.store_twopf_k2 = true; }

                            stored = this->find_comoving_k(ks[l], sn);
                            if(stored) { kconfig.index[2] = sn; kconfig.store_twopf_k3 = false; }
                            else { kconfig.index[2] = this->push_twopf_klist(ks[l]); kconfig.store_twopf_k3 = true; }

                            kconfig.store_background = stored_background ? false : (stored_background=true);

                            this->threepf_config_list.push_back(kconfig);
	                        }
                        else this->integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
	                    }
	                }
	            }
	        }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);

        // need linear spacing to be integrable
        if(!ks.is_simple_linear()) this->integrable = false;
        spacing = (ks.get_max() - ks.get_min())/ks.get_steps();

        this->apply_time_storage_policy();
	    }


    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, i)
	    {
        spacing = reader[__CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_cubic_task<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE]             = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC);
        writer[__CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING] = this->spacing;

        this->threepf_task<number>::serialize(writer);
	    }


    template <typename number>
    class threepf_fls_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta,
        //! with specified storage policies
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                         const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                         typename integration_task<number>::time_config_storage_policy tp,
                         typename threepf_task<number>::threepf_kconfig_storage_policy kp,
                         double smallest_squeezing=__CPP_TRANSPORT_DEFAULT_SMALLEST_SQUEEZING);

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta,
        //! with default storage policies
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                         const range<double>& kts, const range<double>& alphas, const range<double>& betas)
	        : threepf_fls_task(nm, i, t, kts, alphas, betas,
	                           typename integration_task<number>::default_time_config_storage_policy(),
	                           typename threepf_task<number>::default_threepf_kconfig_storage_policy())
	        {
	        }

        //! Deserialization construcitr
        threepf_fls_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->kt_spacing*this->alpha_spacing*this->beta_spacing); }

        //! get measure; here k_t^2 dk_t dalpha dbeta
        virtual number measure(const threepf_kconfig& config) const override { return(static_cast<number>(config.k_t_conventional*config.k_t_conventional)); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_fls_task<number>* clone() const override { return new threepf_fls_task<number>(static_cast<const threepf_fls_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! k_t spacing
        double kt_spacing;

        //! alpha spacing
        double alpha_spacing;

        //! beta spacing
        double beta_spacing;

	    };


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                               const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                                               typename integration_task<number>::time_config_storage_policy tp,
                                               typename threepf_task<number>::threepf_kconfig_storage_policy kp,
                                               double smallest_squeezing)
	    : threepf_task<number>(nm, i, t, tp)
	    {
        bool stored_background = false;

        for(unsigned int j = 0; j < kts.size(); j++)
	        {
            for(unsigned int k = 0; k < alphas.size(); k++)
	            {
                for(unsigned int l = 0; l < betas.size(); l++)
	                {
                    if(betas[l] >= 0.0 && betas[l] <= 1.0 && betas[l]-1.0 <= alphas[k] && alphas[k] <= 1.0-betas[l]  // impose triangle conditions,
	                    && alphas[k] >= 0.0 && betas[l] >= (1.0+alphas[k])/3.0                                        // impose k1 >= k2 >= k3
	                    && fabs(1.0 - betas[l]) > smallest_squeezing                                                  // impose maximum squeezing on k3
	                    && fabs(1.0 + alphas[k] + betas[l]) > smallest_squeezing
	                    && fabs(1.0 - alphas[k] + betas[l]) > smallest_squeezing)                                     // impose maximum squeezing on k1, k2
	                    {
                        threepf_kconfig kconfig;

                        kconfig.k_t_conventional = kts[j];
                        kconfig.k_t_comoving     = this->comoving_normalize(kts[j]);
                        kconfig.alpha            = alphas[k];
                        kconfig.beta             = betas[l];

                        kconfig.k1_conventional = (kts[j]/4.0)*(1.0 + alphas[k] + betas[l]);
                        kconfig.k1_comoving     = this->comoving_normalize(kconfig.k1_conventional);
                        kconfig.k2_conventional = (kts[j]/4.0)*(1.0 - alphas[k] + betas[l]);
                        kconfig.k2_comoving     = this->comoving_normalize(kconfig.k2_conventional);
                        kconfig.k3_conventional = (kts[j]/2.0)*(1.0 - betas[l]);
                        kconfig.k3_comoving     = this->comoving_normalize(kconfig.k3_conventional);

                        kconfig.serial = this->serial++;

                        typename threepf_task<number>::threepf_kconfig_storage_policy_data data(kconfig.k_t_conventional, kconfig.alpha, kconfig.beta, kconfig.serial);
                        if(kconfig.k1_conventional > 0.0 && kconfig.k2_conventional > 0.0 && kconfig.k3_conventional > 0.0 && kp(data))
	                        {
                            // check whether any of these k-wavenumbers have been stored before
                            bool stored;
                            unsigned int sn;

                            stored = this->find_comoving_k(kconfig.k1_conventional, sn);
                            if(stored) { kconfig.index[0] = sn; kconfig.store_twopf_k1 = false; }
                            else { kconfig.index[0] = this->push_twopf_klist(kconfig.k1_conventional); kconfig.store_twopf_k1 = true; }

                            stored = this->find_comoving_k(kconfig.k2_conventional, sn);
                            if(stored) { kconfig.index[1] = sn; kconfig.store_twopf_k2= false; }
                            else { kconfig.index[1] = this->push_twopf_klist(kconfig.k2_conventional); kconfig.store_twopf_k2 = true; }

                            stored = this->find_comoving_k(kconfig.k3_conventional, sn);
                            if(stored) { kconfig.index[2] = sn; kconfig.store_twopf_k3 = false; }
                            else { kconfig.index[2] = this->push_twopf_klist(kconfig.k3_conventional); kconfig.store_twopf_k3 = true; }

                            kconfig.store_background = stored_background ? false : (stored_background=true);

                            this->threepf_config_list.push_back(kconfig);
	                        }
                        else this->integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
	                    }
	                }
	            }
	        }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);

        // need linear spacing to be integrable
        if(!kts.is_simple_linear() || !alphas.is_simple_linear() || !betas.is_simple_linear()) this->integrable = false;
        kt_spacing = (kts.get_max() - kts.get_min())/kts.get_steps();
        alpha_spacing = (alphas.get_max() - alphas.get_min())/alphas.get_steps();
        beta_spacing = (betas.get_max() - betas.get_min())/betas.get_steps();

        this->apply_time_storage_policy();
	    }


    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, i)
	    {
        kt_spacing    = reader[__CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING].asDouble();
        alpha_spacing = reader[__CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING].asDouble();
        beta_spacing  = reader[__CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_fls_task<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE] = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_FLS);

        writer[__CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING]    = this->kt_spacing;
        writer[__CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING] = this->alpha_spacing;
        writer[__CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING]  = this->beta_spacing;

        this->threepf_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //__threepf_task_H_
