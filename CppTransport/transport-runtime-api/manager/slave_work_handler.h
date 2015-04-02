//
// Created by David Seery on 24/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __slave_work_handler_H_
#define __slave_work_handler_H_

#include <list>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/data/data_manager.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_timeseries_compute.h"
#include "transport-runtime-api/derived-products/derived-content/fNL_timeseries_compute.h"

#include "boost/timer/timer.hpp"


namespace transport
	{

		template <typename number>
    class slave_work_handler
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    slave_work_handler() = default;

		    ~slave_work_handler() = default;


		    // INTERFACE

      public:

        //! Handler: zeta twopf task
        void postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
                                     zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: zeta threepf task
        void postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                     zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: fNL task
        void postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                     fNL_batcher<number>& batcher, datapipe<number>& pipe);


		    // PRIVATE DATA

      protected:

        //! compute delegate - zeta products
        derived_data::zeta_timeseries_compute<number> zeta_computer;

        //! compute delegate - fNL products
        derived_data::fNL_timeseries_compute<number> fNL_computer;

	    };


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig>& work,
                                                             zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<twopf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<twopf_kconfig>::device_work_list list = queues[0];

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(time_sns);
        typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(kconfig_sns);
        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        time_config_tag<number> tc_tag = pipe.new_time_config_tag();
        const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
        const std::vector< twopf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, time_sns, time_values, ptk->get_model()->get_N_fields());

		    // buffer for computed values
        std::vector<number> sample;

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            boost::timer::cpu_timer timer;

            this->zeta_computer.twopf(handle, sample, k_values[i]);
            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_twopf(time_sns[j], kconfig_sns[i], sample[j]);
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed configuration " << list[i].serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                                             zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig>::device_work_list list = queues[0];

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(time_sns);
        typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(kconfig_sns);
        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        time_config_tag<number> tc_tag = pipe.new_time_config_tag();
        const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

        threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
        const std::vector< threepf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, time_sns, time_values, ptk->get_model()->get_N_fields());

		    // buffer for computed values
        std::vector<number> sample;

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            boost::timer::cpu_timer timer;

            this->zeta_computer.threepf(handle, sample, k_values[i]);
            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_threepf(time_sns[j], kconfig_sns[i], sample[j]);
	            }

            this->zeta_computer.reduced_bispectrum(handle, sample, k_values[i]);
            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_reduced_bispectrum(time_sns[j], kconfig_sns[i], sample[j]);
	            }

            if(list[i].store_twopf_k1)
	            {
                twopf_configuration k1;

                k1.serial         = k_values[i].k1_serial;
                k1.k_comoving     = k_values[i].k1_comoving;
                k1.k_conventional = k_values[i].k1_conventional;

		            this->zeta_computer.twopf(handle, sample, k1);
                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k1.serial, sample[j]);
	                }
	            }

            if(list[i].store_twopf_k2)
	            {
                twopf_configuration k2;

                k2.serial         = k_values[i].k2_serial;
                k2.k_comoving     = k_values[i].k2_comoving;
                k2.k_conventional = k_values[i].k2_conventional;

                this->zeta_computer.twopf(handle, sample, k2);
                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k2.serial, sample[j]);
	                }
	            }

            if(list[i].store_twopf_k3)
	            {
                twopf_configuration k3;

                k3.serial         = k_values[i].k3_serial;
                k3.k_comoving     = k_values[i].k3_comoving;
                k3.k_conventional = k_values[i].k3_conventional;

                this->zeta_computer.twopf(handle, sample, k3);
                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k3.serial, sample[j]);
	                }
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed configuration " << list[i].serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(fNL_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig>& work,
                                                             fNL_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig>::device_work_list list = queues[0];

        boost::timer::cpu_timer timer;

        // get list of time values at which to sample
        const std::vector<time_config> time_list = ptk->get_time_config_list();

        // reslice to get a vector of serial numbers
        std::vector<unsigned int> time_sns(time_list.size());
        for(unsigned int i = 0; i < time_list.size(); i++)
	        {
            time_sns[i] = time_list[i].serial;
	        }

        // get list of kserial numbers
        std::vector<unsigned int> kconfig_sns(list.size());
        for(unsigned int i = 0; i < list.size(); i++)
	        {
            kconfig_sns[i] = list[i].serial;
	        }

        // set up cache handles
        // look up time values corresponding to these serial numbers
        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(time_sns);

        time_config_tag<number> tc_tag = pipe.new_time_config_tag();
        const std::vector<double> time_values = tc_handle.lookup_tag(tc_tag);

		    // buffer for computed values
        std::vector<number> BT;
        std::vector<number> TT;

		    // set up handle for compute delegate
        std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->fNL_computer.make_handle(pipe, ptk, time_sns, time_values, ptk->get_model()->get_N_fields(), tk->get_template(), kconfig_sns);

        this->fNL_computer.BT(handle, BT);
		    this->fNL_computer.TT(handle, TT);
        for(unsigned int j = 0; j < time_sns.size(); j++)
	        {
            batcher.push_fNL(time_sns[j], BT[j], TT[j]);
	        }

        timer.stop();
        batcher.report_finished_item(timer.elapsed().wall);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed " << list.size() << " k-configurations in time = " << format_time(timer.elapsed().wall);
	    }

	}   // namespace transport


#endif //__slave_work_handler_H_
