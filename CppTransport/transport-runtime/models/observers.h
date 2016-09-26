//
// Created by David Seery on 22/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_OBSERVERS_H
#define CPPTRANSPORT_OBSERVERS_H


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/data/data_manager.h"

#include "transport-runtime/tasks/task_configurations.h"
#include "transport-runtime/tasks/configuration-database/time_config_database.h"
#include "transport-runtime/tasks/configuration-database/twopf_config_database.h"
#include "transport-runtime/tasks/configuration-database/threepf_config_database.h"

#include "transport-runtime/scheduler/work_queue.h"

#include "transport-runtime/utilities/formatter.h"

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
        stepping_observer(const time_config_database& t, unsigned int p)
          : time_db(t),
            precision(p)
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

        //! Numerical precision to be used when logging
        unsigned int precision;

      };


    //! A timing observer is a more sophisticated type of observer; it keeps track
    //! of how long is spent during the integration (and the batching process)
    template <typename number>
    class timing_observer: public stepping_observer<number>
      {

      public:

        //! Create a timing observer object
        timing_observer(const time_config_database& t,
                        boost::timer::nanosecond_type t_int=CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY,
                        bool s=false, unsigned int p=3);


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
        bool                          silent;

        //! Is this the first batching step? Used to decide whether to issue output
        bool first_output;

        //! Last time at which output was emitted;
        //! used to decide whether to emit output during
        //! the next observation
        boost::timer::nanosecond_type last_output;

        //! Time interval at which to issue updates
        boost::timer::nanosecond_type output_interval;

        //! Timer for the integration
        boost::timer::cpu_timer       integration_timer;

        //! Timer for batching
        boost::timer::cpu_timer       batching_timer;

      };


    template <typename number>
    timing_observer<number>::timing_observer(const time_config_database& t, boost::timer::nanosecond_type t_int, bool s, unsigned int p)
      : stepping_observer<number>(t,p),
        output_interval(t_int),
        silent(s),
        first_output(true)
      {
        batching_timer.stop();
        // leave the integration timer running, so it also records start-up time associated with the integration,
        // eg. setting up initial conditions or copying data to an offload device such as a GPU

        last_output = integration_timer.elapsed().wall;
      }


    template <typename number>
    template <typename Level>
    void timing_observer<number>::start_batching(double t, boost::log::sources::severity_logger<Level>& logger, Level lev)
	    {
        this->integration_timer.stop();
        this->batching_timer.resume();

        // should we emit output?
        // only do so if not in silent mode and enough time has elapsed since the last update
        if(!this->silent && (this->integration_timer.elapsed().wall - this->last_output > this->output_interval))
	        {
            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

            std::ostringstream msg;
		        msg << "-- " << boost::posix_time::to_simple_string(now) << ": ";
		        if(this->first_output)
			        {
				        msg << CPPTRANSPORT_OBSERVER_SLOW_INTEGRATION;
			        }
		        else
			        {
				        msg << CPPTRANSPORT_OBSERVER_UPDATE;
			        }

            msg << CPPTRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t << " ";

		        if(this->first_output)
			        {
				        msg << CPPTRANSPORT_OBSERVER_ELAPSED_FIRST;
			        }
		        else
			        {
		            msg << CPPTRANSPORT_OBSERVER_ELAPSED_LATER;
			        }

            msg << " = " << format_time(this->integration_timer.elapsed().wall - this->last_output);
            BOOST_LOG_SEV(logger, lev) << msg.str();

		        this->first_output = false;
            this->last_output = this->integration_timer.elapsed().wall;
          }
	    }


    template <typename number>
    void timing_observer<number>::stop_batching()
      {
        this->batching_timer.stop();
        this->integration_timer.resume();
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
                                          double t_ics, const time_config_database& t,
                                          unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                          unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                          boost::timer::nanosecond_type t_int = CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY,
                                          bool s = false, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        generic_batcher::logger& get_log() { return(this->batcher.get_log()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const twopf_kconfig_record& k_config;

        const double t_initial;

        twopf_batcher<number>& batcher;

        const unsigned int backg_size;
        const unsigned int tensor_size;
        const unsigned int twopf_size;

        const unsigned int backg_start;
        const unsigned int tensor_start;
        const unsigned int twopf_start;

      };


    template <typename number>
    twopf_singleconfig_batch_observer<number>::twopf_singleconfig_batch_observer(twopf_batcher<number>& b, const twopf_kconfig_record& c,
                                                                                 double t_ics, const time_config_database& t,
                                                                                 unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                                                                 unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                                                                 boost::timer::nanosecond_type t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b),
        k_config(c),
        t_initial(t_ics),
        backg_size(bg_sz),
        tensor_size(ten_sz),
        twopf_size(tw_sz),
        backg_start(bg_st),
        tensor_start(ten_st),
        twopf_start(tw_st)
      {
      }


    template <typename number>
    template <typename State>
    void twopf_singleconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            // correlation functions are already dimensionless, so no rescaling needed

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

        std::ostringstream init_time;
        init_time << std::scientific << std::setprecision(this->precision) << this->t_initial;

        std::ostringstream exit_time;
        exit_time << std::scientific << std::setprecision(this->precision) << this->k_config->t_exit;

        std::ostringstream massless_time;
        massless_time << std::scientific << std::setprecision(this->precision) << this->k_config->t_massless;

        std::ostringstream massless_efolds;
        massless_efolds << std::scientific << std::setprecision(this->precision) << (this->k_config->t_massless - this->t_initial);

        std::ostringstream subh_efolds;
        subh_efolds << std::scientific << std::setprecision(this->precision) << (this->k_config->t_exit - this->t_initial);

        BOOST_LOG_SEV(this->batcher.get_log(), generic_batcher::log_severity_level::normal)
	        << "** " << CPPTRANSPORT_SOLVING_CONFIG << " " << this->k_config->serial << ", "
	        << CPPTRANSPORT_INTEGRATION_TIME << " = " << format_time(this->get_integration_time()) << ", "
          << CPPTRANSPORT_INITIAL_TIME << " = " << init_time.str() << ", "
          << CPPTRANSPORT_EXIT_TIME << " = " << exit_time.str() << ", "
          << CPPTRANSPORT_MASSLESS_TIME << " = " << massless_time.str() << ", "
          << CPPTRANSPORT_SUBHORIZON_EFOLDS << " = " << subh_efolds.str() << ", "
          << CPPTRANSPORT_MASSLESS_EFOLDS << " = " << massless_efolds.str();
      }


    // Observer: records results from a single threepf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class threepf_singleconfig_batch_observer: public timing_observer<number>
      {

      public:

        threepf_singleconfig_batch_observer(threepf_batcher<number>& b, const threepf_kconfig_record& c,
                                            double t_ics, const time_config_database& t,
                                            unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                            unsigned int bg_st,
                                            unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                            unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                            unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                            unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                            unsigned int th_st,
                                            boost::timer::nanosecond_type t_int = CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY,
                                            bool s = false, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        generic_batcher::logger& get_log() { return(this->batcher.get_log()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA


      private:

        const threepf_kconfig_record& k_config;
        double k1_rescale;
        double k2_rescale;
        double k3_rescale;
        double shape_rescale;

        const double t_initial;

        threepf_batcher<number>& batcher;

        const unsigned int backg_size;
        const unsigned int tensor_size;
        const unsigned int twopf_size;
        const unsigned int threepf_size;

        const unsigned int backg_start;
        const unsigned int tensor_k1_start;
        const unsigned int tensor_k2_start;
        const unsigned int tensor_k3_start;
        const unsigned int twopf_re_k1_start;
        const unsigned int twopf_im_k1_start;
        const unsigned int twopf_re_k2_start;
        const unsigned int twopf_im_k2_start;
        const unsigned int twopf_re_k3_start;
        const unsigned int twopf_im_k3_start;
        const unsigned int threepf_start;

      };


    template <typename number>
    threepf_singleconfig_batch_observer<number>::threepf_singleconfig_batch_observer(threepf_batcher<number>& b, const threepf_kconfig_record& c,
                                                                                     double t_ics, const time_config_database& t,
                                                                                     unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz, unsigned int th_sz,
                                                                                     unsigned int bg_st,
                                                                                     unsigned int ten_k1_st, unsigned int ten_k2_st, unsigned int ten_k3_st,
                                                                                     unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                                                                     unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                                                                     unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                                                                     unsigned int th_st,
                                                                                     boost::timer::nanosecond_type t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b),
        k_config(c),
        t_initial(t_ics),
        backg_size(bg_sz),
        tensor_size(ten_sz),
        twopf_size(tw_sz),
        threepf_size(th_sz),
        backg_start(bg_st),
        tensor_k1_start(ten_k1_st),
        tensor_k2_start(ten_k2_st),
        tensor_k3_start(ten_k3_st),
        twopf_re_k1_start(tw_re_k1_st),
        twopf_im_k1_start(tw_im_k1_st),
        twopf_re_k2_start(tw_re_k2_st),
        twopf_im_k2_start(tw_im_k2_st),
        twopf_re_k3_start(tw_re_k3_st),
        twopf_im_k3_start(tw_im_k3_st),
        threepf_start(th_st)
      {
        // compute rescaling factors to get correct dimensionless correlation functions
        double k1 = c->k1_comoving;
        double k2 = c->k2_comoving;
        double k3 = c->k3_comoving;
        double kt = c->kt_comoving;
        
        k1_rescale = (k1/kt)*(k1/kt)*(k1/kt);
        k2_rescale = (k2/kt)*(k2/kt)*(k2/kt);
        k3_rescale = (k3/kt)*(k3/kt)*(k3/kt);
        
        shape_rescale = (k1/kt)*(k1/kt) * (k2/kt)*(k2/kt) * (k3/kt)*(k3/kt);
      }


    template <typename number>
    template <typename State>
    void threepf_singleconfig_batch_observer<number>::push(const State& x)
      {
        if(this->store_time_step())
          {
            // the integrator makes each correlation function dimensionless by rescaling by a power of k_t
            // we want proper dimensionless correlation functions, so need to rescale
            
            std::vector<number> bg_x(this->backg_size);
            for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[this->backg_start + i];

            std::vector<number> tensor_tpf_x1(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x1[i] = this->k1_rescale * x[this->tensor_k1_start + i];

            std::vector<number> tpf_x1_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_re[i] = this->k1_rescale * x[this->twopf_re_k1_start + i];
            std::vector<number> tpf_x1_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_im[i] = this->k1_rescale * x[this->twopf_im_k1_start + i];

            std::vector<number> tensor_tpf_x2(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x2[i] = this->k2_rescale * x[this->tensor_k2_start + i];

            std::vector<number> tpf_x2_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_re[i] = this->k2_rescale * x[this->twopf_re_k2_start + i];
            std::vector<number> tpf_x2_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_im[i] = this->k2_rescale * x[this->twopf_im_k2_start + i];

            std::vector<number> tensor_tpf_x3(this->tensor_size);
            for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x3[i] = this->k3_rescale * x[this->tensor_k3_start + i];

            std::vector<number> tpf_x3_re(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_re[i] = this->k3_rescale * x[this->twopf_re_k3_start + i];
            std::vector<number> tpf_x3_im(this->twopf_size);
            for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_im[i] = this->k3_rescale * x[this->twopf_im_k3_start + i];

            std::vector<number> thpf_x(this->threepf_size);
            for(unsigned int i = 0; i < this->threepf_size; ++i) thpf_x[i] = this->shape_rescale * x[this->threepf_start + i];

            if(this->k_config.is_background_stored())
              {
                this->batcher.push_backg(this->store_serial_number(), this->k_config->serial, bg_x);
              }

            if(this->k_config.is_twopf_k1_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tensor_tpf_x1);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tpf_x1_re, bg_x, twopf_type::real);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k1_serial, this->k_config->serial, tpf_x1_im, bg_x, twopf_type::imag);
              }

            if(this->k_config.is_twopf_k2_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tensor_tpf_x2);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tpf_x2_re, bg_x, twopf_type::real);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k2_serial, this->k_config->serial, tpf_x2_im, bg_x, twopf_type::imag);
              }

            if(this->k_config.is_twopf_k3_stored())
              {
                this->batcher.push_tensor_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tensor_tpf_x3);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tpf_x3_re, bg_x, twopf_type::real);
                this->batcher.push_twopf(this->store_serial_number(), this->k_config->k3_serial, this->k_config->serial, tpf_x3_im, bg_x, twopf_type::imag);
              }

            this->batcher.push_threepf(this->store_serial_number(), this->store_time(), *this->k_config, this->k_config->serial,
                                       thpf_x, tpf_x1_re, tpf_x1_im, tpf_x2_re, tpf_x2_im, tpf_x3_re, tpf_x3_im, bg_x);
          }

        this->step();
      }


    template <typename number>
    void threepf_singleconfig_batch_observer<number>::stop_timers(size_t steps, unsigned int refinement)
      {
        this->timing_observer<number>::stop_timers(steps, refinement);
        this->batcher.report_integration_success(this->get_integration_time(), this->get_batching_time(), this->k_config->serial, steps, refinement);

        std::ostringstream init_time;
        init_time << std::scientific << std::setprecision(this->precision) << this->t_initial;

        std::ostringstream exit_time;
        exit_time << std::scientific << std::setprecision(this->precision) << this->k_config->t_exit;

        std::ostringstream massless_time;
        massless_time << std::scientific << std::setprecision(this->precision) << this->k_config->t_massless;

        std::ostringstream massless_efolds;
        massless_efolds << std::scientific << std::setprecision(this->precision) << (this->k_config->t_massless - this->t_initial);

        std::ostringstream subh_efolds;
        subh_efolds << std::scientific << std::setprecision(this->precision) << (this->k_config->t_exit - this->t_initial);

        BOOST_LOG_SEV(this->batcher.get_log(), generic_batcher::log_severity_level::normal)
	        << "** " << CPPTRANSPORT_SOLVING_CONFIG << " " << this->k_config->serial << ", "
          << CPPTRANSPORT_INTEGRATION_TIME << " = " << format_time(this->get_integration_time()) << ", "
          << CPPTRANSPORT_INITIAL_TIME << " = " << init_time.str() << ", "
          << CPPTRANSPORT_EXIT_TIME << " " << CPPTRANSPORT_EXIT_TIME_KT << " = " << exit_time.str() << ", "
          << CPPTRANSPORT_MASSLESS_TIME << " = " << massless_time.str() << ", "
          << CPPTRANSPORT_SUBHORIZON_EFOLDS << " = " << subh_efolds.str() << ", "
          << CPPTRANSPORT_MASSLESS_EFOLDS << " = " << massless_efolds.str();
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
                                         boost::timer::nanosecond_type t_int = CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY,
                                         bool s = false, unsigned int p = 3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        generic_batcher::logger& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const work_queue<twopf_kconfig_record>::device_work_list& work_list;

        twopf_batcher<number>& batcher;

        const unsigned int backg_size;
        const unsigned int tensor_size;
        const unsigned int twopf_size;

        const unsigned int backg_start;
        const unsigned int tensor_start;
        const unsigned int twopf_start;

      };


    template <typename number>
    twopf_groupconfig_batch_observer<number>::twopf_groupconfig_batch_observer(twopf_batcher<number>& b,
                                                                               const work_queue<twopf_kconfig_record>::device_work_list& c,
                                                                               const time_config_database& t,
                                                                               unsigned int bg_sz, unsigned int ten_sz, unsigned int tw_sz,
                                                                               unsigned int bg_st, unsigned int ten_st, unsigned int tw_st,
                                                                               boost::timer::nanosecond_type t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b),
        work_list(c),
        backg_size(bg_sz),
        tensor_size(ten_sz),
        twopf_size(tw_sz),
        backg_start(bg_st),
        tensor_start(ten_st),
        twopf_start(tw_st)
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
                // correlation functions are already dimensionless, so no rescaling needed
                
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
                                           boost::timer::nanosecond_type t_int=CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY,
                                           bool s=false, unsigned int p=3);


        // INTERFACE

      public:

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x);

        //! Return logger
        generic_batcher::logger& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }


        // STOP TIMERS - OVERRIDES A 'timing observer' interface

      public:

        //! Stop timers and report timing details to the batcher
        virtual void stop_timers(size_t steps, unsigned int refinement) override;


        // INTERNAL DATA

      private:

        const work_queue<threepf_kconfig_record>::device_work_list& work_list;

        threepf_batcher<number>& batcher;

        const unsigned int backg_size;
        const unsigned int tensor_size;
        const unsigned int twopf_size;
        const unsigned int threepf_size;

        const unsigned int backg_start;
        const unsigned int tensor_k1_start;
        const unsigned int tensor_k2_start;
        const unsigned int tensor_k3_start;
        const unsigned int twopf_re_k1_start;
        const unsigned int twopf_im_k1_start;
        const unsigned int twopf_re_k2_start;
        const unsigned int twopf_im_k2_start;
        const unsigned int twopf_re_k3_start;
        const unsigned int twopf_im_k3_start;
        const unsigned int threepf_start;

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
                                                                                   boost::timer::nanosecond_type t_int, bool s, unsigned int p)
      : timing_observer<number>(t, t_int, s, p),
        batcher(b),
        work_list(c),
        backg_size(bg_sz),
        tensor_size(ten_sz),
        twopf_size(tw_sz),
        threepf_size(th_sz),
        backg_start(bg_st),
        tensor_k1_start(ten_k1_st),
        tensor_k2_start(ten_k2_st),
        tensor_k3_start(ten_k3_st),
        twopf_re_k1_start(tw_re_k1_st),
        twopf_im_k1_start(tw_im_k1_st),
        twopf_re_k2_start(tw_re_k2_st),
        twopf_im_k2_start(tw_im_k2_st),
        twopf_re_k3_start(tw_re_k3_st),
        twopf_im_k3_start(tw_im_k3_st),
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
                // compute rescaling factors to get correct dimensionless correlation functions
                double k1 = this->work_list[c]->k1_comoving;
                double k2 = this->work_list[c]->k2_comoving;
                double k3 = this->work_list[c]->k3_comoving;
                double kt = this->work_list[c]->kt_comoving;

                // the integrator makes each correlation function dimensionless by rescaling by a power of k_t
                // we want proper dimensionless correlation functions, so need to rescale
                double k1_rescale = (k1/kt)*(k1/kt)*(k1/kt);
                double k2_rescale = (k2/kt)*(k2/kt)*(k2/kt);
                double k3_rescale = (k3/kt)*(k3/kt)*(k3/kt);

                double shape_rescale = (k1/kt)*(k1/kt) * (k2/kt)*(k2/kt) * (k3/kt)*(k3/kt);

                std::vector<number> bg_x(this->backg_size);
                for(unsigned int i = 0; i < this->backg_size; ++i) bg_x[i] = x[(this->backg_start + i)*n + c];

                std::vector<number> tensor_tpf_x1(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x1[i] = k1_rescale * x[(this->tensor_k1_start + i)*n + c];

                std::vector<number> tpf_x1_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_re[i] = k1_rescale * x[(this->twopf_re_k1_start + i)*n + c];
                std::vector<number> tpf_x1_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x1_im[i] = k1_rescale * x[(this->twopf_im_k1_start + i)*n + c];

                std::vector<number> tensor_tpf_x2(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x2[i] = k2_rescale * x[(this->tensor_k2_start + i)*n + c];

                std::vector<number> tpf_x2_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_re[i] = k2_rescale * x[(this->twopf_re_k2_start + i)*n + c];
                std::vector<number> tpf_x2_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x2_im[i] = k2_rescale * x[(this->twopf_im_k2_start + i)*n + c];

                std::vector<number> tensor_tpf_x3(this->tensor_size);
                for(unsigned int i = 0; i < this->tensor_size; ++i) tensor_tpf_x3[i] = k3_rescale * x[(this->tensor_k3_start + i)*n + c];

                std::vector<number> tpf_x3_re(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_re[i] = k3_rescale * x[(this->twopf_re_k3_start + i)*n + c];
                std::vector<number> tpf_x3_im(this->twopf_size);
                for(unsigned int i = 0; i < this->twopf_size; ++i) tpf_x3_im[i] = k3_rescale * x[(this->twopf_im_k3_start + i)*n + c];

                std::vector<number> thpf_x(this->threepf_size);
                for(unsigned int i = 0; i < this->threepf_size; ++i) thpf_x[i] = shape_rescale * x[(this->threepf_start + i)*n + c];

                if(this->work_list[c].is_background_stored())
                  {
                    this->batcher.push_backg(this->store_serial_number(), this->work_list[c]->serial, bg_x);
                  }

                if(this->work_list[c].is_twopf_k1_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tensor_tpf_x1);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tpf_x1_re, bg_x, twopf_type::real);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k1_serial, this->work_list[c]->serial, tpf_x1_im, bg_x, twopf_type::imag);
                  }

                if(this->work_list[c].is_twopf_k2_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tensor_tpf_x2);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tpf_x2_re, bg_x, twopf_type::real);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k2_serial, this->work_list[c]->serial, tpf_x2_im, bg_x, twopf_type::imag);
                  }

                if(this->work_list[c].is_twopf_k3_stored())
                  {
                    this->batcher.push_tensor_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tensor_tpf_x3);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tpf_x3_re, bg_x, twopf_type::real);
                    this->batcher.push_twopf(this->store_serial_number(), this->work_list[c]->k3_serial, this->work_list[c]->serial, tpf_x3_im, bg_x, twopf_type::imag);
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


#endif //CPPTRANSPORT_OBSERVERS_H
