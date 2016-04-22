//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H
#define CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H


namespace transport
  {

    // forward-declare generic batcher
    class generic_batcher;

    class container_dispatch_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        container_dispatch_function() = default;

        //! destructor is default
        virtual ~container_dispatch_function() = default;


        // INTERFACE

      public:

        //! dispatch container to master node
        virtual void operator()(generic_batcher& batcher) = 0;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_CONTAINER_DISPATCH_FUNCTION_H
