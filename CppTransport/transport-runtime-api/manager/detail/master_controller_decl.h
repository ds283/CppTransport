//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H


#include <list>
#include <set>
#include <vector>
#include <memory>
#include <functional>


#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/model_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/repository/json_repository.h"
#include "transport-runtime-api/data/data_manager.h"

#include "transport-runtime-api/manager/master_scheduler.h"
#include "transport-runtime-api/manager/work_journal.h"
#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/message_handlers.h"
#include "transport-runtime-api/manager/task_gallery.h"

#include "transport-runtime-api/manager/detail/job_descriptors.h"
#include "transport-runtime-api/manager/detail/aggregation_forward_declare.h"


#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"


namespace transport
  {

    // aggregator classes forward-declared in aggregation_forward_declare.h
    using namespace master_controller_impl;


    template <typename number>
    class master_controller
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a master controller object
        //! the repository is not set at construction; it has to be provided later via
        //! a command-line or configuration-file option
        master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                          local_environment& le, argument_cache& ac, model_manager<number>& f,
                          task_gallery<number>& g,
                          error_handler eh, warning_handler wh, message_handler mh);

        //! destroy a master manager object
        ~master_controller() = default;


        // INTERFACE

      public:

        //! interpret command-line arguments
        void process_arguments(int argc, char* argv[]);

        //! perform tasks after processing command-line arguments
        void pre_process_tasks();

        //! execute any queued tasks
        void execute_tasks(void);


        // MPI FUNCTIONS

      protected:

        //! Get worker number
        unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

        //! Return MPI rank of this process
        unsigned int get_rank(void) const { return(static_cast<unsigned int>(this->world.rank())); }

        //! Map worker number to communicator rank
        constexpr unsigned int worker_rank(unsigned int worker_number) const { return(worker_number+1); }

        //! Map communicator rank to worker number
        constexpr unsigned int worker_number(unsigned int worker_rank) const { return(worker_rank-1); }


        // MASTER JOB HANDLING

      protected:

        //! Master node: Process a 'task' job.
        //! Some tasks are integrations, others process the numerical output from an integration to product
        //! a derived data product (like fNL).
        //! This function schedules workers to process the task.
        void process_task(const job_descriptor& job);


        // WORKER HANDLING

      protected:

        //! Master node: Terminate all worker processes
        void terminate_workers(void);

        //! Master node: Collect data on workers
        void initialize_workers(void);

        //! Master node: set up workers in preparation for a new task
        void set_up_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log);

        //! Master node: close down workers after completion of a task
        void close_down_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log);

        //! Master node: main loop: poll workers for events
        template <typename WriterObject>
        bool poll_workers(integration_aggregator<number>& int_agg, postintegration_aggregator<number>& post_agg, derived_content_aggregator<number>& derived_agg,
                          integration_metadata& int_metadata, output_metadata& out_metadata, std::list<std::string>& content_groups,
                          WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: generate new work assignments for workers
        void assign_work_to_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log);

        //! Master node: print progress update if it is required
        template <typename WriterObject>
        void check_for_progress_update(WriterObject& writer);

        //! Master node: log current worker metadata
        template <typename WriterObject>
        void log_worker_metadata(WriterObject& writer);


        // MASTER INTEGRATION TASKS

      protected:

        //! Master node: Dispatch an integration task to the worker processes.
        //! Makes a queue then invokes master_dispatch_integration_queue()
        void dispatch_integration_task(integration_task_record<number>& rec, bool seeded, const std::string& seed_group, const std::list<std::string>& tags);

        //! Master node: Dispatch an integration queue to the worker processes.
        template <typename TaskObject>
        void schedule_integration(integration_task_record<number>& rec, TaskObject* tk,
                                  bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                  slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Seed an integration writer using a previous integration
        //! returns list of serial numbers which remain to be integrated
        template <typename TaskObject>
        std::list<unsigned int> seed_writer(integration_writer<number>& writer, TaskObject* tk, const std::string& seed_group);

        //! Master node: Pass new integration task to the workers
        bool integration_task_to_workers(integration_writer<number>& writer,
                                         integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                         slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: debrief after task completion
        void debrief_integration(integration_writer<number>& writer,
                                 integration_metadata& i_metadata, boost::timer::cpu_timer& wallclock_timer);

        //! Master node: respond to an aggregation request
        void aggregate_integration(integration_writer<number>& writer, unsigned int worker, unsigned int id,
                                   MPI::data_ready_payload& payload, integration_metadata &metadata);

        //! Master node: update integration metadata after a worker has finished its tasks
        void update_integration_metadata(MPI::finished_integration_payload& payload, integration_metadata& metadata);


        // MASTER POSTINTEGRATION TASKS

      protected:

        //! Master node: Dispatch a postintegration task to the worker processes.
        void dispatch_postintegration_task(postintegration_task_record<number>& rec, bool seeded, const std::string& seed_group, const std::list<std::string>& tags);

        //! Master node: Dispatch a postintegration queue to the worker processes
        template <typename TaskObject>
        void schedule_postintegration(postintegration_task_record<number>& rec, TaskObject* tk,
                                      bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                      slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Dispatch a paired postintegration queue to the worker processes
        template <typename TaskObject, typename ParentTaskObject>
        void schedule_paired_postintegration(postintegration_task_record<number>& rec, TaskObject* tk, ParentTaskObject* ptk,
                                             bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                             slave_work_event::event_type begin_label,
                                             slave_work_event::event_type end_label);

        //! Master node: Seed an integration writer using a previous integration
        template <typename TaskObject>
        std::list<unsigned int> seed_writer(postintegration_writer<number>& writer, TaskObject* tk, const std::string& seed_group);

        //! Master node: Seed a pair of integration & postintegration writers using a previous integration/postintegration output
        template <typename TaskObject, typename ParentTaskObject>
        std::list<unsigned int> seed_writer_pair(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                 TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group);

        //! Master node: Pass new postintegration task to workers
        bool postintegration_task_to_workers(postintegration_writer<number>& writer, const std::list<std::string>& tags,
                                             integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Pass new paired postintegration task to workers
        bool paired_postintegration_task_to_workers(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                    const std::list<std::string>& tags,
                                                    integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: respond to an aggregation request
        void aggregate_postprocess(postintegration_writer<number>& writer, unsigned int worker, unsigned int id,
                                   MPI::data_ready_payload& payload, output_metadata& metadata);


        // MASTER OUTPUT TASKS

      protected:

        //! Master node: Dispatch an output 'task' (ie., generation of derived data products) to the worker processes
        void dispatch_output_task(output_task_record<number>& rec, const std::list<std::string>& tags);

        //! Master node: Pass new output task to the workers
        bool output_task_to_workers(derived_content_writer<number>& writer, const std::list<std::string>& tags,
                                    integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: debrief after task completion
        template <typename WriterObject>
        void debrief_output(WriterObject& writer, output_metadata& i_metadata, boost::timer::cpu_timer& wallclock_timer);

        //! Master node: respond to a notification of new derived content
        bool aggregate_content(derived_content_writer<number>& writer, unsigned int worker, unsigned int id,
                               MPI::content_ready_payload& payload, output_metadata& metadata);

        //! Master node: update output metadata after a worker has finished its tasks
        template <typename PayloadObject>
        void update_output_metadata(PayloadObject& payload, output_metadata& metadata);

        template <typename PayloadObject>
        void update_content_group_list(PayloadObject& payload, std::list<std::string>& groups);


        // DECLARE AGGREGATOR CLASSES AS FRIENDS

        friend class integration_aggregator<number>;
        friend class postintegration_aggregator<number>;
        friend class derived_content_aggregator<number>;


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

        //! Task gallery
        task_gallery<number>& gallery;


        // RUNTIME AGENTS

        //! Model finder delegate
        model_manager<number>& model_mgr;

        //! Repository manager instance
        std::unique_ptr< json_repository<number> > repo;

        //! Data manager instance
        std::unique_ptr< data_manager<number> > data_mgr;

        //! Event journal
        work_journal journal;


        // DATA AND STATE

        //! scheduler
        master_scheduler work_scheduler;

        //! Queue of tasks to process
        std::list<job_descriptor> job_queue;


        // ERROR CALLBACKS

        //! error callback
        error_handler err;

        //! warning callback
        warning_handler warn;

        //! message callback
        message_handler msg;

      };

  }   // namespace transport

#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H
