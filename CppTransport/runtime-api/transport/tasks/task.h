//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_H_
#define __task_H_


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <sstream>
#include <stdexcept>


#include "transport/db-xml/xml_serializable.h"
#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"
#include "transport/concepts/range.h"
#include "transport/messages_en.h"

#include "transport/utilities/random_string.h"


#define __CPP_TRANSPORT_DEFAULT_K_PRECISION (2)


#define __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE    "twopf-sample-times"
#define __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE    "twopf-sample-wavenumbers"
#define __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE  "threepf-sample-times"
#define __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE  "threepf-sample-wavenumbers"
#define __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE  "wavenumber-grid"
#define __CPP_TRANSPORT_VAL_THREEPF_CUBIC     "cubic-lattice"
#define __CPP_TRANSPORT_VAL_THREEPF_FLS       "fergusson-liguori-shellard"
#define __CPP_TRANSPORT_NODE_THREEPF_KRANGE   "k-range"
#define __CPP_TRANSPORT_NODE_THREEPF_KTRANGE  "kt-range"
#define __CPP_TRANSPORT_NODE_THREEPF_ARANGE   "alpha-range"
#define __CPP_TRANSPORT_NODE_THREEPF_BRANGE   "beta-range"


namespace transport
  {

    // K-CONFIGURATION DATA

    class twopf_kconfig
      {
      public:
        // index of this k into serial list
        // (trivial in this case, and redundant with the k-value 'k' defined below)
        unsigned int index;

        // k-value
        double       k;

        // flag which indicates to the integrator whether to store the background
        bool         store_background;

        // serial number - guaranteed to be unique.
        // used to identify this k-configuration in the SQL database
        unsigned int serial;

        friend std::ostream& operator<<(std::ostream& out, twopf_kconfig& obj);
      };


    std::ostream& operator<<(std::ostream& out, const twopf_kconfig& obj)
      {
        std::ostringstream str;
        str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k;
        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KEQUALS << " " << str.str() << std::endl;

        return(out);
      }


    struct threepf_kconfig
      {
      public:
        // index of k1, k2, k3 into serial list of k-modes
        // used to look up appropriate values of the power spectrum when constructing reduced 3pfs
        std::array<unsigned int, 3> index;

        // plain comoving (k1,k2,k3) coordinates for this k-configuration
        double                      k1;
        double                      k2;
        double                      k3;

        // Fergusson-Shellard-Liguori coordinates for this k-configuration
        double                      k_t;
        double                      alpha;
        double                      beta;

        // flags which indicate to the integrator whether to
        // store the background and twopf results from this integration
        bool                        store_background;
        bool                        store_twopf;

        // serial number - guaranteed to be unique.
        // used to indentify this k-configuration in the SQL database
        unsigned int                serial;

        friend std::ostream& operator<<(std::ostream& out, threepf_kconfig& obj);
      };


    std::ostream& operator<<(std::ostream& out, const threepf_kconfig& obj)
      {
        std::ostringstream kt_str;
        std::ostringstream alpha_str;
        std::ostringstream beta_str;

        kt_str    << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k_t;
        alpha_str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.alpha;
        beta_str  << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.beta;

        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KTEQUALS << " " << kt_str.str()
          << ", " << __CPP_TRANSPORT_KCONFIG_ALPHAEQUALS << " " << alpha_str.str()
          << ", " << __CPP_TRANSPORT_KCONFIG_BETAEQUALS << " " << beta_str.str()
          << std::endl;

        return(out);
      }


    // TASK STRUCTURES

    template <typename number> class task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const task<number>& obj);

    // basic task, from which the more specific tasks are derived
    // contains information on the initial conditions, horizon-crossing time and the sampling times
    template <typename number>
    class task: public xml_serializable
      {
      public:
        typedef std::function<double(task<number>*)> kconfig_kstar;

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named task with supplied initial conditions
        task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t);

        //! Construct an anonymized task with supplied initial conditions
        task(const initial_conditions<number>& i, const range<double>& t)
          : task(random_string(), i, t)
          {
          }

        //! Destroy a task
        virtual ~task()
          {
          }

        // EXTRACT INFORMATION ABOUT THE TASK

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

        //! Get 'range' object representing sample times
        const range<double>& get_times() const { return(this->times); }

        //! Get initial times
        double get_Ninit() const { return(this->times.get_min()); }

        //! Get horizon-crossing time
        double get_Nstar() const { return(this->ics.get_Nstar()); }

        //! Get std::vector of sample times
        const std::vector<double>& get_sample_times() const { return(this->times.get_grid()); }

        //! Get std::vector of initial conditions
        const std::vector<number>& get_initial_conditions() const { return(this->ics.get_vector()); }

        //! Get number of samples
        unsigned int get_N_sample_times() const { return(this->times.size()); }

        //! Get name
        const std::string& get_name() const { return(this->name); }

        friend std::ostream& operator<< <>(std::ostream& out, const task<number>& obj);

        // INTERFACE -- XML SERIALIZATION -- DISALLOW
        virtual void serialize_xml(DbXml::XmlEventWriter& writer) const { throw std::runtime_error(__CPP_TRANSPORT_SERIALIZE_BASE_TASK); }

      protected:
        //! Name of this task
        const std::string&               name;

        //! Initial conditions for this task (including parameter choices)
        const initial_conditions<number> ics;
        //! Range of times at which to sample for this task
        const range<double>              times;
      };


    template <typename number>
    task<number>::task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
      : name(nm), ics(i), times(t)
      {
        // validate relation between Nstar and the sampling time
        assert(times.get_steps() > 0);
        assert(i.get_Nstar() > times.get_min());
        assert(i.get_Nstar() < times.get_max());

        if(times.get_steps() == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NO_TIMES;
            throw std::logic_error(msg.str());
          }

        if(times.get_min() >= ics.get_Nstar())
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_EARLY << " (Ninit = " << times.get_min() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
          }

        if(times.get_max() <= ics.get_Nstar())
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_LATE << " (Nend = " << times.get_max() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
          }
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const task<number>& obj)
      {
        out << obj.ics << std::endl;
        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
        return(out);
      }


    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a flat vector of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public task<number>
      {
      public:
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
          : comoving_normalization(0.0), normalization_set(false), task<number>(nm, i, t)
          {
          }

        ~twopf_list_task()
          {
          }

        //! Set comoving wavenumber normalization constant

        //! It is an error to try to set this twice, since it could lead to an inconsistent state.
        //! If a second attempt is made to set the normalization, throws a RUNTIME_ERROR exception.
        void set_normalization(typename task<number>::kconfig_kstar kstar)
          {
            if(this->normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_NORM);
            normalization_set = true;
            comoving_normalization = kstar(this);
          }

        //! Add a wavenumber to the list. The wavenumber should be conventionally normalized.

        //! Simultaneously pushes to the conventionally-normalized and comoving-normalized lists, so they
        //! are kept in sync. Requires normalization to have been set using set_normalization(),
        //! otherwise throws a RUNTIME_ERROR exception.
        void push_twopf_klist(double k)
          {
            if(!normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_UNSET);
            this->conventional_k.push_back(k);
            this->comoving_k.push_back(k*this->comoving_normalization);
          }

        //! Convert a conventionally-normalized wavenumber to a comoving wavenumber

        //! Requires normalization to have been set using set_normalization(),
        //! otherwise throws a RUNTIME_ERROR exception.
        double comoving_normalize(double k)
          {
            if(!normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_UNSET);
            return(k*this->comoving_normalization);
          }

        //! Get number of entries in the flat twopf k-list
        unsigned int get_k_list_size() const { return(this->conventional_k.size()); }

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<double>& get_k_list() const { return(this->conventional_k); }

        //! Get flattened list of comoving ks at which we sample the two-point function
        const std::vector<double>& get_k_list_comoving() const { return(this->comoving_k); }

        //! Get a conventionally normalized k-number identified by its index
        double get_k(unsigned int d) const
          {
            assert(d < this->conventional_k.size());
            if(d < this->conventional_k.size()) return(this->conventional_k[d]);
            else throw std::out_of_range(__CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE);
          }

        //! Get an comoving k-number identified by its index
        double get_k_comoving(unsigned int d) const
          {
            assert(d < this->comoving_k.size());
            if(d < this->comoving_k.size()) return(this->comoving_k[d]);
            else throw std::out_of_range(__CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE);
          }

        // ADD ITEMS TO THE NORMALIZED LIST

        // INTERNAL DATA

      private:
        //! Normalization constant for comoving ks
        double comoving_normalization;

        //! Has the normalization constant been set? This is a precondition for adding any wavenumbers of the list.
        //! It would be cleaner to pass the normalizaiton constant to the constructor,
        //! but to compute the normalization itself requires a properly-formed task object.
        //! Therefore we cannot do so until the base constructor has completed.
        //! This approach is a workaround.
        bool normalization_set;

        //! Flattened list of conventionally-normalized ks
        std::vector<double> conventional_k;

        //! Flattened list of comoving-normalized ks
        std::vector<double> comoving_k;
      };


    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
      {
      public:
        //! Construct a named two-point function task
        twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename task<number>::kconfig_kstar kstar);

        //! Construct an anonymized two-point function task
        twopf_task(const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename task<number>::kconfig_kstar kstar)
          : twopf_task(random_string(), i, t, ks, kstar)
          {
          }

        //! Destroy a two-point function task
        ~twopf_task()
          {
          }

        //! Get list of k-configurations at which this task will sample the twopf
        const std::vector<twopf_kconfig>& get_sample() const { return(this->config_list); }

        //! Get the number of k-configurations at which this task will sample the twopf
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }

        // INTERFACE -- XML SERIALIZATION

        //! Serialize this task to the Berkeley DB XML repository
        void serialize_xml(DbXml::XmlEventWriter& writer) const;

      protected:
        //! List of k-configurations associated with this task
        std::vector<twopf_kconfig> config_list;

        //! Original range of wavenumber ks used to produce this task. Retained for serialization to the repository
        const range<double>        original_ks;
      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                   const range<double>& ks, typename task<number>::kconfig_kstar kstar)
      : original_ks(ks), twopf_list_task<number>(nm, i, t)
      {
        this->set_normalization(kstar);

        bool stored_background = false;
        // the mapping from the provided list ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            twopf_kconfig kconfig;

            kconfig.index  = j;
            kconfig.serial = j;
            kconfig.k      = this->comoving_normalize(ks[j]);

            kconfig.store_background = stored_background ? false : (stored_background = true);

            config_list.push_back(kconfig);

            this->push_twopf_klist(ks[j]);
          }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);
      }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize_xml(DbXml::XmlEventWriter& writer) const
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE, false);
        this->times.serialize_xml(writer);
        this->end_node(writer, __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE, false);
        this->original_ks.serialize_xml(writer);
        this->end_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE);
      }


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {
      public:
        typedef enum { cubic_lattice, fergusson_liguori_shellard } wavenumber_grid_type;

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename task<number>::kconfig_kstar kstar)
          : threepf_task(random_string(), i, t, ks, kstar)
          {
          }

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling in FLS parameters
        threepf_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename task<number>::kconfig_kstar kstar)
          : threepf_task(random_string(), i, t, kts, alphas, betas, kstar)
          {
          }

        //! Destroy a three-point function task
        ~threepf_task()
          {
          }

        //! Get list of k-configurations at which this task will sample the threepf
        const std::vector<threepf_kconfig>& get_sample() const { return(this->config_list); }

        //! Get the number of k-configurations at which this task will sample the threepf
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }

        // INTERFACE -- XML SERIALIZATION

        //! Serialize this task to the Berkeley DB XML repository
        void serialize_xml(DbXml::XmlEventWriter& writer) const;

      protected:
        //! List of k-configurations associated with this task
        std::vector<threepf_kconfig> config_list;

        //! Wavenumber lattice used to construct this task, retained for serialization
        const wavenumber_grid_type original_lattice;
        //! Original cubic lattice, if used, retained for serialization
        const range<double> original_ks;
        //! Original kt-grid, if used, retained for serialization
        const range<double> original_kts;
        //! Original alpha-grid, if used, retained for serialization
        const range<double> original_alphas;
        //! Original beta-grid, if used, retained for serialization
        const range<double> original_betas;
      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       const range<double>& ks, typename task<number>::kconfig_kstar kstar)
      : original_lattice(cubic_lattice), original_ks(ks),
        original_kts(), original_alphas(), original_betas(),
        twopf_list_task<number>(nm, i, t)
      {
        this->set_normalization(kstar);

        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        bool stored_background = false;

        unsigned int serial = 0;
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            bool stored_twopf = false;

            for(unsigned int k = 0; k <= j; k++)
              {
                for(unsigned int l = 0; l <= k; l++)
                  {
                    threepf_kconfig kconfig;

                    auto maxij  = (ks[j] > ks[k] ? ks[j] : ks[k]);
                    auto maxijk = (maxij > ks[l] ? maxij : ks[l]);

                    if(ks[j] + ks[k] + ks[l] >= 2.0 * maxijk)   // impose the triangle conditions
                      {

                        kconfig.k1  = this->comoving_normalize(ks[j]);
                        kconfig.k2  = this->comoving_normalize(ks[k]);
                        kconfig.k3  = this->comoving_normalize(ks[l]);

                        kconfig.k_t = this->comoving_normalize(ks[j] + ks[k] + ks[l]);
                        kconfig.beta  = 1.0 - 2.0 * ks[l] / (ks[j] + ks[k] + ks[l]);
                        kconfig.alpha = 4.0 * ks[j] / (ks[j] + ks[k] + ks[l]) - 1.0 - kconfig.beta;

                        kconfig.index[0] = j;
                        kconfig.index[1] = k;
                        kconfig.index[2] = l;

                        kconfig.store_background = stored_background ? false : (stored_background = true);
                        kconfig.store_twopf      = stored_twopf      ? false : (stored_twopf = true);

                        kconfig.serial = serial++;

                        config_list.push_back(kconfig);
                      }
                  }
              }

            if(!stored_twopf) throw std::logic_error(__CPP_TRANSPORT_TWOPF_STORE);

            this->push_twopf_klist(ks[j]);
          }

        if(!stored_background) throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
      }


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, const range<double>& kts, const range<double>& alphas, const range<double>& betas, typename task<number>::kconfig_kstar kstar)
      : original_lattice(fergusson_liguori_shellard), original_ks(),
        original_kts(kts), original_alphas(alphas), original_betas(betas),
        twopf_list_task<number>(nm, i, t)
      {
        assert(false);
      }


    // serialize a threepf task to the repository
    template <typename number>
    void threepf_task<number>::serialize_xml(DbXml::XmlEventWriter& writer) const
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE, false);
        this->times.serialize_xml(writer);
        this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE);

       switch(this->original_lattice)
          {
            case cubic_lattice:
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE, false,
                               __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE, __CPP_TRANSPORT_VAL_THREEPF_CUBIC);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KRANGE, false);
              this->original_ks.serialize_xml(writer);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KRANGE);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);
              break;

            case fergusson_liguori_shellard:
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE, false,
                               __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE, __CPP_TRANSPORT_VAL_THREEPF_FLS);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KTRANGE, false);
              this->original_kts.serialize_xml(writer);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KTRANGE);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_ARANGE, false);
              this->original_alphas.serialize_xml(writer);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_ARANGE);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_BRANGE, false);
              this->original_betas.serialize_xml(writer);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_BRANGE);
              this->end_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);
            break;

            default:
              throw std::runtime_error(__CPP_TRANSPORT_TASK_THREEPF_TYPE);
          }
      }


    // functions to construct a task object from an XML schema
    namespace task_dbxml
      {

        template <typename number>
        twopf_task<number>* extract_twopf_task(DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& name,
                                               const initial_conditions<number>& ics, typename task<number>::kconfig_kstar kstar)
          {
            // run queries to find the twopf specification blocks from this schema
            // there are two: a k-sample block and a t-sample block
            std::string query_time = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_TWOPF_TSAMPLE);

            DbXml::XmlValue tsample_node = dbxml_helper::extract_single_node(query_time, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            range<double> tsample = range_dbxml::extract<double>(mgr, tsample_node);

            std::string query_ks = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_TWOPF_KSAMPLE);

            DbXml::XmlValue ksample_node = dbxml_helper::extract_single_node(query_ks, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            range<double> ksample = range_dbxml::extract<double>(mgr, ksample_node);

            return new twopf_task<number>(name, ics, tsample, ksample, kstar);
          }


        template <typename number>
        threepf_task<number>* extract_threepf_task(DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& name,
                                                   const initial_conditions<number>& ics, typename task<number>::kconfig_kstar kstar)
          {
            // run queries to extract the threepf specification blocks from this schema
            // as for the twopf, there are two; but this time the k-range can be more complicated
            std::string query_time = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_TSAMPLE);

            DbXml::XmlValue tsample_node = dbxml_helper::extract_single_node(query_time, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            range<double> tsample = range_dbxml::extract<double>(mgr, tsample_node);

            std::string query_ks = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);

            DbXml::XmlValue ksample_node = dbxml_helper::extract_single_node(query_ks, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            if(ksample_node.getLocalName() != __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            DbXml::XmlResults ksample_attrs = ksample_node.getAttributes();
            if(ksample_attrs.size() != 1) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            DbXml::XmlValue ksample_type;
            ksample_attrs.next(ksample_type);

            if(ksample_type.getLocalName() != __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);

            if(ksample_type.getNodeValue() == __CPP_TRANSPORT_VAL_THREEPF_CUBIC)
              {
                // we expect just a single krange block
                std::string k_query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_KRANGE);
                DbXml::XmlValue krange_node = dbxml_helper::extract_single_node(k_query, mgr, ksample_node, __CPP_TRANSPORT_BADLY_FORMED_TASK);

                range<double> ksample = range_dbxml::extract<double>(mgr, krange_node);

                return new threepf_task<number>(name, ics, tsample, ksample, kstar);
              }
            else if(ksample_type.getNodeValue() == __CPP_TRANSPORT_VAL_THREEPF_FLS)
              {
                // we expect three ranges, specifying the sampling points for k_t, alpha and beta
                std::string kt_query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_KTRANGE);
                DbXml::XmlValue ktrange_node = dbxml_helper::extract_single_node(kt_query, mgr, ksample_node, __CPP_TRANSPORT_BADLY_FORMED_TASK);

                range<double> kt_sample = range_dbxml::extract<double>(mgr, ktrange_node);

                std::string alpha_query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_ARANGE);
                DbXml::XmlValue arange_node = dbxml_helper::extract_single_node(alpha_query, mgr, ksample_node, __CPP_TRANSPORT_BADLY_FORMED_TASK);

                range<double> alpha_sample = range_dbxml::extract<double>(mgr, arange_node);

                std::string beta_query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_THREEPF_BRANGE);
                DbXml::XmlValue brange_node = dbxml_helper::extract_single_node(beta_query, mgr, ksample_node, __CPP_TRANSPORT_BADLY_FORMED_TASK);

                range<double> beta_sample = range_dbxml::extract<double>(mgr, brange_node);

                return new threepf_task<number>(name, ics, tsample, kt_sample, alpha_sample, beta_sample, kstar);
              }
            else throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_TASK);
          }

      }


  }   // namespace transport


#endif //__task_H_
