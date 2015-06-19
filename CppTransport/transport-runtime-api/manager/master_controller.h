//
// Created by David Seery on 19/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __master_controller_H_
#define __master_controller_H_


#include <list>
#include <set>
#include <vector>
#include <memory>
#include <functional>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/repository/json_repository.h"
#include "transport-runtime-api/data/data_manager.h"

#include "transport-runtime-api/manager/master_scheduler.h"
#include "transport-runtime-api/manager/work_journal.h"
#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/environment.h"

#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/scheduler.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"


namespace transport
	{

    // MASTER FUNCTIONS

		template <typename number>
		class master_controller
			{

				// TYPES

		  public:

		    //! Error-reporting callback object
		    typedef std::function<void(const std::string&)> error_callback;

		    //! Warning callback object
		    typedef std::function<void(const std::string&)> warning_callback;

		    //! Message callback object
		    typedef std::function<void(const std::string&)> message_callback;

				//! Integration-task aggregation handler
				typedef std::function<void(unsigned int,unsigned int,MPI::data_ready_payload&,integration_metadata&)> integration_aggregator;

				//! Postintegration-task aggregation handler
				typedef std::function<void(unsigned int,unsigned int,MPI::data_ready_payload&,output_metadata&)> postintegration_aggregator;

				//! Output-task aggregation handler
				typedef std::function<bool(unsigned int,unsigned int,MPI::content_ready_payload&,output_metadata&)> derived_content_aggregator;

				//! Labels for types of job
		    typedef enum { job_task, job_get_package, job_get_task, job_get_product, job_get_content } job_type;


				//! Job descriptor class
		    class job_descriptor
			    {

		      public:

		        job_descriptor(job_type t, const std::string& n, const std::list<std::string>& tg, const std::string& o)
			        : type(t),
			          name(n),
			          tags(tg),
			          output(o),
		            seeded(false)
			        {
			        }

		        job_descriptor(job_type t, const std::string& n, const std::list<std::string>& tg)
			        : type(t),
			          name(n),
			          tags(tg),
		            seeded(false)
			        {
			        }


		        // INTERFACE

		      public:

		        job_type                      get_type()                     const { return(this->type); }

		        const std::string&            get_name()                     const { return(this->name); }

		        const std::list<std::string>& get_tags()                     const { return(this->tags); }

		        const std::string&            get_output()                   const { return(this->output); }

				    void                          set_seed(const std::string& s)       { this->seeded = true; this->seed_group = s; }

				    bool                          is_seeded()                    const { return(this->seeded); }

				    const std::string&            get_seed_group()               const { return(this->seed_group); };


		        // INTERNAL DATA

		      private:

		        //! job type
		        job_type               type;

		        //! job name
		        std::string            name;

		        //! tags associated with job
		        std::list<std::string> tags;

		        //! output destination, if needed
		        std::string            output;

				    //! is this job seeded
				    bool                   seeded;

				    //! seed group, if used
				    std::string            seed_group;

			    };


				class aggregation_record
					{
				  public:
						aggregation_record(unsigned int w, unsigned int i)
							: worker(w),

							  id(i)
							{
							}

						virtual ~aggregation_record() = default;

						virtual void aggregate() = 0;

						unsigned int get_id() const { return(this->id); }

						unsigned int get_worker() const { return(this->worker); }

				  private:
						unsigned int worker;
						unsigned int id;
					};


				class integration_aggregation_record: public aggregation_record
					{
				  public:
						integration_aggregation_record(unsigned int w, unsigned int id, integration_aggregator& agg, integration_metadata& m, MPI::data_ready_payload& p)
							: aggregation_record(w, id),
							  handler(agg),
				        payload(p),
								metadata(m)
							{
							}

						virtual ~integration_aggregation_record() = default;

						virtual void aggregate() { this->handler(this->get_worker(), this->get_id(), payload, metadata); }

				  private:
						integration_aggregator& handler;
						integration_metadata& metadata;
						MPI::data_ready_payload payload;
					};


		    class postintegration_aggregation_record: public aggregation_record
			    {
		      public:
		        postintegration_aggregation_record(unsigned int w, unsigned int id, postintegration_aggregator& agg, output_metadata& m, MPI::data_ready_payload& p)
			        : aggregation_record(w, id),
			          handler(agg),
			          payload(p),
			          metadata(m)
			        {
			        }

		        virtual ~postintegration_aggregation_record() = default;

		        virtual void aggregate() { this->handler(this->get_worker(), this->get_id(), payload, metadata); }

		      private:
		        postintegration_aggregator& handler;
		        output_metadata& metadata;
		        MPI::data_ready_payload payload;
			    };


		    class derived_content_aggregation_record: public aggregation_record
			    {
		      public:
		        derived_content_aggregation_record(unsigned int w, unsigned int id, derived_content_aggregator& agg, output_metadata& m, MPI::content_ready_payload& p)
			        : aggregation_record(w, id),
			          handler(agg),
			          payload(p),
			          metadata(m)
			        {
			        }

		        virtual ~derived_content_aggregation_record() = default;

		        virtual void aggregate() { this->handler(this->get_worker(), this->get_id(), payload, metadata); }

		      private:
		        derived_content_aggregator& handler;
		        output_metadata& metadata;
		        MPI::content_ready_payload payload;
			    };


				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a master controller object with no supplied repository
				//! (one has to be provided in the command line arguments later)
				master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
				                  error_callback err, warning_callback warn, message_callback msg,
				                  unsigned int bcp = CPPTRANSPORT_DEFAULT_BATCHER_STORAGE,
				                  unsigned int pcp = CPPTRANSPORT_DEFAULT_PIPE_STORAGE);

				//! construct a master controller object with a supplied repository
				master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
				                  json_repository<number>* r,
				                  error_callback err, warning_callback warn, message_callback msg,
				                  unsigned int bcp = CPPTRANSPORT_DEFAULT_BATCHER_STORAGE,
				                  unsigned int pcp = CPPTRANSPORT_DEFAULT_PIPE_STORAGE);

				//! destroy a master manager object
				~master_controller();


				// INTERFACE

		  public:

				//! interpret command-line arguments
				void process_arguments(int argc, char* argv[], instance_manager<number>& instance_mgr);
				
		    //! execute any queued tasks
		    void execute_tasks(void);

				//! expose arguments
				const argument_cache& get_arguments(void) { return(this->arg_cache); }


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
				bool poll_workers(integration_aggregator& int_agg, postintegration_aggregator& post_agg, derived_content_aggregator& derived_agg,
				                  integration_metadata& int_metadata, output_metadata& out_metadata, std::list<std::string>& content_groups,
				                  WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

				//! Master node: generate new work assignments for workers
				void assign_work_to_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log);

        //! Master node: print progress update if it is required
				template <typename WriterObject>
        void check_for_progress_update(WriterObject& writer);


		    // MASTER INTEGRATION TASKS

		  protected:

		    //! Master node: Dispatch an integration task to the worker processes.
		    //! Makes a queue then invokes master_dispatch_integration_queue()
		    void dispatch_integration_task(integration_task_record<number>* rec, bool seeded, const std::string& seed_group, const std::list<std::string>& tags);

		    //! Master node: Dispatch an integration queue to the worker processes.
		    template <typename TaskObject>
		    void schedule_integration(integration_task_record<number>* rec, TaskObject* tk,
                                  bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
		                              slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Seed an integration writer using a previous integration
        //! returns list of serial numbers which remain to be integrated
        template <typename TaskObject>
        std::list<unsigned int> seed_writer(std::shared_ptr< integration_writer<number> >& writer, TaskObject* tk, const std::string& seed_group);

		    //! Master node: Pass new integration task to the workers
		    bool integration_task_to_workers(std::shared_ptr< integration_writer<number> >& writer,
                                         integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                         slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: debrief after task completion
        void debrief_integration(std::shared_ptr< integration_writer<number> >& writer,
                                 integration_metadata& i_metadata, boost::timer::cpu_timer& wallclock_timer);

		    //! Master node: respond to an aggregation request
		    void aggregate_integration(std::shared_ptr<integration_writer<number> > &writer, unsigned int worker, unsigned int id,
		                               MPI::data_ready_payload& payload, integration_metadata &metadata);

		    //! Master node: update integration metadata after a worker has finished its tasks
		    void update_integration_metadata(MPI::finished_integration_payload& payload, integration_metadata& metadata);


		    // MASTER POSTINTEGRATION TASKS

		  protected:

		    //! Master node: Dispatch a postintegration task to the worker processes.
		    void dispatch_postintegration_task(postintegration_task_record<number>* rec, bool seeded, const std::string& seed_group, const std::list<std::string>& tags);

		    //! Master node: Dispatch a postintegration queue to the worker processes
		    template <typename TaskObject>
		    void schedule_postintegration(postintegration_task_record<number>* rec, TaskObject* tk,
                                      bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
		                                  slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Dispatch a paired postintegration queue to the worker processes
        template <typename TaskObject, typename ParentTaskObject>
        void schedule_paired_postintegration(postintegration_task_record<number>* rec, TaskObject* tk, ParentTaskObject* ptk,
                                             bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                             slave_work_event::event_type begin_label,
                                             slave_work_event::event_type end_label);

        //! Master node: Seed an integration writer using a previous integration
        template <typename TaskObject>
        std::list<unsigned int> seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, TaskObject* tk, const std::string& seed_group);

        //! Master node: Seed a pair of integration & postintegration writers using a previous integration/postintegration output
        template <typename TaskObject, typename ParentTaskObject>
        std::list<unsigned int> seed_writer_pair(std::shared_ptr<integration_writer<number> >& i_writer, std::shared_ptr<postintegration_writer<number> >& p_writer,
                                                 TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group);

        //! Master node: Pass new postintegration task to workers
		    bool postintegration_task_to_workers(std::shared_ptr< postintegration_writer<number> >& writer, const std::list<std::string>& tags,
                                             integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: Pass new paired postintegration task to workers
        bool paired_postintegration_task_to_workers(std::shared_ptr< integration_writer<number> >& i_writer, std::shared_ptr< postintegration_writer<number> >& p_writer,
                                                    const std::list<std::string>& tags,
                                                    integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

		    //! Master node: respond to an aggregation request
		    void aggregate_postprocess(std::shared_ptr< postintegration_writer<number> >& writer, unsigned int worker, unsigned int id,
		                               MPI::data_ready_payload& payload, output_metadata& metadata);


		    // MASTER OUTPUT TASKS

		  protected:

		    //! Master node: Dispatch an output 'task' (ie., generation of derived data products) to the worker processes
		    void dispatch_output_task(output_task_record<number>* rec, const std::list<std::string>& tags);

		    //! Master node: Pass new output task to the workers
		    bool output_task_to_workers(std::shared_ptr <derived_content_writer<number>>& writer, const std::list<std::string>& tags,
                                    integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label);

        //! Master node: debrief after task completion
				template <typename WriterObject>
        void debrief_output(WriterObject& writer, output_metadata& i_metadata, boost::timer::cpu_timer& wallclock_timer);

		    //! Master node: respond to a notification of new derived content
		    bool aggregate_content(std::shared_ptr< derived_content_writer<number> >& writer, unsigned int worker, unsigned int id,
		                           MPI::content_ready_payload& payload, output_metadata& metadata);

		    //! Master node: update output metadata after a worker has finished its tasks
		    template <typename PayloadObject>
		    void update_output_metadata(PayloadObject& payload, output_metadata& metadata);

        template <typename PayloadObject>
        void update_content_group_list(PayloadObject& payload, std::list<std::string>& groups);


		    // INTERNAL DATA

		  protected:


		    // MPI ENVIRONMENT

		    //! BOOST::MPI environment
		    boost::mpi::environment& environment;

		    //! BOOST::MPI world communicator
		    boost::mpi::communicator& world;


        // LOCAL ENVIRONMENT
        local_environment local_env;


		    // RUNTIME AGENTS

		    //! Repository manager instance
		    json_repository<number>* repo;

		    //! Data manager instance
		    data_manager<number>* data_mgr;

				//! Event journal
				work_journal journal;

				//! Argument cache
				argument_cache arg_cache;


		    // DATA AND STATE

				//! scheduler
				master_scheduler work_scheduler;

		    //! Queue of tasks to process
		    std::list<job_descriptor> job_queue;

		    //! Storage capacity per batcher
		    unsigned int batcher_capacity;

		    //! Data cache capacity per datapipe
		    unsigned int pipe_capacity;


		    // ERROR CALLBACKS

		    //! error callback
		    error_callback error_handler;

		    //! warning callback
		    warning_callback warning_handler;

		    //! message callback
		    message_callback message_handler;

			};


    template <typename number>
    master_controller<number>::master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                                 error_callback err, warning_callback warn, message_callback msg,
                                                 unsigned int bcp, unsigned int pcp)
	    : environment(e),
	      world(w),
	      repo(nullptr),
	      data_mgr(data_manager_factory<number>(bcp, pcp)),
	      journal(w.size()-1),
	      batcher_capacity(bcp),
	      pipe_capacity(pcp),
	      error_handler(err),
	      warning_handler(warn),
	      message_handler(msg)
	    {
	    }


    template <typename number>
    master_controller<number>::master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                                 json_repository<number>* r,
                                                 error_callback err, warning_callback warn, message_callback msg,
                                                 unsigned int bcp, unsigned int pcp)
	    : environment(e),
	      world(w),
	      repo(r),
	      data_mgr(data_manager_factory<number>(bcp, pcp)),
	      journal(w.size()-1),
	      batcher_capacity(bcp),
	      pipe_capacity(pcp),
	      error_handler(err),
	      warning_handler(warn),
	      message_handler(msg)
	    {
		    assert(repo != nullptr);
	    }

		
    template <typename number>
    master_controller<number>::~master_controller()
	    {
		    // delete repository instance, if it is set
        if(this->repo != nullptr)
	        {
            delete this->repo;
	        }
	    }
		
		
		template <typename number>
		void master_controller<number>::process_arguments(int argc, char* argv[], instance_manager<number>& instance_mgr)
			{
        // set up Boost::program_options descriptors for command-line arguments
        boost::program_options::options_description generic("Generic options");
        generic.add_options()
          (CPPTRANSPORT_SWITCH_HELP,    CPPTRANSPORT_HELP_HELP)
          (CPPTRANSPORT_SWITCH_VERSION, CPPTRANSPORT_HELP_VERSION)
          (CPPTRANSPORT_SWITCH_MODELS,  CPPTRANSPORT_HELP_MODELS);

        boost::program_options::options_description configuration("Configuration options");
        configuration.add_options()
          (CPPTRANSPORT_SWITCH_VERBOSE,                                                                                      CPPTRANSPORT_HELP_VERBOSE)
          (CPPTRANSPORT_SWITCH_REPO,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_REPO)
          (CPPTRANSPORT_SWITCH_TAG,              boost::program_options::value< std::vector<std::string> >(),                CPPTRANSPORT_HELP_TAG)
          (CPPTRANSPORT_SWITCH_CAPACITY,         boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CAPACITY)
          (CPPTRANSPORT_SWITCH_BATCHER_CAPACITY, boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_BATCHER_CAPACITY)
          (CPPTRANSPORT_SWITCH_CACHE_CAPACITY,   boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CACHE_CAPACITY)
          (CPPTRANSPORT_SWITCH_GANTT_CHART,      boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_GANTT_CHART);

        boost::program_options::options_description job_options("Job specification");
        job_options.add_options()
          (CPPTRANSPORT_SWITCH_TASK,             boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_TASK)
          (CPPTRANSPORT_SWITCH_SEED,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_SEED);

        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(configuration).add(job_options);

        boost::program_options::variables_map option_map;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), option_map);
        boost::program_options::notify(option_map);

        bool emitted_version = false;

        if(option_map.count(CPPTRANSPORT_SWITCH_VERSION))
          {
            std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << std::endl;
            emitted_version = true;
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_HELP))
          {
            if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << std::endl;
            std::cout << cmdline_options << std::endl;
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_MODELS))
          {
            this->arg_cache.set_model_list(true);
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_REPO))
          {
            try
              {
                this->repo = repository_factory<number>(option_map[CPPTRANSPORT_SWITCH_REPO].as<std::string>(),
                                                        repository<number>::access_type::readwrite,
                                                        this->error_handler, this->warning_handler, this->message_handler);
                this->repo->set_model_finder(instance_mgr.model_finder_factory());
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == runtime_exception::REPO_NOT_FOUND)
                  {
                    this->error_handler(xe.what());
                    repo = nullptr;
                  }
                else
                  {
                    throw xe;
                  }
              }
          }

        // populate list of tags
        std::list<std::string> tags;
        if(option_map.count(CPPTRANSPORT_SWITCH_TAG) > 0)
          {
            std::vector<std::string> tmp = option_map[CPPTRANSPORT_SWITCH_TAG].as<std::vector<std::string> >();

            // copy tags into std::list tags
            // Boost::program_arguments doesn't support lists, so we have to do it this way
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tags));
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_CAPACITY].as<int>() * 1024*1024;
            if(capacity > 0)
              {
                this->batcher_capacity = this->pipe_capacity = static_cast<unsigned int>(capacity);
                this->data_mgr->set_batcher_capacity(this->batcher_capacity);
                this->data_mgr->set_pipe_capacity(this->pipe_capacity);
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_CAPACITY;
                this->error_handler(msg.str());
              }
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_CACHE_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_CACHE_CAPACITY].as<int>() * 1024*1024;
            if(capacity > 0)
              {
                this->pipe_capacity = static_cast<unsigned int>(capacity);
                this->data_mgr->set_pipe_capacity(this->pipe_capacity);
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_CACHE_CAPACITY;
                this->error_handler(msg.str());
              }
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_BATCHER_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_BATCHER_CAPACITY].as<int>() * 1024*1024;
            if(capacity > 0)
              {
                this->batcher_capacity = static_cast<unsigned int>(capacity);
                this->data_mgr->set_batcher_capacity(this->batcher_capacity);
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_BATCHER_CAPACITY;
                this->error_handler(msg.str());
              }
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_GANTT_CHART))
          {
            this->arg_cache.set_gantt_chart(true);
            this->arg_cache.set_gantt_filename(option_map[CPPTRANSPORT_SWITCH_GANTT_CHART].as<std::string>());
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_VERBOSE_LONG))
          {
            this->arg_cache.set_verbose(true);
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_TASK))
          {
            std::vector<std::string> tasks = option_map[CPPTRANSPORT_SWITCH_TASK].as< std::vector<std::string> >();

            for(std::vector<std::string>::const_iterator t = tasks.begin(); t != tasks.end(); ++t)
              {
                job_queue.push_back(job_descriptor(job_task, *t, tags));

                if(option_map.count(CPPTRANSPORT_SWITCH_SEED))
                  {
                    job_queue.back().set_seed(option_map[CPPTRANSPORT_SWITCH_SEED].as<std::string>());
                  }
              }
          }
      }
		

    template <typename number>
    void master_controller<number>::execute_tasks(void)
	    {
        if(!(this->get_rank() == 0)) throw runtime_exception(runtime_exception::MPI_ERROR, CPPTRANSPORT_EXEC_SLAVE);

        if(this->repo == nullptr)
	        {
            this->error_handler(CPPTRANSPORT_REPO_NONE);
	        }
        else
	        {
            boost::timer::cpu_timer timer;

		        // set up workers
		        this->initialize_workers();

		        unsigned int database_tasks = 0;
            for(typename std::list<job_descriptor>::const_iterator t = this->job_queue.begin(); t != this->job_queue.end(); ++t)
	            {
                switch((*t).get_type())
	                {
                    case job_task:
	                    {
                        this->process_task(*t);
		                    database_tasks++;
                        break;
	                    }

                    default:
	                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_UNKNOWN_JOB_TYPE);
	                }
	            }

		        timer.stop();
		        if(database_tasks > 0)
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_PROCESSED_TASKS_A << " " << database_tasks << " ";
				        if(database_tasks > 1)
					        {
						        msg << CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL;
					        }
				        else
					        {
						        msg << CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR;
					        }
		            msg << " " << CPPTRANSPORT_PROCESSED_TASKS_C << " " << format_time(timer.elapsed().wall);
				        msg << " | " << CPPTRANSPORT_PROCESSED_TASKS_D << " ";

		            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
				        msg << boost::posix_time::to_simple_string(now);

				        this->message_handler(msg.str());
			        }
	        }

        // there is no more work, so ask all workers to shut down
        // and then exit ourselves
        this->terminate_workers();

		    if(this->arg_cache.get_gantt_chart()) this->journal.make_gantt_chart(this->arg_cache.get_gantt_filename(), this->local_env);
	    }


    template <typename number>
    void master_controller<number>::process_task(const job_descriptor& job)
	    {
        try
	        {
            // query a task record with the name we're looking for from the database
            std::unique_ptr< task_record<number> > record(this->repo->query_task(job.get_name()));

            switch(record->get_type())
	            {
                case task_record<number>::integration:
	                {
                    integration_task_record<number>* int_rec = dynamic_cast< integration_task_record<number>* >(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->dispatch_integration_task(int_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                    break;
	                }

                case task_record<number>::output:
	                {
                    output_task_record<number>* out_rec = dynamic_cast< output_task_record<number>* >(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->dispatch_output_task(out_rec, job.get_tags());
                    break;
	                }

                case task_record<number>::postintegration:
	                {
                    postintegration_task_record<number>* pint_rec = dynamic_cast< postintegration_task_record<number>* >(record.get());

                    assert(pint_rec != nullptr);
                    if(pint_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->dispatch_postintegration_task(pint_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                    break;
	                }

                default:
	                {
                    assert(false);

                    std::ostringstream msg;
                    msg << CPPTRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << job.get_name() << "'";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	                }
	            }
	        }
        catch (runtime_exception xe)
	        {
            if(xe.get_exception_code() == runtime_exception::RECORD_NOT_FOUND)
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	            || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << CPPTRANSPORT_REPO_FOR_TASK << " '" << job.get_name() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->error_handler(msg.str());
	            }
            else throw xe;
	        }
	    }


		// INTEGRATION TASKS


    template <typename number>
    void master_controller<number>::dispatch_integration_task(integration_task_record<number>* rec, bool seeded, const std::string& seed_group,
                                                              const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        integration_task<number>* tk = rec->get_task();
        model<number>* m = rec->get_task()->get_model();

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast< twopf_task<number>* >(tk)) != nullptr)
	        {
		        this->work_scheduler.set_state_size(m->backend_twopf_state_size());
            this->work_scheduler.prepare_queue(*tka);
            this->schedule_integration(rec, tka, seeded, seed_group, tags, slave_work_event::begin_twopf_assignment, slave_work_event::end_twopf_assignment);
	        }
        else if((tkb = dynamic_cast< threepf_task<number>* >(tk)) != nullptr)
	        {
		        this->work_scheduler.set_state_size(m->backend_threepf_state_size());
            this->work_scheduler.prepare_queue(*tkb);
            this->schedule_integration(rec, tkb, seeded, seed_group, tags, slave_work_event::begin_threepf_assignment, slave_work_event::end_threepf_assignment);
	        }
        else
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject>
    void master_controller<number>::schedule_integration(integration_task_record<number>* rec, TaskObject* tk,
                                                         bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                         slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
        assert(rec != nullptr);

        // create an output writer to commit the result of this integration to the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr< integration_writer<number> > writer = this->repo->new_integration_task_content(rec, tags, this->get_rank(), 0);

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // write the various tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // seed writer if a group has been provided
        if(seeded) this->seed_writer(writer, tk, seed_group);

        // set up aggregators
        integration_aggregator     i_agg = std::bind(&master_controller<number>::aggregate_integration, this, writer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        postintegration_aggregator p_agg;
        derived_content_aggregator d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ NEW INTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << *tk;

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        bool success = this->integration_task_to_workers(writer, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::database_begin, master_work_event::database_end);

        // perform integrity check
        writer->check_integrity(tk);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
	    }


    template <typename number>
    template <typename TaskObject>
    std::list<unsigned int> master_controller<number>::seed_writer(std::shared_ptr< integration_writer<number> >& writer, TaskObject* tk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        std::list< std::shared_ptr< output_group_record<integration_payload> > > list = this->repo->enumerate_integration_task_content(tk->get_name());

        // find the specified group in this list
        std::list< std::shared_ptr< output_group_record<integration_payload> > >::const_iterator t = std::find_if(list.begin(), list.end(),
                                                                                                                  OutputGroupFinder<integration_payload>(seed_group));

        if(t == list.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warning_handler(msg.str());
            return std::list<unsigned int>{};
          }

        // mark writer as seeded
        writer->set_seed(seed_group);

        // get workgroup number used by seed
        unsigned int seed_workgroup = (*t)->get_payload().get_workgroup_number();
        writer->set_workgroup_number(seed_workgroup+1);

        this->data_mgr->seed_writer(writer, tk, *t);
        this->work_scheduler.prepare_queue((*t)->get_payload().get_failed_serials());

        return((*t)->get_payload().get_failed_serials());
      }


    template <typename number>
    bool master_controller<number>::integration_task_to_workers(std::shared_ptr <integration_writer<number>>& writer,
                                                                integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                                                slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;

        // aggregate integration data reported by worker processes
        integration_metadata   i_metadata;
        output_metadata        o_metadata;      // unused
        std::list<std::string> content_groups;  // unused

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer->get_abs_logdir_path();

        {
	        // journal_instrument will log time spent doing MPI when it goes out of scope
	        journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

	        std::vector<boost::mpi::request> requests(this->world.size()-1);
	        MPI::new_integration_payload payload(writer->get_record()->get_name(), tempdir_path, logdir_path, writer->get_workgroup_number());

	        for(unsigned int i = 0; i < this->world.size()-1; ++i)
		        {
	            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_INTEGRATION, payload);
		        }

	        // wait for all messages to be received
	        boost::mpi::wait_all(requests.begin(), requests.end());
	        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ All workers received NEW_INTEGRATION instruction";
        }

		    bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        // push task metadata we have collected to the writer
        writer->set_metadata(i_metadata);
        wallclock_timer.stop();
        this->debrief_integration(writer, i_metadata, wallclock_timer);

		    return(success);
	    }


    template <typename number>
    void master_controller<number>::debrief_integration(std::shared_ptr< integration_writer<number> >& writer,
                                                        integration_metadata& i_metadata, boost::timer::cpu_timer& wallclock_timer)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total wallclock time required by worker processes = " << format_time(i_metadata.total_wallclock_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total aggregation time required by master process = " << format_time(i_metadata.total_aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ AGGREGATE PERFORMANCE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Workers processed " << i_metadata.total_configurations << " individual integrations";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   " << i_metadata.total_failures << " integrations failed, and " << i_metadata.total_refinements << " integrations required mesh refinement (may not match individual k-configurations for some backends)";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total integration time    = " << format_time(i_metadata.total_integration_time) << " | global mean integration time = " << format_time(i_metadata.total_integration_time/(i_metadata.total_configurations > 0 ? i_metadata.total_configurations : 1));
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Min mean integration time = " << format_time(i_metadata.min_mean_integration_time) << " | global min integration time = " << format_time(i_metadata.global_min_integration_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Max mean integration time = " << format_time(i_metadata.max_mean_integration_time) << " | global max integration time = " << format_time(i_metadata.global_max_integration_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total batching time       = " << format_time(i_metadata.total_batching_time) << " | global mean batching time = " << format_time(i_metadata.total_batching_time/(i_metadata.total_configurations > 0 ? i_metadata.total_configurations : 1));
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Min mean batching time    = " << format_time(i_metadata.min_mean_batching_time) << " | global min batching time = " << format_time(i_metadata.global_min_batching_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Max mean batching time    = " << format_time(i_metadata.max_mean_batching_time) << " | global max batching time = " << format_time(i_metadata.global_max_batching_time);
      }


    template <typename number>
    void master_controller<number>::aggregate_integration(std::shared_ptr<integration_writer<number> > &writer, unsigned int worker, unsigned int id,
                                                          MPI::data_ready_payload& payload, integration_metadata &metadata)
	    {
        journal_instrument instrument(this->journal, master_work_event::aggregate_begin, master_work_event::aggregate_end, id);

        // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        writer->aggregate(payload.get_container_path());

        aggregate_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(aggregate_timer.elapsed().wall);
        metadata.total_aggregation_time += aggregate_timer.elapsed().wall;

        // inform scheduler of a new aggregation
        this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error_handler(msg.str());
	        }
	    }


    template <typename number>
    void master_controller<number>::update_integration_metadata(MPI::finished_integration_payload& payload, integration_metadata& metadata)
	    {
        metadata.total_wallclock_time += payload.get_wallclock_time();

        metadata.total_integration_time += payload.get_integration_time();
        boost::timer::nanosecond_type mean_integration_time = payload.get_integration_time() / (payload.get_num_integrations() > 0 ? payload.get_num_integrations() : 1);
        if(metadata.max_mean_integration_time == 0 || mean_integration_time > metadata.max_mean_integration_time) metadata.max_mean_integration_time = mean_integration_time;
        if(metadata.min_mean_integration_time == 0 || mean_integration_time < metadata.min_mean_integration_time) metadata.min_mean_integration_time = mean_integration_time;

        if(metadata.global_max_integration_time == 0 || payload.get_max_integration_time() > metadata.global_max_integration_time) metadata.global_max_integration_time = payload.get_max_integration_time();
        if(metadata.global_min_integration_time == 0 || payload.get_min_integration_time() < metadata.global_min_integration_time) metadata.global_min_integration_time = payload.get_min_integration_time();

        metadata.total_batching_time += payload.get_batching_time();
        boost::timer::nanosecond_type mean_batching_time = payload.get_batching_time() / (payload.get_num_integrations() > 0 ? payload.get_num_integrations() : 1);
        if(metadata.max_mean_batching_time == 0 || mean_batching_time > metadata.max_mean_batching_time) metadata.max_mean_batching_time = mean_batching_time;
        if(metadata.min_mean_batching_time == 0 || mean_batching_time < metadata.min_mean_batching_time) metadata.min_mean_batching_time = mean_batching_time;

        if(metadata.global_max_batching_time == 0 || payload.get_max_batching_time() > metadata.global_max_batching_time) metadata.global_max_batching_time = payload.get_max_batching_time();
        if(metadata.global_min_batching_time == 0 || payload.get_min_batching_time() < metadata.global_min_batching_time) metadata.global_min_batching_time = payload.get_min_batching_time();

        metadata.total_configurations += payload.get_num_integrations();
        metadata.total_failures += payload.get_num_failures();
        metadata.total_refinements += payload.get_num_refinements();
	    }


		// OUTPUT TASKS


    template <typename number>
    void master_controller<number>::dispatch_output_task(output_task_record<number>* rec, const std::list<std::string>& tags)
	    {
		    assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        output_task<number>* tk = rec->get_task();

		    this->work_scheduler.set_state_size(sizeof(number));
		    this->work_scheduler.prepare_queue(*tk);

        // set up an derived_content_writer object to coordinate logging, output destination and commits into the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr< derived_content_writer<number> > writer = this->repo->new_output_task_content(rec, tags, this->get_rank());

        // set up the writer for us
        this->data_mgr->initialize_writer(writer);

        // set up aggregators
        integration_aggregator     i_agg;
        postintegration_aggregator p_agg;
        derived_content_aggregator d_agg = std::bind(&master_controller<number>::aggregate_content, this, writer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ NEW OUTPUT TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << *tk;

        // instruct workers to carry out their tasks
        bool success = this->output_task_to_workers(writer, tags, i_agg, p_agg, d_agg, slave_work_event::begin_output_assignment, slave_work_event::end_output_assignment);

        journal_instrument instrument(this->journal, master_work_event::database_begin, master_work_event::database_end);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output to the database if successful
        if(success) writer->commit();
	    }


    template <typename number>
    bool master_controller<number>::output_task_to_workers(std::shared_ptr< derived_content_writer<number> >& writer, const std::list<std::string>& tags,
                                                           integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                                           slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used in this task
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata;      // unused
        output_metadata        o_metadata;
        std::list<std::string> content_groups;  // unused

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer->get_abs_logdir_path();

        {
	        // journal_instrument will log time spent doing MPI when it goes out of scope
	        journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

	        std::vector<boost::mpi::request> requests(this->world.size()-1);
	        MPI::new_derived_content_payload payload(writer->get_record()->get_name(), tempdir_path, logdir_path, tags);

	        for(unsigned int i = 0; i < this->world.size()-1; ++i)
		        {
	            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_DERIVED_CONTENT, payload);
		        }

	        // wait for all messages to be received
	        boost::mpi::wait_all(requests.begin(), requests.end());
	        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ All workers received NEW_DERIVED_CONTENT instruction";
        }

		    bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        writer->set_metadata(o_metadata);
        wallclock_timer.stop();
        this->debrief_output(writer, o_metadata, wallclock_timer);

        return(success);
	    }


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::debrief_output(WriterObject& writer, output_metadata& o_metadata, boost::timer::cpu_timer& wallclock_timer)
      {
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total work time required by worker processes      = " << format_time(o_metadata.work_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total aggregation time required by master process = " << format_time(o_metadata.aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total time spent reading database                 = " << format_time(o_metadata.db_time);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total time-configuration cache hits               = " << o_metadata.time_config_hits << ", unloads = " << o_metadata.time_config_unloads;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total twopf k-config cache hits                   = " << o_metadata.twopf_kconfig_hits << ", unloads = " << o_metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total threepf k-config cache hits                 = " << o_metadata.threepf_kconfig_hits << ", unloads = " << o_metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total statistics cache hits                       = " << o_metadata.stats_hits << ", unloads = " << o_metadata.stats_unloads;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total data cache hits                             = " << o_metadata.data_hits << ", unloads = " << o_metadata.data_unloads;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total time config cache evictions                 = " << format_time(o_metadata.time_config_evictions);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total twopf k-config cache evictions              = " << format_time(o_metadata.twopf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total threepf k-config cache evictions            = " << format_time(o_metadata.threepf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total statistics cache evictions                  = " << format_time(o_metadata.stats_evictions);
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++   Total data cache evictions                        = " << format_time(o_metadata.data_evictions);
      }


    template <typename number>
    bool master_controller<number>::aggregate_content(std::shared_ptr< derived_content_writer<number> >& writer, unsigned int worker, unsigned int id,
                                                      MPI::content_ready_payload& payload, output_metadata& metadata)
	    {
        journal_instrument instrument(this->journal, master_work_event::aggregate_begin, master_work_event::aggregate_end, id);

        // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        bool success = writer->aggregate(payload.get_product_name(), payload.get_content_groups());

        aggregate_timer.stop();
        metadata.aggregation_time += aggregate_timer.elapsed().wall;

		    // inform scheduler of a new aggregation
		    this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

        return (success);
	    }


    template <typename number>
    template <typename PayloadObject>
    void master_controller<number>::update_output_metadata(PayloadObject& payload, output_metadata& metadata)
	    {
        metadata.work_time                 += payload.get_cpu_time();
        metadata.db_time                   += payload.get_database_time();
        metadata.time_config_hits          += payload.get_time_config_hits();
        metadata.time_config_unloads       += payload.get_time_config_unloads();
        metadata.time_config_evictions     += payload.get_time_config_evictions();
        metadata.twopf_kconfig_hits        += payload.get_twopf_kconfig_hits();
        metadata.twopf_kconfig_unloads     += payload.get_twopf_kconfig_unloads();
        metadata.twopf_kconfig_evictions   += payload.get_twopf_kconfig_evictions();
        metadata.threepf_kconfig_hits      += payload.get_threepf_kconfig_hits();
        metadata.threepf_kconfig_unloads   += payload.get_threepf_kconfig_unloads();
        metadata.threepf_kconfig_evictions += payload.get_threepf_kconfig_evictions();
        metadata.stats_hits                += payload.get_stats_hits();
        metadata.stats_unloads             += payload.get_stats_unloads();
        metadata.stats_evictions           += payload.get_stats_evictions();
        metadata.data_hits                 += payload.get_data_hits();
        metadata.data_unloads              += payload.get_data_unloads();
        metadata.data_evictions            += payload.get_data_evictions();
	    }


    template <typename number>
    template <typename PayloadObject>
    void master_controller<number>::update_content_group_list(PayloadObject& payload, std::list<std::string>& groups)
      {
        std::list<std::string> this_group = payload.get_content_groups();

        this_group.sort();
        groups.merge(this_group);
        groups.unique();
      }


		// POSTINTEGRATION TASKS


    template <typename number>
    void master_controller<number>::dispatch_postintegration_task(postintegration_task_record<number>* rec, bool seeded, const std::string& seed_group,
                                                                  const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, CPPTRANSPORT_TOO_FEW_WORKERS);

        postintegration_task<number>* tk = rec->get_task();

        zeta_twopf_task<number>*   z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>*          zfNL = nullptr;

        if((z2pf = dynamic_cast< zeta_twopf_task<number>* >(tk)) != nullptr)
	        {
            twopf_task<number>* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            // is this 2pf task paired?
            if(z2pf->is_paired())
              {
                model<number>* m = ptk->get_model();
                this->work_scheduler.set_state_size(m->backend_twopf_state_size());
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_paired_postintegration(rec, z2pf, ptk, seeded, seed_group, tags, slave_work_event::begin_twopf_assignment, slave_work_event::end_twopf_assignment);
              }
            else
              {
                this->work_scheduler.set_state_size(sizeof(number));
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_postintegration(rec, z2pf, seeded, seed_group, tags, slave_work_event::begin_zeta_twopf_assignment, slave_work_event::end_zeta_twopf_assignment);
              }
	        }
        else if((z3pf = dynamic_cast< zeta_threepf_task<number>* >(tk)) != nullptr)
	        {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_THREEPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            // is this 3pf task apired?
            if(z3pf->is_paired())
              {
                model<number>* m = ptk->get_model();
                this->work_scheduler.set_state_size(m->backend_threepf_state_size());
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_paired_postintegration(rec, z3pf, ptk, seeded, seed_group, tags, slave_work_event::begin_threepf_assignment, slave_work_event::end_threepf_assignment);
              }
            else
              {
                this->work_scheduler.set_state_size(sizeof(number));
                this->work_scheduler.prepare_queue(*ptk);
                this->schedule_postintegration(rec, z3pf, seeded, seed_group, tags, slave_work_event::begin_zeta_threepf_assignment, slave_work_event::end_zeta_threepf_assignment);
              }
	        }
        else if((zfNL = dynamic_cast< fNL_task<number>* >(tk)) != nullptr)
	        {
            zeta_threepf_task<number>* ptk = dynamic_cast<zeta_threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_THREEPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

		        this->work_scheduler.set_state_size(sizeof(number));
		        this->work_scheduler.prepare_queue(*ptk);
            this->schedule_postintegration(rec, zfNL, false, "", tags, slave_work_event::begin_fNL_assignment, slave_work_event::end_fNL_assignment);
	        }
        else
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject>
    void master_controller<number>::schedule_postintegration(postintegration_task_record<number>* rec, TaskObject* tk,
                                                             bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                             slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
        assert(rec != nullptr);

        // create an output writer to commit our results into the repository
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr< postintegration_writer<number> > writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // create new tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // seed writer if a group has been provided
        if(seeded) this->seed_writer(writer, tk, seed_group);

        // set up aggregators
        integration_aggregator     i_agg;
        postintegration_aggregator p_agg = std::bind(&master_controller<number>::aggregate_postprocess, this, writer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        derived_content_aggregator d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ NEW POSTINTEGRATION TASK '" << tk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << *tk;

        // instruct workers to carry out the calculation
        bool success = this->postintegration_task_to_workers(writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::database_begin, master_work_event::database_end);

        // perform integrity check
        writer->check_integrity(tk);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
	    }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    void master_controller<number>::schedule_paired_postintegration(postintegration_task_record<number>* rec, TaskObject* tk, ParentTaskObject* ptk,
                                                                    bool seeded, const std::string& seed_group, const std::list<std::string>& tags,
                                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(rec != nullptr);

        std::unique_ptr< task_record<number> > pre_prec(this->repo->query_task(ptk->get_name()));
        integration_task_record<number>* prec = dynamic_cast< integration_task_record<number>* >(pre_prec.get());

        assert(prec != nullptr);
        if(prec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // create an output writer for the postintegration task
        std::shared_ptr< postintegration_writer<number> > p_writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());
        this->data_mgr->initialize_writer(p_writer);
        this->data_mgr->create_tables(p_writer, tk);

        // create an output writer for the integration task; use suffix option to add "-paired" to distinguish the different output groups
        std::shared_ptr<integration_writer<number> > i_writer = this->repo->new_integration_task_content(prec, tags, this->get_rank(), 0, "paired");
        this->data_mgr->initialize_writer(i_writer);
        this->data_mgr->create_tables(i_writer, ptk);

        // pair
        p_writer->set_pair(true);
        p_writer->set_parent_group(i_writer->get_name());

        // seed writers if a group has been provided
        if(seeded) this->seed_writer_pair(i_writer, p_writer, tk, ptk, seed_group);

        // set up aggregators
        integration_aggregator     i_agg = std::bind(&master_controller<number>::aggregate_integration, this, i_writer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        postintegration_aggregator p_agg = std::bind(&master_controller<number>::aggregate_postprocess, this, p_writer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        derived_content_aggregator d_agg;

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::normal) << "++ NEW PAIRED POSTINTEGRATION TASKS '" << tk->get_name() << "' & '" << ptk->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(i_writer->get_log(), base_writer::normal) << *ptk;

        // instruct workers to carry out the calculation
        bool success = this->paired_postintegration_task_to_workers(i_writer, p_writer, tags, i_agg, p_agg, d_agg, begin_label, end_label);

        journal_instrument instrument(this->journal, master_work_event::database_begin, master_work_event::database_end);

        // perform integrity check
        i_writer->check_integrity(ptk);
        p_writer->check_integrity(tk);

        // close both writers
        this->data_mgr->close_writer(i_writer);
        this->data_mgr->close_writer(p_writer);

        // commit output if successful
        if(success)
          {
            i_writer->commit();
            p_writer->commit();
          }
      }


    template <typename number>
    template <typename TaskObject>
    std::list<unsigned int> master_controller<number>::seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, TaskObject* tk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        std::list< std::shared_ptr< output_group_record<postintegration_payload> > > list = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list
        std::list< std::shared_ptr< output_group_record<postintegration_payload> > >::const_iterator t = std::find_if(list.begin(), list.end(),
                                                                                                                      OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == list.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warning_handler(msg.str());
            return std::list<unsigned int>{};
          };

        // mark writer as seeded
        writer->set_seed(seed_group);

        this->data_mgr->seed_writer(writer, tk, *t);
        this->work_scheduler.prepare_queue((*t)->get_payload().get_failed_serials());

        return((*t)->get_payload().get_failed_serials());
      }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject>
    std::list<unsigned int> master_controller<number>::seed_writer_pair(std::shared_ptr<integration_writer<number> >& i_writer,
                                                                        std::shared_ptr<postintegration_writer<number> >& p_writer,
                                                                        TaskObject* tk, ParentTaskObject* ptk, const std::string& seed_group)
      {
        // enumerate the content groups available for our own task
        std::list< std::shared_ptr< output_group_record<postintegration_payload> > > list = this->repo->enumerate_postintegration_task_content(tk->get_name());

        // find the specified group in this list
        std::list< std::shared_ptr< output_group_record<postintegration_payload> > >::const_iterator t = std::find_if(list.begin(), list.end(),
                                                                                                                      OutputGroupFinder<postintegration_payload>(seed_group));

        if(t == list.end())   // no record found
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A << " '" << seed_group << "' " << CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B << " '" << tk->get_name() << "'";
            this->warning_handler(msg.str());
            return std::list<unsigned int>{};
          }

        // find parent content group for the seed
        std::string parent_seed_name = (*t)->get_payload().get_parent_group();

        std::list<unsigned int> integration_serials = this->seed_writer(i_writer, ptk, parent_seed_name);

        if(i_writer->is_seeded())
          {
            std::list<unsigned int> postintegration_serials = (*t)->get_payload().get_failed_serials();

            if(postintegration_serials.size() != integration_serials.size())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << (*t)->get_name() << "' "
                  << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                  << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }

            std::list<unsigned int> diff;
            std::set_difference(integration_serials.begin(), integration_serials.end(),
                                postintegration_serials.begin(), postintegration_serials.end(), std::back_inserter(diff));

            if(diff.size() > 0)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A << " '" << (*t)->get_name() << "' "
                  << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B << " '" << parent_seed_name << "' "
                  << CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C;
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
              }

            p_writer->set_seed(seed_group);
            this->data_mgr->seed_writer(p_writer, tk, *t);
          }

        return((*t)->get_payload().get_failed_serials());
      }


    template <typename number>
    bool master_controller<number>::postintegration_task_to_workers(std::shared_ptr< postintegration_writer<number> >& writer, const std::list<std::string>& tags,
                                                                    integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                                                    slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata;  // unused
        output_metadata        o_metadata;
        std::list<std::string> content_groups;

        // get paths the workers will need
        boost::filesystem::path tempdir_path = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path  = writer->get_abs_logdir_path();

        {
	        // journal_instrument will log time spent doing MPI when it goes out of scope
	        journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

	        std::vector<boost::mpi::request> requests(this->world.size()-1);
	        MPI::new_postintegration_payload payload(writer->get_record()->get_name(), tempdir_path, logdir_path, tags);

	        for(unsigned int i = 0; i < this->world.size()-1; ++i)
		        {
	            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
		        }

	        // wait for all messages to be received
	        boost::mpi::wait_all(requests.begin(), requests.end());
	        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";
        }

		    bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, writer, begin_label, end_label);

        if(content_groups.size() > 1) throw runtime_exception(runtime_exception::RUNTIME_ERROR,  CPPTRANSPORT_POSTINTEGRATION_MULTIPLE_GROUPS);

        writer->set_parent_group(content_groups.front());
        writer->set_metadata(o_metadata);
        wallclock_timer.stop();
        this->debrief_output(writer, o_metadata, wallclock_timer);

        return(success);
	    }


    template <typename number>
    bool master_controller<number>::paired_postintegration_task_to_workers(std::shared_ptr< integration_writer<number> >& i_writer,
                                                                           std::shared_ptr< postintegration_writer<number> >& p_writer,
                                                                           const std::list<std::string>& tags,
                                                                           integration_aggregator& i_agg, postintegration_aggregator& p_agg, derived_content_aggregator& d_agg,
                                                                           slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        assert(this->repo != nullptr);

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        integration_metadata   i_metadata;
        output_metadata        o_metadata;
        std::list<std::string> content_groups;  // unused

        // get paths needed by different batchers on each worker
        boost::filesystem::path i_tempdir_path = i_writer->get_abs_tempdir_path();
        boost::filesystem::path i_logdir_path  = i_writer->get_abs_logdir_path();

        boost::filesystem::path p_tempdir_path = p_writer->get_abs_tempdir_path();
        boost::filesystem::path p_logdir_path  = p_writer->get_abs_logdir_path();

        {
          // journal instrument will log time spent doing MPI when it goes out of scope
          journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

          std::vector<boost::mpi::request> requests(this->world.size()-1);
          MPI::new_postintegration_payload payload(p_writer->get_record()->get_name(), p_tempdir_path, p_logdir_path, tags, i_tempdir_path, i_logdir_path, i_writer->get_workgroup_number());

          for(unsigned int i = 0; i < this->world.size()-1; ++i)
            {
              requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
            }

          // wait for all messages to be received
          boost::mpi::wait_all(requests.begin(), requests.end());
          BOOST_LOG_SEV(i_writer->get_log(), base_writer::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";
        }

        bool success = this->poll_workers(i_agg, p_agg, d_agg, i_metadata, o_metadata, content_groups, i_writer, begin_label, end_label);

        if(content_groups.size() > 1) throw runtime_exception(runtime_exception::RUNTIME_ERROR,  CPPTRANSPORT_POSTINTEGRATION_MULTIPLE_GROUPS);

        i_writer->set_metadata(i_metadata);
        p_writer->set_metadata(o_metadata);

        wallclock_timer.stop();
        this->debrief_integration(i_writer, i_metadata, wallclock_timer);

        return(success);
      }


    template <typename number>
    void master_controller<number>::aggregate_postprocess(std::shared_ptr< postintegration_writer<number> >& writer, unsigned int worker, unsigned int id,
                                                          MPI::data_ready_payload& payload, output_metadata& metadata)
	    {
		    journal_instrument instrument(this->journal, master_work_event::aggregate_begin, master_work_event::aggregate_end, id);

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer aggregate_timer;

        writer->aggregate(payload.get_container_path());

        aggregate_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(aggregate_timer.elapsed().wall);
        metadata.aggregation_time += aggregate_timer.elapsed().wall;

        // inform scheduler of a new aggregation
        this->work_scheduler.report_aggregation(aggregate_timer.elapsed().wall);

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error_handler(msg.str());
	        }
	    }


		// WORKER HANDLING


    template <typename number>
    void master_controller<number>::initialize_workers(void)
	    {
        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // we require this->repo not to be null
        assert(this->repo != nullptr);
		    if(this->repo == nullptr) throw std::runtime_error(CPPTRANSPORT_REPO_NOT_SET);

		    // request information from each worker, and pass all necessary setup details
        MPI::slave_setup_payload payload(this->repo->get_root_path(), this->batcher_capacity, this->pipe_capacity);

        for(unsigned int i = 0; i < this->world.size()-1; ++i)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::INFORMATION_REQUEST, payload);
	        }

        // wait for all messages to be received, then return
        boost::mpi::wait_all(requests.begin(), requests.end());
	    }


		template <typename number>
    void master_controller<number>::set_up_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log)
			{
		    // set up instrument to journal the MPI communication if needed
		    journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

		    // rebuild information about our workers; this information
				// it is updated whenever we start a new task, because the details can vary
				// between model instances (eg. CPU or GPU backends)
				this->work_scheduler.reset(this->world.size()-1);

		    while(!this->work_scheduler.is_ready())
			    {
		        boost::mpi::status stat = this->world.probe();

				    switch(stat.tag())
					    {
				        case MPI::INFORMATION_RESPONSE:
					        {
				            MPI::slave_information_payload payload;
						        this->world.recv(stat.source(), MPI::INFORMATION_RESPONSE, payload);
						        this->work_scheduler.initialize_worker(log, this->worker_number(stat.source()), payload);
						        break;
					        }

				        default:
					        {
						        BOOST_LOG_SEV(log, base_writer::normal) << "!! Received unexpected MPI message " << stat.tag() << " from worker " << stat.source() << "; discarding";
						        this->world.recv(stat.source(), stat.tag());
						        break;
					        }
					    };
			    }

				// now all data is available, ask scheduler to fix maximum unit of allocation
		    this->work_scheduler.complete_queue_setup();
	    }


		template <typename number>
		void master_controller<number>::close_down_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log)
			{
		    BOOST_LOG_SEV(log, base_writer::normal) << "++ Notifying workers of end-of-work";

				for(unsigned int i = 0; i < this->world.size()-1; ++i)
					{
						this->world.isend(this->worker_rank(i), MPI::END_OF_WORK);
					}
			}


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::check_for_progress_update(WriterObject& writer)
      {
        // emit update message giving current status if required
        std::string msg;
        bool print_msg = this->work_scheduler.generate_update_string(msg);
        if(print_msg)
          {
            std::ostringstream update_msg;
            update_msg << CPPTRANSPORT_TASK_MANAGER_LABEL << " " << msg;
            this->message_handler(update_msg.str());

						BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "±± Console advisory message: " << update_msg.str();
          }
      }


		template <typename number>
		void master_controller<number>::assign_work_to_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log)
			{
		    // set up instrument to journal the MPI communication if needed
		    journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

        // generate a list of work assignments
        std::list<master_scheduler::work_assignment> work = this->work_scheduler.assign_work(log);

        BOOST_LOG_SEV(log, base_writer::normal) << "++ Generating new work assignments for " << work.size() << " worker" << (work.size() > 1 ? "s" : "") << " (" << this->work_scheduler.get_queue_size() << " work items remain in queue)";

        // push assignments to workers
        std::vector<boost::mpi::request> msg_status(work.size());

        unsigned int c = 0;
        for(std::list<master_scheduler::work_assignment>::const_iterator t = work.begin(); t != work.end(); ++t, ++c)
	        {
            MPI::work_assignment_payload payload(t->get_items());

		        // send message to worker with new assignment information
            msg_status[c] = this->world.isend(this->worker_rank(t->get_worker()), MPI::NEW_WORK_ASSIGNMENT, payload);

		        // mark this worker, and these work items, as assigned
            this->work_scheduler.mark_assigned(*t);
		        BOOST_LOG_SEV(log, base_writer::normal) << "++ Assigned " << t->get_items().size() << " work items to worker " << t->get_worker() << " [MPI rank=" << this->worker_rank(t->get_worker()) << "]";
	        }

        // wait for all assignments to be received
        boost::mpi::wait_all(msg_status.begin(), msg_status.end());
		    BOOST_LOG_SEV(log, base_writer::normal) << "++ All workers accepted new assignments";
			}


    template <typename number>
    void master_controller<number>::terminate_workers(void)
	    {
		    // set up instrument to journal the MPI communication if needed
		    journal_instrument instrument(this->journal, master_work_event::MPI_begin, master_work_event::MPI_end);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        for(unsigned int i = 0; i < this->world.size()-1; ++i)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::TERMINATE);
	        }

        // wait for all messages to be received, then exit ourselves
        boost::mpi::wait_all(requests.begin(), requests.end());
	    }


    template <typename number>
    template <typename WriterObject>
    bool master_controller<number>::poll_workers(integration_aggregator& int_agg, postintegration_aggregator& post_agg, derived_content_aggregator& derived_agg,
                                                 integration_metadata& int_metadata, output_metadata& out_metadata, std::list<std::string>& content_groups,
                                                 WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
	    {
		    bool success = true;

        boost::log::sources::severity_logger <base_writer::log_severity_level>& log = writer->get_log();

        // set up aggregation queues
		    unsigned int aggregation_counter = 0;
        std::list< std::shared_ptr<aggregation_record> > aggregation_queue;

        // wait for workers to report their characteristics
        this->set_up_workers(log);

        // record time of last-received message, so we can determine for how long we have been idle
        boost::posix_time::ptime last_msg = boost::posix_time::second_clock::universal_time();
        bool emit_agg_queue_msg = true;

        // poll workers, scattering work and aggregating the results until work items are exhausted
        bool sent_closedown = false;
        while(!this->work_scheduler.all_inactive())
	        {
            // send closedown instruction if no more work
            if(this->work_scheduler.is_finished() && !sent_closedown)
	            {
                sent_closedown = true;
                this->close_down_workers(log);
	            }

            // generate new work assignments if needed, and push them to the workers
            if(this->work_scheduler.assignable())
              {
                this->check_for_progress_update(writer);
                this->assign_work_to_workers(log);
              }

            // check whether any messages are waiting in the queue
            boost::optional<boost::mpi::status> stat = this->world.iprobe();

            while(stat) // consume messages until no more are available
              {
                // update time of last received message and reset flag which enables logging
                last_msg = boost::posix_time::second_clock::universal_time();
                emit_agg_queue_msg = true;

                switch(stat->tag())
                  {
                    case MPI::INTEGRATION_DATA_READY:
                      {
                        if(int_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::INTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::integration_aggregation, payload.get_timestamp(), aggregation_counter));
		                        aggregation_queue.push_back(std::shared_ptr<integration_aggregation_record>(new integration_aggregation_record(this->worker_number(stat->source()), aggregation_counter++, int_agg, int_metadata, payload)));
                            BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::normal) << "!! Received INTEGRATION_DATA_READY message from worker " << stat->source() << ", but no integration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::DERIVED_CONTENT_READY:
                      {
                        if(derived_agg)
                          {
                            MPI::content_ready_payload payload;
                            this->world.recv(stat->source(), MPI::DERIVED_CONTENT_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::derived_content_aggregation, payload.get_timestamp(), aggregation_counter));
		                        aggregation_queue.push_back(std::shared_ptr<derived_content_aggregation_record>(new derived_content_aggregation_record(this->worker_number(stat->source()), aggregation_counter++, derived_agg, out_metadata, payload)));
                            BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " sent content-ready notification";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::normal) << "!! Received DERIVED_CONTENT_READY message from worker " << stat->source() << ", but no derived content aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::POSTINTEGRATION_DATA_READY:
                      {
                        if(post_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::POSTINTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::postintegration_aggregation, payload.get_timestamp(), aggregation_counter));
		                        aggregation_queue.push_back(std::shared_ptr<postintegration_aggregation_record>(new postintegration_aggregation_record(this->worker_number(stat->source()), aggregation_counter++, post_agg, out_metadata, payload)));
                            BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::normal) << "!! Received POSTINTEGRATION_DATA_READY message from worker " << stat->source() << ", but no postintegration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::NEW_WORK_ACKNOWLEDGMENT:
                      {
                        MPI::work_acknowledgment_payload payload;
                        this->world.recv(stat->source(), MPI::NEW_WORK_ACKNOWLEDGMENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), begin_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " advising receipt of work assignment at time " << boost::posix_time::to_simple_string(payload.get_timestamp());

                        break;
                      }

                    case MPI::FINISHED_INTEGRATION:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_INTEGRATION, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_wallclock_time());

                        // mark this worker as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_integration_time(), payload.get_num_integrations());
                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer->merge_failure_list(payload.get_failed_serials());

                        break;
                      }

                    case MPI::INTEGRATION_FAIL:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::INTEGRATION_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";

                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_integration_time(), payload.get_num_integrations());
                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer->merge_failure_list(payload.get_failed_serials());

                        success = false;
                        break;
                      }

                    case MPI::FINISHED_DERIVED_CONTENT:
                      {
                        MPI::finished_derived_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_DERIVED_CONTENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " advising finished work assignment in CPU time " << format_time(payload.get_cpu_time());

                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_processing_time(), payload.get_items_processed());
                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        break;
                      }

                    case MPI::DERIVED_CONTENT_FAIL:
                      {
                        MPI::finished_derived_payload payload;
                        this->world.recv(stat->source(), MPI::DERIVED_CONTENT_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_processing_time(), payload.get_items_processed());
                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        success = false;
                        break;
                      }

                    case MPI::FINISHED_POSTINTEGRATION:
                      {
                        MPI::finished_postintegration_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_POSTINTEGRATION, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_cpu_time());

                        // mark this worker as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_processing_time(), payload.get_items_processed());
                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        break;
                      }

                    case MPI::POSTINTEGRATION_FAIL:
                      {
                        MPI::finished_postintegration_payload payload;
                        this->world.recv(stat->source(), MPI::POSTINTEGRATION_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

                        // mark this worker as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_processing_time(), payload.get_items_processed());
                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        success = false;
                        break;
                      }

                    case MPI::WORKER_CLOSE_DOWN:
                      {
                        this->world.recv(stat->source(), MPI::WORKER_CLOSE_DOWN);
                        this->work_scheduler.mark_inactive(this->worker_number(stat->source()));
                        BOOST_LOG_SEV(log, base_writer::normal) << "++ Worker " << stat->source() << " advising close-down after end-of-work";
                        break;
                      }

                    default:
                      {
                        BOOST_LOG_SEV(log, base_writer::warning) << "!! Received unexpected message " << stat->tag() << " waiting in the queue; discarding";
                        this->world.recv(stat->source(), stat->tag());
                        break;
                      }
                  }

                stat = this->world.iprobe();  // check for another message
              }

            // we arrive at this point only when no more messages are available to be received

            // check whether any aggregations are in the queue, and process them if we have been idle for sufficiently long
            if(aggregation_queue.size() > 0)
              {
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                boost::posix_time::time_duration idle_time = now - last_msg;
                if(idle_time.total_seconds() > 5)
                  {
                    if(emit_agg_queue_msg)
                      {
                        BOOST_LOG_SEV(log, base_writer::warning) << "++ Idle for more than 5 seconds: processing queued aggregations";
                        emit_agg_queue_msg = false;
                      }

		                aggregation_queue.front()->aggregate();
		                aggregation_queue.pop_front();
                  }
              }
	        }

        this->check_for_progress_update(writer);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(log, base_writer::warning) << "++ Work completed at " << boost::posix_time::to_simple_string(now);

        // process any remaining aggregations
        while(aggregation_queue.size() > 0)
          {
            aggregation_queue.front()->aggregate();
            aggregation_queue.pop_front();
          }

        return(success);
	    }

	}   // namespace transport


#endif //__master_controller_H_
