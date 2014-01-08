//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <string>

#include "transport/manager/instance_manager.h"
#include "transport/models/model.h"
#include "transport/tasks/task.h"
#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {
    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;

    template <typename number>
    class repository
      {
      public:
        typedef enum { node_storage, document_storage } storage_type;

        class integration_container
          {
          public:
            integration_container(boost::filesystem::path& dir, boost::filesystem::path& data, unsigned int n)
              : path_to_directory(dir), path_to_data_container(data), serial_number(n)
              {
              }

          friend class repository<number>;

          protected:
            const boost::filesystem::path& data_container_path() { return(this->path_to_data_container); }

          private:
            boost::filesystem::path path_to_directory;
            boost::filesystem::path path_to_data_container;
            unsigned int            serial_number;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository()
          {
          }


        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the model database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        void write_package(const initial_conditions<number>& ics, const model<number>* m);

        //! Write a threepf integration task to the integration database.
        virtual void write_integration(const twopf_task<number>& t, const model<number>* m) = 0;
        //! Write a twopf integration task to the integration database
        virtual void write_integration(const threepf_task<number>& t, const model<number>* m) = 0;


        // INTERFACE -- PULL TASKS FROM THE REPOSITORY DATABASE

      public:
        //! Query the database for a named task, and reconstruct it if present
        virtual task<number>* query_task(const std::string& name, model<number>*& m, typename instance_manager<number>::model_finder finder) = 0;

        //! Extract the XML document for a named package
        virtual std::string extract_package_document(const std::string& name) = 0;
        //! Extract the XML document for a named integration
        virtual std::string extract_integration_document(const std::string& name) = 0;


        // INTERFACE -- ADD OUTPUT TO TASKS

      public:
        //! Insert a record for new twopf output in the task XML database, and set up paths to a suitable SQL container
        virtual integration_container integration_new_output(twopf_task<number>* tk) = 0;
        //! Insert a record for new threepf output in the task XML database, and set up paths to a suitable SQL container
        virtual integration_container integration_new_output(threepf_task<number>* tk) = 0;
      };


  }   // namespace transport



#endif //__repository_H_
