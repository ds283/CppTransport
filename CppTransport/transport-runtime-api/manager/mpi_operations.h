//
// Created by David Seery on 22/02/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __mpi_operations_H_
#define __mpi_operations_H_

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"


namespace transport
  {

    namespace MPI
      {

        // MPI messages
        namespace
          {

            // MPI message tags
            const unsigned int NEW_INTEGRATION = 0;
            const unsigned int FINISHED_TASK = 1;
            const unsigned int DATA_READY = 2;
            const unsigned int DELETE_CONTAINER = 3;
		        const unsigned int NEW_DERIVED_CONTENT = 4;
		        const unsigned int CONTENT_READY = 5;
            const unsigned int SET_REPOSITORY = 98;
            const unsigned int TERMINATE = 99;

            // MPI ranks
            const unsigned int RANK_MASTER = 0;

            // MPI message payloads

            class set_repository_payload
              {
              public:
                //! Null constructor (used for receiving messages)
                set_repository_payload()
                  {
                  }

                //! Value constructor (used for constructing messages to send)
                set_repository_payload(const boost::filesystem::path& rp)
                : repository(rp.string())
                  {
                  }

                boost::filesystem::path repository_path() const { return(boost::filesystem::path(this->repository)); }

              private:
                //! Pathname to repository
                std::string repository;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & repository;
                  }
              };


            class new_integration_payload
              {
              public:
                //! Null constructor (used for receiving messages)
                new_integration_payload()
                  {
                  }

                //! Value constructor (used for constructing messages to send)
                new_integration_payload(const std::string& tk,
                                        const boost::filesystem::path& tk_f,
                                        const boost::filesystem::path& tmp_d,
                                        const boost::filesystem::path& log_d)
                : task(tk), taskfile(tk_f.string()), tempdir(tmp_d.string()), logdir(log_d.string())
                  {
                  }

                const std::string& task_name()          const { return(this->task); }
                boost::filesystem::path taskfile_path() const { return(boost::filesystem::path(this->taskfile)); }
                boost::filesystem::path tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }
                boost::filesystem::path logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

              private:
                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to taskfile
                std::string taskfile;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & task;
                    ar & taskfile;
                    ar & tempdir;
                    ar & logdir;
                  }
              };


            class data_ready_payload
              {
              public:

                typedef enum { twopf_payload, threepf_payload } payload_type;

                //! Null constructor (used for receiving messages)
                data_ready_payload()
                  {
                  }

                //! Value constructor (used for sending messages)
                data_ready_payload(const std::string& p, payload_type t)
                : container_path(p), payload(t)
                  {
                  }

                //! Get container path
                const std::string& get_container_path() const { return(this->container_path); }

                //! Get payload type
                payload_type get_payload_type() const { return(this->payload); }

              private:
                //! Path to container
                std::string container_path;

                //! Type of container
                payload_type payload;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
                  {
                    ar & container_path;
                    ar & payload;
                  }
              };


            class new_derived_content_payload
	            {
              public:
                //! Null constructor (used for receiving messages)
                new_derived_content_payload()
	                {
	                }

                //! Value constructor (used for constructing messages to send)
                new_derived_content_payload(const std::string& tk,
                                            const boost::filesystem::path& tk_f,
                                            const boost::filesystem::path& tmp_d,
                                            const boost::filesystem::path& log_d)
	                : task(tk), taskfile(tk_f.string()), tempdir(tmp_d.string()), logdir(log_d.string())
	                {
	                }

                const std::string& task_name()          const { return(this->task); }
                boost::filesystem::path taskfile_path() const { return(boost::filesystem::path(this->taskfile)); }
                boost::filesystem::path tempdir_path()  const { return(boost::filesystem::path(this->tempdir)); }
                boost::filesystem::path logdir_path()   const { return(boost::filesystem::path(this->logdir)); }

              private:

                //! Name of task, to be looked up in repository database
                std::string task;

                //! Pathname to taskfile
                std::string taskfile;

                //! Pathname to directory for temporary files
                std::string tempdir;

                //! Pathname to directory for log files
                std::string logdir;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
	                {
                    ar & task;
                    ar & taskfile;
                    ar & tempdir;
                    ar & logdir;
	                }
	            };


            class content_ready_payload
	            {
              public:

                typedef enum { twopf_payload, threepf_payload } payload_type;

                //! Null constructor (used for receiving messages)
                content_ready_payload()
	                {
	                }

                //! Value constructor (used for sending messages)
                content_ready_payload(const std::string& tk, const std::string& dp, unsigned int og)
	                : task(tk), product(dp), group(og)
	                {
	                }

                //! Get container path
                const std::string& get_task_name() const { return(this->task); }

                //! Get payload type
                const std::string& get_product_name() const { return(this->product); }

		            //! Get output group
		            unsigned int get_output_group() const { return(this->group); }

              private:

                //! Name of task
		            std::string task;

		            //! Name of derived product
		            std::string product;

		            //! Serial number of output group
		            unsigned int group;

                // enable boost::serialization support, and hence automated packing for transmission over MPI
                friend class boost::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, unsigned int version)
	                {
                    ar & task;
                    ar & product;
		                ar & group;
	                }
	            };


          }   // unnamed namespace

      }   // namespace MPI

  } // namespace transport


#endif //__mpi_operations_H_
