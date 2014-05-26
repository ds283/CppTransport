//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
#include "field_time_series.h"
#include "zeta_time_series.h"

namespace transport
	{

		namespace derived_data
			{

		    namespace
			    {

		        namespace derived_line_helper
			        {

		            template <typename number>
		            derived_line<number>* deserialize(serialization_reader* reader, typename repository<number>::task_finder finder)
			            {
		                assert(reader != nullptr);

		                if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_READER);

		                derived_line<number>* rval = nullptr;

		                std::string type;
		                reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE, type);

		                if      (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_BACKGROUND)   rval = new background_time_series<number>(reader, finder);
		                else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF)        rval = new twopf_time_series<number>(reader, finder);
		                else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF)      rval = new threepf_time_series<number>(reader, finder);
                    else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_ZETA_TWOPF)   rval = new zeta_twopf_time_series<number>(reader, finder);
                    else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_ZETA_THREEPF) rval = new zeta_threepf_time_series<number>(reader, finder);
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

			}   // namespace transport

	}   // namespace transport


#endif //__general_time_data_helper_H_
