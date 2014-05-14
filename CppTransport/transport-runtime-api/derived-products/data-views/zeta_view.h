//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_view_H_
#define __zeta_view_H_


#include "transport-runtime-api/derived-products/data_view.h"
#include "transport-runtime-api/derived-products/field_views.h"


namespace transport
	{

		//! A zeta-twopf-view is a specialization of a data_view
		//! which makes visible the two-point function of the
		//! curvature perturbation, zeta

		template <typename number>
    class zeta_twopf_view: public data_view<number>
	    {

	    };


		//! A zeta-threepf-view is a specialization of a data_view
		//! which makes visible the three-point function of the
		//! curvature perturbation, zeta

		template <typename number>
		class zeta_threepf_view: public data_view<number>
			{

			};

	}


#endif //__zeta_view_H_
