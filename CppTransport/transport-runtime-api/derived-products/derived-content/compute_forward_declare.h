//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#if !defined(__compute_forward_declare_H_)
#define __compute_forward_declare_H_


namespace transport
	{

		namespace derived_data
			{

#ifndef __zeta_timeseries_compute_H_
		    template <typename number> class zeta_timeseries_compute;
#endif

#ifndef __zeta_kseries_compute_H_
		    template <typename number> class zeta_kseries_compute;
#endif

#ifndef __fNL_timeseries_compute_H_
		    template <typename number> class fNL_timeseries_compute;
#endif

			}   // namespace derived_data

	}   // namespace transport


#endif //__compute_forward_declare_H_
