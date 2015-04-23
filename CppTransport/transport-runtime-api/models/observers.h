//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __observers_H_
#define __observers_H_


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/data/data_manager.h"
#include "transport-runtime-api/tasks/task_configurations.h"
#include "transport-runtime-api/tasks/configuration-database/time_config_database.h"
#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"
#include "transport-runtime-api/tasks/configuration-database/threepf_config_database.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include <boost/timer/timer.hpp>


namespace transport
  {


    //! A stepping observer is the basic type of observer object.
    //! It is capable of keeping track of time steps and matching them
    //! to the list of steps which should be stored according to the
    //! active time-step storage policy
    template <typename number>
    class stepping_observer
      {

      public:

        //! Create a stepping observer object
        stepping_observer(const time_config_database& t)
          : time_db(t)
          {
            current_step = time_db.record_begin();
          }


        // INTERFACE

      public:

        //! Advance time-step counter
        void step() { this->current_step++; }

        //! Query whether the current time step should be stored
        bool store_time_step() const { return(this->current_step->is_stored()); }

        //! Query serial number to use when storing
        unsigned int store_serial_number() const { return((*this->current_step)->serial); }

        //! Query time associated with storing
        double store_time() const { return((*this->current_step)->t); }


        // INTERNAL DATA

      protected:

        //! Pointer to record for current time
        time_config_database::const_record_iterator current_step;

        //! List of steps which should be stored
        const time_config_database& time_db;

      };


    //! A timing observer is a more sophisticated type of observer; it keeps track
    //! of how long is spent during the integration (and the batching process)
    template <typename number>
    class timing_observer: public stepping_observer<number>
      {

      public:

        //! Create a timing observer object
        timing_observer(const time_config_database& t, double t_int=1.0, bool s=false, unsigned int p=3);


        // INTERFACE

      public:

        //! Prepare for a batching step
        template <typename Level>
        void start_batching(double t, boost::log::sources::severity_logger<Level>& logger, Level lev);

        //! Conclude a batching step
        void stop_batching();

        //! Stop the running timers - should only be called at the end of an integration
        virtual void stop_timers(size_t steps, unsigned int refinement);

        //! Get the total elapsed integration time
        boost::timer::nanosecond_type get_integration_time() const { return(this->integration_timer.elapsed().wall); }

        //! Get the total elapsed batching time
        boost::timer::nanosecond_type get_batching_time() const { return(this->batching_timer.elapsed().wall); }


        // INTERNAL DATA

      private:

        //! Do we generate output during observations?
        bool                    silent;

        //! Is this the first batching step? Used to decide whether to issue output
        bool                    first_step;

        //! Last time at which output was emitted;
        //! used to decide whether to emit output during
        //! the next observation
        double                  t_last;

        //! Time interval at which to issue updates
        double                  t_interval;

        //! Numerical precision to be used when issuing updates
        unsigned int            precision;

        //! Timer for the integration
        boost::timer::cpu_timer integration_timer;

        //! Timer for batching
        boost::timer::cpu_timer batching_timer;
      };


    template <typename number>
    timing_observer<number>::timing_observer(const time_config_database& t, double t_int, bool s, unsigned int p)
      : stepping_observer<number>(t), t_interval(t_int), silent(s), first_step(true), t_last(0), precision(p)
      {
        batching_timer.stop();
        // leave the integration timer running, so it also records start-up time associated with the integration,
        // eg. setting up initial conditions or copying data to an offload device such as a GPU
      }


    template <typename number>
    template <typename Level>
    void timing_observer<number>::start_batching(double t, boost::log::sources::severity_logger <Level>& logger, Level lev)
	    {
        std::string rval = "";

        this->integration_timer.stop();
        this->batching_timer.start();

        // should we emit output?
        // only do so if: not silent, and: either, first step, or enough time has elapsed
        if(!this->silent && (this->first_step || t > this->t_last + this->t_interval))
	        {
            this->t_last = t;

            std::ostringstream msg;
            msg << __CPP_TRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t;
            if(first_step)
	            {
                msg << " " << __CPP_TRANSPORT_OBSERVER_ELAPSED << " =" << this->integration_timer.format();
	            }
            BOOST_LOG_SEV(logger, lev) << msg.str();

            first_step = false;
	        }
	    }


    template <typename number>
    void timing_observer<number>::stop_batching()
      {
        this->batching_timer.stop();
        this->integration_timer.start();
      }


    template <typename number>
    void timing_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->batching_timer.stop();
        this->integration_timer.stop();
      }


    // Observer: records results from a single twopf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class twopf_singleconfig_batch_observer: public timing_observer<number>
      {

      public:

        twopf_singleconfig_batch_observer(twopf_batcher<number>& b, const twopf_kconfig_record& c,
                                          const time_config_database& t,
                                          unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                          unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                          double t_int = 1.0, bool s = true, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        boost::log::sources::severity_logger<generic_batcher::log_severity_level>& get_log() { return(this->batcher.get_log()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const twopf_kconfig_record&                   k_config;
        twopf_batcher<number>& batcher;

        unsigned int                                  backg_size;
        unsigned int                                  tensor_size;
        unsigned int                                  twopf_size;

        unsigned int                                  backg_start;
        unsigned int                                  tensor_start;
        unsigned int                                  twopf_start;

      };


    template <typename number>
    twopf_singleconfig_batch_observer<number>::twopf_singleconfig_batch_observer(twopf_batcher<number>& b, const twopf_kconfig_record& c,
                                                                                 const time_config_database& t,
                                                                                 unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                                                                 unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                                                                 double t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b), k_config(c),
        backg_size(bg_sz), tensor_size(ten_sz), twopf_size(tw_sz),
        backg_start(bg_st), tensor_start(ten_st), twopf_start(tw_st)
      {
      }


    template <typename number>
    template <typename State>
    void twopf_singleconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            std::vector<number> bg_x(this->backg_size);
            for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[this->backg_start + i];

            std::vector<number> tensor_tpf_x(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x[i] = x[this->tensor_start + i];

            std::vector<number> tpf_x(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x[i] = x[this->twopf_start + i];

            if(this->k_config.is_background_stored())
              {
                this->batcher.push_backg(this->store_serial_number(), this->k_config->serial, bg_x);
              }
            this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->serial, this->k_config->serial, tensor_tpf_x);
            this->batcher.push_twopf(this->store_serial_number(), this->k_config->serial, this->k_config->serial, tpf_x, bg_x);
          }

        this->step();
      }


    template <typename number>
    void twopf_singleconfig_batch_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->timing_observer<number>::stop_timers(steps, refinement);
        this->batcher.report_integration_success(this->get_integration_time(), this->get_batching_time(), this->k_config->serial, steps, refinement);
      }


    // Observer: records results from a single threepf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class threepf_singleconfig_batch_observer: public timing_observer<number>
      {

      public:

        threepf_singleconfig_batch_observer(threepf_batcher<number>& b, const threepf_kconfig_record& c,
                                            const time_config_database& t,
                                            unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                            unsigned int bg_st,
                                            unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                            unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                            unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                            unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                            unsigned int th_st,
                                            double t_int = 1.0, bool s = true, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        boost::log::sources::severity_logger<generic_batcher::log_severity_level>& get_log() { return(this->batcher.get_log()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA


      private:

        const threepf_kconfig_record&                   k_config;
        threepf_batcher<number>& batcher;

        unsigned int                                    backg_size;
        unsigned int                                    tensor_size;
        unsigned int                                    twopf_size;
        unsigned int                                    threepf_size;

        unsigned int                                    backg_start;
        unsigned int                                    tensor_k1_start;
        unsigned int                                    tensor_k2_start;
        unsigned int                                    tensor_k3_start;
        unsigned int                                    twopf_re_k1_start;
        unsigned int                                    twopf_im_k1_start;
        unsigned int                                    twopf_re_k2_start;
        unsigned int                                    twopf_im_k2_start;
        unsigned int                                    twopf_re_k3_start;
        unsigned int                                    twopf_im_k3_start;
        unsigned int                                    threepf_start;

      };


    template <typename number>
    threepf_singleconfig_batch_observer<number>::threepf_singleconfig_batch_observer(threepf_batcher<number>& b, const threepf_kconfig_record& c,
                                                                                     const time_config_database& t,
                                                                                     unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                                                                     unsigned int bg_st,
                                                                                     unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                                                                     unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                                                                     unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                                                                     unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                                                                     unsigned int th_st,
                                                                                     double t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b), k_config(c),
        backg_size(bg_sz), tensor_size(ten_sz), twopf_size(tw_sz), threepf_size(th_sz),
        backg_start(bg_st),
        tensor_k1_start(ten_k1_st), tensor_k2_start(ten_k2_st), tensor_k3_start(ten_k3_st),
        twopf_re_k1_start(tw_re_k1_st), twopf_im_k1_start(tw_im_k1_st),
        twopf_re_k2_start(tw_re_k2_st), twopf_im_k2_start(tw_im_k2_st),
        twopf_re_k3_start(tw_re_k3_st), twopf_im_k3_start(tw_im_k3_st),
        threepf_start(th_st)
      {
      }


    template <typename number>
    template <typename State>
    void threepf_singleconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            std::vector<number> bg_x(this->backg_size);
            for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[this->backg_start + i];

            std::vector<number> tensor_tpf_x1(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x1[i] = x[this->tensor_k1_start + i];

            std::vector<number> tpf_x1_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_re[i] = x[this->twopf_re_k1_start + i];
            std::vector<number> tpf_x1_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_im[i] = x[this->twopf_im_k1_start + i];

            std::vector<number> tensor_tpf_x2(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x2[i] = x[this->tensor_k2_start + i];

            std::vector<number> tpf_x2_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_re[i] = x[this->twopf_re_k2_start + i];
            std::vector<number> tpf_x2_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_im[i] = x[this->twopf_im_k2_start + i];

            std::vector<number> tensor_tpf_x3(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x3[i] = x[this->tensor_k3_start + i];

            std::vector<number> tpf_x3_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_re[i] = x[this->twopf_re_k3_start + i];
            std::vector<number> tpf_x3_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_im[i] = x[this->twopf_im_k3_start + i];

            std::vector<number> thpf_x(this->threepf_size);
            for(unsigned int i = 0; i < this->threepf_size; ++i) thpf_x[i] = x[this->threepf_start + i];

            if(this->k_config.is_background_stored())
              {
                this->batcher.push_backg(this->store_serial_number(), this->k_config->serial, bg_x);
              }

            if(this->k_config.is_twopf_k1_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tensor_tpf_x1);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tpf_x1_re, bg_x, threepf_batcher<number>::real_twopf);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tpf_x1_im, bg_x, threepf_batcher<number>::imag_twopf);
              }

            if(this->k_config.is_twopf_k2_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tensor_tpf_x2);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tpf_x2_re, bg_x, threepf_batcher<number>::real_twopf);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tpf_x2_im, bg_x, threepf_batcher<number>::imag_twopf);
              }

            if(this->k_config.is_twopf_k3_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tensor_tpf_x3);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tpf_x3_re, bg_x, threepf_batcher<number>::real_twopf);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tpf_x3_im, bg_x, threepf_batcher<number>::imag_twopf);
              }

            this->batcher.push_threepf(this->store_serial_number(), this->store_time(), *this->k_config, this->k_config->serial, thpf_x, tpf_x1_re, tpf_x1_im, tpf_x2_re, tpf_x2_im, tpf_x3_re, tpf_x3_im, bg_x);
          }

        this->step();
      }


    template <typename number>
    void threepf_singleconfig_batch_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->timing_observer<number>::stop_timers(steps, refinement);
        this->batcher.report_integration_success(this->get_integration_time(), this->get_batching_time(), this->k_config->serial, steps, refinement);
      }


    // Observer: records results from a batch of twopf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class twopf_groupconfig_batch_observer: public timing_observer<number>
      {

      public:

        twopf_groupconfig_batch_observer(twopf_batcher<number>& b,
                                         const work_queue<twopf_kconfig_record>::device_work_list& c,
                                         const time_config_database& t,
                                         unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                         unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                         double t_int = 1.0, bool s = false, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        boost::log::sources::severity_logger<generic_batcher::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const work_queue<twopf_kconfig_record>::device_work_list& work_list;
        twopf_batcher<number>&                                    batcher;

        unsigned int                                              backg_size;
        unsigned int                                              tensor_size;
        unsigned int                                              twopf_size;

        unsigned int                                              backg_start;
        unsigned int                                              tensor_start;
        unsigned int                                              twopf_start;

      };


    template <typename number>
    twopf_groupconfig_batch_observer<number>::twopf_groupconfig_batch_observer(twopf_batcher<number>& b,
                                                                               const work_queue<twopf_kconfig_record>::device_work_list& c,
                                                                               const time_config_database& t,
                                                                               unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                                                               unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                                                               double t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b), work_list(c),
        backg_size(bg_sz), tensor_size(ten_sz), twopf_size(tw_sz),
        backg_start(bg_st), tensor_start(ten_st), twopf_start(tw_st)
      {
      }


    template <typename number>
    template <typename State>
    void twopf_groupconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            unsigned int n = work_list.size();

            // loop through all k-configurations
            for(unsigned int c = 0; c < n; ++c)
              {
                std::vector<number> bg_x(this->backg_size);
                for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[(this->backg_start + i)*n + c];

                std::vector<number> tensor_tpf_x(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x[i] = x[(this->tensor_start + i)*n + c];

                std::vector<number> tpf_x(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x[i] = x[(this->twopf_start + i)*n + c];

                if(this->work_list[c].is_background_stored())
                  {
                    this->batcher.push_backg(this->store_serial_number(), this->work_list[c]->serial, bg_x);
                  }
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->serial, this->work_list[c]->serial, tensor_tpf_x);
                this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->serial, this->work_list[c]->serial, tpf_x, bg_x);
              }
          }

        this->step();
      }


    template <typename number>
    void twopf_groupconfig_batch_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->timing_observer<number>::stop_timers(steps, refinement);
        this->batcher.report_integration_success(this->get_integration_time(), this->get_batching_time(), steps, refinement);
      }


    // Observer: records results from a batch of threepf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class threepf_groupconfig_batch_observer: public timing_observer<number>
      {

      public:

        threepf_groupconfig_batch_observer(threepf_batcher<number>& b,
                                                   const work_queue<threepf_kconfig_record>::device_work_list& c,
                                                   const time_config_database& t,
                                                   unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                                   unsigned int bg_st,
                                                   unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                                   unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                                   unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                                   unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                                   unsigned int th_st,
                                                   double t_int=1.0, bool s=false, unsigned int p=3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        boost::log::sources::severity_logger<generic_batcher::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const work_queue<threepf_kconfig_record>::device_work_list& work_list;
        threepf_batcher<number>&                                    batcher;

        unsigned int                                                backg_size;
        unsigned int                                                tensor_size;
        unsigned int                                                twopf_size;
        unsigned int                                                threepf_size;

        unsigned int                                                backg_start;
        unsigned int                                                tensor_k1_start;
        unsigned int                                                tensor_k2_start;
        unsigned int                                                tensor_k3_start;
        unsigned int                                                twopf_re_k1_start;
        unsigned int                                                twopf_im_k1_start;
        unsigned int                                                twopf_re_k2_start;
        unsigned int                                                twopf_im_k2_start;
        unsigned int                                                twopf_re_k3_start;
        unsigned int                                                twopf_im_k3_start;
        unsigned int                                                threepf_start;

      };


    template <typename number>
    threepf_groupconfig_batch_observer<number>::threepf_groupconfig_batch_observer(threepf_batcher<number>& b,
                                                                                   const work_queue<threepf_kconfig_record>::device_work_list& c,
                                                                                   const time_config_database& t,
                                                                                   unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                                                                   unsigned int bg_st,
                                                                                   unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                                                                   unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                                                                   unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                                                                   unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                                                                   unsigned int th_st,
                                                                                   double t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b), work_list(c),
        backg_size(bg_sz), tensor_size(ten_sz), twopf_size(tw_sz), threepf_size(th_sz),
        backg_start(bg_st),
        tensor_k1_start(ten_k1_st), tensor_k2_start(ten_k2_st), tensor_k3_start(ten_k3_st),
        twopf_re_k1_start(tw_re_k1_st), twopf_im_k1_start(tw_im_k1_st),
        twopf_re_k2_start(tw_re_k2_st), twopf_im_k2_start(tw_im_k2_st),
        twopf_re_k3_start(tw_re_k3_st), twopf_im_k3_start(tw_im_k3_st),
        threepf_start(th_st)
      {
      }


    template <typename number>
    template <typename State>
    void threepf_groupconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            unsigned int n = this->work_list.size();

            // loop through all k-configurations
            for(unsigned int c = 0; c < n; ++c)
              {
                std::vector<number> bg_x(this->backg_size);
                for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[(this->backg_start + i)*n + c];

                std::vector<number> tensor_tpf_x1(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x1[i] = x[(this->tensor_k1_start + i)*n + c];

                std::vector<number> tpf_x1_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_re[i] = x[(this->twopf_re_k1_start + i)*n + c];
                std::vector<number> tpf_x1_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_im[i] = x[(this->twopf_im_k1_start + i)*n + c];

                std::vector<number> tensor_tpf_x2(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x2[i] = x[(this->tensor_k2_start + i)*n + c];

                std::vector<number> tpf_x2_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_re[i] = x[(this->twopf_re_k2_start + i)*n + c];
                std::vector<number> tpf_x2_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_im[i] = x[(this->twopf_im_k2_start + i)*n + c];

                std::vector<number> tensor_tpf_x3(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x3[i] = x[(this->tensor_k3_start + i)*n + c];

                std::vector<number> tpf_x3_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_re[i] = x[(this->twopf_re_k3_start + i)*n + c];
                std::vector<number> tpf_x3_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_im[i] = x[(this->twopf_im_k3_start + i)*n + c];

                std::vector<number> thpf_x(this->threepf_size);
                for(unsigned int i = 0; i < this->threepf_size; ++i) thpf_x[i] = x[(this->threepf_start + i)*n + c];

                if(this->work_list[c].is_background_stored())
                  {
                    this->batcher.push_backg(this->store_serial_number(), this->work_list[c]->serial, bg_x);
                  }

                if(this->work_list[c].is_twopf_k1_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tensor_tpf_x1);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tpf_x1_re, bg_x, threepf_batcher<number>::real_twopf);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tpf_x1_im, bg_x, threepf_batcher<number>::imag_twopf);
                  }

                if(this->work_list[c].is_twopf_k2_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tensor_tpf_x2);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tpf_x2_re, bg_x, threepf_batcher<number>::real_twopf);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tpf_x2_im, bg_x, threepf_batcher<number>::imag_twopf);
                  }

                if(this->work_list[c].is_twopf_k3_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tensor_tpf_x3);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tpf_x3_re, bg_x, threepf_batcher<number>::real_twopf);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tpf_x3_im, bg_x, threepf_batcher<number>::imag_twopf);
                  }

                this->batcher.push_threepf(this->store_serial_number(), this->store_time(), *(this->work_list[c]), this->work_list[c]->serial, thpf_x, tpf_x1_re, tpf_x1_im, tpf_x2_re, tpf_x2_im, tpf_x3_re, tpf_x3_im, bg_x);
              }
          }

        this->step();
      }


    template <typename number>
    void threepf_groupconfig_batch_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->timing_observer<number>::stop_timers(steps, refinement);
        this->batcher.report_integration_success(this->get_integration_time(), this->get_batching_time(), steps, refinement);
      }


  } // namespace transport


#endif //__observers_H_
