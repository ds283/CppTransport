//
// Created by David Seery on 31/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __exceptions_H_
#define __exceptions_H_


#include <string>
#include <stdexcept>


namespace transport
  {

    class runtime_exception: public std::runtime_error
      {
        // CONSTRUCTOR, DESTRUCTOR

      public:

        typedef enum { RUNTIME_ERROR,                                     // generic runtime error
                       MPI_ERROR,                                         // error associated with MPI message passing
                       TASK_NOT_FOUND, MODEL_NOT_FOUND, REPO_NOT_FOUND,   // missing items in the repository
                       REPOSITORY_ERROR,                                  // generic repository error
                       DATA_CONTAINER_ERROR,                              // generic data-container error
                       BADLY_FORMED_XML,                                  // badly formed XML document in repository
                       MISSING_MODEL_INSTANCE,                            // could not find model instance to dispatch to
                       STORAGE_ERROR,                                     // error while storing the output of an integration
                       BACKEND_ERROR,                                     // problem encountered by a compute backend
		                   SERIALIZATION_ERROR                                // error encountered during serialization
                     } exception_type;
        runtime_exception(exception_type t, const std::string msg)
          : type(t), std::runtime_error(msg)
          {
          }

        exception_type get_exception_code() { return(this->type); }

        // INTERNAL DATA
      protected:
        exception_type type;
      };

  }   // namespace transport



#endif //__exceptions_H_
