//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_CONTROLLER_DECL_H
#define CPPTRANSPORT_SLAVE_CONTROLLER_DECL_H


namespace transport
  {

    // SLAVE FUNCTIONS


    // forward declare dispatch function classes
    template <typename number> class slave_dispatch_function;
    template <typename number> class slave_null_dispatch_function;


    template <typename number>
    class slave_controller
      {


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a slave controller object
        //! unlike a master controller, there is no option to supply a repository;
        //! one has to be provided by a master controller over MPI later
        slave_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                         local_environment& le, argument_cache& ac, model_manager<number>& f,
                         error_handler eh, warning_handler wh, message_handler mh);

        //! destroy a slave manager object
        ~slave_controller() = default;


        // INTERFACE

      public:

        //! poll for instructions to perform work
        void wait_for_tasks(void);


        // MPI FUNCTIONS

      protected:

        //! Get worker number
        unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

        //! Return MPI rank of this process
        unsigned int get_rank(void) const { return(static_cast<unsigned int>(this->world.rank())); }


        // SLAVE JOB HANDLING

      protected:

        //! Slave node: initialize ourselves
        void initialize(const MPI::slave_setup_payload& payload);

        //! Slave node: Pass scheduling data to the master node
        void send_worker_data(model<number>* m);

        //! Slave node: Pass scheduling data to the master node
        void send_worker_data(void);


        // SLAVE INTEGRATION TASKS

      protected:

        //! Slave node: Process a new integration task instruction
        void process_task(const MPI::new_integration_payload& payload);

        //! Slave node: process an integration task
        void dispatch_integration_task(integration_task<number>* tk, const MPI::new_integration_payload& payload);

        //! Slave node: process an integration queue
        template <typename TaskObject, typename BatchObject>
        void schedule_integration(TaskObject* tk, model<number>* m, BatchObject& batcher, unsigned int state_size);

        //! Push a temporary container to the master process
        void push_temp_container(generic_batcher* batcher, unsigned int message, std::string log_message);

        //! Construct a work item filter for a twopf task
        work_item_filter<twopf_kconfig> work_item_filter_factory(twopf_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter<twopf_kconfig>(items); }

        //! Construct a work item filter for a threepf task
        work_item_filter<threepf_kconfig> work_item_filter_factory(threepf_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter<threepf_kconfig>(items); }

        //! Construct a work item filter for a zeta threepf task
        work_item_filter<threepf_kconfig> work_item_filter_factory(zeta_threepf_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter<threepf_kconfig>(items); }

        //! Construct a work item filter factory for an output task
        work_item_filter< output_task_element<number> > work_item_filter_factory(output_task<number>* tk, const std::list<unsigned int>& items) const { return work_item_filter< output_task_element<number> >(items); }


        // SLAVE POSTINTEGRATION TASKS

      protected:

        //! Slave node: Process a new postintegration task instruction
        void process_task(const MPI::new_postintegration_payload& payload);

        //! Slave node: Process a postintegration task
        void dispatch_postintegration_task(postintegration_task<number>* tk, const MPI::new_postintegration_payload& payload);

        //! Slave node: process a postintegration queue
        template <typename TaskObject, typename ParentTaskObject, typename BatchObject>
        void schedule_postintegration(TaskObject* tk, ParentTaskObject* ptk,
                                      const MPI::new_postintegration_payload& payload, BatchObject& batcher);

        //! No-op push content for connexion to datapipe
        void disallow_push_content(datapipe<number>* pipe, typename derived_data::derived_product<number>* product);


        // SLAVE OUTPUT TASKS

      protected:

        //! Slave node: Process a new output task instruction
        void process_task(const MPI::new_derived_content_payload& payload);

        //! Slave node: Process an output task
        void schedule_output(output_task<number>* tk, const MPI::new_derived_content_payload& payload);

        //! Push new derived content to the master process
        void push_derived_content(datapipe<number>* pipe, typename derived_data::derived_product<number>* product, const std::list<std::string>& used_groups);


        friend class slave_dispatch_function<number>;
        friend class slave_null_dispatch_function<number>;


        // INTERNAL DATA

      protected:


        // MPI ENVIRONMENT

        //! BOOST::MPI environment
        boost::mpi::environment& environment;

        //! BOOST::MPI world communicator
        boost::mpi::communicator& world;


        // LOCAL ENVIRONMENT

        //! environment data
        local_environment& local_env;

        //! Argument cache
        argument_cache& arg_cache;


        // MODEL FINDER REFERENCE
        model_manager<number>& finder;


        // RUNTIME AGENTS

        //! Repository manager instance
        std::unique_ptr< json_repository<number> > repo;

        //! Data manager instance
        std::unique_ptr< data_manager<number> > data_mgr;

        //! Handler for postintegration and output tasks
        slave_work_handler<number> work_handler;


        // ERROR CALLBACKS

        //! error callback
        error_handler err;

        //! warning callback
        warning_handler warn;

        //! message callback
        message_handler msg;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_CONTROLLER_DECL_H
