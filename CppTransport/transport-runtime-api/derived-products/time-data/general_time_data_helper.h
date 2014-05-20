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


#include "transport-runtime-api/derived-products/time-data/general_time_data.h"
#include "transport-runtime-api/derived-products/time-data/field_time_data.h"


namespace transport
	{

		namespace derived_data
			{

		    namespace
			    {

		        namespace general_time_data_helper
			        {

		            template <typename number>
		            general_time_data<number>* deserialize(serialization_reader* reader, typename repository<number>::task_finder finder)
			            {
		                assert(reader != nullptr);

		                if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		                general_time_data<number>* rval = nullptr;

		                std::string type;
		                reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE, type);

		                if      (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_BACKGROUND) rval = new background_time_data<number>(reader, finder);
		                else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF)      rval = new twopf_time_data<number>(reader, finder);
		                else if (type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF)    rval = new threepf_time_data<number>(reader, finder);
		                else
			                {
		                    std::ostringstream msg;
		                    msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LINE_TYPE_UNKNOWN << " '" << type << "'";
		                    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			                }

		                return(rval);
			            }

			        }   // namespace general_time_data_helper

			    }   // unnamed namespace

			}   // namespace transport

	}   // namespace transport


#endif //__general_time_data_helper_H_
