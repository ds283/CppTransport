//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H


#include "transport-runtime-api/manager/detail/master_controller_decl.h"
#include "transport-runtime-api/repository/repository_toolkit.h"


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
        msg(mh),
        cmdline_reports(le, ac, eh, wh, mh),
        HTML_reports(le, ac, eh, wh, mh)
      {
      }


    template <typename number>
    void master_controller<number>::process_arguments(int argc, char* argv[])
      {
        unsigned int width = this->local_env.detect_terminal_width();

        // set up Boost::program_options descriptors for command-line arguments
        boost::program_options::options_description generic("Generic options", width);
        generic.add_options()
          (CPPTRANSPORT_SWITCH_HELP,                                                                                         CPPTRANSPORT_HELP_HELP)
          (CPPTRANSPORT_SWITCH_VERSION,                                                                                      CPPTRANSPORT_HELP_VERSION)
          (CPPTRANSPORT_SWITCH_INCLUDE,          boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_INCLUDE)
          (CPPTRANSPORT_SWITCH_MODELS,                                                                                       CPPTRANSPORT_HELP_MODELS)
          (CPPTRANSPORT_SWITCH_NO_COLOUR,                                                                                    CPPTRANSPORT_HELP_NO_COLOUR);

        boost::program_options::options_description configuration("Configuration options", width);
        configuration.add_options()
          (CPPTRANSPORT_SWITCH_VERBOSE,                                                                                      CPPTRANSPORT_HELP_VERBOSE)
          (CPPTRANSPORT_SWITCH_REPO,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_REPO)
          (CPPTRANSPORT_SWITCH_CAPACITY,         boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CAPACITY)
          (CPPTRANSPORT_SWITCH_BATCHER_CAPACITY, boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_BATCHER_CAPACITY)
          (CPPTRANSPORT_SWITCH_CACHE_CAPACITY,   boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CACHE_CAPACITY);

        boost::program_options::options_description plotting("Plot style", width);
        plotting.add_options()
          (CPPTRANSPORT_PLOT_STYLE,              boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_PLOT_STYLE);

        boost::program_options::options_description journaling("Journaling options", width);
        journaling.add_options()
          (CPPTRANSPORT_SWITCH_GANTT_CHART,      boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_GANTT_CHART)
          (CPPTRANSPORT_SWITCH_JOURNAL,          boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_JOURNAL);

        boost::program_options::options_description job_options("Job specification", width);
        job_options.add_options()
          (CPPTRANSPORT_SWITCH_CREATE,                                                                                       CPPTRANSPORT_HELP_CREATE)
          (CPPTRANSPORT_SWITCH_TASK,             boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_TASK)
          (CPPTRANSPORT_SWITCH_TAG,              boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_TAG)
          (CPPTRANSPORT_SWITCH_CHECKPOINT,       boost::program_options::value< int >(),                                     CPPTRANSPORT_HELP_CHECKPOINT)
          (CPPTRANSPORT_SWITCH_SEED,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_SEED);

        boost::program_options::options_description action_options("Repository actions", width);
        action_options.add_options()
          (CPPTRANSPORT_SWITCH_OBJECT,           boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_OBJECT)
          (CPPTRANSPORT_SWITCH_ADD_TAG,          boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_ADD_TAG)
          (CPPTRANSPORT_SWITCH_DELETE_TAG,       boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_DELETE_TAG)
          (CPPTRANSPORT_SWITCH_ADD_NOTE,         boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_ADD_NOTE)
          (CPPTRANSPORT_SWITCH_DELETE_NOTE,      boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_DELETE_NOTE)
          (CPPTRANSPORT_SWITCH_DELETE,                                                                                       CPPTRANSPORT_HELP_DELETE);

        boost::program_options::options_description report_options("Repository reporting and status", width);
        report_options.add_options()
          (CPPTRANSPORT_SWITCH_RECOVER,                                                                                      CPPTRANSPORT_HELP_RECOVER)
          (CPPTRANSPORT_SWITCH_STATUS,                                                                                       CPPTRANSPORT_HELP_STATUS)
          (CPPTRANSPORT_SWITCH_INFLIGHT,                                                                                     CPPTRANSPORT_HELP_INFLIGHT)
          (CPPTRANSPORT_SWITCH_INFO,             boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_INFO)
          (CPPTRANSPORT_SWITCH_PROVENANCE,       boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_PROVENANCE)
          (CPPTRANSPORT_SWITCH_HTML,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_HTML);

        boost::program_options::options_description hidden_options;
        hidden_options.add_options()
          (CPPTRANSPORT_SWITCH_NO_COLOR, CPPTRANSPORT_HELP_NO_COLOR);

        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling).add(hidden_options);

        boost::program_options::options_description config_file_options;
        config_file_options.add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling).add(hidden_options);

        boost::program_options::options_description output_options;
        output_options.add(generic).add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling);

        // parse options from the command line; we do this first so that any options
        // supplied on the command line override options specified in a configuration file
        boost::program_options::parsed_options cmdline_parsed = boost::program_options::command_line_parser(argc, argv).options(cmdline_options).allow_unregistered().run();
        boost::program_options::store(cmdline_parsed, this->option_map);
        this->warn_unrecognized_switches(cmdline_parsed);

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
                    boost::program_options::store(file_parsed, this->option_map);
                    this->warn_unrecognized_switches(file_parsed);
                  }
              }
          }

        // inform the Boost::Program_Options library that all option parsing is complete
        boost::program_options::notify(this->option_map);


        // HANDLE SUPPLIED OPTIONS

        this->recognize_generic_switches(this->option_map, output_options);

        if(this->option_map.count(CPPTRANSPORT_SWITCH_REPO_LONG))
          {
            try
              {
                this->repo = repository_factory<number>(this->option_map[CPPTRANSPORT_SWITCH_REPO_LONG].template as<std::string>(),
                                                        this->model_mgr, repository_mode::readwrite,
                                                        this->err, this->warn, this->msg);
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == exception_type::REPO_NOT_FOUND)
                  {
#ifdef TRACE_OUTPUT
                    std::cout << "TRACE_OUTPUT H" << '\n';
#endif
                    this->err(xe.what());
                    repo = nullptr;
                  }
                else
                  {
                    throw xe;
                  }
              }
          }

        this->recognize_configuration_switches(this->option_map);
        this->recognize_repository_switches(this->option_map);
        this->recognize_journal_switches(this->option_map);
        this->recognize_job_switches(this->option_map);
        this->recognize_plot_switches(this->option_map);
        // action switches not handled at this point, but later when all models have been registered by the client
        // (in case repository access is needed, which can depend on model instances being available)

        // populate list of tags
        std::list<std::string> tags;
        if(this->option_map.count(CPPTRANSPORT_SWITCH_TAG) > 0)
          {
            std::vector<std::string> tmp = this->option_map[CPPTRANSPORT_SWITCH_TAG].template as<std::vector<std::string> >();

            // copy tags into std::list tags
            // Boost::program_arguments doesn't support lists, only vectors, so we have to do it this way
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tags));
          }

        // process task item
        // this can depend on prior information specified by other switches, eg. --seed
        if(this->option_map.count(CPPTRANSPORT_SWITCH_TASK))
          {
            std::vector<std::string> tasks = this->option_map[CPPTRANSPORT_SWITCH_TASK].template as< std::vector<std::string> >();

            for(const std::string& task : tasks)
              {
                job_queue.emplace_back(job_type::job_task, task, tags);

                if(this->option_map.count(CPPTRANSPORT_SWITCH_SEED))
                  {
                    job_queue.back().set_seed(this->option_map[CPPTRANSPORT_SWITCH_SEED].template as<std::string>());
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::warn_unrecognized_switches(boost::program_options::parsed_options& options)
      {
        std::vector<std::string> unrecognized_options = boost::program_options::collect_unrecognized(options.options, boost::program_options::exclude_positional);
        if(unrecognized_options.size() > 0)
          {
            for(const std::string& option : unrecognized_options)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_SWITCH << " '" << option << "'";
                this->warn(msg.str());
              }
          }
      }


    template <typename number>
    void master_controller<number>::recognize_generic_switches(boost::program_options::variables_map& option_map, boost::program_options::options_description& description)
      {
        bool emitted_version = false;

        if(option_map.count(CPPTRANSPORT_SWITCH_VERSION))
          {
            std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_HELP))
          {
            if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            std::cout << description << '\n';
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_MODELS))
          {
            this->arg_cache.set_model_list(true);
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_NO_COLOUR) || option_map.count(CPPTRANSPORT_SWITCH_NO_COLOR)) this->arg_cache.set_colour_output(false);
      }


    template <typename number>
    void master_controller<number>::recognize_configuration_switches(boost::program_options::variables_map& option_map)
      {
        if(option_map.count(CPPTRANSPORT_SWITCH_VERBOSE_LONG)) this->arg_cache.set_verbose(true);

        // add search paths if any were specified
        if(option_map.count(CPPTRANSPORT_SWITCH_INCLUDE_LONG)) this->arg_cache.set_search_paths(option_map[CPPTRANSPORT_SWITCH_INCLUDE_LONG].as< std::vector<std::string> >());

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
      }


    template <typename number>
    void master_controller<number>::recognize_repository_switches(boost::program_options::variables_map& option_map)
      {
        if(option_map.count(CPPTRANSPORT_SWITCH_RECOVER)) this->arg_cache.set_recovery_mode(true);

        if(option_map.count(CPPTRANSPORT_SWITCH_STATUS))
          {
            this->cmdline_reports.task_list();
            this->cmdline_reports.inflight_list();
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_INFLIGHT)) this->cmdline_reports.inflight_list();

        if(option_map.count(CPPTRANSPORT_SWITCH_INFO)) this->cmdline_reports.info(option_map[CPPTRANSPORT_SWITCH_INFO].as< std::vector<std::string> >());
        if(option_map.count(CPPTRANSPORT_SWITCH_PROVENANCE)) this->cmdline_reports.provenance(option_map[CPPTRANSPORT_SWITCH_PROVENANCE].as< std::vector<std::string> >());

        if(option_map.count(CPPTRANSPORT_SWITCH_HTML)) this->HTML_reports.set_root(option_map[CPPTRANSPORT_SWITCH_HTML].as< std::string >());
      }


    template <typename number>
    void master_controller<number>::recognize_action_switches(boost::program_options::variables_map& option_map)
      {
        repository_toolkit<number> toolkit(*this->repo, this->err, this->warn, this->msg);

        if(option_map.count(CPPTRANSPORT_SWITCH_OBJECT))
          {
            std::vector<std::string> add_tag;
            std::vector<std::string> delete_tag;
            std::vector<std::string> add_note;
            std::vector<std::string> delete_note;

            if(option_map.count(CPPTRANSPORT_SWITCH_DELETE))
              {
                if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG) ||
                  option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG) ||
                  option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE) ||
                  option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE))
                  {
                    this->err(CPPTRANSPORT_SWITCH_DELETE_SINGLET);
                  }
                else
                  {
                    try
                      {
                        toolkit.delete_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >());
                      }
                    catch(runtime_exception& xe)
                      {
                        if(xe.get_exception_code() == exception_type::REPOSITORY_ERROR)
                          {
#ifdef TRACE_OUTPUT
                            std::cout << "TRACE_OUTPUT G" << '\n';
#endif
                            this->err(xe.what());
                          }
                        else throw;
                      }
                  }
              }
            else
              {
                if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG)) add_tag = option_map[CPPTRANSPORT_SWITCH_ADD_TAG].as< std::vector<std::string> >();
                if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG)) delete_tag = option_map[CPPTRANSPORT_SWITCH_DELETE_TAG].as< std::vector<std::string> >();
                if(option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE)) add_note = option_map[CPPTRANSPORT_SWITCH_ADD_NOTE].as< std::vector<std::string> >();
                if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE)) delete_note = option_map[CPPTRANSPORT_SWITCH_DELETE_NOTE].as< std::vector<std::string> >();

                try
                  {
                    toolkit.update_tags_notes(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >(),
                                              add_tag, delete_tag, add_note, delete_note);
                  }
                catch(runtime_exception& xe)
                  {
                    if(xe.get_exception_code() == exception_type::REPOSITORY_ERROR)
                      {
#ifdef TRACE_OUTPUT
                        std::cout << "TRACE_OUTPUT I" << '\n';
#endif
                        this->err(xe.what());
                      }
                    else throw;
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::recognize_journal_switches(boost::program_options::variables_map& option_map)
      {
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
      }


    template <typename number>
    void master_controller<number>::recognize_job_switches(boost::program_options::variables_map& option_map)
      {
        if(option_map.count(CPPTRANSPORT_SWITCH_CREATE)) this->arg_cache.set_create_mode(true);

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
      }


    template <typename number>
    void master_controller<number>::recognize_plot_switches(boost::program_options::variables_map& option_map)
      {
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
      }


    template <typename number>
    void master_controller<number>::pre_process_tasks()
      {
        // perform recovery if requested
        if(this->arg_cache.get_recovery_mode() && this->repo) this->repo->perform_recovery(*this->data_mgr, this->get_rank());

        if(this->arg_cache.get_model_list())                  this->model_mgr.write_models(std::cout);
        if(this->arg_cache.get_create_model() && this->repo)  this->gallery.commit(*this->repo);

        // handle repository action switches
        // these were left unhandled during option parsing
        this->recognize_action_switches(this->option_map);
      }


    template <typename number>
    void master_controller<number>::post_process_tasks()
      {
        if(this->repo)
          {
            this->cmdline_reports.report(*this->repo);
          }

        if(this->repo && this->data_mgr)
          {
            this->HTML_reports.report(*this->repo, *this->data_mgr);
          }
      }


    template <typename number>
    void master_controller<number>::autocomplete_task_schedule()
      {
        // build distance matrix for tasks
        repository_toolkit<number> toolkit(*this->repo, this->err, this->warn, this->msg);
        std::unique_ptr<repository_distance_matrix> dmat = toolkit.task_distance_matrix();

        std::list< std::string > required_tasks;
        for(const job_descriptor& job : this->job_queue)
          {
            switch(job.get_type())
              {
                case job_type::job_task:
                  {
                    std::unique_ptr< std::list<std::string> > depends = dmat->find_dependencies(job.get_name());

                    if(depends)
                      {
//                        std::cout << "-- Task '" << job.get_name() << "' depends on tasks:" << '\n';
//                        for(const std::string& tk : *depends)
//                          {
//                            std::cout << "   " << tk << '\n';
//                          }
                        required_tasks.merge(*depends);
                      }
                  }
              }
          }

        // remove duplicates
        required_tasks.sort();
        required_tasks.unique();

        // required_tasks holds a list of all tasks needed to generate output for the set of tasks specified on
        // the command line
        // Of these, we want to check which have no current content
        this->prune_tasks_with_content(required_tasks);

        // required_tasks now contains only those tasks for which output is needed;
        // we need to prune it for paired tasks and then insert job records for any
        // remaining tasks which do not already have one
        this->prune_paired_tasks(required_tasks);

        // insert any required job descriptors and sort task list into order
        std::unique_ptr< std::list<std::string> > topological_order = dmat->compute_topological_order();
        if(topological_order) this->insert_job_descriptors(required_tasks, *topological_order);
      }


    template <typename number>
    void master_controller<number>::prune_tasks_with_content(std::list<std::string>& required_tasks)
      {
        for(std::list<std::string>::const_iterator t = required_tasks.begin(); t != required_tasks.end(); /* intentionally no update step */)
          {
            std::unique_ptr< task_record<number> > rec = this->repo->query_task(*t);
            if(rec)
              {
                if(rec->get_output_groups().size() > 0)
                  {
//                    std::cout << "Task '" << *t << "' is needed, but already has content; evicting from list" << '\n';
                    t = required_tasks.erase(t);
                  }
                else
                  {
//                    std::cout << "Task '" << *t << "' is needed and has no content; retaining in list" << '\n';
                    ++t;
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::prune_paired_tasks(std::list<std::string>& required_tasks)
      {
        for(const std::string& task : required_tasks)
          {
            std::unique_ptr< task_record<number> > rec = this->repo->query_task(task);
            if(rec)
              {
                std::string paired_task;

                if(rec->get_type() == task_type::postintegration)
                  {
                    const postintegration_task_record<number>& prec = dynamic_cast< const postintegration_task_record<number>& >(*rec);
                    const postintegration_task<number>& tk = *prec.get_task();

                    // only zeta 2pf and zeta 3pf tasks can be paired
                    if(tk.get_task_type() == postintegration_task_type::twopf)
                      {
                        const zeta_twopf_task<number>& ztk = dynamic_cast< const zeta_twopf_task<number>& >(tk);
                        if(ztk.is_paired()) paired_task = ztk.get_parent_task()->get_name();
                      }
                    else if(tk.get_task_type() == postintegration_task_type::threepf)
                      {
                        const zeta_threepf_task<number>& ztk = dynamic_cast< const zeta_threepf_task<number>& >(tk);
                        if(ztk.is_paired()) paired_task = ztk.get_parent_task()->get_name();
                      }
                  }

                if(!paired_task.empty())
                  {
                    std::list<std::string>::const_iterator u = std::find(required_tasks.begin(), required_tasks.end(), paired_task);
                    if(u != required_tasks.end()) required_tasks.erase(u);
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::insert_job_descriptors(const std::list<std::string>& tasks, const std::list<std::string>& order)
      {
        // set up tags
        std::list<std::string> tags;
        if(this->option_map.count(CPPTRANSPORT_SWITCH_TAG) > 0)
          {
            std::vector<std::string> tmp = this->option_map[CPPTRANSPORT_SWITCH_TAG].template as<std::vector<std::string> >();
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tags));
          }

        for(const std::string& task : tasks)
          {
            std::list<job_descriptor>::const_iterator j = std::find_if(this->job_queue.cbegin(), this->job_queue.cend(), FindJobDescriptorByName(task));

            if(j == this->job_queue.cend())   // no corresponding job in queue, so we need to insert one
              {
                this->job_queue.emplace_back(job_type::job_task, task, tags);
              }
          }

        // sort job queue into topological order
        this->job_queue.sort(CompareJobDescriptorByList(order));
      }


    template <typename number>
    void master_controller<number>::execute_tasks()
      {
        if(!(this->get_rank() == 0)) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_EXEC_SLAVE);

        if(!this->repo)
          {
            this->err(CPPTRANSPORT_REPO_NONE);
          }
        else
          {
            boost::timer::cpu_timer timer;

            // set up workers
            this->initialize_workers();

            // schedule extra tasks if any explicitly-required tasks depend on content from
            // a second task, but no content group is available
            this->autocomplete_task_schedule();

            unsigned int tasks_complete = 0;
            unsigned int tasks_processed = 0;
            for(const job_descriptor& job : this->job_queue)
              {
                // emit notification if we are processing multiple tasks
                if(this->job_queue.size() > 1)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_PROCESSING_TASK_A << " '" << job.get_name() << "' (" << tasks_processed+1 << " " << CPPTRANSPORT_PROCESSING_TASK_OF << " " << this->job_queue.size() << ")";
                    this->msg(msg.str(), message_handler::highlight::heading);
                  }

                switch(job.get_type())
                  {
                    case job_type::job_task:
                      {
                        try
                          {
                            this->process_task(job);
                            tasks_complete++;
                          }
                        catch(runtime_exception& xe)
                          {
#ifdef TRACE_OUTPUT
                            std::cout << "TRACE_OUTPUT J" << '\n';
#endif
                            this->err(xe.what());
                          }
                        catch(std::exception& xe)
                          {
#ifdef TRACE_OUTPUT
                            std::cout << "TRACE_OUTPUT K" << '\n';
#endif
                            this->err(xe.what());
                          }
                        break;
                      }
                  }

                ++tasks_processed;
              }

            timer.stop();

            if(tasks_complete > 0)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PROCESSED_TASKS_A << " " << tasks_complete << " ";
                if(tasks_complete > 1)
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

            // Because we don't hold any open transactions at this point the record is returned in readonly mode
            // When commits need to occur, the record is looked up again with an active transaction
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

        // introspect task type
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

        std::vector<boost::mpi::request> requests(this->world.size()-1);
        for(unsigned int i = 0; i < this->world.size()-1; ++i)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::END_OF_WORK);
          }

        // wait for all messages to be received, then return
        boost::mpi::wait_all(requests.begin(), requests.end());
      }


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::check_for_progress_update(WriterObject& writer)
      {
        // emit update message giving current status if required
        if(this->work_scheduler.update_available())     // an update exists
          {
            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

            std::ostringstream update_msg;
            update_msg << CPPTRANSPORT_TASK_MANAGER_LABEL << " " << boost::posix_time::to_simple_string(now);

            // emit update
            reporting::key_value update_data(this->local_env, this->arg_cache);
            reporting::key_value notifications(this->local_env, this->arg_cache);
            this->work_scheduler.populate_update_information(update_data, notifications);

            if(this->arg_cache.get_verbose())
              {
                bool colour = this->local_env.has_colour_terminal_support() && this->arg_cache.get_colour_output();

                if(colour) std::cout << ColourCode(ANSI_colour::magenta);
                std::cout << update_msg.str();
                if(colour) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';

                update_data.set_tiling(true);
                notifications.set_tiling(true);
                update_data.write(std::cout);
                notifications.write(std::cout);
              }

            // update main database with new estimate of our completion time
            this->repo->advise_completion_time(writer.get_name(), this->work_scheduler.get_estimated_completion());

            update_msg << '\n';
            update_data.write(update_msg, reporting::key_value::print_options::force_simple);
            notifications.write(update_msg, reporting::key_value::print_options::force_simple);

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
                        this->world.recv(stat->source(), MPI::REPORT_ERROR, payload);

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
