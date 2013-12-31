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


#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"
#include "transport/concepts/range.h"
#include "transport/messages_en.h"

#include "transport/utilities/random_string.h"


#define DEFAULT_K_PRECISION (2)


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

        // serial number - guaranteed to be unique
        unsigned int serial;

        friend std::ostream& operator<<(std::ostream& out, twopf_kconfig& obj);
      };


    std::ostream& operator<<(std::ostream& out, const twopf_kconfig& obj)
      {
        std::ostringstream str;
        str << std::setprecision(DEFAULT_K_PRECISION) << obj.k;
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

        // serial number - guaranteed to be unique
        unsigned int                serial;

        friend std::ostream& operator<<(std::ostream& out, threepf_kconfig& obj);
      };


    std::ostream& operator<<(std::ostream& out, const threepf_kconfig& obj)
      {
        std::ostringstream kt_str;
        std::ostringstream alpha_str;
        std::ostringstream beta_str;

        kt_str    << std::setprecision(DEFAULT_K_PRECISION) << obj.k_t;
        alpha_str << std::setprecision(DEFAULT_K_PRECISION) << obj.alpha;
        beta_str  << std::setprecision(DEFAULT_K_PRECISION) << obj.beta;

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
    class task
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


    // a task which can be interpreted as a list of ks associated with a two-point function
    // that could be either a literal twopf task, or a threepf task restricted to its twopf component
    template <typename number>
    class twopf_list_task: public task<number>
      {
      public:
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
          : task<number>(nm, i, t)
          {
          }

        ~twopf_list_task()
          {
          }

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<double>& get_k_list() const { return(this->flat_k); }

        //! Get flattened list of comoving ks at which we sample the two-point function
        const std::vector<double>& get_k_list_comoving() const { return(this->comoving_k); }

        //! Get an comoving k-number identified by its index
        double get_k_comoving(unsigned int d) const
          {
            assert(d < this->comoving_k.size());
            if(d < this->comoving_k.size())
              {
                return(this->comoving_k[d]);
              }
            else
              {
                throw std::out_of_range(__CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE);
              }
          }

      protected:
        //! Flattened list of conventionally-normalized ks
        std::vector<double> flat_k;

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

      protected:
        std::vector<twopf_kconfig> config_list;
      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                   const range<double>& ks, typename task<number>::kconfig_kstar kstar)
      : twopf_list_task<number>(nm, i, t)
      {
        double normalization = kstar(this);

        bool stored_background = false;
        // the mapping from the provided list ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            twopf_kconfig kconfig;

            kconfig.index  = j;
            kconfig.serial = j;
            kconfig.k      = ks[j] * normalization;

            kconfig.store_background = stored_background ? false : (stored_background = true);

            config_list.push_back(kconfig);

            this->flat_k.push_back(ks[j]);
            this->comoving_k.push_back(ks[j] * normalization);
          }

        if(!stored_background)
          {
            throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
          }
      }


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {
      public:
        //! Construct a named three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename task<number>::kconfig_kstar kstar)
          : threepf_task(random_string(), i, t, ks, kstar)
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

      protected:
        std::vector<threepf_kconfig> config_list;
      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       const range<double>& ks, typename task<number>::kconfig_kstar kstar)
      : twopf_list_task<number>(nm, i, t)
      {
        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        bool stored_background = false;

        double normalization = kstar(this);

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

                        kconfig.k1  = ks[j] * normalization;
                        kconfig.k2  = ks[k] * normalization;
                        kconfig.k3  = ks[l] * normalization;

                        kconfig.k_t = (ks[j] + ks[k] + ks[l]) * normalization;
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

            if(!stored_twopf)
              {
                throw std::logic_error(__CPP_TRANSPORT_TWOPF_STORE);
              }

            this->flat_k.push_back(ks[j]);
            this->comoving_k.push_back(ks[j] * normalization);
          }

        if(!stored_background)
          {
            throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
          }
      }


  }   // namespace transport


#endif //__task_H_
