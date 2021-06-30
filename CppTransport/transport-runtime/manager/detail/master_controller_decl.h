//
// Created by David Seery on 22/01/2016.
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


#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H


#include <list>
#include <set>
#include <vector>
#include <memory>
#include <functional>


#include "transport-runtime/models/model.h"
#include "transport-runtime/manager/model_manager.h"
#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/output_tasks.h"

#include "transport-runtime/repository/json_repository.h"
#include "transport-runtime/data/data_manager.h"

#include "transport-runtime/manager/worker_scheduler.h"
#include "transport-runtime/manager/worker_manager.h"
#include "transport-runtime/manager/work_journal.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/message_handlers.h"
#include "transport-runtime/manager/task_gallery.h"
#include "transport-runtime/manager/report_manager.h"

#include "transport-runtime/manager/detail/job_descriptors.h"
#include "transport-runtime/manager/detail/aggregation_forward_declare.h"

#include "transport-runtime/reporting/command_line.h"
#include "transport-runtime/reporting/HTML_report.h"

#include "transport-runtime/instruments/busyidle_timer_set.h"


#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/program_options.hpp"


namespace transport
  {

    namespace master_controller_impl
      {
        template <typename number> class Checkpoint_Context;
        template <typename number> class CloseDown_Context;
      }

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
                          task_gallery<number>& g);

        //! destroy a master manager object
        ~master_controller() = default;


        // INTERFACE

      public:

        //! interpret command-line arguments
        void process_arguments(int argc, char* argv[]);

        //! perform errands after processing command-line arguments, but before executing main tasks
        void pre_process_tasks();

        //! perform errands after executing main tasks
        void post_process_tasks();

        //! execute any queued tasks
        void execute_tasks();


      protected:

        //! warn user of unrecognized options or command-line switches
        void warn_unrecognized_switches(boost::program_options::parsed_options& options);

        //! recognize generic options
        void recognize_generic_switches(boost::program_options::variables_map& option_map, boost::program_options::options_description& description);

        //! recognize configuration switches
        void recognize_configuration_switches(boost::program_options::variables_map& option_map);

        //! utility funciton used by recognize_configuration_switches() to read numerical capacities from the
        //! command line
        size_t parse_capacity(const boost::program_options::variables_map& option_map, std::string switch_name);

        //! recognize repository & reporting switches
        void recognize_repository_switches(boost::program_options::variables_map& option_map);

        //! recognize repository action switches
        void recognize_action_switches(boost::program_options::variables_map& option_map);

        //! recognize journalling switches
        void recognize_journal_switches(boost::program_options::variables_map& option_map);

        //! recognize job control switches
        void recognize_job_switches(boost::program_options::variables_map& option_map);

        //! recognize plotting control switches
        void recognize_plot_switches(boost::program_options::variables_map& option_map);
        
        //! recognize task progress reporting switches
        void recognize_report_switches(boost::program_options::variables_map& option_map);

        //! validate tasks supplied on the command line
        void validate_tasks();

        //! autocomplete content groups by scheduling tasks which are needed, but do not yet have content
        void autocomplete_task_schedule();

        //! prune autocomplete task list of tasks which already have content
        void prune_tasks_with_content(record_name_set& required_tasks);

        //! prune autocomplete task list of tasks which are paired with other tasks
        void prune_paired_tasks(record_name_set& required_tasks);

        //! insert job descriptors for required jobs, and sort jobs into correct topological order
        void insert_job_descriptors(const record_name_set& required_tasks, const ordered_record_name_set& order);


        // MPI FUNCTIONS

      protected:

        //! Get worker number
        // TODO: replace with a better abstraction
        unsigned int worker_number() { return(static_cast<unsigned int>(this->world.rank()-1)); }

        //! Return MPI rank of this process
        // TODO: replace with a better abstraction
        unsigned int get_rank() const { return(static_cast<unsigned int>(this->world.rank())); }

        //! Map worker number to communicator rank (note: has to be duplicated in WorkerBundle)
        // TODO: replace with a better abstraction (which can be shared with WorkerPool_Context)
        constexpr unsigned int worker_rank(unsigned int worker_number) const { return(worker_number+1); }

        //! Map communicator rank to worker number
        // TODO: replace with a better abstraction
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

        //! Master node: capture properties reported by workers
        //! (via WORKER_IDENTIFICATION messages) in preparation for a new task
        template <typename WriterObject>
        void capture_worker_properties(WriterObject& writer);
        
        //! Master node: query workers for global load averages
        load_data compute_worker_loads();

        //! Master node: inform workers that there is no more work associated
        //! with the current task
        void workers_end_of_task(base_writer::logger& log);

        //! Master node: main loop: poll workers for events
        template <typename WriterObject>
        bool poll_workers(integration_aggregator<number>& int_agg, postintegration_aggregator<number>& post_agg, derived_content_aggregator<number>& derived_agg,
                          integration_metadata& int_metadata, output_metadata& out_metadata, content_group_name_set& content_groups,
                          WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: generate new work assignments for workers
        void assign_work_to_workers(base_writer::logger& log);
        
        //! Master node: clean up after a work assignment; updates estimate of time-to-completion
        //! and pushes this value to the repository
        template <typename WriterObject, typename PayloadObject>
        void unassign_worker(unsigned int worker, WriterObject& writer, PayloadObject& payload);
    
        //! Master node: log current worker metadata
        template <typename WriterObject>
        void log_worker_metadata(WriterObject& writer);

        //! Master node: instruct workers to change their checkpoint interval
        void set_local_checkpoint_interval(unsigned int m);

        //! Master node: instruct workers to unset any local checkpoint interval
        void unset_local_checkpoint_interval();

        friend class Checkpoint_Context<number>;
        friend class CloseDown_Context<number>;


        // MASTER INTEGRATION TASKS

      protected:

        //! Master node: Dispatch an integration task to the worker processes.
        //! Makes a queue then invokes master_dispatch_integration_queue()
        void dispatch_integration_task(integration_task_record<number>& rec, bool seeded, const std::string& seed_group, const tag_list& tags);

        //! Master node: Dispatch an integration queue to the worker processes.
        template <typename TaskObject>
        void schedule_integration(integration_task_record<number>& rec, TaskObject* tk,
                                  bool seeded, const std::string& seed_group, const tag_list& tags,
                                  slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Seed an integration writer using a previous integration
        //! returns list of serial numbers which remain to be integrated
        template <typename TaskObject>
        serial_number_list seed_writer(integration_writer<number>& writer, TaskObject* tk, const std::string& seed_group);

        //! Master node: Pass new integration task to the workers
        bool integration_task_to_workers(integration_writer<number>& writer,
                                         integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                         slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: debrief after task completion
        void debrief_integration(integration_writer<number>& writer, integration_metadata& i_metadata);

        //! Master node: respond to an aggregation request
        void aggregate_integration(integration_writer<number>& writer, unsigned int worker, unsigned int id,
                                   MPI::data_ready_payload& payload, integration_metadata &metadata);

        //! Master node: update integration metadata after a worker has finished its tasks
        void update_integration_metadata(MPI::finished_integration_payload& payload, integration_metadata& metadata);


        // MASTER POSTINTEGRATION TASKS

      protected:

        //! Master node: Dispatch a postintegration task to the worker processes.
        void dispatch_postintegration_task(postintegration_task_record<number>& rec, bool seeded, const std::string& seed_group, const tag_list& tags);

        //! Master node: validate that a suitable content group exists before scheduling a postintegration task
        void validate_content_group(integration_task<number>* tk, const tag_list& tags);

        //! Master node: validate that a suitable content group exists before scheduling a postintegration task
        void validate_content_group(postintegration_task<number>* tk, const tag_list& tags);

        //! Master node: Dispatch a postintegration queue to the worker processes
        template <typename TaskObject>
        void schedule_postintegration(postintegration_task_record<number>& rec, TaskObject* tk,
                                      bool seeded, const std::string& seed_group, const tag_list& tags,
                                      slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Dispatch a paired postintegration queue to the worker processes
        template <typename TaskObject, typename ParentTaskObject>
        void schedule_paired_postintegration(postintegration_task_record<number>& rec, TaskObject* tk, ParentTaskObject* ptk,
                                             bool seeded, const std::string& seed_group, const tag_list& tags,
                                             slave_work_event::event_type begin_label,
                                             slave_work_event::event_type end_label);

        //! Master node: use parent content group to push settings for a postintegration writer
        void postintegration_settings_from_integration_content(postintegration_writer<number>& writer,
                                                               const std::string& ptk_name, const tag_list& tags);

        //! Master node: use parent content group to push settings for a postintegration writer
        void postintegration_settings_from_postintegration_content(postintegration_writer<number>& writer,
                                                                   const std::string& ptk_name, const tag_list& tags);

        //! Master node: Seed an integration writer using a previous integration
        template <typename TaskObject>
        serial_number_list seed_writer(postintegration_writer<number>& writer, TaskObject* tk, const std::string& seed_group);

        //! Master node: Seed a pair of integration & postintegration writers using a previous integration/postintegration output
        template <typename TaskObject, typename ParentTaskObject>
        serial_number_list seed_writer_pair(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                 TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group);

        //! Master node: Pass new postintegration task to workers
        bool postintegration_task_to_workers(postintegration_writer<number>& writer, const tag_list& tags,
                                             integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Pass new paired postintegration task to workers
        bool paired_postintegration_task_to_workers(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                    const tag_list& tags,
                                                    integration_aggregator<number>& i_agg, postintegration_aggregator<number>& p_agg, derived_content_aggregator<number>& d_agg,
                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: respond to an aggregation request
        void aggregate_postprocess(postintegration_writer<number>& writer, unsigned int worker, unsigned int id,
                                   MPI::data_ready_payload& payload, output_metadata& metadata);


        // MASTER OUTPUT TASKS

      protected:

        //! Master node: Dispatch an output 'task' (ie., generation of derived data products) to the worker processes
        void dispatch_output_task(output_task_record<number>& rec, const tag_list& tags);

        //! Master node: Pass new output task to the workers
        bool output_task_to_workers(derived_content_writer<number>& writer, const tag_list& tags,
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
        void update_content_group_list(PayloadObject& payload, content_group_name_set& groups);


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

        //! Boost::ProgramArguments variables_map
        boost::program_options::variables_map option_map;


        // RUNTIME AGENTS

        //! Model finder delegate
        model_manager<number>& model_mgr;

        //! Repository manager instance
        std::unique_ptr< json_repository<number> > repo;

        //! Data manager instance
        std::unique_ptr< data_manager<number> > data_mgr;

        //! Event journal
        work_journal journal;

        //! Command-line reporting tool
        reporting::command_line cmdline_reports;

        //! HTML reporting tool
        reporting::HTML_report HTML_reports;
    
    
        // PROFILING SUPPORT
    
        //! list of aggregation profile records
        std::list< aggregation_profiler > aggregation_profiles;
    
        //! root directory for aggregation profile report, if used
        boost::optional< boost::filesystem::path > aggregation_profile_root;
    
    
        // TIMERS
    
        //! track time spent performing work
        busyidle_timer_set busyidle_timers;
        
        //! when was estimated completion data last pushed to the repository?
        boost::posix_time::ptime last_push_to_repo;


        // AGENTS

        //! work scheduler
        worker_scheduler work_scheduler;
        
        //! worker manager
        worker_manager work_manager;
        
        //! report manager
        report_manager reporter;

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


    namespace master_controller_impl
      {

        template <typename number>
        class WorkerPool_Context
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor performs setup of workers belonging to the MPI environment
            WorkerPool_Context(boost::mpi::environment& e, boost::mpi::communicator& c,
                         repository<number>* r, work_journal& j, argument_cache& a,
                         busyidle_timer_set& t);

            //! destructor handles terminatiaon of workers belonging to the MPI environment
            ~WorkerPool_Context();


            // INTERNAL FUNCTIONS

          protected:

            //! Map worker number to communicator rank
            // TODO: replace with a better abstraction
            constexpr unsigned int worker_rank(unsigned int worker_number) const { return(worker_number+1); }


            // INTERNAL DATA

          private:

            //! reference to MPI environment
            boost::mpi::environment& env;

            //! reference to MPI communicator
            boost::mpi::communicator& world;

            //! pointer to repository object
            repository<number>* repo;

            //! reference to work journal
            work_journal& journal;

            //! reference to argument cache
            argument_cache& args;
            
            //! busy/idle timing collection
            busyidle_timer_set& busyidle_timers;
            
          };


        template <typename number>
        WorkerPool_Context<number>::WorkerPool_Context(boost::mpi::environment& e, boost::mpi::communicator& c,
                                           repository<number>* r, work_journal& j, argument_cache& a,
                                           busyidle_timer_set& t)
          : env(e),
            world(c),
            repo(r),
            journal(j),
            args(a),
            busyidle_timers(t)
          {
            // capture busy/idle timers and switch to busy mode
            busyidle_instrument timers(busyidle_timers);
            
            // set up instrument to journal the MPI communication if needed
            journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

            std::vector<boost::mpi::request> requests(world.size()-1);

            // if repository is initialized, send WORKER_SETUP message
            if(repo != nullptr)
              {
                // request information from each worker, and pass all necessary setup details
                MPI::slave_setup_payload payload(this->repo->get_root_path(), args);

                for(unsigned int i = 0; i < world.size()-1; ++i)
                  {
                    requests[i] = world.isend(this->worker_rank(i), MPI::WORKER_SETUP, payload);
                  }

                // wait for all messages to be received, then return
                // there is no scheduled response to this message, so we don't need to wait for one
                timers.idle();
                boost::mpi::wait_all(requests.begin(), requests.end());
              }
          }


        template <typename number>
        WorkerPool_Context<number>::~WorkerPool_Context()
          {
            // capture busy/idle timers and switch to busy mode
            busyidle_instrument timers(this->busyidle_timers);

            // set up instrument to journal the MPI communication if needed
            journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

            std::vector<boost::mpi::request> requests(this->world.size()-1);

            for(unsigned int i = 0; i < this->world.size()-1; ++i)
              {
                requests[i] = this->world.isend(this->worker_rank(i), MPI::TERMINATE);
              }

            // wait for all messages to be received, then exit ourselves
            timers.idle();
            boost::mpi::wait_all(requests.begin(), requests.end());
          }


        template <typename number>
        class Checkpoint_Context
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor accepts and stores reference to controller object
            Checkpoint_Context(master_controller<number>& c)
              : controller(c),
                unset(false)
              {
              }

            //! destructor arranges for MPI message to reset checkpoint interval, if required
            ~Checkpoint_Context();


            // INTERFACE

          public:

            //! instruct us to unset the local checkpoint interval on destruction
            void requires_unset() { this->unset = true; }


            // INTERNAL DATA

          private:

            //! reference to controller object
            master_controller<number>& controller;

            //! does a local checkpoint setting need removing?
            bool unset;

          };


        template <typename number>
        Checkpoint_Context<number>::~Checkpoint_Context()
          {
            if(this->unset) controller.unset_local_checkpoint_interval();
          }


        template <typename number>
        class CloseDown_Context
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor accepts and stores reference to controller object
            CloseDown_Context(master_controller<number>& c, base_writer::logger& l)
              : controller(c),
                log(l),
                sent_closedown(false)
              {
              }

            //! destructor arranges for MPI closedown message to be sent, if required
            ~CloseDown_Context() { if(!this->sent_closedown) this->send_closedown(); }


            // INTERFACE

          public:

            //! check whether close down message has been sent
            bool operator()() const { return(this->sent_closedown); }

            //! send closedown message
            void send_closedown()
              {
                this->controller.workers_end_of_task(log);
                this->sent_closedown = true;
              }


            // INTERNAL DATA

          private:

            //! reference to controller object
            master_controller<number>& controller;

            //! flag representing whether closedown message has been sent
            bool sent_closedown;

            //! reference to logger
            base_writer::logger& log;

          };

      }

  }   // namespace transport

#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_DECL_H
