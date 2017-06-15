//
// Created by David Seery on 30/11/2015.
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


#include <iostream>
#include <fstream>

#include "argument_cache.h"

#include "core.h"
#include "switches.h"
#include "msg_en.h"

#include "build_data.h"


namespace argument_cache_impl
  {

    inline void show_license_data()
      {
        std::cout << '\n';
        std::cout << "The CppTransport translator is free software, and you are welcome to redistribute" << '\n'
                  << "it under certain conditions; see the bundled LICENSE.txt file for details." << '\n';
        std::cout << "Contact: David Seery <D.Seery@sussex.ac.uk>" << '\n';

        std::cout << '\n';

        std::cout << "There is no warranty for this program, to the extent permitted by applicable law." << '\n'
                  << "Except when otherwise stated in writing the copyright holders and/or other parties" << '\n'
                  << "provide the program \"as is\" without warranty of any kind, either expressed or implied," << '\n'
                  << "including, but not limited to, the implied warranties of merchantability and" << '\n'
                  << "fitness for a particular purpose. The entire risk as to the quality and performance" << '\n'
                  << "of the program is with you. Should the program prove defective, you assume the" << '\n'
                  << "cost of all necessary servicing, repair or correction." << '\n';

        std::cout << '\n';

        std::cout << "The CppTransport translator contains or depends on portions of" << '\n';
        std::cout << "the following open source projects:" << '\n' << '\n';

        std::cout << "- The GiNaC computer algebra library (http://www.ginac.de) GPL-2" << '\n';
        std::cout << "- The Boost C++ libraries (http://www.boost.org) Boost Software License" << '\n';
        std::cout << "- Common-subexpression elimination for GiNaC is adapted from the" << '\n';
        std::cout << "  GinacPrint implementation by Doug Baker" << '\n';
        std::cout << "  (http://www.ginac.de/pipermail/ginac-list/2010-May/001631.html) GPL-2" << '\n';

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

  }   // namespace argument_cache_impl


using namespace argument_cache_impl;

argument_cache::argument_cache(int argc, const char** argv, local_environment& env)
  : verbose_flag(false),
    colour_flag(true),
    cse_flag(true),
    no_search_environment(false),
    annotate_flag(false),
    unroll_policy_size(DEFAULT_UNROLL_MAX),
    fast_flag(false)
  {
    // set up Boost::program_options descriptors for command-line arguments
    boost::program_options::options_description generic(MISC_OPTIONS);
    generic.add_options()
      (VERSION_SWITCH, VERSION_HELP)
      (HELP_SWITCH,    HELP_HELP)
      (LICENSE_SWITCH, LICENSE_HELP);

    boost::program_options::options_description configuration(CONFIG_OPTIONS);
    configuration.add_options()
      (VERBOSE_SWITCH,                                                                                         VERBOSE_HELP)
      (INCLUDE_SWITCH,               boost::program_options::value< std::vector<std::string> >()->composing(), INCLUDE_HELP)
      (NO_ENV_SEARCH_SWITCH,                                                                                   NO_ENV_SEARCH_HELP)
      (CORE_OUTPUT_SWITCH,           boost::program_options::value< std::string >()->default_value(""),        CORE_OUTPUT_HELP)
      (IMPLEMENTATION_OUTPUT_SWITCH, boost::program_options::value< std::string >()->default_value(""),        IMPLEMENTATION_OUTPUT_HELP)
      (NO_COLOUR_SWITCH,                                                                                       NO_COLOUR_HELP);

    boost::program_options::options_description generation(GENERATION_OPTIONS);
    generation.add_options()
      (NO_CSE_SWITCH,                                                                                            NO_CSE_HELP)
      (ANNOTATE_SWITCH,                                                                                          ANNOTATE_HELP)
      (UNROLL_POLICY_SWITCH, boost::program_options::value< unsigned int >()->default_value(DEFAULT_UNROLL_MAX), UNROLL_POLICY_HELP)
      (FAST_SWITCH,                                                                                              FAST_HELP);

    boost::program_options::options_description hidden(HIDDEN_OPTIONS);
    hidden.add_options()
      (INPUT_FILE_SWITCH, boost::program_options::value< std::vector<std::string> >(), INPUT_FILE_HELP)
      (NO_COLOR_SWITCH,                                                                NO_COLOR_HELP)
      (BUILDDATA_SWITCH,                                                               BUILDDATA_HELP);

    boost::program_options::positional_options_description positional_options;
    positional_options.add(INPUT_FILE_SWITCH, -1);

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(configuration).add(hidden).add(generation);

    boost::program_options::options_description config_file_options;
    config_file_options.add(configuration).add(generation);

    boost::program_options::options_description visible;
    visible.add(generic).add(configuration).add(generation);

    boost::program_options::variables_map option_map;

    // parse options from command line; we do this first so that any options specified on the command line
    // override those specified in a configuration file
    try
      {
        boost::program_options::parsed_options cmdline_parsed = boost::program_options::command_line_parser(argc, argv).options(cmdline_options).positional(positional_options).allow_unregistered().run();
        boost::program_options::store(cmdline_parsed, option_map);

        // inform the user that we have ignored any unrecognized options
        std::vector<std::string> unrecognized_cmdline_options = boost::program_options::collect_unrecognized(cmdline_parsed.options, boost::program_options::exclude_positional);
        if(unrecognized_cmdline_options.size() > 0)
          {
            for(const std::string& option : unrecognized_cmdline_options)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_NAME << ": " << WARNING_UNKNOWN_SWITCH << " '" << option << "'" << '\n';
                this->err_msgs.push_back(std::make_pair(false, msg.str()));
              }
          }
      }
    catch(boost::program_options::ambiguous_option& xe)
      {
        this->err_msgs.push_back(std::make_pair(true, xe.what()));
      }
    catch(boost::program_options::invalid_command_line_style& xe)
      {
        this->err_msgs.push_back(std::make_pair(true, xe.what()));
      }
    catch(boost::program_options::invalid_command_line_syntax& xe)
      {
        this->err_msgs.push_back(std::make_pair(true, xe.what()));
      }
    catch(boost::program_options::invalid_syntax& xe)
      {
        this->err_msgs.push_back(std::make_pair(true, xe.what()));
      }


    // parse options from configuration file
    boost::optional< boost::filesystem::path > config_path = env.config_file_path();
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
                    boost::program_options::store(file_parsed, option_map);
                    boost::program_options::notify(option_map);

                    std::vector<std::string> unrecognized_config_options = boost::program_options::collect_unrecognized(file_parsed.options, boost::program_options::exclude_positional);
                    if(unrecognized_config_options.size() > 0)
                      {
                        for(const std::string& option : unrecognized_config_options)
                          {
                            std::ostringstream msg;
                            msg << CPPTRANSPORT_NAME << ": " << WARNING_UNKNOWN_SWITCH << " '" << option << "'" << '\n';
                            this->err_msgs.push_back(std::make_pair(false, msg.str()));
                          }
                      }
                  }
                catch(boost::program_options::ambiguous_option& xe)
                  {
                    this->err_msgs.push_back(std::make_pair(true, xe.what()));
                  }
                catch(boost::program_options::invalid_config_file_syntax& xe)
                  {
                    this->err_msgs.push_back(std::make_pair(true, xe.what()));
                  }
                catch(boost::program_options::invalid_syntax& xe)
                  {
                    this->err_msgs.push_back(std::make_pair(true, xe.what()));
                  }
              }
          }
      }

    // inform the Boost::Program_Options library that option parsing is complete
    boost::program_options::notify(option_map);

    // HANDLE OPTIONS

    bool emitted_version = false;

    // MISCELLANEOUS OPTIONS
    if(option_map.count(VERSION_SWITCH))
      {
        std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << '\n';
        emitted_version = true;
      }

    if(option_map.count(HELP_SWITCH))
      {
        if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << '\n';
        emitted_version = true;
        std::cout << visible << '\n';
      }

    if(option_map.count(LICENSE_SWITCH))
      {
        if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << '\n';
        emitted_version = true;
        show_license_data();
      }

    if(option_map.count(BUILDDATA_SWITCH)) show_build_data();

    // CODE GENERATION OPTIONS
    if(option_map.count(NO_CSE_SWITCH)) this->cse_flag = false;
    if(option_map.count(ANNOTATE_SWITCH)) this->annotate_flag = true;
    if(option_map.count(UNROLL_POLICY_SWITCH)) this->unroll_policy_size = option_map[UNROLL_POLICY_SWITCH].as<unsigned int>();
    if(option_map.count(FAST_SWITCH)) this->fast_flag = true;

    // CONFIGURATION OPTIONS
    if(option_map.count(VERBOSE_SWITCH_LONG)) this->verbose_flag = true;
    if(option_map.count(NO_ENV_SEARCH_SWITCH)) this->no_search_environment = true;
    if(option_map.count(CORE_OUTPUT_SWITCH) > 0) this->core_output = option_map[CORE_OUTPUT_SWITCH].as<std::string>();
    if(option_map.count(IMPLEMENTATION_OUTPUT_SWITCH) > 0) this->implementation_output = option_map[IMPLEMENTATION_OUTPUT_SWITCH].as<std::string>();
    if(option_map.count(NO_COLOUR_SWITCH) || option_map.count(NO_COLOR_SWITCH)) this->colour_flag = false;

    if(option_map.count(INCLUDE_SWITCH_LONG) > 0)
      {
        std::vector<std::string> include_paths = option_map[INCLUDE_SWITCH_LONG].as< std::vector<std::string> >();

        for(const std::string& path : include_paths)
          {
            boost::filesystem::path p = path;
            if(!p.is_absolute()) p = boost::filesystem::absolute(p);

            if(p.has_leaf() && p.leaf() == boost::filesystem::path(CPPTRANSPORT_TEMPLATE_PATH))
              {
                std::ostringstream msg;
                msg << NOTIFY_PATH_INCLUDES_TEMPLATES << ": '" << p.string() << "'";
                this->err_msgs.push_back(std::make_pair(false, msg.str()));
              }

            this->search_path_list.emplace_back(p / CPPTRANSPORT_TEMPLATE_PATH);
          }
      }

    if(option_map.count(INPUT_FILE_SWITCH) > 0)
      {
        std::vector<std::string> input_files = option_map[INPUT_FILE_SWITCH].as< std::vector<std::string> >();
        std::copy(input_files.cbegin(), input_files.cend(), std::back_inserter(this->input_file_list));
      }
  }
