//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <string>

#include "transport/manager/instance_manager.h"
#include "transport/tasks/task.h"
#include "transport/concepts/initial_conditions.h"
#include "transport/concepts/parameters.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    // forward-declare model to avoid circular inclusion
    template <typename number> class model;

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
            //! Construct an integration container object. It is not associated with anything in the data_manager backend; that must be done later
            integration_container(const boost::filesystem::path& dir, const boost::filesystem::path& data,
                                  const boost::filesystem::path& log, const boost::filesystem::path& task,
                                  const boost::filesystem::path& temp, unsigned int n)
              : path_to_directory(dir), path_to_data_container(data),
                path_to_log_directory(log), path_to_taskfile(task),
                path_to_temp_directory(temp),
                serial_number(n),
                data_manager_handle(nullptr), data_manager_taskfile(nullptr)
              {
              }

            //! Set data_manager handle for data container
            template <typename data_manager_type>
            void set_data_manager_handle(data_manager_type data)
              {
                this->data_manager_handle = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
              }

            //! Return data_manager handle for data container

            //! Throws a REPOSTORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_handle(data_manager_type* data)
              {
                if(this->data_manager_handle == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_INTCTR_UNSETHANDLE);
                *data = static_cast<data_manager_type>(this->data_manager_handle);
              }

            //! Set data_manager handle for taskfile
            template <typename data_manager_type>
            void set_data_manager_taskfile(data_manager_type data)
              {
                this->data_manager_taskfile = static_cast<void*>(data);   // will fail if data_manager_type not (static)-castable to void*
              }

            //! Return data_manager handle for taskfile

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_taskfile(data_manager_type* data)
              {
                if(this->data_manager_taskfile == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_INTCTR_UNSETTASK);
                *data = static_cast<data_manager_type>(this->data_manager_taskfile);
              }

            //! Return path to data container
            const boost::filesystem::path& data_container_path() { return(this->path_to_data_container); }

            //! Return path to log directory
            const boost::filesystem::path& log_directory_path() { return(this->path_to_log_directory); }

            //! Return path to task-data container
            const boost::filesystem::path& taskfile_path() { return(this->path_to_taskfile); }

            //! Return path to directory for temporary files
            const boost::filesystem::path& temporary_files_path() { return(this->path_to_temp_directory); }

          private:
            const boost::filesystem::path path_to_directory;

            const boost::filesystem::path path_to_data_container;
            const boost::filesystem::path path_to_log_directory;
            const boost::filesystem::path path_to_taskfile;
            const boost::filesystem::path path_to_temp_directory;

            const unsigned int            serial_number;

            void*                         data_manager_handle;
            void*                         data_manager_taskfile;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository()
          {
          }


        // INTERFACE -- PATHS

        //! Get path to root of repository
        virtual const boost::filesystem::path& get_root_path() = 0;


        // INTERFACE -- PUSH TASKS TO THE REPOSITORY DATABASE

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the model database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void write_package(const initial_conditions<number>& ics, const model<number>* m) = 0;

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
