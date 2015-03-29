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
        typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(kconfig_sns);
        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
        const std::vector< twopf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            boost::timer::cpu_timer timer;

            zeta_twopf_time_data_tag<number> tag = pipe.new_zeta_twopf_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& twopf = z_handle.lookup_tag(tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_twopf(time_sns[j], kconfig_sns[i], twopf[j]);
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
        typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(kconfig_sns);
        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
        const std::vector< threepf_configuration > k_values = kc_handle.lookup_tag(k_tag);

        for(unsigned int i = 0; i < list.size(); i++)
	        {
            boost::timer::cpu_timer timer;

            zeta_threepf_time_data_tag<number> tpf_tag = pipe.new_zeta_threepf_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& threepf = z_handle.lookup_tag(tpf_tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_threepf(time_sns[j], kconfig_sns[i], threepf[j]);
	            }

            zeta_reduced_bispectrum_time_data_tag<number> rbs_tag = pipe.new_zeta_reduced_bispectrum_time_data_tag(k_values[i]);
            // safe to use a reference here to avoid a copy
            const std::vector<number>& redbsp = z_handle.lookup_tag(rbs_tag);

            for(unsigned int j = 0; j < time_sns.size(); j++)
	            {
                batcher.push_reduced_bispectrum(time_sns[j], kconfig_sns[i], redbsp[j]);
	            }

            if(list[i].store_twopf_k1)
	            {
                twopf_configuration k1;

                k1.serial         = k_values[i].k1_serial;
                k1.k_comoving     = k_values[i].k1_comoving;
                k1.k_conventional = k_values[i].k1_conventional;

                zeta_twopf_time_data_tag<number> k1_tag = pipe.new_zeta_twopf_time_data_tag(k1);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k1_twopf = z_handle.lookup_tag(k1_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k1.serial, k1_twopf[j]);
	                }
	            }

            if(list[i].store_twopf_k2)
	            {
                twopf_configuration k2;

                k2.serial         = k_values[i].k2_serial;
                k2.k_comoving     = k_values[i].k2_comoving;
                k2.k_conventional = k_values[i].k2_conventional;

                zeta_twopf_time_data_tag<number> k2_tag = pipe.new_zeta_twopf_time_data_tag(k2);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k2_twopf = z_handle.lookup_tag(k2_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k2.serial, k2_twopf[j]);
	                }
	            }

            if(list[i].store_twopf_k3)
	            {
                twopf_configuration k3;

                k3.serial         = k_values[i].k3_serial;
                k3.k_comoving     = k_values[i].k3_comoving;
                k3.k_conventional = k_values[i].k3_conventional;

                zeta_twopf_time_data_tag<number> k3_tag = pipe.new_zeta_twopf_time_data_tag(k3);
                // safe to use a reference here to avoid a copy
                const std::vector<number>& k3_twopf = z_handle.lookup_tag(k3_tag);

                for(unsigned int j = 0; j < time_sns.size(); j++)
	                {
                    batcher.push_twopf(time_sns[j], k3.serial, k3_twopf[j]);
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
        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(time_sns);

        BT_time_data_tag<number> BT_tag = pipe.new_BT_time_data_tag(tk->get_template(), kconfig_sns);
        TT_time_data_tag<number> TT_tag = pipe.new_TT_time_data_tag(tk->get_template(), kconfig_sns);

        const std::vector<number> BT = z_handle.lookup_tag(BT_tag);
        // safe to use a reference here to avoid a copy
        const std::vector<number>& TT = z_handle.lookup_tag(TT_tag);

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
