//
// Created by David Seery on 31/03/15.
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


#ifndef CPPTRANSPORT_LINE_VALUES_H
#define CPPTRANSPORT_LINE_VALUES_H


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
						    case value_type::field:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FIELD);
						    case value_type::momentum:             return std::string(CPPTRANSPORT_VALUE_NON_LATEX_MOMENTUM);
						    case value_type::correlation_function: return std::string(CPPTRANSPORT_VALUE_NON_LATEX_CORRELATION_FUNCTION);
						    case value_type::fNL:                  return std::string(CPPTRANSPORT_VALUE_NON_LATEX_FNL);
						    case value_type::r:                    return std::string(CPPTRANSPORT_VALUE_NON_LATEX_R);
						    case value_type::spectral_index:       return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SPECTRAL_INDEX);
						    case value_type::dimensionless:        return std::string("");
						    case value_type::time:                 return std::string(CPPTRANSPORT_VALUE_NON_LATEX_SECONDS);
						    case value_type::steps:                return std::string(CPPTRANSPORT_VALUE_NON_LATEX_STEPS);
              }
					}

		    inline std::string value_type_to_string_LaTeX(value_type type)
			    {
		        switch(type)
			        {
		            case value_type::field:                return std::string(CPPTRANSPORT_VALUE_LATEX_FIELD);
		            case value_type::momentum:             return std::string(CPPTRANSPORT_VALUE_LATEX_MOMENTUM);
		            case value_type::correlation_function: return std::string(CPPTRANSPORT_VALUE_LATEX_CORRELATION_FUNCTION);
		            case value_type::fNL:                  return std::string(CPPTRANSPORT_VALUE_LATEX_FNL);
		            case value_type::r:                    return std::string(CPPTRANSPORT_VALUE_LATEX_R);
		            case value_type::spectral_index:       return std::string(CPPTRANSPORT_VALUE_LATEX_SPECTRAL_INDEX);
		            case value_type::dimensionless:        return std::string("");
		            case value_type::time:                 return std::string(CPPTRANSPORT_VALUE_LATEX_SECONDS);
		            case value_type::steps:                return std::string(CPPTRANSPORT_VALUE_LATEX_STEPS);
              }
			    }

			}   // namespace derived_data


	}   // namespace transport


#endif //CPPTRANSPORT_LINE_VALUES_H
