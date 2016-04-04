//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTAINER_REPLACE_FUNCTION_H
#define CPPTRANSPORT_CONTAINER_REPLACE_FUNCTION_H


namespace transport
  {

    // forward-declare generic batcher
    class generic_batcher;

    //! Internal flag indicating the action which should be taken by
    //! a batcher when its temporary container is replaced.
    enum class replacement_action { action_replace, action_close };

    class container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        container_replace_function() = default;

        //! destructor is default
        virtual ~container_replace_function() = default;


        // INTERFACE

      public:

        //! dispatch container to master node
        virtual void operator()(generic_batcher& batcher, replacement_action action) = 0;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_CONTAINER_REPLACE_FUNCTION_H
