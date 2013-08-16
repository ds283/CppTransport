//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __sample_gadget_H_
#define __sample_gadget_H_

#include <iostream>
#include <fstream>

#include <array>
#include <vector>
#include <string>

#include <math.h>

#include "messages_en.h"

#define DEFAULT_SAMPLE_POINTS  (500)
#define DEFAULT_SAMPLE_SPACING (sample_gadget_linear)


namespace transport
  {

    enum sample_gadget_spacing
      {
        sample_gadget_linear, sample_gadget_logarithmic
      };


    template <typename number>
    class sample_gadget
      {
      public:
        sample_gadget(number a, number b, unsigned int N = DEFAULT_SAMPLE_POINTS, enum sample_gadget_spacing spc = DEFAULT_SAMPLE_SPACING);

        void set_sample(number a, number b, unsigned int N = DEFAULT_SAMPLE_POINTS, enum sample_gadget_spacing spc = DEFAULT_SAMPLE_SPACING);

        void get_limits(number& a, number& b)        { if(this->sampled) { a = this->sample_min; b = this->sample_max; } else { a = min; b = max; } }

        unsigned int get_size()                      { if(this->sampled) return(this->sample_points); else return(this->npoints); }

        enum sample_gadget_spacing get_spacing()     { if(this->sampled) return(this->sample_spacing); else return(this->spacing); }

        const std::vector<number>& get_axis()        { if(this->sampled) return(this->sample_axis); else return(this->axis); }

        void clear_sample()                          { this->sampled = false; }

        bool is_sampled()                            { return(this->sampled); }

        void get_raw_limits(number& a, number& b)    { a = this->min; b = this->max; }

        unsigned int get_raw_size()                  { return(this->npoints); }

        enum sample_gadget_spacing get_raw_spacing() { return(this->spacing); }

        const std::vector<number>& get_raw_axis()    { return(this->axis); }

      protected:
        number min;
        number max;

        unsigned int npoints;
        enum sample_gadget_spacing spacing;

        std::vector<number> axis;

        number sample_min;
        number sample_max;

        bool sampled;
        unsigned int sample_points;
        enum sample_gadget_spacing sample_spacing;

        std::vector<number> sample_axis;
      };


    template <typename number>
    sample_gadget<number>::sample_gadget(number a, number b, unsigned int N, enum sample_gadget_spacing spc)
    : min(a), max(b), npoints(N), spacing(spc), sampled(false)
      {
        // set up axis; after creation, this cannot be changed - only resampled
        axis.resize(npoints);

        switch(spacing)
          {
            case sample_gadget_linear:
              {
                for(int i = 0; i < npoints; i++)
                  {
                    axis[i] = min + (double) i * (max - min) / npoints;
                  }
              }
            break;

            case sample_gadget_logarithmic:
              {
                for(int i = 0; i < npoints; i++)
                  {
                    axis[i] = min * pow(max / min, (double) i / (double) npoints);
                  }
              }
            break;

            default:
              assert(false);
          }
      }


    template <typename number>
    void sample_gadget<number>::set_sample(number a, number b, unsigned int N, enum sample_gadget_spacing spc)
      {
        this->sampled = true;
        this->sample_points = N;
        this->sample_spacing = spc;

        this->sample_axis.resize(N);

        auto minab = a < b ? a : b;
        auto min = this->min < minab ? this->min : minab;
        auto maxab = a > b ? a : b;
        auto max   = this->max > maxab ? maxab     : this->max;

        switch(spc)
          {
            case sample_gadget_linear:
              {
                for(int i = 0; i < N; i++)
                  {
                    this->sample_axis[i] = min + (double) i * (max - min) / N;
                  }
              }
            break;

            case sample_gadget_logarithmic:
              {
                for(int i = 0; i < N; i++)
                  {
                    this->sample_axis[i] = min * pow(max / min, (double) i / (double) N);
                  }
              }
            break;

            default:
              assert(false);
          }

        this->sample_min = min;
        this->sample_max = max;
      }


    template <typename number>
    class threepf_kconfig
      {
      public:
        threepf_kconfig(number k1, number k2, number k3, bool bg, bool tpf);

        bool store_background() { return(this->store_bg); }
        bool store_twopf()      { return(this->store_tpf); }}

      protected:
        std::array<number, 3>       ks;
        std::array<unsigned int, 3> indices;

        number                      k_t;
        number                      alpha;
        number                      beta;

        bool                        store_bg;
        bool                        store_tpf;
      };


    template <typename number>
    class threepf_sample_gadget
      {
      public:
        threepf_sample_gadget(sample_gadget<number>& axis);

      protected:
        number kt_min;
        number kt_max;

        number alpha_min;
        number alpha_max;

        number beta_min;
        number beta_max;

        sample_gadget<number>& ksample;

        std::vector<class threepf_kconfig> config_list;
      };


    template <typename number>
    threepf_kconfig<number>::threepf_kconfig(number k1, number k2, number k3, bool bg, bool tpf)
      : store_bg(bg), store_tpf(tpf)
      {
        k_t = k1 + k2 + k3;

        auto max12  = (k1 > k2    ? k1 : k2);
        auto max123 = (k3 > max12 ? k3 : max12);

        if(k_t >= 2.0*max123)
          {
            beta  = 1.0 - 2.0*k3/k_t;
            alpha = 4.0*k1/k_t - 1.0 - beta;

            ks[0] = k1;
            ks[1] = k2;
            ks[2] = k3;
          }
        else
          {
            assert(false);  // should not happen
          }
      }


    template <typename number>
    threepf_sample_gadget<number>::threepf_sample_gadget(sample_gadget<number>& axis)
      : ksample(axis)
      {
        bool stored_background = false;

        const std::vector<number>& mesh = axis.get_axis();

        for(int i = 0; i < mesh.length(); i++)
          {
            bool stored_twopf = false;

            for(int j = 0; j <= i; j++)
              {
                for(int k = 0; k <= j; k++)
                  {
                    auto k_t = mesh[i] + mesh[j] + mesh[k];

                    auto maxij  = (mesh[i] > mesh[j] ? mesh[i] : mesh[j]);
                    auto maxijk = (mesh[k] > maxij   ? mesh[k] : maxij);

                    if(k_t >= 2.0 * maxijk)
                      {
                        threepf_kconfig<number> config(mesh[i], mesh[j], mesh[k],
                                                       stored_background ? false : (stored_background=true),
                                                       stored_twopf      ? false : (stored_twopf=true));

                        config_list.push_back(config);
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
      }


    class null_config_gadget
      {
        public:
          unsigned int get_size()     { return(1); }

          unsigned int get_raw_size() { return(1); }
      };


  }   // namespace transport


#endif // __sample_gadget_H_
