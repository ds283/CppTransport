//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __filter_H_
#define __filter_H_


#include <vector>
#include <limits>

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


            // CONSTRUCTOR, DESTRUCTOR

          public:

            filter(double tol= __CPP_TRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION);

            ~filter() = default;


		        // FILTERS

		      public:

		        //! time filter: filter out the serial numbers we want from a list
		        void filter_time_sample(time_filter t_filter, const std::vector<time_config>& t_samples,
                                    std::vector<unsigned int>& t_serials) const;

		        //! 2pf k-configuration filter
		        void filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const twopf_kconfig_database& twopf_db,
                                             std::vector<unsigned int>& k_serials) const;

		        //! 3pf k-configuration filter
		        void filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const threepf_kconfig_database& threepf_db,
                                               std::vector<unsigned int>& k_serials) const;


            // INTERNAL DATA

          private:

            double tolerance;

			    };


        filter::filter(double tol)
          : tolerance(fabs(tol))
          {
          }


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


		    void filter::filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const twopf_kconfig_database& twopf_db,
                                                 std::vector<unsigned int>& k_serials) const
			    {
		        double k_min = std::numeric_limits<double>::max();
		        double k_max = -std::numeric_limits<double>::max();

		        // scan through to find min and max values of time
            for(twopf_kconfig_database::const_config_iterator t = twopf_db.config_begin(); t != twopf_db.config_end(); t++)
			        {
		            if(t->k_conventional < k_min) { k_min = t->k_conventional; }
		            if(t->k_conventional > k_max) { k_max = t->k_conventional; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
            for(twopf_kconfig_database::const_config_iterator t = twopf_db.config_begin(); t != twopf_db.config_end(); t++)
			        {
		            twopf_kconfig_filter_data data;

				        data.serial = t->serial;

		            data.max = fabs(t->k_conventional - k_max) < this->tolerance;
		            data.min = fabs(t->k_conventional - k_min) < this->tolerance;
		            data.k = t->k_conventional;

		            if(k_filter(data)) k_serials.push_back(t->serial);
			        }

            if(k_serials.size() == 0) throw runtime_exception(runtime_exception::FILTER_EMPTY, "");
			    }


		    void filter::filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const threepf_kconfig_database& threepf_db,
                                                   std::vector<unsigned int>& k_serials) const
			    {
		        double kt_min = std::numeric_limits<double>::max();
		        double kt_max = -std::numeric_limits<double>::max();

            double alpha_min = std::numeric_limits<double>::max();
		        double alpha_max = -std::numeric_limits<double>::max();

		        double beta_min = std::numeric_limits<double>::max();
		        double beta_max = -std::numeric_limits<double>::max();

		        double k1_min = std::numeric_limits<double>::max();
		        double k1_max = -std::numeric_limits<double>::max();

		        double k2_min = std::numeric_limits<double>::max();
		        double k2_max = -std::numeric_limits<double>::max();

		        double k3_min = std::numeric_limits<double>::max();
		        double k3_max = -std::numeric_limits<double>::max();

		        // scan through to find min and max values of time
            for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); t++)
			        {
		            if(t->k_t_conventional < kt_min) { kt_min = t->k_t_conventional; }
		            if(t->k_t_conventional > kt_max) { kt_max = t->k_t_conventional; }

		            if(t->alpha < alpha_min) { alpha_min = t->alpha; }
		            if(t->alpha > alpha_max) { alpha_max = t->alpha; }

		            if(t->beta < beta_min) { beta_min = t->beta; }
		            if(t->beta > beta_max) { beta_max = t->beta; }

		            if(t->k1_conventional < k1_min) { k1_min = t->k1_conventional; }
		            if(t->k1_conventional > k1_max) { k1_max = t->k1_conventional; }

		            if(t->k2_conventional < k2_min) { k2_min = t->k2_conventional; }
		            if(t->k2_conventional > k2_max) { k2_max = t->k2_conventional; }

		            if(t->k3_conventional < k3_min) { k3_min = t->k3_conventional; }
		            if(t->k3_conventional > k3_max) { k3_max = t->k3_conventional; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
            for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); t++)
			        {
		            threepf_kconfig_filter_data data;

				        data.serial = t->serial;

		            data.kt_max = fabs(t->k_t_conventional - kt_max) < this->tolerance;
		            data.kt_min = fabs(t->k_t_conventional - kt_min) < this->tolerance;
		            data.kt = t->k_t_conventional;

		            data.alpha_max = fabs(t->alpha - alpha_max) < this->tolerance;
		            data.alpha_min = fabs(t->alpha - alpha_min) < this->tolerance;
		            data.alpha = t->alpha;

		            data.beta_max = fabs(t->beta - beta_max) < this->tolerance;
		            data.beta_min = fabs(t->beta - beta_min) < this->tolerance;
		            data.beta = t->beta;

		            data.k1_max = fabs(t->k1_conventional - k1_max) < this->tolerance;
		            data.k1_min = fabs(t->k1_conventional - k1_min) < this->tolerance;
		            data.k1 = t->k1_conventional;

		            data.k2_max = fabs(t->k2_conventional - k2_max) < this->tolerance;
		            data.k2_min = fabs(t->k2_conventional - k2_min) < this->tolerance;
		            data.k2 = t->k2_conventional;

		            data.k3_max = fabs(t->k3_conventional - k3_max) < this->tolerance;
		            data.k3_min = fabs(t->k3_conventional - k3_min) < this->tolerance;
		            data.k3 = t->k3_conventional;

		            if(k_filter(data)) k_serials.push_back(t->serial);
			        }

            if(k_serials.size() == 0) throw runtime_exception(runtime_exception::FILTER_EMPTY, "");
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__filter_H_
