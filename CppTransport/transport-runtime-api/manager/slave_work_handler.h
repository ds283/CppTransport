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
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h"
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h"

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
        void postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig_record>& work,
                                     zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: zeta threepf task
        void postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig_record>& work,
                                     zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: fNL task
        void postintegration_handler(fNL_task<number>* tk, zeta_threepf_task<number>* ptk, work_queue<threepf_kconfig_record>& work,
                                     fNL_batcher<number>& batcher, datapipe<number>& pipe);


		    // PRIVATE DATA

      protected:

        //! compute delegate - zeta products
        derived_data::zeta_timeseries_compute<number> zeta_computer;

        //! compute delegate - fNL products
        derived_data::fNL_timeseries_compute<number> fNL_computer;

	    };


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, work_queue<twopf_kconfig_record>& work,
                                                             zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<twopf_kconfig_record>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<twopf_kconfig_record>::device_work_list list = queues[0];

		    // set up query representing time values at which to sample; we just want all of them,
		    // so we need a condition which always evaluates to TRUE.
		    // SQL has no boolean literals, so use 1=1 instead
		    const derived_data::SQL_time_config_query tquery("1=1");

        // pull time configuration information from the database
        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(tquery);
        time_config_tag<number>                        tc_tag      = pipe.new_time_config_tag();
        const std::vector<time_config>                 time_values = tc_handle.lookup_tag(tc_tag);

		    // get list of k-configurations
		    const twopf_kconfig_database& twopf_db = ptk->get_twopf_database();

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, tquery, ptk->get_model()->get_N_fields());

        // buffer for computed values
        std::vector<number> sample;

        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

		        // compute zeta twopf
            this->zeta_computer.twopf(handle, sample, *(list[i]));
						assert(sample.size() == time_values.size());

		        for(unsigned int j = 0; j < time_values.size(); ++j)
			        {
                batcher.push_twopf(time_values[j].serial, list[i]->serial, sample[j]);
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, work_queue<threepf_kconfig_record>& work,
                                                             zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig_record>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig_record>::device_work_list list = queues[0];

        // set up query representing time values at which to sample
        const derived_data::SQL_time_config_query tquery("1=1");

        // pull time configuration information from the database
        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(tquery);
        time_config_tag<number>                        tc_tag      = pipe.new_time_config_tag();
        const std::vector<time_config>                 time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        std::shared_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, tquery, ptk->get_model()->get_N_fields());

		    // buffer for computed values
        std::vector<number> sample;

        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

            this->zeta_computer.threepf(handle, sample, *(list[i]));
		        assert(sample.size() == time_values.size());

            for(unsigned int j = 0; j < time_values.size(); ++j)
	            {
                batcher.push_threepf(time_values[j].serial, list[i]->serial, sample[j]);
	            }

            this->zeta_computer.reduced_bispectrum(handle, sample, *(list[i]));

            for(unsigned int j = 0; j < time_values.size(); ++j)
	            {
                batcher.push_reduced_bispectrum(time_values[j].serial, list[i]->serial, sample[j]);
	            }

            if(list[i].is_twopf_k1_stored())
	            {
                twopf_kconfig k1;

                k1.serial         = list[i]->k1_serial;
                k1.k_comoving     = list[i]->k1_comoving;
                k1.k_conventional = list[i]->k1_conventional;

		            this->zeta_computer.twopf(handle, sample, k1);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k1.serial, sample[j]);
	                }
	            }

            if(list[i].is_twopf_k2_stored())
	            {
                twopf_kconfig k2;

                k2.serial         = list[i]->k2_serial;
                k2.k_comoving     = list[i]->k2_comoving;
                k2.k_conventional = list[i]->k2_conventional;

                this->zeta_computer.twopf(handle, sample, k2);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k2.serial, sample[j]);
	                }
	            }

            if(list[i].is_twopf_k3_stored())
	            {
                twopf_kconfig k3;

                k3.serial         = list[i]->k3_serial;
                k3.k_comoving     = list[i]->k3_comoving;
                k3.k_conventional = list[i]->k3_conventional;

                this->zeta_computer.twopf(handle, sample, k3);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k3.serial, sample[j]);
	                }
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(fNL_task<number>* tk, zeta_threepf_task<number>* ptk, work_queue<threepf_kconfig_record>& work,
                                                             fNL_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const typename work_queue<threepf_kconfig_record>::device_queue queues = work[0];
        assert(queues.size() == 1);

        const typename work_queue<threepf_kconfig_record>::device_work_list list = queues[0];

        boost::timer::cpu_timer timer;

        // set up query representing time values at which to sample
        const derived_data::SQL_time_config_query tquery("1=1");

        // pull time configuration information from the database
        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(tquery);
		    time_config_tag<number>                        tc_tag      = pipe.new_time_config_tag();
        const std::vector<time_config>                 time_values = tc_handle.lookup_tag(tc_tag);

		    // buffer for computed values
        std::vector<number> BT;
        std::vector<number> TT;

		    // set up handle for compute delegate
        std::shared_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->fNL_computer.make_handle(pipe, ptk, tquery, tk->get_template(), list);

        this->fNL_computer.BT(handle, BT);
		    this->fNL_computer.TT(handle, TT);
        for(unsigned int j = 0; j < time_values.size(); ++j)
	        {
            batcher.push_fNL(time_values[j].serial, BT[j], TT[j]);
	        }

        timer.stop();
        batcher.report_finished_item(timer.elapsed().wall);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "-- Processed " << list.size() << " k-configurations in time = " << format_time(timer.elapsed().wall);
	    }

	}   // namespace transport


#endif //__slave_work_handler_H_
