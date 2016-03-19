//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H


#include "transport-runtime-api/manager/detail/master_controller_decl.h"


#include "boost/program_options.hpp"
#include "boost/timer/timer.hpp"


namespace transport
  {

    using namespace master_controller_impl;

    template <typename number>
    master_controller<number>::master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                                 local_environment& le, argument_cache& ac, model_manager<number>& f,
                                                 task_gallery<number>& g,
                                                 error_handler eh, warning_handler wh, message_handler mh)
      : environment(e),
        world(w),
        local_env(le),
        arg_cache(ac),
        model_mgr(f),
        gallery(g),
        data_mgr(data_manager_factory<number>(ac.get_batcher_capacity(), ac.get_datapipe_capacity(), ac.get_checkpoint_interval())),
        journal(w.size()-1),
        err(eh),
        warn(wh),
        msg(mh)
      {
      }


    template <typename number>
    void master_controller<number>::process_arguments(int argc, char* argv[])
      {
        // set up Boost::program_options descriptors for command-line arguments
        boost::program_options::options_description generic("Generic options");
        generic.add_options()
          (CPPTRANSPORT_SWITCH_HELP,      CPPTRANSPORT_HELP_HELP)
          (CPPTRANSPORT_SWITCH_VERSION,   CPPTRANSPORT_HELP_VERSION)
          (CPPTRANSPORT_SWITCH_MODELS,    CPPTRANSPORT_HELP_MODELS)
          (CPPTRANSPORT_SWITCH_NO_COLOUR, CPPTRANSPORT_HELP_NO_COLOUR);

        boost::program_options::options_description configuration("Configuration options");
        configuration.add_options()
          (CPPTRANSPORT_SWITCH_VERBOSE,                                                                                      CPPTRANSPORT_HELP_VERBOSE)
          (CPPTRANSPORT_SWITCH_REPO,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_REPO)
          (CPPTRANSPORT_SWITCH_TAG,              boost::program_options::value< std::vector<std::string> >()->composing(),   CPPTRANSPORT_HELP_TAG)
          (CPPTRANSPORT_SWITCH_CHECKPOINT,       boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CHECKPOINT)
          (CPPTRANSPORT_SWITCH_RECOVER,                                                                                      CPPTRANSPORT_HELP_RECOVER)
          (CPPTRANSPORT_SWITCH_CAPACITY,         boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CAPACITY)
          (CPPTRANSPORT_SWITCH_BATCHER_CAPACITY, boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_BATCHER_CAPACITY)
          (CPPTRANSPORT_SWITCH_CACHE_CAPACITY,   boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CACHE_CAPACITY);

        boost::program_options::options_description plotting("Plot style");
        plotting.add_options()
          (CPPTRANSPORT_PLOT_STYLE,              boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_PLOT_STYLE);

        boost::program_options::options_description journaling("Journaling options");
        journaling.add_options()
          (CPPTRANSPORT_SWITCH_GANTT_CHART,      boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_GANTT_CHART)
          (CPPTRANSPORT_SWITCH_JOURNAL,          boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_JOURNAL);

        boost::program_options::options_description job_options("Job specification");
        job_options.add_options()
          (CPPTRANSPORT_SWITCH_CREATE,                                                                                       CPPTRANSPORT_HELP_CREATE)
          (CPPTRANSPORT_SWITCH_TASK,             boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_TASK)
          (CPPTRANSPORT_SWITCH_SEED,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_SEED);

        boost::program_options::options_description hidden_options;
        hidden_options.add_options()
          (CPPTRANSPORT_SWITCH_NO_COLOR, CPPTRANSPORT_HELP_NO_COLOR);

        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(configuration).add(plotting).add(journaling).add(job_options).add(hidden_options);

        boost::program_options::options_description config_file_options;
        config_file_options.add(configuration).add(plotting).add(journaling).add(job_options).add(hidden_options);

        boost::program_options::options_description output_options;
        output_options.add(generic).add(configuration).add(plotting).add(journaling).add(job_options);

        boost::program_options::variables_map option_map;

        // parse options from the command line; we do this first so that any options
        // supplied on the command line override options specified in a configuration file
        boost::program_options::parsed_options cmdline_parsed = boost::program_options::command_line_parser(argc, argv).options(cmdline_options).allow_unregistered().run();
        boost::program_options::store(cmdline_parsed, option_map);

        // parse options from configuration file
        boost::optional< boost::filesystem::path > config_path = this->local_env.config_file_path();
        if(config_path)
          {
            if(boost::filesystem::exists(*config_path) && boost::filesystem::is_regular_file(*config_path))
              {
                std::ifstream instream((*config_path).string());
                if(instream)
                  {
                    // parse contents of file; 'true' means allow unregistered options
                    boost::program_options::parsed_options file_parsed = boost::program_options::parse_config_file(instream, config_file_options, true);
                    boost::program_options::store(file_parsed, option_map);

                    std::vector<std::string> unrecognized_config_options = boost::program_options::collect_unrecognized(file_parsed.options, boost::program_options::exclude_positional);
                    if(unrecognized_config_options.size() > 0)
                      {
                        for(const std::string& option : unrecognized_config_options)
                          {
                            std::ostringstream msg;
                            msg << CPPTRANSPORT_UNKNOWN_SWITCH << " '" << option << "'";
                            this->warn(msg.str());
                          }
                      }
                  }
              }
          }

        // inform the Boost::Program_Options library that all option parsing is complete
        boost::program_options::notify(option_map);

        // inform the user that we have ignored any recongized options
        std::vector<std::string> unrecognized_cmdline_options = boost::program_options::collect_unrecognized(cmdline_parsed.options, boost::program_options::exclude_positional);
        if(unrecognized_cmdline_options.size() > 0)
          {
            for(const std::string& option : unrecognized_cmdline_options)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_SWITCH << " '" << option << "'";
                this->warn(msg.str());
              }
          }

        // HANDLE SUPPLIED OPTIONS

        bool emitted_version = false;

        if(option_map.count(CPPTRANSPORT_SWITCH_VERSION))
          {
            std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_HELP))
          {
            if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            std::cout << output_options << '\n';
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
                                                        this->model_mgr, repository_mode::readwrite,
                                                        this->err, this->warn, this->msg);
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == exception_type::REPO_NOT_FOUND)
                  {
                    this->err(xe.what());
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

        // process global capacity specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_CAPACITY].as<int>() * 1024*1024;            // argument size interpreted in Mb
            if(capacity > 0)
              {
                unsigned int cp = static_cast<unsigned int>(capacity);

                this->arg_cache.set_batcher_capacity(cp);
                this->arg_cache.set_datapipe_capacity(cp);

                this->data_mgr->set_batcher_capacity(cp);                         // probably not required; only slaves need these values set
                this->data_mgr->set_pipe_capacity(cp);                            // probably not required; only slaves need these values set
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_CAPACITY;
                this->err(msg.str());
              }
          }

        // process datapipe capacity specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CACHE_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_CACHE_CAPACITY].as<int>() * 1024*1024;      // argument size interpreted in Mb
            if(capacity > 0)
              {
                unsigned int cp = static_cast<unsigned int>(capacity);

                this->arg_cache.set_datapipe_capacity(cp);

                this->data_mgr->set_pipe_capacity(cp);                            // probably not required; only slaves need these values set
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_CACHE_CAPACITY;
                this->err(msg.str());
              }
          }

        // process batcher capacity specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_BATCHER_CAPACITY))
          {
            int capacity = option_map[CPPTRANSPORT_SWITCH_BATCHER_CAPACITY].as<int>() * 1024*1024;    // argument size interpreted in Mb
            if(capacity > 0)
              {
                unsigned int cp = static_cast<unsigned int>(capacity);

                this->arg_cache.set_batcher_capacity(cp);

                this->data_mgr->set_batcher_capacity(cp);                         // probably not required; only slaves need these values set
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " << CPPTRANSPORT_SWITCH_BATCHER_CAPACITY;
                this->err(msg.str());
              }
          }

        // process checkpoint timer specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CHECKPOINT))
          {
            int interval = option_map[CPPTRANSPORT_SWITCH_CHECKPOINT].as<int>() * 60;                 // argument size interpreted in minutes; convert value to seconds
            if(interval > 0)
              {
                unsigned int ck = static_cast<unsigned int>(interval);

                this->arg_cache.set_checkpoint_interval(ck);
                this->data_mgr->set_checkpoint_interval(ck);                   // probably not required; only slaves need these values set
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_EXPECTED_POSITIVE << " " CPPTRANSPORT_SWITCH_CHECKPOINT;
                this->err(msg.str());
              }
          }

        // process Gantt chart specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_GANTT_CHART))
          {
            this->arg_cache.set_gantt_chart(true);
            this->arg_cache.set_gantt_filename(option_map[CPPTRANSPORT_SWITCH_GANTT_CHART].as<std::string>());
          }

        // process journal specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_JOURNAL))
          {
            this->arg_cache.set_journal(true);
            this->arg_cache.set_journal_filename(option_map[CPPTRANSPORT_SWITCH_JOURNAL].as<std::string>());
          }

        // process plotting environment, if provided
        if(option_map.count(CPPTRANSPORT_PLOT_STYLE_LONG))
          {
            if(!this->arg_cache.set_plot_environment(option_map[CPPTRANSPORT_PLOT_STYLE_LONG].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_PLOT_STYLE << " '" << option_map[CPPTRANSPORT_PLOT_STYLE_LONG].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_VERBOSE_LONG))                                                this->arg_cache.set_verbose(true);
        if(option_map.count(CPPTRANSPORT_SWITCH_RECOVER))                                                     this->arg_cache.set_recovery_mode(true);
        if(option_map.count(CPPTRANSPORT_SWITCH_NO_COLOUR) || option_map.count(CPPTRANSPORT_SWITCH_NO_COLOR)) this->arg_cache.set_colour_output(false);
        if(option_map.count(CPPTRANSPORT_SWITCH_CREATE))                                                      this->arg_cache.set_create_mode(true);

        // process task item
        // this can depend on prior information specified by other switches, eg. --seed
        if(option_map.count(CPPTRANSPORT_SWITCH_TASK))
          {
            std::vector<std::string> tasks = option_map[CPPTRANSPORT_SWITCH_TASK].as< std::vector<std::string> >();

            for(std::vector<std::string>::const_iterator t = tasks.begin(); t != tasks.end(); ++t)
              {
                job_queue.push_back(job_descriptor(job_type::job_task, *t, tags));

                if(option_map.count(CPPTRANSPORT_SWITCH_SEED))
                  {
                    job_queue.back().set_seed(option_map[CPPTRANSPORT_SWITCH_SEED].as<std::string>());
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::pre_process_tasks()
      {
        if(this->arg_cache.get_model_list())   this->model_mgr.write_models(std::cout);
        if(this->arg_cache.get_create_model()) this->gallery.commit(*this->repo);
      }


    template <typename number>
    void master_controller<number>::execute_tasks(void)
      {
        if(!(this->get_rank() == 0)) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_EXEC_SLAVE);

        if(!this->repo)
          {
            this->err(CPPTRANSPORT_REPO_NONE);
          }
        else
          {
            boost::timer::cpu_timer timer;

            // perform recovery if requested
            if(this->arg_cache.get_recovery_mode()) this->repo->perform_recovery(*this->data_mgr, this->get_rank());

            // set up workers
            this->initialize_workers();

            unsigned int database_tasks = 0;
            for(const job_descriptor& job : this->job_queue)
              {
                switch(job.get_type())
                  {
                    case job_type::job_task:
                      {
                        this->process_task(job);
                        database_tasks++;
                        break;
                      }
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

                this->msg(msg.str());
              }
          }

        // there is no more work, so ask all workers to shut down
        // and then exit ourselves
        this->terminate_workers();

        if(this->arg_cache.get_gantt_chart()) this->journal.make_gantt_chart(this->arg_cache.get_gantt_filename(), this->local_env, this->arg_cache);
        if(this->arg_cache.get_journal())     this->journal.make_journal(this->arg_cache.get_journal_filename(), this->local_env, this->arg_cache);
      }


    template <typename number>
    void master_controller<number>::process_task(const job_descriptor& job)
      {
        std::unique_ptr< task_record<number> > record;

        try
          {
            // query a task record with the name we're looking for from the database;
            // will throw RECORD_NOT_FOUND if the task record is not present

            // notice that this record will persist throughout the entire job, until this function exits
            // however, we have to be cautious about using it because other processes may write to the
            // repository for the same job

            // for this reason, we don't hold copies to this record in any of the writers, but only
            // a copy of the job name. When commits need to occur, the record is looked up again
            record = this->repo->query_task(job.get_name());
          }
        catch (runtime_exception xe)
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
                return;
              }
            else if(xe.get_exception_code() == exception_type::MISSING_MODEL_INSTANCE
                    || xe.get_exception_code() == exception_type::REPOSITORY_BACKEND_ERROR)
              {
                std::ostringstream msg;
                msg << xe.what() << " " << CPPTRANSPORT_REPO_FOR_TASK << " '" << job.get_name() << "'" << CPPTRANSPORT_REPO_SKIPPING_TASK;
                this->err(msg.str());
                return;
              }
            else throw xe;
          }

        switch(record->get_type())
          {
            case task_type::integration:
              {
                integration_task_record<number>* int_rec = dynamic_cast< integration_task_record<number>* >(record.get());

                assert(int_rec != nullptr);
                if(int_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_integration_task(*int_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                break;
              }

            case task_type::output:
              {
                output_task_record<number>* out_rec = dynamic_cast< output_task_record<number>* >(record.get());

                assert(out_rec != nullptr);
                if(out_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_output_task(*out_rec, job.get_tags());
                break;
              }

            case task_type::postintegration:
              {
                postintegration_task_record<number>* pint_rec = dynamic_cast< postintegration_task_record<number>* >(record.get());

                assert(pint_rec != nullptr);
                if(pint_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_postintegration_task(*pint_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                break;
              }
          }
      }

    // WORKER HANDLING


    template <typename number>
    void master_controller<number>::initialize_workers(void)
      {
        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // we require this->repo not to be null
        assert(this->repo);
        if(!this->repo) throw std::runtime_error(CPPTRANSPORT_REPO_NOT_SET);

        // request information from each worker, and pass all necessary setup details
        MPI::slave_setup_payload payload(this->repo->get_root_path(), this->arg_cache);

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
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

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
                    BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received unexpected MPI message " << stat.tag() << " from worker " << stat.source() << "; discarding";
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
        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Notifying workers of end-of-work";

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
            this->msg(update_msg.str());

            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "±± Console advisory message: " << update_msg.str();
            this->log_worker_metadata(writer);
          }
      }


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::log_worker_metadata(WriterObject& writer)
      {
        std::vector< master_scheduler::worker_metadata > metadata = this->work_scheduler.get_metadata();

        for(const master_scheduler::worker_metadata& t : metadata)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "## Worker " << t.get_number() << ": active = " << t.get_active()
                                                                                     << ", assigned = " << t.get_assigned()
                                                                                     << ", total time = " << format_time(t.get_total_elapsed_time())
                                                                                     << ", items processed = " << t.get_total_items_processed()
                                                                                     << ", mean time per item = " << (t.get_total_items_processed() > 0 ? format_time(t.get_total_elapsed_time() / t.get_total_items_processed()) : "n/a")
                                                                                     << ", last contact at " << boost::posix_time::to_simple_string(t.get_last_contact_time());
          }
      }


    template <typename number>
    void master_controller<number>::assign_work_to_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log)
      {
        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        // generate a list of work assignments
        std::list<master_scheduler::work_assignment> work = this->work_scheduler.assign_work(log);

        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Generating new work assignments for " << work.size() << " worker" << (work.size() > 1 ? "s" : "") << " (" << this->work_scheduler.get_queue_size() << " work items remain in queue)";

        // push assignments to workers
        std::vector<boost::mpi::request> msg_status(work.size());

        unsigned int c = 0;
        for(const master_scheduler::work_assignment& assgn : work)
          {
            MPI::work_assignment_payload payload(assgn.get_items());

            // send message to worker with new assignment information
            msg_status[c] = this->world.isend(this->worker_rank(assgn.get_worker()), MPI::NEW_WORK_ASSIGNMENT, payload);

            // mark this worker, and these work items, as assigned
            this->work_scheduler.mark_assigned(assgn);

            // issue message to log
            unsigned int items = assgn.get_items().size();
            unsigned int worker = assgn.get_worker();
            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Assigned " << items << " work item" << (items == 1 ? "" : "s") << " to worker " << worker << " [MPI rank=" << this->worker_rank(worker) << "]";
          }

        // wait for all assignments to be received
        boost::mpi::wait_all(msg_status.begin(), msg_status.end());
      }


    template <typename number>
    void master_controller<number>::terminate_workers(void)
      {
        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

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
    bool master_controller<number>::poll_workers(integration_aggregator<number>& int_agg, postintegration_aggregator<number>& post_agg, derived_content_aggregator<number>& derived_agg,
                                                 integration_metadata& int_metadata, output_metadata& out_metadata, std::list<std::string>& content_groups,
                                                 WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        bool success = true;

        boost::log::sources::severity_logger< base_writer::log_severity_level >& log = writer.get_log();

        // set up aggregation queues
        unsigned int aggregation_counter = 0;
        std::list< std::unique_ptr<aggregation_record> > aggregation_queue;

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

                this->work_scheduler.update_contact_time(this->worker_number(stat->source()), last_msg);

                switch(stat->tag())
                  {
                    case MPI::INTEGRATION_DATA_READY:
                      {
                        if(int_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::INTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::integration_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< integration_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, int_agg, int_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received INTEGRATION_DATA_READY message from worker " << stat->source() << ", but no integration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::DERIVED_CONTENT_READY:
                      {
                        if(derived_agg)
                          {
                            MPI::content_ready_payload payload;
                            this->world.recv(stat->source(), MPI::DERIVED_CONTENT_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::derived_content_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< derived_content_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, derived_agg, out_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent content-ready notification";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received DERIVED_CONTENT_READY message from worker " << stat->source() << ", but no derived content aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::POSTINTEGRATION_DATA_READY:
                      {
                        if(post_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::POSTINTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::postintegration_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< postintegration_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, post_agg, out_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received POSTINTEGRATION_DATA_READY message from worker " << stat->source() << ", but no postintegration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::NEW_WORK_ACKNOWLEDGMENT:
                      {
                        MPI::work_acknowledgment_payload payload;
                        this->world.recv(stat->source(), MPI::NEW_WORK_ACKNOWLEDGMENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), begin_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising receipt of work assignment at time " << boost::posix_time::to_simple_string(payload.get_timestamp());

                        break;
                      }

                    case MPI::FINISHED_INTEGRATION:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_INTEGRATION, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_wallclock_time());

                        // mark this worker as unassigned, and update its mean time per work item
                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_integration_time(), payload.get_num_integrations());
                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer.merge_failure_list(payload.get_failed_serials());

                        break;
                      }

                    case MPI::INTEGRATION_FAIL:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::INTEGRATION_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";

                        this->work_scheduler.mark_unassigned(this->worker_number(stat->source()), payload.get_integration_time(), payload.get_num_integrations());
                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer.merge_failure_list(payload.get_failed_serials());

                        success = false;
                        break;
                      }

                    case MPI::FINISHED_DERIVED_CONTENT:
                      {
                        MPI::finished_derived_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_DERIVED_CONTENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in CPU time " << format_time(payload.get_cpu_time());

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
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

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
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_cpu_time());

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
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

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

                        unsigned int num_active = this->work_scheduler.get_number_active();
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising close-down after end-of-work; " << num_active << " worker" << (num_active != 1 ? "s" : "") << " still active";
                        break;
                      }

                    case MPI::REPORT_ERROR:
                      {
                        MPI::error_report payload;
                        this->world.recv(stat->source(), MPI::REPORT_ERROR);

                        std::ostringstream msg;
                        msg << payload.get_message() << " " << CPPTRANSPORT_MASTER_REPORTED_BY_WORKER << " " << stat->source();
                        this->err(msg.str());
                        break;
                      }

                    default:
                      {
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::warning) << "!! Received unexpected message " << stat->tag() << " waiting in the queue; discarding";
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
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::warning) << "++ Idle for more than 5 seconds: processing queued aggregations";
                        emit_agg_queue_msg = false;
                      }

                    aggregation_queue.front()->aggregate();
                    aggregation_queue.pop_front();
                  }
              }
          }

        this->check_for_progress_update(writer);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(log, base_writer::log_severity_level::warning) << "++ All work items completed at " << boost::posix_time::to_simple_string(now);

        // process any remaining aggregations
        while(aggregation_queue.size() > 0)
          {
            aggregation_queue.front()->aggregate();
            aggregation_queue.pop_front();
          }

        return(success);
      }

  }


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
