//
// Created by David Seery on 20/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __scheduler_H_
#define __scheduler_H_


#include <iostream>
#include <array>
#include <vector>

#include "transport/scheduler/context.h"
#include "transport/scheduler/work_queue.h"


namespace transport
  {

    class twopf_kconfig
      {
      public:
        bool validate_index(unsigned int size)
          {
            return(this->index < size);
          }

        // index of this k into serial list
        // (trivial in this case, and redundant with the k-value 'k' defined below)
        unsigned int index;

        // k-value
        double       k;

        // 'serial' is guaranteed to be a unique number which identifies
        // the component of work
        // however, the kconfigs in a particular work queue are not guaranteed
        // to be in any particular order
        unsigned     serial;
      };

    struct threepf_kconfig
      {
      public:
        inline bool validate_index(unsigned int size)
          {
            return(this->index[0] < size && this->index[1] < size && this->index[2] < size);
          }

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

        // 'serial' is guaranteed to be a unique number which identifies
        // this component of work
        // however, the kconfigs in a work queue are not guaranteed
        // to be in any particular order
        unsigned                    serial;
      };

    template <typename SizeFunctor>
    class scheduler
      {
      public:
        scheduler(context c)
          : ctx(c)
          {
          }

        // set up a work queue for a two-point function
        // here, we expect a list of k-modes which tell us the values at which
        // we want to sample the twopf
        work_queue<twopf_kconfig> make_twopf_queue(const SizeFunctor sf, const std::vector<double>& ks);

        // set up a work queue for a three-point function
        // this uses a simple cubic lattice
        work_queue<threepf_kconfig> make_threepf_queue(const SizeFunctor sf, const std::vector<double>& ks);

        // set up a work queue for a three-point function
        // this uses a lattice in Fergusson-Shellard-Liguori coordinates
        work_queue<threepf_kconfig> make_threepf_queue(const SizeFunctor sf, const std::vector<double>& kts, const std::vector<double>& alphas, const std::vector<double>& betas);

      protected:
        const context&    ctx;
      };


    template <typename SizeFunctor>
    work_queue<twopf_kconfig> scheduler<SizeFunctor>::make_twopf_queue(const SizeFunctor sf, const std::vector<double>& ks)
      {
        work_queue<twopf_kconfig> work(this->ctx, sf, ks);

        // for the 2pf there is almost nothing to do
        // the mapping from the provided list ks to the work list is just one-to-one
        for(unsigned int i = 0; i < ks.size(); i++)
          {
            twopf_kconfig kconfig;

            kconfig.index  = i;
            kconfig.serial = i;
            kconfig.k      = ks[i];

            work.enqueue_work_item(kconfig);
          }

        return(work);
      }


    template <typename SizeFunctor>
    work_queue<threepf_kconfig> scheduler<SizeFunctor>::make_threepf_queue(const SizeFunctor sf, const std::vector<double>& ks)
      {
        work_queue<threepf_kconfig> work(this->ctx, sf, ks);

        // build a work queue for a 3pf from a cubic lattice of k-modes

        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        bool stored_background = false;

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

                    if(kconfig.k_t >= 2.0 * maxijk)   // impose the triangle conditions
                      {

                        kconfig.k1  = ks[i];
                        kconfig.k2  = ks[j];
                        kconfig.k3  = ks[k];

                        kconfig.k_t = ks[i] + ks[j] + ks[k];
                        kconfig.beta  = 1.0 - 2.0 * ks[k] / kconfig.k_t;
                        kconfig.alpha = 4.0 * ks[i] / kconfig.k_t - 1.0 - kconfig.beta;

                        kconfig.index[0] = i;
                        kconfig.index[1] = j;
                        kconfig.index[2] = k;

                        kconfig.store_background = stored_background ? false : (stored_background = true);
                        kconfig.store_twopf      = stored_twopf      ? false : (stored_twopf = true);

                        work.enqueue_work_item(kconfig);
                      }
                  }
              }

            if(stored_twopf == false)
              {
              std::cerr << __CPP_TRANSPORT_TWOPF_STORE << std::endl;
              exit(1);  // this error shouldn't happen. TODO: tidy this up; could do with a proper error handler
              }
          }

        if(stored_background == false)
          {
            std::cerr << __CPP_TRANSPORT_BACKGROUND_STORE << std::endl;
            exit(1);  // this error shouldn't happen. TODO: tidy this up; could do with a proper error handler
          }

        return(work);
      }

  } // / namespace transport



#endif //__scheduler_H_
