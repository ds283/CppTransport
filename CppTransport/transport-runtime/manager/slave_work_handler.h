//
// Created by David Seery on 24/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __slave_work_handler_H_
#define __slave_work_handler_H_

#include <list>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime/models/model.h"
#include "transport-runtime/manager/model_manager.h"
#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/output_tasks.h"

#include "transport-runtime/data/data_manager.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h"

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
        std::unique_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, tquery, ptk->get_model()->get_N_fields());

        // buffer for computed values
        std::vector<number> zeta_npf;
        std::vector< std::vector<number> > gauge_xfm1;

        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

		        // compute zeta twopf
            this->zeta_computer.twopf(*handle, zeta_npf, gauge_xfm1, *(list[i]));
						assert(zeta_npf.size() == time_values.size());

		        for(unsigned int j = 0; j < time_values.size(); ++j)
			        {
                batcher.push_twopf(time_values[j].serial, list[i]->serial, zeta_npf[j]);
                batcher.push_gauge_xfm1(time_values[j].serial, list[i]->serial, gauge_xfm1[j]);
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
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
        unsigned int N_fields = ptk->get_model()->get_N_fields();
        std::unique_ptr<typename derived_data::zeta_timeseries_compute<number>::handle> handle = this->zeta_computer.make_handle(pipe, ptk, tquery, N_fields);

		    // buffer for computed values
        std::vector<number> zeta_npf;
        std::vector<number> redbsp;
        std::vector< std::vector<number> > gauge_xfm2_123(time_values.size());
        std::vector< std::vector<number> > gauge_xfm2_213(time_values.size());
        std::vector< std::vector<number> > gauge_xfm2_312(time_values.size());

        // size gauge xfm caches appropriately
        for(unsigned int j = 0; j < time_values.size(); ++j)
          {
            gauge_xfm2_123[j].resize(2*N_fields * 2*N_fields);
            gauge_xfm2_213[j].resize(2*N_fields * 2*N_fields);
            gauge_xfm2_312[j].resize(2*N_fields * 2*N_fields);
          }

        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

            this->zeta_computer.threepf(*handle, zeta_npf, redbsp, gauge_xfm2_123, gauge_xfm2_213, gauge_xfm2_312, *(list[i]));
		        assert(zeta_npf.size() == time_values.size());

            for(unsigned int j = 0; j < time_values.size(); ++j)
	            {
                batcher.push_threepf(time_values[j].serial, list[i]->serial, zeta_npf[j], redbsp[j]);
                batcher.push_gauge_xfm2_123(time_values[j].serial, list[i]->serial, gauge_xfm2_123[j]);
                batcher.push_gauge_xfm2_213(time_values[j].serial, list[i]->serial, gauge_xfm2_213[j]);
                batcher.push_gauge_xfm2_312(time_values[j].serial, list[i]->serial, gauge_xfm2_312[j]);
	            }

            if(list[i].is_twopf_k1_stored())
	            {
                twopf_kconfig k1;

                k1.serial         = list[i]->k1_serial;
                k1.k_comoving     = list[i]->k1_comoving;
                k1.k_conventional = list[i]->k1_conventional;

		            this->zeta_computer.twopf(*handle, zeta_npf, gauge_xfm2_123, k1);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k1.serial, zeta_npf[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k1.serial, gauge_xfm2_123[j]);
	                }
	            }

            if(list[i].is_twopf_k2_stored())
	            {
                twopf_kconfig k2;

                k2.serial         = list[i]->k2_serial;
                k2.k_comoving     = list[i]->k2_comoving;
                k2.k_conventional = list[i]->k2_conventional;

                this->zeta_computer.twopf(*handle, zeta_npf, gauge_xfm2_123, k2);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k2.serial, zeta_npf[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k2.serial, gauge_xfm2_123[j]);
	                }
	            }

            if(list[i].is_twopf_k3_stored())
	            {
                twopf_kconfig k3;

                k3.serial         = list[i]->k3_serial;
                k3.k_comoving     = list[i]->k3_comoving;
                k3.k_conventional = list[i]->k3_conventional;

                this->zeta_computer.twopf(*handle, zeta_npf, gauge_xfm2_123, k3);
                for(unsigned int j = 0; j < time_values.size(); ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k3.serial, zeta_npf[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k3.serial, gauge_xfm2_123[j]);
	                }
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
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
        std::vector<number> BB;
        std::vector<number> BT;
        std::vector<number> TT;

		    // set up handle for compute delegate
        std::unique_ptr<typename derived_data::fNL_timeseries_compute<number>::handle> handle = this->fNL_computer.make_handle(pipe, ptk, tquery, tk->get_template(), list);

		    this->fNL_computer.components(*handle, BB, BT, TT);
        assert(BB.size() == time_values.size());
        assert(BT.size() == time_values.size());
        assert(TT.size() == time_values.size());

        for(unsigned int j = 0; j < time_values.size(); ++j)
	        {
            batcher.push_fNL(time_values[j].serial, BB[j], BT[j], TT[j]);
	        }

        timer.stop();
        batcher.report_finished_item(timer.elapsed().wall);
        batcher.set_items_processed(list.size());   // mark number of k-configurations processed, so in-flight items are correctly accounted for

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Processed " << list.size() << " k-configurations in time = " << format_time(timer.elapsed().wall);
	    }

	}   // namespace transport


#endif //__slave_work_handler_H_
