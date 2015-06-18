//
// Created by David Seery on 31/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __line_values_H_
#define __line_values_H_


#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


// axis classes
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_TIME                  "time-axis"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_WAVENUMBER            "k-axis"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_THREEPF_CONFIGURATION "threepf-axis"

#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS                "efolds"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER            "k-value"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT           "efolds-exit"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA                 "alpha"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA                  "beta"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1 "squeezing-fraction-k1"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2 "squeezing-fraction-k2"
#define CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3 "squeezing-fraction-k3"


namespace transport
	{

		namespace derived_data
			{

				//! derived lines are associated with one a three classes of axis
				typedef enum { time_axis, wavenumber_axis, threepf_kconfig_axis } axis_class;

				//! possible axis choices for a given derived line
				typedef enum
					{
				    efolds_axis, k_axis, efolds_exit_axis, alpha_axis, beta_axis,
				    squeezing_fraction_k1_axis,
						squeezing_fraction_k2_axis,
						squeezing_fraction_k3_axis,
						unset_axis
					} axis_value;

		    typedef enum { field_value, momentum_value, correlation_function_value, fNL_value,
		                   r_value, spectral_index_value, dimensionless_value,
			                 time_value, steps_value } value_type;

				inline std::string value_type_to_string_non_LaTeX(value_type type)
					{
						switch(type)
							{
						    case field_value:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FIELD);
						    case momentum_value:             return std::string(CPPTRANSPORT_VALUE_NON_LATEX_MOMENTUM);
						    case correlation_function_value: return std::string(CPPTRANSPORT_VALUE_NON_LATEX_CORRELATION_FUNCTION);
						    case fNL_value:                  return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FNL);
						    case r_value:                    return std::string(CPPTRANSPORT_VALUE_NON_LATEX_R);
						    case spectral_index_value:       return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SPECTRAL_INDEX);
						    case dimensionless_value:        return std::string("");
						    case time_value:                 return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SECONDS);
						    case steps_value:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_STEPS);
						    default:
							    {
						        std::stringstream msg;
								    msg << CPPTRANSPORT_VALUE_UNKNOWN_TYPE << " " << type;
								    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							    }
							}
					}

		    inline std::string value_type_to_string_LaTeX(value_type type)
			    {
		        switch(type)
			        {
		            case field_value:                return std::string(CPPTRANSPORT_VALUE_LATEX_FIELD);
		            case momentum_value:             return std::string(CPPTRANSPORT_VALUE_LATEX_MOMENTUM);
		            case correlation_function_value: return std::string(CPPTRANSPORT_VALUE_LATEX_CORRELATION_FUNCTION);
		            case fNL_value:                  return std::string(CPPTRANSPORT_VALUE_LATEX_FNL);
		            case r_value:                    return std::string(CPPTRANSPORT_VALUE_LATEX_R);
		            case spectral_index_value:       return std::string(CPPTRANSPORT_VALUE_LATEX_SPECTRAL_INDEX);
		            case dimensionless_value:        return std::string("");
		            case time_value:                 return std::string(CPPTRANSPORT_VALUE_LATEX_SECONDS);
		            case steps_value:                return std::string(CPPTRANSPORT_VALUE_LATEX_STEPS);
		            default:
			            {
		                std::stringstream msg;
		                msg << CPPTRANSPORT_VALUE_UNKNOWN_TYPE << " " << type;
		                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
			            }
			        }
			    }

			}   // namespace derived_data


	}   // namespace transport


#endif //__line_values_H_
