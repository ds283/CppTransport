//
// Created by David Seery on 15/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __derived_product_helper_H_
#define __derived_product_helper_H_


#include "transport-runtime-api/derived-products/line-collections/line_plot2d.h"
#include "transport-runtime-api/derived-products/line-collections/line_asciitable.h"


namespace transport
	{

		namespace derived_data
			{

        namespace derived_product_helper
          {

            template <typename number>
            derived_data::derived_product<number>* deserialize(const std::string& name, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
              {
                std::string type = reader[__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE].asString();

                if (type == __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D)         return new line_plot2d<number>(name, reader, finder);
                else if(type == __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE) return new line_asciitable<number>(name, reader, finder);

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_UNKNOWN_TYPE << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

          }   // namespace derived_product_helper

			}   // namespace derived_data

	}


#endif //__derived_product_helper_H_
