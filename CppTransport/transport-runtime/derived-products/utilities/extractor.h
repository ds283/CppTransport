//
// Created by David Seery on 03/06/2014.
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


#ifndef __extractor_H_
#define __extractor_H_


#include <assert.h>

#include "transport-runtime/data/data_manager.h"


namespace transport
	{

		namespace derived_data
			{

		    class extractor
			    {

		      public:

		        extractor(unsigned int n)
			        : num(n)
			        {
		            if(this->num < 1) this->num = 1;
		            if(this->num > 3) this->num = 3;
			        }


		        ~extractor() = default;


		        double comoving(const threepf_kconfig& c) const
			        {
		            if(this->num == 1) return (c.k1_comoving);
		            if(this->num == 2) return (c.k2_comoving);
		            if(this->num == 3) return (c.k3_comoving);

		            assert(false);
		            return (0.0);
			        }


		        double conventional(const threepf_kconfig& c) const
			        {
		            if(this->num == 1) return (c.k1_conventional);
		            if(this->num == 2) return (c.k2_conventional);
		            if(this->num == 3) return (c.k3_conventional);

		            assert(false);
		            return (0.0);
			        }


		        unsigned int serial(const threepf_kconfig& c) const
			        {
		            if(this->num == 1) return (c.k1_serial);
		            if(this->num == 2) return (c.k2_serial);
		            if(this->num == 3) return (c.k3_serial);

		            assert(false);
		            return (0);
			        }


		      private:

		        unsigned int num;

			    };

			}   // namespace derived data

	}   // namespace transport


#endif //__extractor_H_
