//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __extractor_H_
#define __extractor_H_


#include <assert.h>

#include "transport-runtime-api/data/data_manager.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class extractor
			    {

		      public:

		        extractor(unsigned int n, const threepf_kconfig& c)
			        : num(n), config(c)
			        {
		            if(this->num < 1) this->num = 1;
		            if(this->num > 3) this->num = 3;
			        }


		        ~extractor() = default;


		        double comoving() const
			        {
		            if(this->num == 1) return (this->config.k1_comoving);
		            if(this->num == 2) return (this->config.k2_comoving);
		            if(this->num == 3) return (this->config.k3_comoving);

		            assert(false);
		            return (0.0);
			        }


		        double conventional() const
			        {
		            if(this->num == 1) return (this->config.k1_conventional);
		            if(this->num == 2) return (this->config.k2_conventional);
		            if(this->num == 3) return (this->config.k3_conventional);

		            assert(false);
		            return (0.0);
			        }


		        unsigned int serial() const
			        {
		            if(this->num == 1) return (this->config.k1_serial);
		            if(this->num == 2) return (this->config.k2_serial);
		            if(this->num == 3) return (this->config.k3_serial);

		            assert(false);
		            return (0);
			        }


		      private:

		        unsigned int num;

		        const threepf_kconfig& config;

			    };

			}   // namespace derived data

	}   // namespace transport


#endif //__extractor_H_
