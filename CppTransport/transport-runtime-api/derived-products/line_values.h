//
// Created by David Seery on 31/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __line_values_H_
#define __line_values_H_


namespace transport
	{

		namespace derived_data
			{

		    typedef enum { time_axis, wavenumber_axis, angle_axis, squeezing_fraction_axis } axis_type;

		    typedef enum { background_field_value, correlation_function_value, fNL_value, r_value, spectral_index_value } value_type;

			}   // namespace derived_data


	}   // namespace transport


#endif //__line_values_H_
