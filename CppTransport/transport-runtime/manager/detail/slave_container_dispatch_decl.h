//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H
#define CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H


#include <string>

#include "transport-runtime/data/batchers/container_dispatch_function.h"


namespace transport
  {

    // forward declare slave controller
    template <typename number> class slave_controller;

    template <typename number>
    class slave_container_dispatch: public container_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures slave controller, MPI message, and logging tag
        slave_container_dispatch(slave_controller<number>& c, unsigned int m, std::string t)
          : container_dispatch_function(),
            controller(c),
            message(m),
            tag(std::move(t))
          {
          }

        //! destructor is default
        ~slave_container_dispatch() = default;


        // INTERFACE

      public:

        //! dispatch container
        void operator()(generic_batcher& batcher) override;


        // INTERNAL DATA

      private:

        //! reference to controller
        slave_controller<number>& controller;

        //! MPI message
        unsigned int message;

        //! logging tag
        std::string tag;

      };

  }


#endif //CPPTRANSPORT_SLAVE_CONTAINER_DISPATCH_DECL_H
