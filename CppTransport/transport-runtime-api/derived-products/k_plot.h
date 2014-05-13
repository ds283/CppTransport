//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __k_plot_H_
#define __k_plot_H_


#include "transport-runtime-api/derived-products/derived_product.h"


namespace transport
	{

		//! A k-plot is a specialization of a data-product that produces
		//! a 'spectrum' or plot of a component of a correlation function,
		//! (or a group of components) as a function of momentum.

		template <typename number>
    class k_plot: public derived_product<number>
	    {

	    };

	}   // namespace transport


#endif //__k_plot_H_
