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

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H


#include "transport-runtime/manager/detail/master_controller_decl.h"
#include "transport-runtime/repository/repository_toolkit.h"
#include "transport-runtime/repository/repository_graphkit.h"

#include "transport-runtime/build_data.h"
#include "transport-runtime/defaults.h"

#include "boost/timer/timer.hpp"


namespace transport
  {

    namespace master_controller_impl
      {

        inline void show_license_data()
          {
            std::cout << '\n';
            std::cout << "The CppTransport platform is free software, and you are welcome to redistribute" << '\n'
                      << "it under certain conditions; for details see the LICENSE.txt file which came bundled" << '\n'
                      << "with the platform source files. Different licensing conditions may apply to" << '\n';
            std::cout << "individual model description files." << '\n';
            std::cout << "For the CppTransport platform: contact David Seery <D.Seery@sussex.ac.uk>" << '\n';
            std::cout << "For model licensing information: contact details should be embedded within the model file." << '\n';

            std::cout << '\n';

            std::cout << "There is no warranty for this program, to the extent permitted by applicable law." << '\n'
                      << "Except when otherwise stated in writing the copyright holders and/or other parties" << '\n'
                      << "provide the program \"as is\" without warranty of any kind, either expressed or implied," << '\n'
                      << "including, but not limited to, the implied warranties of merchantability and" << '\n'
                      << "fitness for a particular purpose. The entire risk as to the quality and performance" << '\n'
                      << "of the program is with you. Should the program prove defective, you assume the" << '\n'
                      << "cost of all necessary servicing, repair or correction." << '\n';

            std::cout << '\n';

            std::cout << "The CppTransport runtime system contains, depends on, or bundles portions of" << '\n';
            std::cout << "the following open source projects:" << '\n' << '\n';

            std::cout << "- The Boost C++ libraries (http://www.boost.org) Boost Software License" << '\n';
            std::cout << "- The JsonCpp JSON parser (https://github.com/open-source-parsers/jsoncpp) MIT license" << '\n';
            std::cout << "- The SPLINTER spline library (https://github.com/bgrimstad/splinter) Mozilla license" << '\n';
            std::cout << "- The jQuery library (https://jquery.com/download/) MIT license" << '\n';
            std::cout << "- The Bootstrap framework (http://getbootstrap.com) MIT license" << '\n';
            std::cout << "- The bootstrap-tab-history plugin (http://mnarayan01.github.io/bootstrap-tab-history/) Apache license" << '\n';
            std::cout << "- The DataTables library (https://datatables.net) MIT license" << '\n';
            std::cout << "- The Prism.js library (http://prismjs.com) MIT license" << '\n';

            std::cout << '\n';
            std::cout << "For further details, see the NOTICE.txt file which came bundled with the" << '\n'
                      << "platform source files." << '\n';
          }


        inline void show_build_data()
          {
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_TIMESTAMP      << ": " << build_data::config_timestamp << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_C_COMPILER     << ": " << build_data::c_compiler << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_CPP_COMPILER   << ": " << build_data::cpp_compiler << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_TYPE           << ": " << build_data::build_type << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_SYSTEM         << ": " << build_data::system_name << '\n';
          }

      }   // namespace master_controller_impl

    using namespace master_controller_impl;

    template <typename number>
    master_controller<number>::master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                                 local_environment& le, argument_cache& ac, model_manager<number>& f,
                                                 task_gallery<number>& g)
      : environment(e),
        world(w),
        local_env(le),
        arg_cache(ac),
        model_mgr(f),
        gallery(g),
        data_mgr(data_manager_factory<number>(le, ac)),
        journal(w.size()-1),
        err(error_handler(le, ac)),
        warn(warning_handler(le, ac)),
        msg(message_handler(le, ac)),
        cmdline_reports(le, ac),
        HTML_reports(le, ac),
        work_scheduler(w.size() > 0 ? static_cast<unsigned int>(w.size()-1) : 0),
        worker_manager(w.size() > 0 ? static_cast<unsigned int>(w.size()-1) : 0)
      {
        // create global busy/idle timer
        busyidle_timers.add_new_timer(CPPTRANSPORT_DEFAULT_TIMER);
      }


    template <typename number>
    void master_controller<number>::process_arguments(int argc, char* argv[])
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        unsigned int width = this->local_env.detect_terminal_width();

        // set up Boost::program_options descriptors for command-line arguments
        boost::program_options::options_description generic("Generic options", width);
        generic.add_options()
          (CPPTRANSPORT_SWITCH_HELP,                                                                                         CPPTRANSPORT_HELP_HELP)
          (CPPTRANSPORT_SWITCH_VERSION,                                                                                      CPPTRANSPORT_HELP_VERSION)
          (CPPTRANSPORT_SWITCH_LICENSE,                                                                                      CPPTRANSPORT_HELP_LICENSE)
          (CPPTRANSPORT_SWITCH_INCLUDE,          boost::program_options::value< std::vector< std::string > >()->composing(), CPPTRANSPORT_HELP_INCLUDE)
          (CPPTRANSPORT_SWITCH_MODELS,                                                                                       CPPTRANSPORT_HELP_MODELS)
          (CPPTRANSPORT_SWITCH_NO_COLOUR,                                                                                    CPPTRANSPORT_HELP_NO_COLOUR);

        boost::program_options::options_description configuration("Configuration options", width);
        configuration.add_options()
          (CPPTRANSPORT_SWITCH_VERBOSE,                                                                                      CPPTRANSPORT_HELP_VERBOSE)
          (CPPTRANSPORT_SWITCH_REPO,             boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_REPO)
          (CPPTRANSPORT_SWITCH_CAPACITY,         boost::program_options::value< long int >(),                                CPPTRANSPORT_HELP_CAPACITY)
          (CPPTRANSPORT_SWITCH_BATCHER_CAPACITY, boost::program_options::value< long int >(),                                CPPTRANSPORT_HELP_BATCHER_CAPACITY)
          (CPPTRANSPORT_SWITCH_CACHE_CAPACITY,   boost::program_options::value< long int >(),                                CPPTRANSPORT_HELP_CACHE_CAPACITY)
          (CPPTRANSPORT_SWITCH_NETWORK_MODE,                                                                                 CPPTRANSPORT_HELP_NETWORK_MODE);

        boost::program_options::options_description plotting("Plot style", width);
        plotting.add_options()
          (CPPTRANSPORT_SWITCH_PLOT_STYLE,        boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_PLOT_STYLE)
          (CPPTRANSPORT_SWITCH_MPL_BACKEND,       boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_MPL_BACKEND);

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
          (CPPTRANSPORT_SWITCH_LOCK,                                                                                         CPPTRANSPORT_HELP_LOCK)
          (CPPTRANSPORT_SWITCH_UNLOCK,                                                                                       CPPTRANSPORT_HELP_UNLOCK)
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
          (CPPTRANSPORT_SWITCH_NO_COLOR,                                                                                     CPPTRANSPORT_HELP_NO_COLOR)
          (CPPTRANSPORT_SWITCH_BUILDDATA,                                                                                    CPPTRANSPORT_HELP_BUILDDATA)
          (CPPTRANSPORT_SWITCH_PROF_AGGREGATE,   boost::program_options::value< std::string >(),                             CPPTRANSPORT_HELP_PROF_AGGREGATE);

        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling).add(hidden_options);

        boost::program_options::options_description config_file_options;
        config_file_options.add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling).add(hidden_options);

        boost::program_options::options_description output_options;
        output_options.add(generic).add(configuration).add(job_options).add(action_options).add(report_options).add(plotting).add(journaling);

        // parse options from the command line; we do this first so that any options
        // supplied on the command line override options specified in a configuration file
        try
          {
            boost::program_options::parsed_options cmdline_parsed = boost::program_options::command_line_parser(argc, argv).options(cmdline_options).allow_unregistered().run();
            boost::program_options::store(cmdline_parsed, this->option_map);
            this->warn_unrecognized_switches(cmdline_parsed);
          }
        catch(boost::program_options::ambiguous_option& xe)
          {
            this->warn(xe.what());
          }
        catch(boost::program_options::invalid_command_line_style& xe)
          {
            this->warn(xe.what());
          }
        catch(boost::program_options::invalid_command_line_syntax& xe)
          {
            this->warn(xe.what());
          }
        catch(boost::program_options::invalid_syntax& xe)
          {
            this->warn(xe.what());
          }


        // parse options from configuration file
        boost::optional< boost::filesystem::path > config_path = this->local_env.config_file_path();
        if(config_path)
          {
            if(boost::filesystem::exists(*config_path) && boost::filesystem::is_regular_file(*config_path))
              {
                std::ifstream instream((*config_path).string());
                if(instream)
                  {
                    try
                      {
                        // parse contents of file; 'true' means allow unregistered options
                        boost::program_options::parsed_options file_parsed = boost::program_options::parse_config_file(instream, config_file_options, true);
                        boost::program_options::store(file_parsed, this->option_map);
                        this->warn_unrecognized_switches(file_parsed);
                      }
                    catch(boost::program_options::ambiguous_option& xe)
                      {
                        this->warn(xe.what());
                      }
                    catch(boost::program_options::invalid_config_file_syntax& xe)
                      {
                        this->warn(xe.what());
                      }
                    catch(boost::program_options::invalid_syntax& xe)
                      {
                        this->warn(xe.what());
                      }
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
                                                        this->model_mgr, repository_mode::readwrite, this->local_env, this->arg_cache);
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
                else throw;
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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        bool emitted_version = false;

        if(option_map.count(CPPTRANSPORT_SWITCH_VERSION))
          {
            std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_HELP))
          {
            if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
            std::cout << description << '\n';
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_LICENSE))
          {
            if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | " << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
            show_license_data();
          }

        if(option_map.count(CPPTRANSPORT_SWITCH_BUILDDATA)) show_build_data();
        if(option_map.count(CPPTRANSPORT_SWITCH_PROF_AGGREGATE))
          {
            boost::filesystem::path r(option_map[CPPTRANSPORT_SWITCH_PROF_AGGREGATE].as< std::string >());
            if(!r.is_absolute()) r = boost::filesystem::absolute(r);
            this->aggregation_profile_root = r;
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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(option_map.count(CPPTRANSPORT_SWITCH_VERBOSE_LONG)) this->arg_cache.set_verbose(true);

        // add search paths if any were specified
        if(option_map.count(CPPTRANSPORT_SWITCH_INCLUDE_LONG)) this->arg_cache.set_search_paths(option_map[CPPTRANSPORT_SWITCH_INCLUDE_LONG].as< std::vector<std::string> >());

        if(option_map.count(CPPTRANSPORT_SWITCH_NETWORK_MODE)) this->arg_cache.set_network_mode(true);

        // process global capacity specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CAPACITY))
          {
            long int capacity = option_map[CPPTRANSPORT_SWITCH_CAPACITY].as<long int>() * 1024*1024;            // argument size interpreted in Mb
            if(capacity > 0)
              {
                size_t cp = static_cast<size_t>(capacity);

                this->arg_cache.set_batcher_capacity(cp);
                this->arg_cache.set_datapipe_capacity(cp);
                // no need to inform data_manager of the change in capacity, since it will pick it up via the argument_cache object
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
            long int capacity = option_map[CPPTRANSPORT_SWITCH_CACHE_CAPACITY].as<long int>() * 1024*1024;      // argument size interpreted in Mb
            if(capacity > 0)
              {
                size_t cp = static_cast<size_t>(capacity);

                this->arg_cache.set_datapipe_capacity(cp);
                // no need to inform data_manager of the change in capacity, since it will pick it up via the argument_cache object
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
            long int capacity = option_map[CPPTRANSPORT_SWITCH_BATCHER_CAPACITY].as<long int>() * 1024*1024;    // argument size interpreted in Mb
            if(capacity > 0)
              {
                size_t cp = static_cast<size_t>(capacity);

                this->arg_cache.set_batcher_capacity(cp);
                // no need to inform data_manager of the change in capacity, since it will pick it up via the argument_cache object
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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        repository_toolkit<number> toolkit(*this->repo, this->err, this->warn, this->msg);

        try
          {
            if(option_map.count(CPPTRANSPORT_SWITCH_OBJECT))
              {
                if(option_map.count(CPPTRANSPORT_SWITCH_DELETE))
                  {
                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG)  || option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG) ||
                       option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE) || option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE) ||
                       option_map.count(CPPTRANSPORT_SWITCH_LOCK)     || option_map.count(CPPTRANSPORT_SWITCH_UNLOCK))
                      {
                        this->err(CPPTRANSPORT_SWITCH_DELETE_SINGLET);
                      }
                    else
                      {
                        toolkit.delete_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >());
                      }
                  }
                else
                  {
                    std::vector<std::string> add_tag;
                    std::vector<std::string> delete_tag;
                    std::vector<std::string> add_note;
                    std::vector<std::string> delete_note;

                    // unlock happens first, so it can be chained with other update flags
                    if(option_map.count(CPPTRANSPORT_SWITCH_UNLOCK)) toolkit.unlock_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >());

                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG)) add_tag = option_map[CPPTRANSPORT_SWITCH_ADD_TAG].as< std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG)) delete_tag = option_map[CPPTRANSPORT_SWITCH_DELETE_TAG].as< std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE)) add_note = option_map[CPPTRANSPORT_SWITCH_ADD_NOTE].as< std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE)) delete_note = option_map[CPPTRANSPORT_SWITCH_DELETE_NOTE].as< std::vector<std::string> >();

                    if(!add_tag.empty() || !delete_tag.empty() || !add_note.empty() || !delete_note.empty())
                      {
                        toolkit.update_tags_notes(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >(), add_tag, delete_tag, add_note, delete_note);
                      }

                    // lock happens last so it does not interfere with other update flags
                    if(option_map.count(CPPTRANSPORT_SWITCH_LOCK)) toolkit.lock_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as< std::vector<std::string> >());
                  }

              }
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


    template <typename number>
    void master_controller<number>::recognize_journal_switches(boost::program_options::variables_map& option_map)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(option_map.count(CPPTRANSPORT_SWITCH_CREATE)) this->arg_cache.set_create_mode(true);

        // process checkpoint timer specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CHECKPOINT))
          {
            int interval = option_map[CPPTRANSPORT_SWITCH_CHECKPOINT].as<int>() * 60;                 // argument size interpreted in minutes; convert value to seconds
            if(interval > 0)
              {
                unsigned int ck = static_cast<unsigned int>(interval);

                this->arg_cache.set_checkpoint_interval(ck);
                // no need to inform data_manager of new checkpoint interval; it will be pick up via the argument_cache object
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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // process plotting environment, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_PLOT_STYLE_LONG))
          {
            if(!this->arg_cache.set_plot_environment(option_map[CPPTRANSPORT_SWITCH_PLOT_STYLE_LONG].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_PLOT_STYLE << " '" << option_map[CPPTRANSPORT_SWITCH_PLOT_STYLE_LONG].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }

        // process Matplotlib environment, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_MPL_BACKEND))
          {
            if(!this->arg_cache.set_matplotlib_backend(option_map[CPPTRANSPORT_SWITCH_MPL_BACKEND].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_MPL_BACKEND << " '" << option_map[CPPTRANSPORT_SWITCH_MPL_BACKEND].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }
      }


    template <typename number>
    void master_controller<number>::pre_process_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(this->repo)
          {
            this->cmdline_reports.report(*this->repo);
          }

        if(this->repo && this->data_mgr)
          {
            this->HTML_reports.report(*this->repo, *this->data_mgr);
          }

        if(this->aggregation_profile_root)
          {
            for(const aggregation_profiler& profiler : this->aggregation_profiles)
              {
                profiler.write_to_csv(*this->aggregation_profile_root);
              }
          }
      }


    template <typename number>
    void master_controller<number>::autocomplete_task_schedule()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // check whether any work is required: if there are no jobs there is no need for autocompletion
        if(this->job_queue.empty()) return;

        // build distance matrix for tasks
        repository_graphkit<number> graphkit(*this->repo, this->err, this->warn, this->msg);
        std::unique_ptr<repository_distance_matrix> dmat = graphkit.task_distance_matrix();

        // step through list of command-line supplied tasks, building a list
        // of tasks on which they depend
        std::set< std::string > required_tasks;
        for(const job_descriptor& job : this->job_queue)
          {
            switch(job.get_type())
              {
                case job_type::job_task:
                  {
                    // use repository_graphkit to compute dependencies for this task
                    std::unique_ptr< std::set<std::string> > depends = dmat->find_dependencies(job.get_name());

                    // if there were dependencies, merge them into the list
                    if(depends) required_tasks.insert(depends->begin(), depends->end());
                  }
              }
          }

        // required_tasks holds a list of all tasks needed to generate output for the set of tasks specified on
        // the command line
        // Of these, we want to check which have no current content
        this->prune_tasks_with_content(required_tasks);

        // required_tasks now contains only those tasks for which output is needed;
        // we need to prune it for paired tasks, because these do not need to be handled separately
        this->prune_paired_tasks(required_tasks);

        // insert any required job descriptors and sort task list into order
        std::unique_ptr< std::list<std::string> > topological_order = dmat->get_graph().compute_topological_order();
        if(topological_order) this->insert_job_descriptors(required_tasks, *topological_order);
      }


    template <typename number>
    void master_controller<number>::prune_tasks_with_content(std::set<std::string>& required_tasks)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // work through list of tasks needed as dependencies, deciding whether they have content groups
        for(std::set<std::string>::const_iterator t = required_tasks.begin(); t != required_tasks.end(); /* intentionally no update step */)
          {
            std::unique_ptr< task_record<number> > rec = this->repo->query_task(*t);
            if(rec)
              {
                // check for presence of content group
                if(rec->get_content_groups().size() > 0) t = required_tasks.erase(t);   // group present; no need to schedule this task, so erase it from the list. Leaves t pointing to next item.
                else                                     ++t;                           // no groups present; move on to next item
              }
          }
      }


    template <typename number>
    void master_controller<number>::prune_paired_tasks(std::set<std::string>& required_tasks)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // build list of all tasks that will be processed, including tasks explicitly specified on the command line
        std::set<std::string>& tasks_to_schedule = required_tasks;
        for(const job_descriptor& job : this->job_queue)
          {
            tasks_to_schedule.insert(job.get_name());
          }

        // filter out any tasks in required_tasks which are only there because they're paired
        for(const std::string& task : tasks_to_schedule)
          {
            // determine whether this task has any paired tasks
            // if so, remove them from required_tasks unless they were specified on the command line
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

                // if there is a paired task, check whether it should be removed from required_tasks
                if(!paired_task.empty())
                  {
                    std::set<std::string>::const_iterator u = std::find(required_tasks.begin(), required_tasks.end(), paired_task);
                    std::list<job_descriptor>::const_iterator j = std::find_if(this->job_queue.begin(), this->job_queue.end(), FindJobDescriptorByName(paired_task));

                    // if this paired task wasn't specified on the command line, but is present in required_tasks, then remove it
                    if(j == this->job_queue.end() && u != required_tasks.end()) required_tasks.erase(u);
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::insert_job_descriptors(const std::set<std::string>& required_tasks, const std::list<std::string>& order)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up tags
        std::list<std::string> tags;
        if(this->option_map.count(CPPTRANSPORT_SWITCH_TAG) > 0)
          {
            std::vector<std::string> tmp = this->option_map[CPPTRANSPORT_SWITCH_TAG].template as<std::vector<std::string> >();
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(tags));
          }

        // add extra tasks to job queue (not yet in any particular order)
        for(const std::string& task : required_tasks)
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
    void master_controller<number>::validate_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        for(std::list<job_descriptor>::const_iterator t = this->job_queue.begin(); t != this->job_queue.end(); /* intentionally no update step */)
          {
            const job_descriptor& job = *t;

            try
              {
                std::unique_ptr< task_record<number> > record = this->repo->query_task(job.get_name());
                ++t;
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_TASK_MISSING << " '" << job.get_name() << "'";
                    this->warn(msg.str());
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_TASK_CANT_BE_READ << " '" << job.get_name() << "' (" << xe.what() << ")";
                    this->warn(msg.str());
                  }
                t = this->job_queue.erase(t);
              }
          }
      }


    template <typename number>
    void master_controller<number>::execute_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(!(this->get_rank() == 0)) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_EXEC_SLAVE);

        // set up worker scope
        // WorkerBundle will send WORKER_SETUP messages to all workers if the repository is not null
        // when it goes out of scope its destructor will send TERMINATE messages
        // this way, even if we break out of this function by an exception,
        // the workers should all get TERMINATE instructions
        WorkerBundle<number> bundle(this->environment, this->world, this->repo.get(), this->journal, this->arg_cache, this->busyidle_timers);

        if(!this->repo)
          {
            this->err(CPPTRANSPORT_REPO_NONE);
            return;
          }
        
        // validate that requested tasks can be read from the database
        // any that can't be read are removed from the job queue and a warning issued
        this->validate_tasks();
    
        // schedule extra tasks if any explicitly-required tasks depend on content from
        // a second task, but no content group is available
        this->autocomplete_task_schedule();
    
        unsigned int tasks_complete = 0;
        unsigned int tasks_processed = 0;
        for(const job_descriptor& job : this->job_queue)
          {
            // emit notification of next task to process, if we are processing multiple tasks
            if(this->job_queue.size() > 1)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PROCESSING_TASK_A << " '" << job.get_name() << "' (" << tasks_processed + 1 << " "
                    << CPPTRANSPORT_PROCESSING_TASK_OF << " " << this->job_queue.size() << ")";
                this->msg(msg.str(), message_handler::highlight::heading);
              }
        
            switch(job.get_type())
              {
                case job_type::job_task:
                  {
                    try
                      {
                        this->process_task(job);
                        ++tasks_complete;           // increment number of tasks successfully completed
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
                        std::ostringstream msg;
                        msg << CPPTRANSPORT_UNEXPECTED_UNHANDLED << " " << xe.what();
                        this->err(msg.str());
                      }
                    break;
                  }
              }
        
            ++tasks_processed;                      // increment number of tasks processed (not necessarily successfully)
          }
    
        // produce activity reports if they have been requested
        if(this->arg_cache.get_gantt_chart())
          {
            this->msg(CPPTRANSPORT_PROCESSING_GANTT_CHART, message_handler::highlight::heading);
            this->journal.make_gantt_chart(this->arg_cache.get_gantt_filename(), this->local_env, this->arg_cache);
          }
        
        if(this->arg_cache.get_journal())
          {
            this->msg(CPPTRANSPORT_PROCESSING_ACTIVITY_JOURNAL, message_handler::highlight::heading);
            this->journal.make_journal(this->arg_cache.get_journal_filename(), this->local_env, this->arg_cache);
          }
    
        // issue advisory
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

            // compute total elapsed time
            boost::timer::nanosecond_type total_time = this->busyidle_timers.get_total_time(CPPTRANSPORT_DEFAULT_TIMER);

            msg << " " << CPPTRANSPORT_PROCESSED_TASKS_C << " " << format_time(total_time);
            msg << " | " << CPPTRANSPORT_PROCESSED_TASKS_D << " ";
        
            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
            msg << boost::posix_time::to_simple_string(now);
        
            this->msg(msg.str());
          }
      }


    template <typename number>
    void master_controller<number>::process_task(const job_descriptor& job)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        catch (runtime_exception xe)    // exceptions should not occur, since records were validated. But catch exceptions just to be sure
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_MISSING << " '" << job.get_name() << "'";
                this->err(msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_CANT_BE_READ << " '" << job.get_name() << "' (" << xe.what() << ")";
                this->err(msg.str());
              }
            return;
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
    void master_controller<number>::capture_worker_properties(boost::log::sources::severity_logger<base_writer::log_severity_level>& log)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        // rebuild information about our workers; this information
        // it is updated whenever we start a new task, because the details can vary
        // between model instances (eg. CPU or GPU backends)
        this->work_scheduler.reset();
        this->worker_manager.new_task();

        while(!this->work_scheduler.is_ready())
          {
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
              {
                case MPI::WORKER_IDENTIFICATION:
                  {
                    MPI::slave_information_payload payload;
                    this->world.recv(stat.source(), MPI::WORKER_IDENTIFICATION, payload);
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
    void master_controller<number>::workers_end_of_task(boost::log::sources::severity_logger<base_writer::log_severity_level>& log)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

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
            update_data.write(update_msg, reporting::key_value::print_options::fixed_width);
            notifications.write(update_msg, reporting::key_value::print_options::fixed_width);

            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "±± Console advisory message: " << update_msg.str();
            this->log_worker_metadata(writer);
          }
      }


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::log_worker_metadata(WriterObject& writer)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        bool write_separator = false;
        for(unsigned int i = 0; i < this->work_scheduler.size(); ++i)
          {
            const worker_information& info = this->work_scheduler[i];
            const worker_management_data& data = this->worker_manager[i];
            
            reporting::key_value kv(this->local_env, this->arg_cache);
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_NUMBER, boost::lexical_cast<std::string>(info.get_number()));
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_PROCESSED_ITEMS, boost::lexical_cast<std::string>(info.get_number_items()));
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_ASSIGNED, info.is_assigned() ? CPPTRANSPORT_WORKER_DATA_YES : CPPTRANSPORT_WORKER_DATA_NO);
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_INTEGRATION_TIME, format_time(info.get_total_time()));
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_MEAN_TIME_PER_ITEM, format_time(info.get_mean_time_per_work_item()));
            kv.insert_back(CPPTRANSPORT_WORKER_DATA_LAST_CONTACT, boost::posix_time::to_simple_string(data.get_last_contact_time()));
            
            std::ostringstream msg;
            kv.set_tiling(true);
            kv.write(msg);
            
            if(!write_separator)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "####################";
                write_separator = true;
              }
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << msg.str();
          }
    
        // omit closing #s if we didn't write any opening ones
        if(write_separator) BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "####################";
      }


    template <typename number>
    void master_controller<number>::set_local_checkpoint_interval(unsigned int m)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        std::vector<boost::mpi::request> requests(world.size()-1);

        for(unsigned int i = 0; i < world.size()-1; ++i)
          {
            requests[i] = world.isend(this->worker_rank(i), MPI::SET_LOCAL_CHECKPOINT, m*60);
          }

        timers.idle();
        boost::mpi::wait_all(requests.begin(), requests.end());
        timers.busy();

        this->data_mgr->set_local_checkpoint_interval(m*60);
      }


    template <typename number>
    void master_controller<number>::unset_local_checkpoint_interval()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        std::vector<boost::mpi::request> requests(world.size()-1);

        for(unsigned int i = 0; i < world.size()-1; ++i)
          {
            requests[i] = world.isend(this->worker_rank(i), MPI::UNSET_LOCAL_CHECKPOINT);
          }

        timers.idle();
        boost::mpi::wait_all(requests.begin(), requests.end());
        timers.busy();

        this->data_mgr->unset_local_checkpoint_interval();
      }


    template <typename number>
    void master_controller<number>::assign_work_to_workers(boost::log::sources::severity_logger< base_writer::log_severity_level >& log)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        // generate a list of work assignments
        std::list<work_assignment> work = this->work_scheduler.assign_work(log);

        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Generating new work assignments for " << work.size() << " worker" << (work.size() > 1 ? "s" : "") << " (" << this->work_scheduler.get_queue_size() << " work items remain in queue)";

        // push assignments to workers
        std::vector<boost::mpi::request> msg_status(work.size());

        unsigned int c = 0;
        for(const work_assignment& assgn : work)
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
        timers.idle();
        boost::mpi::wait_all(msg_status.begin(), msg_status.end());
        timers.busy();
      }


    template <typename number>
    template <typename WriterObject>
    bool master_controller<number>::poll_workers(integration_aggregator<number>& int_agg, postintegration_aggregator<number>& post_agg, derived_content_aggregator<number>& derived_agg,
                                                 integration_metadata& int_metadata, output_metadata& out_metadata, std::list<std::string>& content_groups,
                                                 WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        bool success = true;

        boost::log::sources::severity_logger< base_writer::log_severity_level >& log = writer.get_log();

        // set up aggregation queues
        unsigned int aggregation_counter = 0;
        std::list< std::unique_ptr<aggregation_record> > aggregation_queue;

        // wait for workers to report their characteristics
        this->capture_worker_properties(log);

        // CloseDownContext object is responsible for calling this->workers_end_of_task()
        // when needed; should do so even if we exit this function via an exception
        CloseDownContext<number> closedown_handler(*this, log);

        // record time of last-received message, so we can determine for how long we have been idle
        boost::posix_time::ptime last_msg_time = boost::posix_time::second_clock::universal_time();
        bool emit_agg_queue_msg = true;

        // poll workers, scattering work and aggregating the results until work items are exhausted
        timers.idle();
        while(!this->work_scheduler.all_inactive())
          {
            timers.busy();
            // send closedown instruction if no more work
            if(this->work_scheduler.is_finished() && !closedown_handler()) closedown_handler.send_closedown();

            // generate new work assignments if needed, and push them to the workers
            if(this->work_scheduler.assignable())
              {
                this->check_for_progress_update(writer);
                this->assign_work_to_workers(log);
              }

            // check whether any messages are waiting in the queue
            timers.idle();
            boost::optional<boost::mpi::status> stat = this->world.iprobe();

            while(stat) // consume messages until no more are available
              {
                timers.busy();

                // update time of last received message and reset flag which enables logging
                last_msg_time = boost::posix_time::second_clock::universal_time();
                emit_agg_queue_msg = true;

                this->worker_manager.update_contact_time(this->worker_number(stat->source()), last_msg_time);

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
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path().string() << "'";
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
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path().string() << "'";
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

                timers.idle();
                stat = this->world.iprobe();  // check for another message
              }

            // we arrive at this point only when no more messages are available to be received

            // check whether any aggregations are in the queue, and process them if we have been idle for sufficiently long
            if(aggregation_queue.size() > 0)
              {
                timers.busy();

                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                boost::posix_time::time_duration idle_time = now - last_msg_time;
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

        timers.busy();
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
