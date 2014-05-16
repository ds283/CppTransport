//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __observers_H_
#define __observers_H_


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/manager/data_manager.h"

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

        unsigned int get_time_serial() { return(this->time_serial); }

        void step() { this->time_serial++; }

      private:
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

        template <typename Level>
        void start(double t, boost::log::sources::severity_logger<Level>& logger, Level lev)
          {
            std::string rval = "";

            if(!ready || t > this->t_last + this->t_interval)
              {
                this->timer.stop();
                this->t_last = t;

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t;
                if(ready) msg << " " << __CPP_TRANSPORT_OBSERVER_ELAPSED << " =" << this->timer.format();

                BOOST_LOG_SEV(logger, lev) << msg.str();

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

      private:
        bool                    silent;      // whether to generate output during observations
        bool                    ready;
        double                  t_last;      // last value at which we emitted output
        double                  t_interval;  // t-interval at which to emit output
        unsigned int            precision;   // precision used in output

        boost::timer::cpu_timer timer;
      };


    // Observer: records results from a single twopf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class twopf_singleconfig_batch_observer: public timing_observer
      {
      public:
        twopf_singleconfig_batch_observer(typename data_manager<number>::twopf_batcher& b, const twopf_kconfig& c,
                                          unsigned int bg_sz, unsigned int tw_sz,
                                          unsigned int bg_st, unsigned int tw_st,
                                          double t_int=1.0, bool s=false, unsigned int p=3)
          : timing_observer(t_int, s, p), batcher(b), k_config(c),
            backg_size(bg_sz), twopf_size(tw_sz),
            backg_start(bg_st), twopf_start(tw_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->k_config.store_background)
              {
                std::vector<number> bg_x(this->backg_size);

                for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[this->backg_start + i];
                this->batcher.push_backg(this->get_time_serial(), bg_x);
              }

            std::vector<number> tpf_x(this->twopf_size);

            for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_start + i];
            this->batcher.push_twopf(this->get_time_serial(), this->k_config.serial, tpf_x);

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

      private:
        const twopf_kconfig&                          k_config;
        typename data_manager<number>::twopf_batcher& batcher;

        unsigned int                                  backg_size;
        unsigned int                                  twopf_size;

        unsigned int                                  backg_start;
        unsigned int                                  twopf_start;
      };


    // Observer: records results from a single threepf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class threepf_singleconfig_batch_observer: public timing_observer
      {
      public:
        threepf_singleconfig_batch_observer(typename data_manager<number>::threepf_batcher& b, const threepf_kconfig& c,
                                            unsigned int bg_sz, unsigned int tw_sz, unsigned int th_sz,
                                            unsigned int bg_st, unsigned int tw_re_st, unsigned int tw_im_st, unsigned int th_st,
                                            double t_int=1.0, bool s=false, unsigned int p=3)
          : timing_observer(t_int, s, p), batcher(b), k_config(c),
            backg_size(bg_sz), twopf_size(tw_sz), threepf_size(th_sz),
            backg_start(bg_st), twopf_re_start(tw_re_st), twopf_im_start(tw_im_st), threepf_start(th_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->k_config.store_background)
              {
                std::vector<number> bg_x(this->backg_size);

                for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[this->backg_start + i];
                this->batcher.push_backg(this->get_time_serial(), bg_x);
              }

            if(this->k_config.store_twopf)
              {
                std::vector<number> tpf_x(this->twopf_size);

                for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_re_start + i];
                this->batcher.push_twopf(this->get_time_serial(), this->k_config.index[0], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_im_start + i];
                this->batcher.push_twopf(this->get_time_serial(), this->k_config.index[0], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
              }

            std::vector<number> thpf_x(this->threepf_size);
            for(unsigned int i = 0; i < this->threepf_size; i++) thpf_x[i] = x[this->threepf_start + i];
            this->batcher.push_threepf(this->get_time_serial(), this->k_config.serial, thpf_x);

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

      private:
        const threepf_kconfig&                          k_config;
        typename data_manager<number>::threepf_batcher& batcher;

        unsigned int                                    backg_size;
        unsigned int                                    twopf_size;
        unsigned int                                    threepf_size;

        unsigned int                                    backg_start;
        unsigned int                                    twopf_re_start;
        unsigned int                                    twopf_im_start;
        unsigned int                                    threepf_start;
      };


    // Observer: records results from a batch of twopf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class twopf_groupconfig_batch_observer: public timing_observer
      {
      public:
        twopf_groupconfig_batch_observer(typename data_manager<number>::twopf_batcher& b,
                                         const work_queue<twopf_kconfig>::device_work_list& c,
                                         unsigned int bg_sz, unsigned int tw_sz,
                                         unsigned int bg_st, unsigned int tw_st,
                                         double t_int = 1.0, bool s = false, unsigned int p = 3)
	        : timing_observer(t_int, s, p), batcher(b), work_list(c),
	          backg_size(bg_sz), twopf_size(tw_sz),
	          backg_start(bg_st), twopf_start(tw_st)
	        {
	        }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            unsigned int n = work_list.size();

            // loop through all k-configurations
            for(unsigned int c = 0; c < n; c++)
              {
                if(this->work_list[c].store_background)
                  {
                    std::vector<number> bg_x(this->backg_size);

                    for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[(this->backg_start + i)*n + c];
                    this->batcher.push_backg(this->get_time_serial(), bg_x);
                  }

                std::vector<number> tpf_x(this->twopf_size);

                for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_start + i)*n + c];
                this->batcher.push_twopf(this->get_time_serial(), this->work_list[c].serial, tpf_x);
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }

      private:
        const work_queue<twopf_kconfig>::device_work_list& work_list;
        typename data_manager<number>::twopf_batcher&      batcher;

        unsigned int                                       backg_size;
        unsigned int                                       twopf_size;

        unsigned int                                       backg_start;
        unsigned int                                       twopf_start;
      };


    // Observer: records results from a batch of threepf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class threepf_groupconfig_batch_observer: public timing_observer
      {
      public:
        threepf_groupconfig_batch_observer(typename data_manager<number>::threepf_batcher& b,
                                           const work_queue<threepf_kconfig>::device_work_list& c,
                                           unsigned int bg_sz, unsigned int tw_sz, unsigned int th_sz,
                                           unsigned int bg_st, unsigned int tw_re_st, unsigned int tw_im_st, unsigned int th_st,
                                           double t_int=1.0, bool s=false, unsigned int p=3)
          : timing_observer(t_int, s, p), batcher(b), work_list(c),
            backg_size(bg_sz), twopf_size(tw_sz), threepf_size(th_sz),
            backg_start(bg_st), twopf_re_start(tw_re_st), twopf_im_start(tw_im_st), threepf_start(th_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            unsigned int n = this->work_list.size();

            // loop through all k-configurations
            for(unsigned int c = 0; c < n; c++)
              {
                if(this->work_list[c].store_background)
                  {
                    std::vector<number> bg_x(this->backg_size);

                    for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[(this->backg_start + i)*n + c];
                    this->batcher.push_backg(this->get_time_serial(), bg_x);
                  }

                if(this->work_list[c].store_twopf)
                  {
                    std::vector<number> tpf_x(this->twopf_size);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_re_start + i)*n + c];
                    this->batcher.push_twopf(this->get_time_serial(), this->work_list[c].index[0], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_im_start + i)*n + c];
                    this->batcher.push_twopf(this->get_time_serial(), this->work_list[c].index[0], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                  }

                std::vector<number> thpf_x(this->threepf_size);
                for(unsigned int i = 0; i < this->threepf_size; i++) thpf_x[i] = x[(this->threepf_start + i)*n + c];
                this->batcher.push_threepf(this->get_time_serial(), this->work_list[c].serial, thpf_x);
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }

      private:
        const work_queue<threepf_kconfig>::device_work_list& work_list;
        typename data_manager<number>::threepf_batcher&      batcher;

        unsigned int                                         backg_size;
        unsigned int                                         twopf_size;
        unsigned int                                         threepf_size;

        unsigned int                                         backg_start;
        unsigned int                                         twopf_re_start;
        unsigned int                                         twopf_im_start;
        unsigned int                                         threepf_start;
      };

  } // namespace transport


#endif //__observers_H_
