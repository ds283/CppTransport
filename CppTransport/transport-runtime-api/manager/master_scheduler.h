//
// Created by David Seery on 22/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __master_scheduler_H_
#define __master_scheduler_H_


#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/mpi_operations.h"


namespace transport
	{

    class master_scheduler
	    {

      public:

        //! Labels for types of workers
        typedef enum { cpu, gpu } worker_type;

        //! Worker information class
        class worker_information
	        {

          public:

            //! construct a worker information record
            worker_information()
	            : type(cpu),
	              capacity(0),
	              priority(0),
	              initialized(false)
	            {
	            }

            // INTERFACE

          public:

            //! get worker type
            worker_type get_type() const { return(this->type); }

            //! get worker capacity
            unsigned int get_capacity() const { return(this->capacity); }

            //! get worker priority
            unsigned int get_priority() const { return(this->priority); }

            //! get initialization status
            bool get_initialization_status() const { return(this->initialized); }

            //! set data
            void set_data(worker_type t, unsigned int c, unsigned int p) { this->type = t; this->capacity = c; this->priority = p; this->initialized = true; }


            // INTERNAL DATA

          private:

            //! capacity type -- are integrations on this worker limited by memory?
            worker_type type;

            //! worker's memory capacity (for integrations only)
            unsigned int capacity;

            //! worker's priority
            unsigned int priority;

            //! received initialization data from this worker?
            bool initialized;

	        };


		    // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! construct a scheduler object
		    master_scheduler() = default;

		    ~master_scheduler() = default;


		    // INTERFACE

      public:

		    //! reset scheduler and all scheduling data; prepare for new scheduling
		    //! task with the given number of workers
		    void reset(unsigned int worker);

		    //! initialization complete and ready to proceed with scheduling?
		    bool ready() const { return(this->waiting_for_setup == 0); }

		    //! initialize a worker
		    //! reduces count of workers waiting for initialization if successful, and logs the data using the supplied WriterObject
		    //! otherwise, logs an error.
		    template <typename number, typename WriterObject>
		    void initialize_worker(WriterObject& writer, unsigned int worker, MPI::slave_information_payload& payload);


		    // INTERNAL DATA

      private:

        //! Information about workers
        std::vector<worker_information> worker_data;

		    //! Number of workers still to be initialized
		    unsigned int waiting_for_setup;

	    };


		void master_scheduler::reset(unsigned int workers)
			{
				this->worker_data.clear();
				this->worker_data.resize(workers);

				this->waiting_for_setup = workers;
			}


		template <typename number, typename WriterObject>
		void master_scheduler::initialize_worker(WriterObject& writer, unsigned int worker, MPI::slave_information_payload& payload)
			{

		    if(!(this->worker_data[worker].get_initialization_status()))
			    {
		        worker_type type = cpu;
		        if(payload.get_type() == MPI::slave_information_payload::cpu) type = cpu;
		        else if(payload.get_type() == MPI::slave_information_payload::gpu) type = gpu;

		        this->worker_data[worker].set_data(type, payload.get_capacity(), payload.get_priority());
				    this->waiting_for_setup--;

		        std::ostringstream msg;
		        msg << "** Worker " << worker << " identified as ";
		        if(type == cpu)
			        {
		            msg << "CPU";
			        }
		        else if(type == gpu)
			        {
		            msg << "GPU, available memory = " << format_memory(payload.get_capacity());
			        }

		        msg << " and priority " << payload.get_priority();

		        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << msg.str();
			    }
		    else
			    {
		        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "!! Unexpected double identification for worker  " << worker;
			    }

			}

	}   // namespace transport


#endif //__master_scheduler_H_
