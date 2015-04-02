//
// Created by David Seery on 31/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __line_values_H_
#define __line_values_H_


#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


namespace transport
	{

		namespace derived_data
			{

		    typedef enum { time_axis, wavenumber_axis, angle_axis, squeezing_fraction_axis } axis_type;

		    typedef enum { field_value, momentum_value, correlation_function_value, fNL_value, r_value, spectral_index_value, dimensionless_value } value_type;

				inline std::string value_type_to_string_non_LaTeX(value_type type)
					{
						switch(type)
							{
						    case field_value:                return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_FIELD);
						    case momentum_value:             return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_MOMENTUM);
						    case correlation_function_value: return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_CORRELATION_FUNCTION);
						    case fNL_value:                  return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_FNL);
						    case r_value:                    return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_R);
						    case spectral_index_value:       return std::string(__CPP_TRANSPORT_VALUE_NON_LATEX_SPECTRAL_INDEX);
						    case dimensionless_value:        return std::string("");
						    default:
							    {
						        std::stringstream msg;
								    msg << __CPP_TRANSPORT_VALUE_UNKNOWN_TYPE << " " << type;
								    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							    }
							}
					}

		    inline std::string value_type_to_string_LaTeX(value_type type)
			    {
		        switch(type)
			        {
		            case field_value:                return std::string(__CPP_TRANSPORT_VALUE_LATEX_FIELD);
		            case momentum_value:             return std::string(__CPP_TRANSPORT_VALUE_LATEX_MOMENTUM);
		            case correlation_function_value: return std::string(__CPP_TRANSPORT_VALUE_LATEX_CORRELATION_FUNCTION);
		            case fNL_value:                  return std::string(__CPP_TRANSPORT_VALUE_LATEX_FNL);
		            case r_value:                    return std::string(__CPP_TRANSPORT_VALUE_LATEX_R);
		            case spectral_index_value:       return std::string(__CPP_TRANSPORT_VALUE_LATEX_SPECTRAL_INDEX);
		            case dimensionless_value:        return std::string("");
		            default:
			            {
		                std::stringstream msg;
		                msg << __CPP_TRANSPORT_VALUE_UNKNOWN_TYPE << " " << type;
		                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
			            }
			        }
			    }

			}   // namespace derived_data


	}   // namespace transport


#endif //__line_values_H_
