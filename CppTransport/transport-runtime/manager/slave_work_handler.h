//
// Created by David Seery on 24/03/15.
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


#ifndef CPPTRANSPORT_SLAVE_WORK_HANDLER_H
#define CPPTRANSPORT_SLAVE_WORK_HANDLER_H

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
        void postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, device_queue_manager<twopf_kconfig_record>& work,
                                     zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: zeta threepf task
        void postintegration_handler(zeta_threepf_task<number>* tk, threepf_task<number>* ptk, device_queue_manager<threepf_kconfig_record>& work,
                                     zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe);

        //! Handler: fNL task
        void postintegration_handler(fNL_task<number>* tk, zeta_threepf_task<number>* ptk, device_queue_manager<threepf_kconfig_record>& work,
                                     fNL_batcher<number>& batcher, datapipe<number>& pipe);


		    // PRIVATE DATA

      protected:

        //! compute delegate - zeta products
        derived_data::zeta_timeseries_compute<number> zeta_computer;

        //! compute delegate - fNL products
        derived_data::fNL_timeseries_compute<number> fNL_computer;

	    };


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(zeta_twopf_task<number>* tk, twopf_task<number>* ptk, device_queue_manager<twopf_kconfig_record>& work,
                                                             zeta_twopf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const auto& queues = work[0];
        assert(queues.size() == 1);

        const auto& list = queues[0];

        // check that the datapipe is attached to an integration content group
        if(!pipe.is_integration_attached())
          throw runtime_exception(exception_type::BACKEND_ERROR, CPPTRANSPORT_SLAVE_NOT_INTEGRATION_CONTENT);

		    // set up query representing time values at which to sample; we just want all of them,
		    // so we need a condition which always evaluates to TRUE.
		    // SQL has no boolean literals, so use 1=1 instead
		    const derived_data::SQL_time_query tquery("1=1");

        // pull time configuration information from the database
        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(tquery);
        time_config_tag<number>                        tc_tag      = pipe.new_time_config_tag();
        const std::vector<time_config>                 time_values = tc_handle.lookup_tag(tc_tag);

		    // get list of k-configurations
		    const twopf_kconfig_database& twopf_db = ptk->get_twopf_database();

        // set up handle for compute delegate
        auto handle = this->zeta_computer.make_handle(pipe, ptk, tquery, ptk->get_model()->get_N_fields());

        // buffer for computed values
        std::vector<number> zeta_tpf;
        std::vector<number> zeta_ns;
        std::vector< std::vector<number> > gauge_xfm1;

        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

		        // compute zeta twopf; if no spectral data is available, the zeta_ns output will have no meaning
            this->zeta_computer.twopf(*handle, zeta_tpf, zeta_ns, gauge_xfm1, *(list[i]));
						assert(zeta_tpf.size() == time_values.size());

		        for(unsigned int j = 0; j < time_values.size(); ++j)
			        {
                batcher.push_twopf(time_values[j].serial, list[i]->serial, zeta_tpf[j], zeta_ns[j]);
                batcher.push_gauge_xfm1(time_values[j].serial, list[i]->serial, gauge_xfm1[j]);
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler
      (zeta_threepf_task<number>* tk, threepf_task<number>* ptk, device_queue_manager<threepf_kconfig_record>& work,
       zeta_threepf_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        // extract work queues
        const auto& queues = work[0];
        assert(queues.size() == 1);

        // extract work list from queue
        const auto& list = queues[0];

        // check that the datapipe is attached to an integration content group
        if(!pipe.is_integration_attached())
          throw runtime_exception(exception_type::BACKEND_ERROR, CPPTRANSPORT_SLAVE_NOT_INTEGRATION_CONTENT);

        // set up query representing time values at which to sample
        const derived_data::SQL_time_query tquery("1=1");

        // pull time configuration information from the database
        auto& tc_handle   = pipe.new_time_config_handle(tquery);
        auto  tc_tag      = pipe.new_time_config_tag();
        auto  time_values = tc_handle.lookup_tag(tc_tag);

        // set up handle for compute delegate
        unsigned int N_fields = ptk->get_model()->get_N_fields();
        auto handle = this->zeta_computer.make_handle(pipe, ptk, tquery, N_fields);

		    // set up reusable buffers to hold computed values, and intermediate working products
		    // first, decide what size buffers are required
        const auto time_size = time_values.size();
        const auto gauge_xfm1_size = 2*N_fields;
        const auto gauge_xfm2_size = 2*N_fields * 2*N_fields;

        // second, allocate storage
        std::vector<number> zeta_npf(time_size);
        std::vector<number> zeta_ns(time_size);
        std::vector<number> redbsp(time_size);
        std::vector< std::vector<number> > gauge_xfm1(time_size);
        std::vector< std::vector<number> > gauge_xfm2_123(time_size);
        std::vector< std::vector<number> > gauge_xfm2_213(time_size);
        std::vector< std::vector<number> > gauge_xfm2_312(time_size);

        // size gauge xfm caches appropriately
        for(unsigned int j = 0; j < time_size; ++j)
          {
            gauge_xfm1[j].resize(gauge_xfm1_size);
            gauge_xfm2_123[j].resize(gauge_xfm2_size);
            gauge_xfm2_213[j].resize(gauge_xfm2_size);
            gauge_xfm2_312[j].resize(gauge_xfm2_size);
          }

        // step through required k-configurations, evaluating the zeta 2pf, 3pf and spectral index at each
        // time sample point
        for(unsigned int i = 0; i < list.size(); ++i)
	        {
            boost::timer::cpu_timer timer;

            // compute 3pf and reduced bispectrum
            // The buffers zeta_npf, redbsp and gauge_* are re-used each time
            this->zeta_computer.threepf(*handle, zeta_npf, redbsp, gauge_xfm2_123, gauge_xfm2_213, gauge_xfm2_312, *(list[i]));
		        assert(zeta_npf.size() == time_size);
		        assert(redbsp.size() == time_size);
		        assert(gauge_xfm1.size() == time_size);
		        assert(gauge_xfm2_123.size() == time_size);
		        assert(gauge_xfm2_213.size() == time_size);
		        assert(gauge_xfm2_312.size() == time_size);

		        // push computed values into the batcher
            for(unsigned int j = 0; j < time_size; ++j)
	            {
                batcher.push_threepf(time_values[j].serial, list[i]->serial, zeta_npf[j], redbsp[j]);

                assert(gauge_xfm2_123[j].size() == gauge_xfm2_size);
                assert(gauge_xfm2_213[j].size() == gauge_xfm2_size);
                assert(gauge_xfm2_312[j].size() == gauge_xfm2_size);
                batcher.push_gauge_xfm2_123(time_values[j].serial, list[i]->serial, gauge_xfm2_123[j]);
                batcher.push_gauge_xfm2_213(time_values[j].serial, list[i]->serial, gauge_xfm2_213[j]);
                batcher.push_gauge_xfm2_312(time_values[j].serial, list[i]->serial, gauge_xfm2_312[j]);
	            }

            // if 2pf for configuration k1 is being stores, compute it and push to batcher
            if(list[i].is_twopf_k1_stored())
	            {
                twopf_kconfig k1;

                k1.serial         = list[i]->k1_serial;
                k1.k_comoving     = list[i]->k1_comoving;
                k1.k_conventional = list[i]->k1_conventional;

		            this->zeta_computer.twopf(*handle, zeta_npf, zeta_ns, gauge_xfm1, k1);
                for(unsigned int j = 0; j < time_size; ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k1.serial, zeta_npf[j], zeta_ns[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k1.serial, gauge_xfm1[j]);
	                }
	            }

            // if 2pf for configuration k2 is being stores, compute it and push to batcher
            if(list[i].is_twopf_k2_stored())
	            {
                twopf_kconfig k2;

                k2.serial         = list[i]->k2_serial;
                k2.k_comoving     = list[i]->k2_comoving;
                k2.k_conventional = list[i]->k2_conventional;

                this->zeta_computer.twopf(*handle, zeta_npf, zeta_ns, gauge_xfm1, k2);
                for(unsigned int j = 0; j < time_size; ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k2.serial, zeta_npf[j], zeta_ns[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k2.serial, gauge_xfm1[j]);
	                }
	            }

            // if 2pf for configuration k3 is being stores, compute it and push to batcher
            if(list[i].is_twopf_k3_stored())
	            {
                twopf_kconfig k3;

                k3.serial         = list[i]->k3_serial;
                k3.k_comoving     = list[i]->k3_comoving;
                k3.k_conventional = list[i]->k3_conventional;

                this->zeta_computer.twopf(*handle, zeta_npf, zeta_ns, gauge_xfm1, k3);
                for(unsigned int j = 0; j < time_size; ++j)
	                {
                    batcher.push_twopf(time_values[j].serial, k3.serial, zeta_npf[j], zeta_ns[j]);
                    batcher.push_gauge_xfm1(time_values[j].serial, k3.serial, gauge_xfm1[j]);
	                }
	            }

            timer.stop();
            batcher.report_finished_item(timer.elapsed().wall);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "-- Processed configuration " << list[i]->serial << " (" << i+1
		            << " of " << list.size() << "), processing time = " << format_time(timer.elapsed().wall);
	        }
	    }


    template <typename number>
    void slave_work_handler<number>::postintegration_handler(fNL_task<number>* tk, zeta_threepf_task<number>* ptk, device_queue_manager<threepf_kconfig_record>& work,
                                                             fNL_batcher<number>& batcher, datapipe<number>& pipe)
	    {
        assert(tk != nullptr);
        assert(ptk != nullptr);

        const auto& queues = work[0];
        assert(queues.size() == 1);

        const auto& list = queues[0];

        // check that the datapipe is attached to an postintegration content group
        if(!pipe.is_postintegration_attached())
          throw runtime_exception(exception_type::BACKEND_ERROR, CPPTRANSPORT_SLAVE_NOT_POSTINTEGRATION_CONTENT);

        boost::timer::cpu_timer timer;

        // set up query representing time values at which to sample
        const derived_data::SQL_time_query tquery("1=1");

        // pull time configuration information from the database
        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(tquery);
		    time_config_tag<number>                        tc_tag      = pipe.new_time_config_tag();
        const std::vector<time_config>                 time_values = tc_handle.lookup_tag(tc_tag);

		    // buffer for computed values
        std::vector<number> BB;
        std::vector<number> BT;
        std::vector<number> TT;

		    // set up handle for compute delegate
        auto handle = this->fNL_computer.make_handle(pipe, ptk, tquery, tk->get_template(), list);

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


#endif //CPPTRANSPORT_SLAVE_WORK_HANDLER_H
