//
// Created by David Seery on 30/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __kconfig_series_H_
#define __kconfig_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_KDATA_K_SERIAL_NUMBERS "kconfig-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_KDATA_K_SERIAL_NUMBER  "sn"

#define __CPP_TRANSPORT_NODE_PRODUCT_KDATA_T_SERIAL_NUMBERS "time-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_KDATA_T_SERIAL_NUMBER  "sn"

// maximum number of serial numbers to output when writing ourselves
// to a standard stream
#define __CPP_TRANSPORT_PRODUCT_KDATA_MAX_SN (15)

namespace transport
  {

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes
    // task.h includes this header, so we cannot include task.h
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class twopf_list_task;

    namespace derived_data
      {

        //! general twopf-kconfig-series content producer, suitable
        //! for producing content usable in eg. a 2d plot or table
        template <typename number>
        class twopf_kconfig_series: public derived_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! Basic user-facing constructor
            twopf_kconfig_series(const integration_task<number>& tk, model<number>* s, filter::twopf_kconfig_filter kfilter,
                                 typename derived_line<number>::value_type vt, unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

          };

      }

  }   // namespace transport


#endif //__kconfig_series_H_
