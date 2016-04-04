//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H
#define CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H


#include <string>

#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"


namespace transport
  {

    // forward-declare datapipe
    template <typename number> class datapipe;

    //! dispatch_function is a visitor-pattern type callback used to 'dispatch' derived content (ie. send to the client)
    template <typename number>
    class datapipe_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        datapipe_dispatch_function() = default;

        //! destructor is default
        virtual ~datapipe_dispatch_function() = default;


        // INTERFACE

      public:

        //! dispatch
        virtual void operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& used_groups) = 0;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATAPIPE_DISPATCH_FUNCTION_H
