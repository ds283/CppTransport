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
#include "transport/messages_en.h"


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


    // basic task, from which the more specific tasks are derived
    // contains information on the initial conditions, horizon-crossing time and the sampling times
    template <typename number>
    class task
      {
      public:
        typedef std::function<double(task<number>*)> kconfig_kstar;

        // construct a task with supplied initial conditions
        task(const initial_conditions<number>& i, double Nst, const std::vector<double>& ts);

        virtual ~task()
          {
          }

        // get initial conditions
        // guaranteed to be validated
        const std::vector<number>& get_ics() const { return(ic.get_ics()); }

        // get Nstar
        // horizon-crossing is taken to occur Nstar e-folds after the initial conditions
        double get_Nstar() const { return(this->Nstar); }

        // get initial time
        double get_Ninit() const;

        // get sampling times
        const std::vector<double>& get_sample_times() const { return(this->times); }

        // get number of points at which to sample
        size_t get_number_times() const { return(this->times.size()); }

      protected:
        initial_conditions<number> ic;

        // horizon-crossing time
        const double               Nstar;

        // list of times at which to sample
        const std::vector<double>  times;
      };


    template <typename number>
    task<number>::task(const initial_conditions<number>& i, double Nst, const std::vector<double>& ts)
      : ic(i), Nstar(Nst), times(ts)
      {
        // validate relation between Nstar and the sampling time
        assert(times.size() > 0);
        assert(Nstar > times.front());
        assert(Nstar < times.back());

        if(times.size() == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NO_TIMES;
            throw std::logic_error(msg.str());
          }

        if(times.front() >= Nstar)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_EARLY << " (Ninit = " << times.front() << ", Nstar = " << Nstar << ")";
            throw std::logic_error(msg.str());
          }

        if(times.back() <= Nstar)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_LATE << " (Nend = " << times.back() << ", Nstar = " << Nstar << ")";
            throw std::logic_error(msg.str());
          }
      }


    template <typename number>
    double task<number>::get_Ninit() const
      {
        assert(this->times.size() > 0);

        if(this->times.size() > 0)
          {
            return(this->times.front());
          }
        else
          {
            throw std::out_of_range(__CPP_TRANSPORT_TIMES_RANGE);
          }
      }


    // a task which can be interpreted as a list of ks associated with a two-point function
    // that could be either a literal twopf task, or a threepf task restricted to its twopf component
    template <typename number>
    class twopf_list_task: public task<number>
      {
      public:
        twopf_list_task(const initial_conditions<number>& i, double Nst, const std::vector<double>& ts)
          : task<number>(i, Nst, ts)
          {
          }

        ~twopf_list_task()
          {
          }

        // get flattened list of ks at which we sample the threepf
        const std::vector<double>& get_k_list() const { return(this->flat_k); }

        // get flattened list of ks at which we sample the threepf
        const std::vector<double>& get_k_list_comoving() const { return(this->comoving_k); }

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
        // flattened list of conventionally-normalized ks
        std::vector<double>        flat_k;

        // flattened list of comoving-normalzied ks
        std::vector<double>        comoving_k;
      };

    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
      {
      public:
        twopf_task(const initial_conditions<number>& i, const std::vector<double>& ks, double Nst,
                   const std::vector<double>& ts, typename task<number>::kconfig_kstar kstar);

        ~twopf_task()
          {
          }

        // get list of k-configurations at which to sample the twopf
        const std::vector<twopf_kconfig>& get_sample() const { return(this->config_list); }

        // get number of k-configurations at which to sample
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }

      protected:
        std::vector<twopf_kconfig> config_list;
      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const initial_conditions<number>& i, const std::vector<double>& ks, double Nst,
                                   const std::vector<double>& ts, typename task<number>::kconfig_kstar kstar)
      : twopf_list_task<number>(i, Nst, ts)
      {
        double normalization = kstar(this);

        bool stored_background = false;
        // the mapping from the provided list ks to the work list is just one-to-one
        for(unsigned int i = 0; i < ks.size(); i++)
          {
            twopf_kconfig kconfig;

            kconfig.index  = i;
            kconfig.serial = i;
            kconfig.k      = ks[i] * normalization;

            kconfig.store_background = stored_background ? false : (stored_background = true);

            config_list.push_back(kconfig);

            this->flat_k.push_back(ks[i]);
            this->comoving_k.push_back(ks[i] * normalization);
          }

        if(stored_background == false)
          {
            throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
          }
      }


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {
      public:
        // construct a task based on sampling from a cubic lattice of ks
        threepf_task(const initial_conditions<number>& i, const std::vector<double>& ks, double Nst,
                     const std::vector<double>& ts, typename task<number>::kconfig_kstar kstar);

        ~threepf_task()
          {
          }

        // get list of k-configurations at which to sample the threepf
        const std::vector<threepf_kconfig>& get_sample() const { return(this->config_list); }

        // get number of k-configurations at which to sample
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }

      protected:
        std::vector<threepf_kconfig> config_list;
      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_task<number>::threepf_task(const initial_conditions<number>& i, const std::vector<double>& ks, double Nst,
                                       const std::vector<double>& ts, typename task<number>::kconfig_kstar kstar)
      : twopf_list_task<number>(i, Nst, ts)
      {
        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        bool stored_background = false;

        double normalization = kstar(this);

        unsigned int serial = 0;
        for(int i = 0; i < ks.size(); i++)
          {
            bool stored_twopf = false;

            for(int j = 0; j <= i; j++)
              {
                for(int k = 0; k <= j; k++)
                  {
                    threepf_kconfig kconfig;

                    auto maxij  = (ks[i] > ks[j] ? ks[i] : ks[j]);
                    auto maxijk = (maxij > ks[k] ? maxij : ks[k]);

                    if(ks[i] + ks[j] + ks[k] >= 2.0 * maxijk)   // impose the triangle conditions
                      {

                        kconfig.k1  = ks[i] * normalization;
                        kconfig.k2  = ks[j] * normalization;
                        kconfig.k3  = ks[k] * normalization;

                        kconfig.k_t = (ks[i] + ks[j] + ks[k]) * normalization;
                        kconfig.beta  = 1.0 - 2.0 * ks[k] / (ks[i] + ks[j] + ks[k]);
                        kconfig.alpha = 4.0 * ks[i] / (ks[i] + ks[j] + ks[k]) - 1.0 - kconfig.beta;

                        kconfig.index[0] = i;
                        kconfig.index[1] = j;
                        kconfig.index[2] = k;

                        kconfig.store_background = stored_background ? false : (stored_background = true);
                        kconfig.store_twopf      = stored_twopf      ? false : (stored_twopf = true);

                        kconfig.serial = serial++;

                        config_list.push_back(kconfig);
                      }
                  }
              }

            if(stored_twopf == false)
              {
                throw std::logic_error(__CPP_TRANSPORT_TWOPF_STORE);
              }

            this->flat_k.push_back(ks[i]);
            this->comoving_k.push_back(ks[i] * normalization);
          }

        if(stored_background == false)
          {
            throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
          }
      }


  }   // namespace transport


#endif //__task_H_
