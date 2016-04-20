//
// Created by David Seery on 22/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H
#define CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H


#include <string>


#include "transport-runtime/messages.h"


namespace transport
  {

    namespace derived_data
      {

        enum class derived_product_type
          {
            line_2dplot,
            line_table
          };


        inline std::string derived_product_type_to_string(derived_product_type type)
          {
            switch(type)
              {
                case derived_product_type::line_2dplot:
                  return std::string(CPPTRANSPORT_DERIVED_PRODUCT_LINE_2D_PLOT);

                case derived_product_type::line_table:
                  return std::string(CPPTRANSPORT_DERIVED_PRODUCT_LINE_TABLE);
              }
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_TYPE_H
