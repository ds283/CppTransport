//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __filter_H_
#define __filter_H_


#include <vector>

#include "transport-runtime-api/tasks/task_configurations.h"


namespace transport
	{

		namespace derived_data
			{

		    //! Filters for time and k-configurations
		    class filter
			    {

		      public:

		        class time_filter_data
			        {
		          public:
				        unsigned int serial;
		            bool max;
		            bool min;
		            double time;
			        };

		        class twopf_kconfig_filter_data
			        {
		          public:
				        unsigned int serial;
		            bool max;
		            bool min;
		            double k;
			        };

		        class threepf_kconfig_filter_data
			        {
		          public:
				        unsigned int serial;
		            double kt;
		            double alpha;
		            double beta;
		            double k1;
		            double k2;
		            double k3;
		            bool kt_max;
		            bool kt_min;
		            bool alpha_max;
		            bool alpha_min;
		            bool beta_max;
		            bool beta_min;
		            bool k1_max;
		            bool k1_min;
		            bool k2_max;
		            bool k2_min;
		            bool k3_max;
		            bool k3_min;
			        };

		      public:

		        typedef std::function<bool(const time_filter_data&)> time_filter;

		        typedef std::function<bool(const twopf_kconfig_filter_data&)> twopf_kconfig_filter;

		        typedef std::function<bool(const threepf_kconfig_filter_data&)> threepf_kconfig_filter;


		        // FILTERS

		      public:

		        //! time filter: filter out the serial numbers we want from a list
		        void filter_time_sample(time_filter t_filter, const std::vector<time_config>& t_samples,
                                    std::vector<unsigned int>& t_serials) const;

		        //! 2pf k-configuration filter
		        void filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const std::vector<twopf_kconfig>& k_samples,
		                                         std::vector<unsigned int>& k_serials) const;

		        //! 3pf k-configuration filter
		        void filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const std::vector<threepf_kconfig>& k_samples,
		                                           std::vector<unsigned int>& k_serials) const;

			    };


		    void filter::filter_time_sample(time_filter t_filter, const std::vector<time_config>& t_samples, std::vector<unsigned int>& t_serials) const
			    {
		        int min_pos = -1;
		        double t_min = 0.0;
		        int max_pos = -1;
		        double t_max = 0.0;

		        // scan through to find min and max values of time
            for(unsigned int i = 0; i < t_samples.size(); i++)
              {
		            if(min_pos == -1 || t_samples[i].t < t_min) { t_min = t_samples[i].t; min_pos = i; }
		            if(max_pos == -1 || t_samples[i].t > t_max) { t_max = t_samples[i].t; max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        t_serials.clear();
		        for(unsigned int i = 0; i < t_samples.size(); i++)
			        {
		            time_filter_data data;

				        data.serial = t_samples[i].serial;

		            data.max = (i == max_pos ? true : false);
		            data.min = (i == min_pos ? true : false);

		            data.time = t_samples[i].t;

		            if(t_filter(data)) t_serials.push_back(t_samples[i].serial);
			        }

            if(t_serials.size() == 0) throw runtime_exception(runtime_exception::FILTER_EMPTY, "");
			    }


		    void filter::filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const std::vector<twopf_kconfig>& k_samples,
		                                             std::vector<unsigned int>& k_serials) const
			    {
		        int min_pos = -1;
		        double k_min = 0.0;
		        int max_pos = -1;
		        double k_max = 0.0;

		        // scan through to find min and max values of time
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            if(min_pos == -1 || k_samples[i].k_conventional < k_min) { k_min = k_samples[i].k_conventional; min_pos = i; }
		            if(max_pos == -1 || k_samples[i].k_conventional > k_max) { k_max = k_samples[i].k_conventional; max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            twopf_kconfig_filter_data data;

				        data.serial = i;

		            data.max = (i == max_pos ? true : false);
		            data.min = (i == min_pos ? true : false);
		            data.k = k_samples[i].k_conventional;

		            if(k_filter(data)) k_serials.push_back(k_samples[i].serial);
			        }

            if(k_serials.size() == 0) throw runtime_exception(runtime_exception::FILTER_EMPTY, "");
			    }


		    void filter::filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const std::vector<threepf_kconfig>& k_samples,
		                                               std::vector<unsigned int>& k_serials) const
			    {
		        int kt_min_pos = -1;
		        double kt_min = 0.0;
		        int kt_max_pos = -1;
		        double kt_max = 0.0;

		        int alpha_min_pos = -1;
		        double alpha_min = 0.0;
		        int alpha_max_pos = -1;
		        double alpha_max = 0.0;

		        int beta_min_pos = -1;
		        double beta_min = 0.0;
		        int beta_max_pos = -1;
		        double beta_max = 0.0;

		        int k1_min_pos = -1;
		        double k1_min = 0.0;
		        int k1_max_pos = -1;
		        double k1_max = 0.0;

		        int k2_min_pos = -1;
		        double k2_min = 0.0;
		        int k2_max_pos = -1;
		        double k2_max = 0.0;

		        int k3_min_pos = -1;
		        double k3_min = 0.0;
		        int k3_max_pos = -1;
		        double k3_max = 0.0;

		        // scan through to find min and max values of time
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            if(kt_min_pos == -1 || k_samples[i].k_t_conventional < kt_min) { kt_min = k_samples[i].k_t_conventional; kt_min_pos = i; }
		            if(kt_max_pos == -1 || k_samples[i].k_t_conventional > kt_max) { kt_max = k_samples[i].k_t_conventional; kt_max_pos = i; }

		            if(alpha_min_pos == -1 || k_samples[i].alpha < alpha_min) { alpha_min = k_samples[i].alpha; alpha_min_pos = i; }
		            if(alpha_max_pos == -1 || k_samples[i].alpha > alpha_max) { alpha_max = k_samples[i].alpha; alpha_max_pos = i; }

		            if(beta_min_pos == -1 || k_samples[i].beta < beta_min) { beta_min = k_samples[i].beta; beta_min_pos = i; }
		            if(beta_max_pos == -1 || k_samples[i].beta > beta_max) { beta_max = k_samples[i].beta; beta_max_pos = i; }

		            if(k1_min_pos == -1 || k_samples[i].k1 < k1_min) { k1_min = k_samples[i].k1; k1_min_pos = i; }
		            if(k1_max_pos == -1 || k_samples[i].k1 > k1_max) { k1_max = k_samples[i].k1; k1_max_pos = i; }

		            if(k2_min_pos == -1 || k_samples[i].k2 < k2_min) { k2_min = k_samples[i].k2; k2_min_pos = i; }
		            if(k2_max_pos == -1 || k_samples[i].k2 > k2_max) { k2_max = k_samples[i].k2; k2_max_pos = i; }

		            if(k3_min_pos == -1 || k_samples[i].k3 < k3_min) { k3_min = k_samples[i].k3; k3_min_pos = i; }
		            if(k3_max_pos == -1 || k_samples[i].k3 > k3_max) { k3_max = k_samples[i].k3; k3_max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            threepf_kconfig_filter_data data;

				        data.serial = i;

		            data.kt_max = (i == kt_max_pos ? true : false);
		            data.kt_min = (i == kt_min_pos ? true : false);
		            data.kt = k_samples[i].k_t_conventional;

		            data.alpha_max = (i == alpha_max_pos ? true : false);
		            data.alpha_min = (i == alpha_min_pos ? true : false);
		            data.alpha = k_samples[i].alpha;

		            data.beta_max = (i == beta_max_pos ? true : false);
		            data.beta_min = (i == beta_min_pos ? true : false);
		            data.beta = k_samples[i].beta;

		            data.k1_max = (i == k1_max_pos ? true : false);
		            data.k1_min = (i == k1_min_pos ? true : false);
		            data.k1 = k_samples[i].k1;

		            data.k2_max = (i == k2_max_pos ? true : false);
		            data.k2_min = (i == k2_min_pos ? true : false);
		            data.k2 = k_samples[i].k2;

		            data.k3_max = (i == k3_max_pos ? true : false);
		            data.k3_min = (i == k3_min_pos ? true : false);
		            data.k3 = k_samples[i].k3;

		            if(k_filter(data)) k_serials.push_back(k_samples[i].serial);
			        }

            if(k_serials.size() == 0) throw runtime_exception(runtime_exception::FILTER_EMPTY, "");
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__filter_H_
