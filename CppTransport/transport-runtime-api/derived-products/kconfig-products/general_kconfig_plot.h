//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __kconfig_plot_H_
#define __kconfig_plot_H_


#include "transport-runtime-api/derived-products/plot2d_product.h"


namespace transport
	{

		namespace derived_data
			{

			  //! A konfig-plot is a specialization of a plot2d-product that produces
		    //! a 'spectrum' or plot of a component of a correlation function,
		    //! (or a group of components) as a function of a k-configuration.

		    template <typename number>
		    class general_kconfig_plot : public plot2d_product<number>
			    {

			    };

			}   // namespace derived_data

	}   // namespace transport


#endif //__kconfig_plot_H_
