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
                       TASK_STRUCTURE_ERROR,                              // disallowed configuration while building task hierarchy
                       MPI_ERROR,                                         // error associated with MPI message passing
                       RECORD_NOT_FOUND, REPO_NOT_FOUND,                  // missing items in the repository
                       REPOSITORY_ERROR,                                  // generic repository error
                       REPOSITORY_BACKEND_ERROR,                          // error from repository database backend
		                   REPOSITORY_JX9_ERROR,                              // error from Jx9 script (different from a repository backend error)
                       DATA_CONTAINER_ERROR,                              // generic data-container error
		                   DATA_MANAGER_BACKEND_ERROR,                        // error from data manager database backend
                       MISSING_MODEL_INSTANCE,                            // could not find model instance to dispatch to
                       STORAGE_ERROR,                                     // error while storing the output of an integration
                       BACKEND_ERROR,                                     // problem encountered by a compute backend
		                   SERIALIZATION_ERROR,                               // error encountered during serialization
		                   DATAPIPE_ERROR,                                    // generic datapipe error
		                   DERIVED_PRODUCT_ERROR,                             // error encountered when generating a derived product
                       FILTER_EMPTY,                                      // filtering produced no values
                       JSON_FORMAT_ERROR                                  // error encountered while handling a JSON document
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
