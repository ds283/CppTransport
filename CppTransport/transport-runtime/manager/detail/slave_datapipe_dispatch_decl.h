//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_DISPATCH_DECL_H
#define CPPTRANSPORT_SLAVE_DISPATCH_DECL_H


#include "transport-runtime-api/data/datapipe/datapipe_dispatch_function.h"


namespace transport
  {

    // forward declare slave controller
    template <typename number> class slave_controller;

    template <typename number>
    class slave_datapipe_dispatch : public datapipe_dispatch_function<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures slave controller
        slave_datapipe_dispatch(slave_controller<number>& c)
          : datapipe_dispatch_function<number>(),
            controller(c)
          {
          }

        //! destructor is default
        ~slave_datapipe_dispatch() = default;


        // INTERFACE

      public:

        //! dispatch derived product
        void operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& tags) override;


        // INTERNAL DATA

      private:

        //! reference to controller
        slave_controller<number>& controller;

      };


    template <typename number>
    class slave_null_dispatch_function: public datapipe_dispatch_function<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures slave controller
        slave_null_dispatch_function(slave_controller<number>& c)
          : datapipe_dispatch_function<number>(),
            controller(c)
          {
          }

        //! destructor is default
        ~slave_null_dispatch_function() = default;


        // INTERFACE

      public:

        void operator()(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& tags) override;


        // INTERNAL DATA

      private:

        //! reference to controller
        slave_controller<number>& controller;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_DISPATCH_DECL_H
