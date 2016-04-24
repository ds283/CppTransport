//
// Created by David Seery on 31/03/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef __line_values_H_
#define __line_values_H_


#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


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

				inline std::string value_type_to_string_non_LaTeX(value_type type)
					{
						switch(type)
							{
						    case value_type::field_value:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FIELD);
						    case value_type::momentum_value:             return std::string(CPPTRANSPORT_VALUE_NON_LATEX_MOMENTUM);
						    case value_type::correlation_function_value: return std::string(CPPTRANSPORT_VALUE_NON_LATEX_CORRELATION_FUNCTION);
						    case value_type::fNL_value:                  return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FNL);
						    case value_type::r_value:                    return std::string(CPPTRANSPORT_VALUE_NON_LATEX_R);
						    case value_type::spectral_index_value:       return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SPECTRAL_INDEX);
						    case value_type::dimensionless_value:        return std::string("");
						    case value_type::time_value:                 return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SECONDS);
						    case value_type::steps_value:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_STEPS);
              }
					}

		    inline std::string value_type_to_string_LaTeX(value_type type)
			    {
		        switch(type)
			        {
		            case value_type::field_value:                return std::string(CPPTRANSPORT_VALUE_LATEX_FIELD);
		            case value_type::momentum_value:             return std::string(CPPTRANSPORT_VALUE_LATEX_MOMENTUM);
		            case value_type::correlation_function_value: return std::string(CPPTRANSPORT_VALUE_LATEX_CORRELATION_FUNCTION);
		            case value_type::fNL_value:                  return std::string(CPPTRANSPORT_VALUE_LATEX_FNL);
		            case value_type::r_value:                    return std::string(CPPTRANSPORT_VALUE_LATEX_R);
		            case value_type::spectral_index_value:       return std::string(CPPTRANSPORT_VALUE_LATEX_SPECTRAL_INDEX);
		            case value_type::dimensionless_value:        return std::string("");
		            case value_type::time_value:                 return std::string(CPPTRANSPORT_VALUE_LATEX_SECONDS);
		            case value_type::steps_value:                return std::string(CPPTRANSPORT_VALUE_LATEX_STEPS);
              }
			    }

			}   // namespace derived_data


	}   // namespace transport


#endif //__line_values_H_
