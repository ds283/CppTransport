//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __repository_H_
#define __repository_H_


#include <iostream>
#include <string>

#include "transport-runtime-api/manager/instance_manager.h"
//#include "transport-runtime-api/tasks/model_list.h"

#include "boost/filesystem/operations.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


// log file name
#define __CPP_TRANSPORT_LOG_FILENAME_A  "worker_"
#define __CPP_TRANSPORT_LOG_FILENAME_B  "_%3N.log"


namespace transport
	{

    // forward-declare model to avoid circular inclusion
    template <typename number>
    class model;


    // forward-declare initial_conditions
    template <typename number>
    class initial_conditions;


    // forward-declare tasks
    template <typename number>
    class task;


    template <typename number>
    class integration_task;


    template <typename number>
    class twopf_task;


    template <typename number>
    class threepf_task;


    template <typename number>
    class output_task;

    // forward-declare derived_product object to avoid circular inclusion
    namespace derived_data
	    {
        template <typename number>
        class derived_product;
	    }

    // forward-declare 'key' class used to create repositories
    // the complete declaration is in a separate file,
    // which must be included to allow creation of repositories
    class repository_creation_key;


    template <typename number>
    class repository
	    {

      public:

		    //! Task finder service
		    typedef std::function<task<number>*(const std::string&,model<number>*&)> task_finder;

        //! Types needed for logging
        typedef enum
	        {
            normal, notification, warning, error, critical
	        }                                                                               log_severity_level;
        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;

        //! Read-only/Read-write access to the repository
        typedef enum
	        {
            readonly, readwrite
	        }                                                                               access_type;


        // DATA CONTAINER WRITE HANDLE

        //! Integration container writer: forms a handle for a data container when writing the output of an integration

        class integration_writer
	        {

          public:

            //! Construct an integration container object.
            //! After creation it is not yet associated with anything in the data_manager backend; that must be done later
            //! by the task_manager, which can depute a data_manager object of its choice to do the work.
            integration_writer(const boost::filesystem::path& dir, const boost::filesystem::path& data,
                               const boost::filesystem::path& log, const boost::filesystem::path& task,
                               const boost::filesystem::path& temp, unsigned int n, unsigned int w);

            //! Destroy an integration container object
            ~integration_writer();

            //! Set data_manager handle for data container
            template <typename data_manager_type>
            void set_data_manager_handle(data_manager_type data);

            //! Return data_manager handle for data container

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_handle(data_manager_type* data);

            //! Set data_manager handle for taskfile
            template <typename data_manager_type>
            void set_data_manager_taskfile(data_manager_type data);

            //! Return data_manager handle for taskfile

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_taskfile(data_manager_type* data);


            //! Return logger
            boost::log::sources::severity_logger<log_severity_level>& get_log()
	            {
                return (this->log_source);
	            }


            //! Return path to data container
            const boost::filesystem::path& data_container_path() const
	            {
                return (this->path_to_data_container);
	            }


            //! Return path to log directory
            const boost::filesystem::path& log_directory_path() const
	            {
                return (this->path_to_log_directory);
	            }


            //! Return path to task-data container
            const boost::filesystem::path& taskfile_path() const
	            {
                return (this->path_to_taskfile);
	            }


            //! Return path to directory for temporary files
            const boost::filesystem::path& temporary_files_path() const
	            {
                return (this->path_to_temp_directory);
	            }


            // INTERNAL DATA

          private:

            const boost::filesystem::path path_to_directory;

            const boost::filesystem::path path_to_data_container;
            const boost::filesystem::path path_to_log_directory;
            const boost::filesystem::path path_to_taskfile;
            const boost::filesystem::path path_to_temp_directory;

            const unsigned int serial_number;
            const unsigned int worker_number;

            void* data_manager_handle;
            void* data_manager_taskfile;

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr<sink_t> log_sink;
	        };


        // OUTPUT GROUPS


        //! Data product descriptor. Used to enumerate the data products associated with an output group.
        class data_product
	        {

          public:

            //! Create a data_product descriptor
            data_product(const std::string& nm, const std::string& ctime, const std::string& pt)
	            : name(nm), path(pt), created(boost::posix_time::time_from_string(ctime))
	            {
	            }


            //! Get dataproduct name
            const std::string& get_product_name() const
	            {
                return (this->name);
	            }


            //! Write self to output stream
            void write(std::ostream& out) const;


            // INTERNAL DATA

          protected:

            //! Name of this data product
            const std::string name;

            //! Path to this product in the repository
            const boost::filesystem::path path;

            //! Creation date
            const boost::posix_time::ptime created;
	        };


        //! Derived product descriptor. Used to enumerate the derived product types associated with a particular task
        class derived_product
	        {

          public:

            //! Create a derived_product descriptor
            derived_product(const std::string& tk, const std::string& pn)
	            : parent_task(tk), name(pn)
	            {
	            }


            //! Destroy a derived_product descriptor
            ~derived_product() = default;


            // INTERFACE

            //! Get product name
            const std::string& get_name() const
	            {
                return (this->name);
	            }


            // INTERNAL DATA

          protected:

            //! Name of parent task. Not ordinarily user-accessible.
            const std::string& parent_task;

            //! Name of this derived product specification
            const std::string& name;
	        };


		    // PAYLOADS FOR OUTPUT GROUPS

		    //! Integration payload
		    class integration_payload
			    {

		      public:

				    //! Create a payload
				    integration_payload(const std::string& be, const std::string& ctr)
		          : backend(be), container(ctr)
					    {
					    }

				    //! Destroy a payload
				    ~integration_payload() = default;


				    // ADMIN

		      public:

		        //! Get name of backend used to generate this output group
		        const std::string& get_backend() const { return(this->backend); }

				    //! Get path of data container
				    const boost::filesystem::path& get_container_path() const { return(this->container); }

				    // INTERNAL DATA

		      protected:

				    //! Backend used to generate this payload
				    const std::string& backend;

				    //! Path to data container
				    boost::filesystem::path container;
			    };


        //! Derived product payload
        class derived_product_payload
	        {

          public:

						//! Create a payload
            derived_product_payload(const std::string& pname, const std::string& fnam)
              : product_name(pname), filename(fnam)
							{
							}

		        //! Destroy a payload
		        ~derived_product_payload() = default;


		        // ADMIN

          public:

		        //! Get name of derived product
	        };


        //! Output group descriptor. Used to enumerate the output groups available for a particular task
		    template <typename Payload>
        class output_group
	        {

          public:

            //! Create an output_group descriptor
            output_group(const std::string& tn, const std::string& be,
                         const boost::filesystem::path& root, const std::string& path,
                         const std::string& ctime, bool lock, const std::list<std::string>& nt,
                         const std::list<std::string>& tg, Payload pld);

            //! Destroy an output_group descriptor
            ~output_group() = default;

            //! Write self to output stream
            void write(std::ostream& out) const;


            // INTERFACE

          public:

            //! Get creation time
            const boost::posix_time::ptime& get_creation_time() const { return (this->created); }


            //! Get path to repository root
            const boost::filesystem::path& get_repo_root_path() const { return (this->repo_root_path); }


            //! Get path to output root
            const boost::filesystem::path& get_data_root_path() const { return (this->data_root_path); }


            //! Get locked flag
            bool get_lock_status() const { return (this->locked); }


            //! Get notes
            const std::list<std::string>& get_notes() const { return (this->notes); }


            //! Get tags
            const std::list<std::string>& get_tags() const { return (this->tags); }

            //! Check whether we match a set of tags
            bool check_tags(std::list<std::string> match_tags) const;

		        //! Get payload
		        const Payload& get_payload() const { return(this->payload); }

            // PRIVATE DATA

          private:

            //! Parent task associated with this output. Not ordinarily user-visible.
            const std::string task;

            //! Path to root of repository. Other paths are relative to this.
            const boost::filesystem::path repo_root_path;

            //! Path of parent directory containing the output, usually residing within the repository
            const boost::filesystem::path data_root_path;

            //! Creation time
            const boost::posix_time::ptime created;

            //! Flag indicating whether or not this output group is locked
            const bool locked;

            //! Array of strings representing notes attached to this group
            const std::list<std::string> notes;

            //! Array of strings representing metadata tags
            const std::list<std::string> tags;

		        //! Payload
		        const Payload payload;
	        };


        // DATA CONTAINER READ HANDLE

        //! Integration container reader: forms a handle for a data container when reading the an integration from the database

        class derived_content_writer
	        {

          public:

            //! Construct a derived-content writer object
            derived_content_writer(const boost::filesystem::path& dir, const boost::filesystem::path& log,
                                   const boost::filesystem::path& task, const boost::filesystem::path& temp,
                                   unsigned int w);

            //! Destroy a derived-content writer object
            ~derived_content_writer();

            //! Set data_manager handle for taskfile
            template <typename data_manager_type>
            void set_data_manager_taskfile(data_manager_type data);

            //! Return data_manager handle for data container

            //! Throws a REPOSITORY_ERROR exception if the handle is unset
            template <typename data_manager_type>
            void get_data_manager_taskfile(data_manager_type* data);


            //! Return logger
            boost::log::sources::severity_logger<log_severity_level>& get_log() { return (this->log_source); }


            //! Return path to log directory
            const boost::filesystem::path& log_directory_path() const { return (this->path_to_log_directory); }


            //! Return path to task-data container
            const boost::filesystem::path& taskfile_path() const { return (this->path_to_taskfile); }


            //! Return path to directory for temporary files
            const boost::filesystem::path& temporary_files_path() const {  return (this->path_to_temp_directory); }


            // INTERNAL DATA

          private:

            const boost::filesystem::path path_to_directory;

            const boost::filesystem::path path_to_log_directory;
            const boost::filesystem::path path_to_taskfile;
            const boost::filesystem::path path_to_temp_directory;

            const unsigned int worker_number;

            void* data_manager_taskfile;

            //! Logger source
            boost::log::sources::severity_logger<log_severity_level> log_source;

            //! Logger sink
            boost::shared_ptr<sink_t> log_sink;

	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a repository object
        repository(const std::string& path, access_type mode)
	        : root_path(path), access_mode(mode)
	        {
	        }


        //! Close a repository, including the corresponding containers and environment. In practice this would always be delegated to the implementation class
        virtual ~repository()
	        {
	        }


        // ADMIN DATA

      public:

        //! Get path to root of repository
        const boost::filesystem::path& get_root_path() const { return (this->root_path); };


        //! Get access mode
        const access_type& get_access_mode() const { return (this->access_mode); }


        // PUSH TASKS TO THE REPOSITORY DATABASE

      public:

        //! Write a 'model/initial conditions/parameters' combination (a 'package') to the package database.
        //! No combination with the supplied name should already exist; if it does, this is considered an error.
        virtual void write_package(const initial_conditions<number>& ics, const model<number>* m) = 0;

        //! Write a threepf integration task to the database.
        virtual void write_task(const twopf_task<number>& t, const model<number>* m) = 0;

        //! Write a twopf integration task to the database
        virtual void write_task(const threepf_task<number>& t, const model<number>* m) = 0;

        //! Write an output task to the database
        virtual void write_task(const output_task<number>& t) = 0;


        // PULL TASKS FROM THE REPOSITORY DATABASE

      public:

        //! Query the database for a named task, and reconstruct it if present.
        //! Supports both integration_task<> and output_task<> items.
        //! Output tasks write nullptr to the model* handle.
        virtual task<number>* lookup_task(const std::string& name, model<number>* m,
                                          typename instance_manager<number>::model_finder finder) = 0;


        // ADD AN OUTPUT-GROUP TO A TASK

      public:

        //! Insert a record for new twopf output in the task database, and set up paths to a suitable data container
        virtual integration_writer new_integration_task_output(twopf_task<number>* tk, const std::list<std::string>& tags,
                                                               const std::string& backend, unsigned int worker) = 0;

        //! Insert a record for new threepf output in the task database, and set up paths to a suitable data container
        virtual integration_writer new_integration_task_output(threepf_task<number>* tk, const std::list<std::string>& tags,
                                                               const std::string& backend, unsigned int worker) = 0;


        // PULL OUTPUT-GROUPS FROM A TASK

      public:

        //! Enumerate the output groups available from a named task
        virtual std::list<output_group> enumerate_integration_task_output(const std::string& name) = 0;


        // PUSH DERIVED-PRODUCT SPECIFICATIONS TO THE DATABASE

      public:

        //! Write a derived product specification
        virtual void write_derived_product(const derived_data::derived_product<number>& d) = 0;


        // PULL DERIVED-PRODUCT SPECIFICATIONS FROM THE DATABASE

      public:

        //! Query a derived product specification
        virtual derived_data::derived_product<number>* lookup_derived_product(const std::string& product, typename instance_manager<number>::model_finder finder) = 0;


        // ADD DERIVED CONTENT FROM AN OUTPUT TASK

      public:

        //! Add derived content
        virtual derived_content_writer new_output_task_output(output_task<number>* tk, const std::list<std::string>& tags,
                                                              unsigned int worker) = 0;

        //! Lookup the output group for a task+derived-product, given a set of tags
        virtual output_group find_derived_product_output_group(const derived_data::derived_product<number>* product, const std::list<std::string>& tags) = 0;


        // PRIVATE DATA

      protected:

        //! Access mode
        const access_type access_mode;

        //! BOOST path to the repository root directory
        const boost::filesystem::path root_path;

	    };


    // INTEGRATION_WRITER METHODS


    template <typename number>
    repository<number>::integration_writer::integration_writer(const boost::filesystem::path& dir, const boost::filesystem::path& data,
                                                               const boost::filesystem::path& log, const boost::filesystem::path& task,
                                                               const boost::filesystem::path& temp, unsigned int n, unsigned int w)
	    : path_to_directory(dir), path_to_data_container(data),
	      path_to_log_directory(log), path_to_taskfile(task),
	      path_to_temp_directory(temp),
	      serial_number(n), worker_number(w),
	      data_manager_handle(nullptr), data_manager_taskfile(nullptr)
	    {
        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;
        boost::filesystem::path log_path = path_to_log_directory / log_file.str();

        boost::shared_ptr<boost::log::core> core                        = boost::log::core::get();

        std::ostringstream                                      log_file_path;
        boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
	                                                                boost::make_shared<boost::log::sinks::text_file_backend>(boost::log::keywords::file_name = log_path.string());

        // enable auto-flushing of log entries
        // this degrades performance, but we are not writing many entries and they
        // will not be lost in the event of a crash
        backend->auto_flush(true);

        // Wrap it into the frontend and register in the core.
        // The backend requires synchronization in the frontend.
        this->log_sink = boost::shared_ptr<sink_t>(new sink_t(backend));

        core->add_sink(this->log_sink);

        boost::log::add_common_attributes();
	    }


    template <typename number>
    repository<number>::integration_writer::~integration_writer()
	    {
        // remove logging objects
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::integration_writer::set_data_manager_handle(data_manager_type data)
	    {
        this->data_manager_handle = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::integration_writer::get_data_manager_handle(data_manager_type* data)
	    {
        if(this->data_manager_handle == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE);
        *data = static_cast<data_manager_type>(this->data_manager_handle);
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::integration_writer::set_data_manager_taskfile(data_manager_type data)
	    {
        this->data_manager_taskfile = static_cast<void*>(data);   // will fail if data_manager_type not (static)-castable to void*
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::integration_writer::get_data_manager_taskfile(data_manager_type* data)
	    {
        if(this->data_manager_taskfile == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETTASK);
        *data = static_cast<data_manager_type>(this->data_manager_taskfile);
	    }


    // output a data_product descriptor to a standard stream
    template <typename number>
    std::ostream& operator<<(std::ostream& out, const typename repository<number>::data_product& product)
	    {
        product.write(out);
        return (out);
	    }


    // DATA_PRODUCT METHODS


    template <typename number>
    void repository<number>::data_product::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_DATAPRODUCT_NAME << " = " << this->name
	        << __CPP_TRANSPORT_DATAPRODUCT_PATH << " = " << this->path
	        << __CPP_TRANSPORT_DATAPRODUCT_CREATED << " " << this->created;
	    }


    // DERIVED_CONTENT_WRITER METHODS


    template <typename number>
    repository<number>::derived_content_writer::derived_content_writer(const boost::filesystem::path& dir, const boost::filesystem::path& log,
                                                                       const boost::filesystem::path& task, const boost::filesystem::path& temp,
                                                                       unsigned int w)
	    : path_to_directory(dir), path_to_log_directory(log),
	      path_to_taskfile(task), path_to_temp_directory(temp),
	      worker_number(w), data_manager_taskfile(nullptr)
	    {
        std::ostringstream log_file;
        log_file << __CPP_TRANSPORT_LOG_FILENAME_A << worker_number << __CPP_TRANSPORT_LOG_FILENAME_B;
        boost::filesystem::path log_path = path_to_log_directory / log_file.str();

        boost::shared_ptr<boost::log::core> core                        = boost::log::core::get();

        std::ostringstream                                      log_file_path;
        boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
	                                                                boost::make_shared<boost::log::sinks::text_file_backend>(boost::log::keywords::file_name = log_path.string());

        // enable auto-flushing of log entries
        // this degrades performance, but we are not writing many entries and they
        // will not be lost in the event of a crash
        backend->auto_flush(true);

        // Wrap it into the frontend and register in the core.
        // The backend requires synchronization in the frontend.
        this->log_sink = boost::shared_ptr<sink_t>(new sink_t(backend));

        core->add_sink(this->log_sink);

        boost::log::add_common_attributes();
	    }


    template <typename number>
    repository<number>::derived_content_writer::~derived_content_writer()
	    {
        // remove logging objects
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        core->remove_sink(this->log_sink);
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::derived_content_writer::set_data_manager_taskfile(data_manager_type data)
	    {
        this->data_manager_taskfile = static_cast<void*>(data);  // will fail if data_manager_type not (static-)castable to void*
	    }


    template <typename number>
    template <typename data_manager_type>
    void repository<number>::derived_content_writer::get_data_manager_taskfile(data_manager_type* data)
	    {
        if(this->data_manager_taskfile == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_DERIVED_WRITER_UNSETTASK);
        *data = static_cast<data_manager_type>(this->data_manager_taskfile);
	    }


    // OUTPUT_GROUP METHODS


    template <typename number>
    repository<number>::output_group::output_group(const std::string& tn, const std::string& be,
                                                   const boost::filesystem::path& root, const std::string& path, const std::string& ctr,
                                                   const std::string& ctime, bool lock, const std::list<std::string>& nt, const std::list<std::string>& tg)
	    : task(tn), backend(be), repo_root_path(root), data_root_path(path), database_path(ctr),
	      created(boost::posix_time::time_from_string(ctime)), locked(lock),
	      notes(nt), tags(tg)
	    {
	    }


    template <typename number>
    void repository<number>::output_group::write(std::ostream& out) const
	    {
        out << __CPP_TRANSPORT_OUTPUT_GROUP
	        << " (" << __CPP_TRANSPORT_OUTPUT_GROUP_BACKEND << " '" << this->backend << "'";
        if(this->locked) out << ", " << __CPP_TRANSPORT_OUTPUT_GROUP_LOCKED;
        out << ")" << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_REPO_ROOT << " = " << this->repo_root_path << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_DATA_ROOT << " = " << this->data_root_path << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_DATA << " = " << this->database_path << std::endl;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_CREATED << " = " << this->created << std::endl;

        unsigned int count = 0;

        for(std::list<std::string>::const_iterator t = this->notes.begin(); t != this->notes.end(); t++)
	        {
            out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_NOTE << " " << count << std::endl;
            out << "    " << *t << std::endl;
            count++;
	        }

        count = 0;
        out << "  " << __CPP_TRANSPORT_OUTPUT_GROUP_TAGS << ": ";
        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); t++)
	        {
            if(count > 0) out << ", ";
            out << *t;
            count++;
	        }
        out << std::endl;

        out << std::endl;
	    }


    template <typename number>
    bool repository<number>::output_group::check_tags(std::list<std::string> match_tags) const
	    {
        // remove all this group's tags from the matching set.
        // If any remain after this process, then the match set isn't a subset of the group's tags.
        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); t++)
	        {
            match_tags.remove(*t);
	        }

        return (!match_tags.empty());
	    }


    // output an output_group descriptor to a standard stream
    template <typename number>
    std::ostream& operator<<(std::ostream& out, const typename repository<number>::output_group& group)
	    {
        group.write(out);
        return (out);
	    }


    namespace output_group_helper
	    {

        // used for sorting a list of output_groups into decreasing chronological order
        template <typename number>
        bool comparator(const typename repository<number>::output_group& A, const typename repository<number>::output_group& B)
	        {
            return (A.get_creation_time() > B.get_creation_time());
	        }

	    }


	}   // namespace transport



#endif //__repository_H_
