//
// Created by David Seery on 15/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_helper_H_
#define __derived_product_helper_H_


#include "transport-runtime-api/derived-products/line_plot2d.h"


namespace transport
	{

		namespace derived_data
			{

		      namespace
			    {

		        namespace derived_product_helper
			        {

		            template <typename number>
		            derived_data::derived_product<number>* deserialize(const std::string& name, serialization_reader* reader,
		                                                               typename repository<number>::task_finder finder)
			            {
		                assert(reader != nullptr);

		                std::string type;

		                derived_data::derived_product<number>* rval = nullptr;

		                reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, type);

		                if (type == __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D) rval = new line_plot2d<number>(name, reader, finder);
				            else
			                {
		                    std::ostringstream msg;
		                    msg << __CPP_TRANSPORT_PRODUCT_UNKNOWN_TYPE << " '" << type << "'";
		                    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			                }

		                return(rval);
			            }

			        }   // namespace derived_product_helper

			    }   // unnamed namespace

			}   // namespace derived_data

	}


#endif //__derived_product_helper_H_
