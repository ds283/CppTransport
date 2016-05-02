//
// Created by David Seery on 24/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_TASK_MANAGER_H
#define CPPTRANSPORT_TASK_MANAGER_H


#include <functional>

#include "transport-runtime/manager/model_manager.h"
#include "transport-runtime/repository/json_repository.h"

#include "transport-runtime/manager/master_controller.h"
#include "transport-runtime/manager/slave_controller.h"

#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/message_handlers.h"
#include "transport-runtime/manager/task_gallery.h"

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"
#include "transport-runtime/ansi_colour_codes.h"


namespace transport
  {

    //! Task manager is a managed interface to CppTransport's integration and output stacks,
    //! relying on a JSON-aware repository interface 'json_repository'
    //! to handle storage and serialization, and MPI to handle task communication.
    //! The parameter 'number' determines the data type used by the integration engine
    //! and defaults to double
    template <typename number=default_number_type>
    class task_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a task manager using command-line arguments. The repository must exist and be named on the command line.
        task_manager(int argc, char* argv[]);

        //! Destroy a task manager object
        ~task_manager() = default;


		    // INTERFACE -- MANAGE WORK

      public:

		    //! process work
		    void process(void);


        // INTERFACE -- MANAGE TASK GALLERY

        // Tasks in the gallery are available to be written to the database
        // if needed.
        // First, the main database is checked and if

      public:

        //! declare gallery generator
        template <typename GeneratorObject>
        void add_generator(GeneratorObject generator);

        //! declare


        // INTERFACE -- MODEL MANAGEMENT

      public:

        //! Create a model instance of templated type and register it; delegate to instance manager
        template <typename Model>
        std::shared_ptr<Model> create_model() { return(this->model_mgr.template create_model<Model>()); }


        // INTERNAL DATA

      protected:


		    // MPI ENVIRONMENT

        //! BOOST::MPI environment
        boost::mpi::environment environment;

        //! BOOST::MPI world communicator
        boost::mpi::communicator world;


        // LOCAL ENVIRONMENT

        // local environment and argument cache must be declared *first*, so that they are available
        // during construction of the other objects

        // All local objects (model_mgr, gallery, local_env, arg_cache) must be
        // declared before the slave/master component managers, so that
        // they are constructed before slave/master components

        //! environment agent (handles interactions with local machine such as location of Python interpreter)
        local_environment local_env;

        //! Argument cache
        argument_cache arg_cache;

        //! instance manager
        model_manager<number> model_mgr;

        //! task gallery
        task_gallery<number> gallery;


        // SLAVE AND MASTER COMPONENT MANAGERS

        //! slave controller
        slave_controller<number> slave;

        //! master controller
        master_controller<number> master;

      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[])
	    : environment(argc, argv),
        // it's safe to assume local_env and arg_cache have been constructed at this point
        model_mgr(local_env, arg_cache),
	      // note it is safe to assume environment and world have been constructed when the constructor for
	      // slave and master are invoked, because environment and world are declared
	      // prior to slave and master in the class declaration.
        // It's also safe to assume local_env, arg_cache, model_mgr and gallery have been constructed
	      slave(environment, world, local_env, arg_cache, model_mgr,
	            error_handler(local_env, arg_cache),
	            warning_handler(local_env, arg_cache),
	            message_handler(local_env, arg_cache)),
	      master(environment, world, local_env, arg_cache, model_mgr, gallery,
               error_handler(local_env, arg_cache),
               warning_handler(local_env, arg_cache),
               message_handler(local_env, arg_cache))
      {
        if(world.rank() == MPI::RANK_MASTER)  // process command-line arguments if we are the master node
	        {
            master.process_arguments(argc, argv);
	        }
      }


		template <typename number>
		void task_manager<number>::process(void)
			{
        error_handler err(local_env, arg_cache);

				if(this->world.rank() == MPI::RANK_MASTER)
					{
            // output model list, perform other tasks specifically on master node
            try
              {
                this->master.pre_process_tasks();
              }
            catch(runtime_exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT A" << '\n';
#endif
                err(xe.what());
              }
            catch(std::exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT B" << '\n';
#endif
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNEXPECTED_UNHANDLED << " " << xe.what();
                err(msg.str());
              }

            try
              {
                // we must get to execute_tasks() if at all possible, because it sets up a WorkerBundle
                // instance that is responsible for issuing SETUP/TERMINATION notices to workers
                // if the WorkerBundle never gets instantiated then no TERMINATION notices will ever be issued,
                // so the MPI job will hang even if the master node exits
                this->master.execute_tasks();
              }
            catch(runtime_exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT C" << '\n';
#endif
                err(xe.what());
              }
            catch(std::exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT D" << '\n';
#endif
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNEXPECTED_UNHANDLED << " " << xe.what();
                err(msg.str());
              }

            try
              {
                this->master.post_process_tasks();
              }
            catch(runtime_exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT E" << '\n';
#endif
                err(xe.what());
              }
            catch(std::exception& xe)
              {
#ifdef TRACE_OUTPUT
                std::cout << "TRACE_OUTPUT F" << '\n';
#endif
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNEXPECTED_UNHANDLED << " " << xe.what();
                err(msg.str());
              }
					}
				else
					{
						this->slave.wait_for_tasks();
					}
			}


    template <typename number>
    template <typename GeneratorObject>
    void task_manager<number>::add_generator(GeneratorObject obj)
      {
        // pass through to gallery manager
        this->gallery.add_generator(obj);
      }


  } // namespace transport


#endif //CPPTRANSPORT_TASK_MANAGER_H
