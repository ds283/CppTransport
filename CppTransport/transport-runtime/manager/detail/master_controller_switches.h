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

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_SWITCHES_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_SWITCHES_H


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
            std::cout << "For model licensing information: contact details should be embedded within the model file."
                      << '\n';
            
            std::cout << '\n';
            
            std::cout << "There is no warranty for this program, to the extent permitted by applicable law." << '\n'
                      << "Except when otherwise stated in writing the copyright holders and/or other parties" << '\n'
                      << "provide the program \"as is\" without warranty of any kind, either expressed or implied,"
                      << '\n'
                      << "including, but not limited to, the implied warranties of merchantability and" << '\n'
                      << "fitness for a particular purpose. The entire risk as to the quality and performance" << '\n'
                      << "of the program is with you. Should the program prove defective, you assume the" << '\n'
                      << "cost of all necessary servicing, repair or correction." << '\n';
            
            std::cout << '\n';
            
            std::cout << "The CppTransport runtime system contains, depends on, or bundles portions of" << '\n';
            std::cout << "the following open source projects:" << '\n' << '\n';
            
            std::cout << "- The Boost C++ libraries (http://www.boost.org) Boost Software License" << '\n';
            std::cout << "- The JsonCpp JSON parser (https://github.com/open-source-parsers/jsoncpp) MIT license"
                      << '\n';
            std::cout << "- The SPLINTER spline library (https://github.com/bgrimstad/splinter) Mozilla license"
                      << '\n';
            std::cout << "- The jQuery library (https://jquery.com/download/) MIT license" << '\n';
            std::cout << "- The Bootstrap framework (http://getbootstrap.com) MIT license" << '\n';
            std::cout
              << "- The bootstrap-tab-history plugin (http://mnarayan01.github.io/bootstrap-tab-history/) Apache license"
              << '\n';
            std::cout << "- The DataTables library (https://datatables.net) MIT license" << '\n';
            std::cout << "- The Prism.js library (http://prismjs.com) MIT license" << '\n';
            
            std::cout << '\n';
            std::cout << "For further details, see the NOTICE.txt file which came bundled with the" << '\n'
                      << "platform source files." << '\n';
          }
        
        
        inline void show_build_data()
          {
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_TIMESTAMP << ": " << build_data::config_timestamp << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_C_COMPILER << ": " << build_data::c_compiler << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_CPP_COMPILER << ": " << build_data::cpp_compiler << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_TYPE << ": " << build_data::build_type << '\n';
            std::cout << build_data::CPPTRANSPORT_BUILD_DATA_SYSTEM << ": " << build_data::system_name << '\n';
          }
        
      }   // namespace master_controller_impl
    
    using namespace master_controller_impl;
    
    
    template <typename number>
    void master_controller<number>::process_arguments(int argc, char* argv[])
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        unsigned int width = this->local_env.detect_terminal_width();
        
        // set up Boost::program_options descriptors for command-line arguments
        boost::program_options::options_description generic("Generic options", width);
        generic.add_options()
          (CPPTRANSPORT_SWITCH_HELP, CPPTRANSPORT_HELP_HELP)
          (CPPTRANSPORT_SWITCH_VERSION, CPPTRANSPORT_HELP_VERSION)
          (CPPTRANSPORT_SWITCH_LICENSE, CPPTRANSPORT_HELP_LICENSE)
          (CPPTRANSPORT_SWITCH_INCLUDE, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_INCLUDE)
          (CPPTRANSPORT_SWITCH_MODELS, CPPTRANSPORT_HELP_MODELS)
          (CPPTRANSPORT_SWITCH_NO_COLOUR, CPPTRANSPORT_HELP_NO_COLOUR);
        
        boost::program_options::options_description configuration("Configuration options", width);
        configuration.add_options()
          (CPPTRANSPORT_SWITCH_VERBOSE, CPPTRANSPORT_HELP_VERBOSE)
          (CPPTRANSPORT_SWITCH_REPO, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_REPO)
          (CPPTRANSPORT_SWITCH_CAPACITY, boost::program_options::value<long int>(), CPPTRANSPORT_HELP_CAPACITY)
          (CPPTRANSPORT_SWITCH_BATCHER_CAPACITY, boost::program_options::value<long int>(),
           CPPTRANSPORT_HELP_BATCHER_CAPACITY)
          (CPPTRANSPORT_SWITCH_CACHE_CAPACITY, boost::program_options::value<long int>(),
           CPPTRANSPORT_HELP_CACHE_CAPACITY)
          (CPPTRANSPORT_SWITCH_NETWORK_MODE, CPPTRANSPORT_HELP_NETWORK_MODE);
        
        boost::program_options::options_description plotting("Plot styling", width);
        plotting.add_options()
          (CPPTRANSPORT_SWITCH_PLOT_STYLE, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_PLOT_STYLE)
          (CPPTRANSPORT_SWITCH_MPL_BACKEND, boost::program_options::value<std::string>(),
           CPPTRANSPORT_HELP_MPL_BACKEND);
        
        boost::program_options::options_description task_reporting("In-progress task reporting", width);
        task_reporting.add_options()
          (CPPTRANSPORT_SWITCH_REPORT_PERCENT, boost::program_options::value<int>(), CPPTRANSPORT_HELP_REPORT_PERCENT)
          (CPPTRANSPORT_SWITCH_REPORT_INTERVAL, boost::program_options::value<std::string>(),
           CPPTRANSPORT_HELP_REPORT_INTERVAL)
          (CPPTRANSPORT_SWITCH_REPORT_EMAIL, boost::program_options::value<std::string>(),
           CPPTRANSPORT_HELP_REPORT_EMAIL)
          (CPPTRANSPORT_SWITCH_REPORT_DELAY, boost::program_options::value<std::string>(),
           CPPTRANSPORT_HELP_REPORT_DELAY);
        
        boost::program_options::options_description journaling("Journaling options", width);
        journaling.add_options()
          (CPPTRANSPORT_SWITCH_GANTT_CHART, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_GANTT_CHART)
          (CPPTRANSPORT_SWITCH_JOURNAL, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_JOURNAL);
        
        boost::program_options::options_description job_options("Job specification", width);
        job_options.add_options()
          (CPPTRANSPORT_SWITCH_CREATE, CPPTRANSPORT_HELP_CREATE)
          (CPPTRANSPORT_SWITCH_TASK, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_TASK)
          (CPPTRANSPORT_SWITCH_TAG, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_TAG)
          (CPPTRANSPORT_SWITCH_CHECKPOINT, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_CHECKPOINT)
          (CPPTRANSPORT_SWITCH_SEED, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_SEED);
        
        boost::program_options::options_description action_options("Repository actions", width);
        action_options.add_options()
          (CPPTRANSPORT_SWITCH_OBJECT, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_OBJECT)
          (CPPTRANSPORT_SWITCH_LOCK, CPPTRANSPORT_HELP_LOCK)
          (CPPTRANSPORT_SWITCH_UNLOCK, CPPTRANSPORT_HELP_UNLOCK)
          (CPPTRANSPORT_SWITCH_ADD_TAG, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_ADD_TAG)
          (CPPTRANSPORT_SWITCH_DELETE_TAG, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_DELETE_TAG)
          (CPPTRANSPORT_SWITCH_ADD_NOTE, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_ADD_NOTE)
          (CPPTRANSPORT_SWITCH_DELETE_NOTE, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_DELETE_NOTE)
          (CPPTRANSPORT_SWITCH_DELETE, CPPTRANSPORT_HELP_DELETE);
        
        boost::program_options::options_description report_options("Repository reporting and status", width);
        report_options.add_options()
          (CPPTRANSPORT_SWITCH_RECOVER, CPPTRANSPORT_HELP_RECOVER)
          (CPPTRANSPORT_SWITCH_STATUS, CPPTRANSPORT_HELP_STATUS)
          (CPPTRANSPORT_SWITCH_INFLIGHT, CPPTRANSPORT_HELP_INFLIGHT)
          (CPPTRANSPORT_SWITCH_INFO, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_INFO)
          (CPPTRANSPORT_SWITCH_PROVENANCE, boost::program_options::value<std::vector<std::string> >()->composing(),
           CPPTRANSPORT_HELP_PROVENANCE)
          (CPPTRANSPORT_SWITCH_HTML, boost::program_options::value<std::string>(), CPPTRANSPORT_HELP_HTML);
        
        boost::program_options::options_description hidden_options;
        hidden_options.add_options()
          (CPPTRANSPORT_SWITCH_NO_COLOR, CPPTRANSPORT_HELP_NO_COLOR)
          (CPPTRANSPORT_SWITCH_BUILDDATA, CPPTRANSPORT_HELP_BUILDDATA)
          (CPPTRANSPORT_SWITCH_PROF_AGGREGATE, boost::program_options::value<std::string>(),
           CPPTRANSPORT_HELP_PROF_AGGREGATE);
        
        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(configuration).add(job_options).add(task_reporting).add(action_options).add(
          report_options).add(plotting).add(journaling).add(hidden_options);
        
        boost::program_options::options_description config_file_options;
        config_file_options.add(configuration).add(job_options).add(task_reporting).add(action_options).add(
          report_options).add(plotting).add(journaling).add(hidden_options);
        
        boost::program_options::options_description output_options;
        output_options.add(generic).add(configuration).add(job_options).add(task_reporting).add(action_options).add(
          report_options).add(plotting).add(journaling);
        
        // parse options from the command line; we do this first so that any options
        // supplied on the command line override options specified in a configuration file
        try
          {
            boost::program_options::parsed_options cmdline_parsed = boost::program_options::command_line_parser(argc,
                                                                                                                argv).options(
              cmdline_options).allow_unregistered().run();
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
        boost::optional<boost::filesystem::path> config_path = this->local_env.config_file_path();
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
                        boost::program_options::parsed_options file_parsed = boost::program_options::parse_config_file(
                          instream, config_file_options, true);
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
                this->repo = repository_factory<number>(
                  this->option_map[CPPTRANSPORT_SWITCH_REPO_LONG].template as<std::string>(),
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
        this->recognize_report_switches(this->option_map);
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
            std::vector<std::string> tasks = this->option_map[CPPTRANSPORT_SWITCH_TASK].template as<std::vector<std::string> >();
            
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
        
        std::vector<std::string> unrecognized_options = boost::program_options::collect_unrecognized(options.options,
                                                                                                     boost::program_options::exclude_positional);
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
    void master_controller<number>::recognize_generic_switches(boost::program_options::variables_map& option_map,
                                                               boost::program_options::options_description& description)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        bool emitted_version = false;
        
        if(option_map.count(CPPTRANSPORT_SWITCH_VERSION))
          {
            std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | "
                      << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_HELP))
          {
            if(!emitted_version)
              std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | "
                        << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
            std::cout << description << '\n';
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_LICENSE))
          {
            if(!emitted_version)
              std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << " | "
                        << CPPTRANSPORT_RUNTIME_API << '\n';
            emitted_version = true;
            show_license_data();
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_BUILDDATA)) show_build_data();
        if(option_map.count(CPPTRANSPORT_SWITCH_PROF_AGGREGATE))
          {
            boost::filesystem::path r(option_map[CPPTRANSPORT_SWITCH_PROF_AGGREGATE].as<std::string>());
            if(!r.is_absolute()) r = boost::filesystem::absolute(r);
            this->aggregation_profile_root = r;
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_MODELS))
          {
            this->arg_cache.set_model_list(true);
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_NO_COLOUR) || option_map.count(CPPTRANSPORT_SWITCH_NO_COLOR))
          this->arg_cache.set_colour_output(false);
      }
    
    
    template <typename number>
    void master_controller<number>::recognize_configuration_switches(boost::program_options::variables_map& option_map)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        if(option_map.count(CPPTRANSPORT_SWITCH_VERBOSE_LONG)) this->arg_cache.set_verbose(true);
        
        // add search paths if any were specified
        if(option_map.count(CPPTRANSPORT_SWITCH_INCLUDE_LONG))
          this->arg_cache.set_search_paths(
            option_map[CPPTRANSPORT_SWITCH_INCLUDE_LONG].as<std::vector<std::string> >());
        
        if(option_map.count(CPPTRANSPORT_SWITCH_NETWORK_MODE)) this->arg_cache.set_network_mode(true);
        
        // process global capacity specification, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_CAPACITY))
          {
            long int capacity = -1;
            try
              {
                capacity = option_map[CPPTRANSPORT_SWITCH_CAPACITY].as < long
                int > () * 1024 * 1024;            // argument size interpreted in Mb
              }
            catch(boost::exception& xe)
              {
              }
            
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
            long int capacity = -1;
            try
              {
                capacity = option_map[CPPTRANSPORT_SWITCH_CACHE_CAPACITY].as < long
                int > () * 1024 * 1024;      // argument size interpreted in Mb
              }
            catch(boost::exception& xe)
              {
              }
            
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
            long int capacity = -1;
            try
              {
                capacity = option_map[CPPTRANSPORT_SWITCH_BATCHER_CAPACITY].as < long
                int > () * 1024 * 1024;    // argument size interpreted in Mb
              }
            catch(boost::exception& xe)
              {
              }
            
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
        
        if(option_map.count(CPPTRANSPORT_SWITCH_INFO))
          this->cmdline_reports.info(option_map[CPPTRANSPORT_SWITCH_INFO].as<std::vector<std::string> >());
        if(option_map.count(CPPTRANSPORT_SWITCH_PROVENANCE))
          this->cmdline_reports.provenance(option_map[CPPTRANSPORT_SWITCH_PROVENANCE].as<std::vector<std::string> >());
        
        if(option_map.count(CPPTRANSPORT_SWITCH_HTML))
          this->HTML_reports.set_root(option_map[CPPTRANSPORT_SWITCH_HTML].as<std::string>());
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
                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG) ||
                       option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG) ||
                       option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE) ||
                       option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE) ||
                       option_map.count(CPPTRANSPORT_SWITCH_LOCK) || option_map.count(CPPTRANSPORT_SWITCH_UNLOCK))
                      {
                        this->err(CPPTRANSPORT_SWITCH_DELETE_SINGLET);
                      }
                    else
                      {
                        toolkit.delete_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as<std::vector<std::string> >());
                      }
                  }
                else
                  {
                    std::vector<std::string> add_tag;
                    std::vector<std::string> delete_tag;
                    std::vector<std::string> add_note;
                    std::vector<std::string> delete_note;
                    
                    // unlock happens first, so it can be chained with other update flags
                    if(option_map.count(CPPTRANSPORT_SWITCH_UNLOCK))
                      toolkit.unlock_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as<std::vector<std::string> >());
                    
                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_TAG))
                      add_tag = option_map[CPPTRANSPORT_SWITCH_ADD_TAG].as<std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_TAG))
                      delete_tag = option_map[CPPTRANSPORT_SWITCH_DELETE_TAG].as<std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_ADD_NOTE))
                      add_note = option_map[CPPTRANSPORT_SWITCH_ADD_NOTE].as<std::vector<std::string> >();
                    if(option_map.count(CPPTRANSPORT_SWITCH_DELETE_NOTE))
                      delete_note = option_map[CPPTRANSPORT_SWITCH_DELETE_NOTE].as<std::vector<std::string> >();
                    
                    if(!add_tag.empty() || !delete_tag.empty() || !add_note.empty() || !delete_note.empty())
                      {
                        toolkit.update_tags_notes(
                          option_map[CPPTRANSPORT_SWITCH_OBJECT].as<std::vector<std::string> >(), add_tag, delete_tag,
                          add_note, delete_note);
                      }
                    
                    // lock happens last so it does not interfere with other update flags
                    if(option_map.count(CPPTRANSPORT_SWITCH_LOCK))
                      toolkit.lock_content(option_map[CPPTRANSPORT_SWITCH_OBJECT].as<std::vector<std::string> >());
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
            if(!this->arg_cache.set_checkpoint_interval(option_map[CPPTRANSPORT_SWITCH_CHECKPOINT].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_CHECKPOINT_INTERVAL << " '"
                    << option_map[CPPTRANSPORT_SWITCH_CHECKPOINT].as<std::string>() << "'";
                this->warn(msg.str());
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
                msg << CPPTRANSPORT_UNKNOWN_PLOT_STYLE << " '"
                    << option_map[CPPTRANSPORT_SWITCH_PLOT_STYLE_LONG].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }
        
        // process Matplotlib environment, if provided
        if(option_map.count(CPPTRANSPORT_SWITCH_MPL_BACKEND))
          {
            if(!this->arg_cache.set_matplotlib_backend(option_map[CPPTRANSPORT_SWITCH_MPL_BACKEND].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_MPL_BACKEND << " '"
                    << option_map[CPPTRANSPORT_SWITCH_MPL_BACKEND].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }
      }
    
    
    template <typename number>
    void master_controller<number>::recognize_report_switches(boost::program_options::variables_map& option_map)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        if(option_map.count(CPPTRANSPORT_SWITCH_REPORT_PERCENT))
          {
            int value = -1;
            try
              {
                value = option_map[CPPTRANSPORT_SWITCH_REPORT_PERCENT].as<int>();
              }
            catch(boost::exception& xe)
              {
              }
            
            if(value < 0 || !this->arg_cache.set_report_percent_interval(static_cast<unsigned int>(value)))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_REPORT_INTERVAL << " '"
                    << option_map[CPPTRANSPORT_SWITCH_REPORT_PERCENT].as<int>() << "%'";
                this->warn(msg.str());
              }
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_REPORT_INTERVAL))
          {
            if(!this->arg_cache.set_report_time_interval(
              option_map[CPPTRANSPORT_SWITCH_REPORT_INTERVAL].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_REPORT_INTERVAL << " '"
                    << option_map[CPPTRANSPORT_SWITCH_REPORT_INTERVAL].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_REPORT_DELAY))
          {
            if(!this->arg_cache.set_report_time_delay(option_map[CPPTRANSPORT_SWITCH_REPORT_DELAY].as<std::string>()))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_UNKNOWN_REPORT_DELAY << " '"
                    << option_map[CPPTRANSPORT_SWITCH_REPORT_DELAY].as<std::string>() << "'";
                this->warn(msg.str());
              }
          }
        
        if(option_map.count(CPPTRANSPORT_SWITCH_REPORT_EMAIL))
          this->arg_cache.set_report_email(option_map[CPPTRANSPORT_SWITCH_REPORT_EMAIL].as<std::string>());
      }
    
  }   // namespace transport

#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_SWITCHES_H
