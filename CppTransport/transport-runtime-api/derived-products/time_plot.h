//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __time_plot_H_
#define __time_plot_H_


#include "transport-runtime-api/derived-products/derived_product.h"


namespace transport
	{

		//! A time_plot is a specialization of a data_product that produces
		//! an time-evolution plot of some components (or multiple components)
		//! of one or more correlation functions.

		template <typename number>
    class time_plot: public derived_product<number>
	    {

	    };

	}   // namespace transport


#endif //__time_plot_H_
