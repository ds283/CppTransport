//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include <iostream>
#include <algorithm>

#include "argument_cache.h"

#include "core.h"
#include "switches.h"


argument_cache::argument_cache(int argc, const char** argv)
  : verbose_flag(false),
    colour_flag(true),
    cse_flag(true),
    no_search_environment(false),
    annotate_flag(false),
    unroll_policy_size(DEFAULT_UNROLL_MAX)
  {
    // set up Boost::program_options descriptors for command-line arguments
    boost::program_options::options_description generic(MISC_OPTIONS);
    generic.add_options()
      (VERSION_SWITCH, VERSION_HELP)
      (HELP_SWITCH,    HELP_HELP);

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
      (UNROLL_POLICY_SWITCH, boost::program_options::value< unsigned int >()->default_value(DEFAULT_UNROLL_MAX), UNROLL_POLICY_HELP);

    boost::program_options::options_description hidden(HIDDEN_OPTIONS);
    hidden.add_options()
      (INPUT_FILE_SWITCH, boost::program_options::value< std::vector<std::string> >(), INPUT_FILE_HELP)
      (NO_COLOR_SWITCH,                                                                NO_COLOR_HELP);

    boost::program_options::positional_options_description positional_options;
    positional_options.add(INPUT_FILE_SWITCH, -1);

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(configuration).add(hidden).add(generation);

    boost::program_options::options_description config_file_options;
    config_file_options.add(configuration).add(generation);

    boost::program_options::options_description visible;
    visible.add(generic).add(configuration).add(generation);

    boost::program_options::variables_map option_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).positional(positional_options).run(), option_map);
    boost::program_options::notify(option_map);

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
        std::cout << visible << '\n';
      }

    // CODE GENERATION OPTIONS
    if(option_map.count(NO_CSE_SWITCH)) this->cse_flag = false;
    if(option_map.count(ANNOTATE_SWITCH)) this->annotate_flag = true;
    if(option_map.count(UNROLL_POLICY_SWITCH)) this->unroll_policy_size = option_map[UNROLL_POLICY_SWITCH].as<unsigned int>();

    // CONFIGURATION OPTIONS
    if(option_map.count(VERBOSE_SWITCH_LONG)) this->verbose_flag = true;
    if(option_map.count(NO_ENV_SEARCH_SWITCH)) this->no_search_environment = true;
    if(option_map.count(CORE_OUTPUT_SWITCH) > 0) this->core_output = option_map[CORE_OUTPUT_SWITCH].as<std::string>();
    if(option_map.count(IMPLEMENTATION_OUTPUT_SWITCH) > 0) this->implementation_output = option_map[IMPLEMENTATION_OUTPUT_SWITCH].as<std::string>();
    if(option_map.count(NO_COLOUR_SWITCH) || option_map.count(NO_COLOR_SWITCH)) this->colour_flag = false;

    if(option_map.count(INCLUDE_SWITCH_LONG) > 0)
      {
        std::vector<std::string> include_paths = option_map[INCLUDE_SWITCH_LONG].as< std::vector<std::string> >();
        std::copy(include_paths.cbegin(), include_paths.cend(), std::back_inserter(this->search_path_list));
      }

    if(option_map.count(INPUT_FILE_SWITCH) > 0)
      {
        std::vector<std::string> input_files = option_map[INPUT_FILE_SWITCH].as< std::vector<std::string> >();
        std::copy(input_files.cbegin(), input_files.cend(), std::back_inserter(this->input_file_list));
      }
  }
