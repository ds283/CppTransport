//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __general_time_data_helper_H_
#define __general_time_data_helper_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>


#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/field_time_series.h"
#include "transport-runtime-api/derived-products/derived-content/tensor_time_series.h"
#include "transport-runtime-api/derived-products/derived-content/tensor_wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_time_series.h"
#include "transport-runtime-api/derived-products/derived-content/field_wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/fNL_time_series.h"
#include "transport-runtime-api/derived-products/derived-content/r_time_series.h"
#include "transport-runtime-api/derived-products/derived-content/r_wavenumber_series.h"


namespace transport
	{

		namespace derived_data
			{

		    namespace
			    {

		        namespace derived_line_helper
			        {

		            template <typename number>
		            derived_line<number>* deserialize(Json::Value& reader, typename repository_finder<number>::task_finder finder)
			            {
		                derived_line<number>* rval = nullptr;

		                std::string type = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE].asString();

		                if     (type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND)                                rval = new background_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES)                         rval = new twopf_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES)                       rval = new threepf_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES)                  rval = new tensor_twopf_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES)            rval = new tensor_twopf_wavenumber_series<number>(reader, finder);
                    else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES)                    rval = new zeta_twopf_time_series<number>(reader, finder);
                    else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES)                  rval = new zeta_threepf_time_series<number>(reader, finder);
                    else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES)       rval = new zeta_reduced_bispectrum_time_series<number>(reader, finder);
			              else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES)                   rval = new twopf_wavenumber_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES)                 rval = new threepf_wavenumber_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES)              rval = new zeta_twopf_wavenumber_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES)            rval = new zeta_threepf_wavenumber_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES) rval = new zeta_reduced_bispectrum_wavenumber_series<number>(reader, finder);
                    else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES)                           rval = new fNL_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES)                             rval = new r_time_series<number>(reader, finder);
		                else if(type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES)                       rval = new r_wavenumber_series<number>(reader, finder);
		                else
			                {
		                    std::ostringstream msg;
		                    msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_CONTENT_TYPE_UNKNOWN << " '" << type << "'";
		                    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			                }

		                return(rval);
			            }

			        }   // namespace derived_line_helper

			    }   // unnamed namespace

			}   // namespace derived_data

	}   // namespace transport


#endif //__general_time_data_helper_H_
