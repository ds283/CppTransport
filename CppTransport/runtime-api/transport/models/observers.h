//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __observers_H_
#define __observers_H_


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport/messages_en.h"
#include "transport/manager/data_manager.h"

#include <boost/timer/timer.hpp>


namespace transport
  {


    class stepping_observer
      {
      public:
        stepping_observer()
          : time_serial(0)
          {
          }

        unsigned int get_time_serial() { return(this->time_serial++); }

      protected:
        unsigned int time_serial;
      };


    class timing_observer: public stepping_observer
      {
      public:
        timing_observer(double t_int=1.0, bool s=false, unsigned int p=3)
          : stepping_observer(), t_interval(t_int), silent(s), ready(false), t_last(0), precision(p)
          {
            timer.stop();
          }

        void start(double t)
          {
            if(!ready || t > this->t_last + this->t_interval)
              {
                this->timer.stop();
                this->t_last = t;
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t;
                std::cout << msg.str();
                if(ready)
                  {
                    std::cout << " " << __CPP_TRANSPORT_OBSERVER_ELAPSED << " =" << this->timer.format();
                  }
                else
                  {
                    std::cout << std::endl;
                  }
                ready = false;
              }
          }

        void stop()
          {
            if(!ready)
              {
                this->timer.start();
                ready = true;
              }
          }

      protected:
        bool                    silent;      // whether to generate output during observations
        bool                    ready;
        double                  t_last;      // last value at which we emitted output
        double                  t_interval;  // t-interval at which to emit output
        unsigned int            precision;   // precision used in output

        boost::timer::cpu_timer timer;
      };


    template <typename number>
    class twopf_singleconfig_batch_observer: public timing_observer
      {
      public:
        twopf_singleconfig_batch_observer(typename data_manager<number>::twopf_batcher& b, const twopf_kconfig& c)
          : batcher(b), k_config(c)
          {
          }

        unsigned int get_config_serial() { return(this->k_config.serial); }

        bool store_background() { return(this->k_config.store_background); }

        typename data_manager<number>::twopf_batcher& get_batcher() { return(this->batcher); }

      protected:
        const twopf_kconfig& k_config;
        typename data_manager<number>::twopf_batcher& batcher;
      };


    template <typename number>
    class threepf_singleconfig_batch_observer: public timing_observer
      {
      public:
        threepf_singleconfig_batch_observer(typename data_manager<number>::threepf_batcher& b, const threepf_kconfig& c)
          : batcher(b), k_config(c)
          {
          }

        unsigned int get_twopf_config_serial() { return(this->k_config.index[0]); }   // if we store the twopf, then it is the k1 configuration

        unsigned int get_threepf_config_serial() { return(this->k_config.serial); }

        bool store_background() { return(this->k_config.store_background); }

        bool store_twopf() { return(this->k_config.store_twopf); }

        typename data_manager<number>::threepf_batcher& get_batcher() { return(this->batcher); }

      protected:
        const threepf_kconfig& k_config;
        typename data_manager<number>::threepf_batcher& batcher;
      };

  } // namespace transport


#endif //__observers_H_
