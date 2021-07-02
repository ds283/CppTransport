//
// Created by David Seery on 19/05/2014.
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


#ifndef CPPTRANSPORT_DERIVED_LINE_HELPER_H
#define CPPTRANSPORT_DERIVED_LINE_HELPER_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>


#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/field_time_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/tensor_time_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/tensor_wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/zeta_time_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/field_wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/zeta_wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/fNL_time_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/r_time_series.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/r_wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/integration-analysis/cost_wavenumber.h"
#include "transport-runtime/derived-products/derived-content/background-quantites/background_line.h"
#include "transport-runtime/derived-products/derived-content/background-quantites/u2_line.h"
#include "transport-runtime/derived-products/derived-content/background-quantites/u3_line.h"
#include "transport-runtime/derived-products/derived-content/background-quantites/largest_u2_line.h"
#include "transport-runtime/derived-products/derived-content/background-quantites/largest_u3_line.h"


namespace transport
	{

		namespace derived_data
			{

		    namespace
			    {

		        namespace derived_line_helper
			        {

		            template <typename number>
		            std::unique_ptr< derived_line<number> > deserialize(Json::Value& reader, task_finder<number> finder)
			            {
		                std::string type = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE].asString();

		                if     (type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND)                                return std::make_unique< background_time_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES)                         return std::make_unique< twopf_time_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES)                       return std::make_unique< threepf_time_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES)                  return std::make_unique< tensor_twopf_time_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_NT_TIME_SERIES)               return std::make_unique< tensor_nt_time_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES)                    return std::make_unique< zeta_twopf_time_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_NS_TIME_SERIES)                 return std::make_unique< zeta_ns_time_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES)                  return std::make_unique< zeta_threepf_time_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES)       return std::make_unique< zeta_reduced_bispectrum_time_series<number> >(reader, finder);
			              else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES)                   return std::make_unique< twopf_wavenumber_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES)                 return std::make_unique< threepf_wavenumber_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES)            return std::make_unique< tensor_twopf_wavenumber_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_NT_WAVENUMBER_SERIES)         return std::make_unique< tensor_nt_wavenumber_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES)              return std::make_unique< zeta_twopf_wavenumber_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_NS_WAVENUMBER_SERIES)           return std::make_unique< zeta_ns_wavenumber_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES)            return std::make_unique< zeta_threepf_wavenumber_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES) return std::make_unique< zeta_reduced_bispectrum_wavenumber_series<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES)                           return std::make_unique< fNL_time_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES)                             return std::make_unique< r_time_series<number> >(reader, finder);
		                else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES)                       return std::make_unique< r_wavenumber_series<number> >(reader, finder);
				            else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND_LINE)                           return std::make_unique< background_line<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U2_LINE)                                   return std::make_unique< u2_line<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U3_LINE)                                   return std::make_unique< u3_line<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U2_LINE)                           return std::make_unique< largest_u2_line<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U3_LINE)                           return std::make_unique< largest_u3_line<number> >(reader, finder);
                    else if(type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_INTEGRATION_COST)                          return std::make_unique< cost_wavenumber<number> >(reader, finder);

                    std::ostringstream msg;
                    msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CONTENT_TYPE_UNKNOWN << " '" << type << "'";
                    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
			            }

			        }   // namespace derived_line_helper

			    }   // unnamed namespace

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVED_LINE_HELPER_H
