//
//  main.cpp
//  CppTransport
//
//  Created by David Seery on 12/06/2013.
//  Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include <iostream>
#include <deque>

#include "boost/program_options.hpp"
#include "boost/timer/timer.hpp"

#include "core.h"
#include "translation_unit.h"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    boost::timer::auto_cpu_timer timer;

    set_up_error_environment();

    // set up the initial search path to consist only of CWD
    std::unique_ptr<finder> path = std::make_unique<finder>();

    // set up Boost::program_options descriptors for command-line arguments
    boost::program_options::options_description generic(MISC_OPTIONS);
    generic.add_options()
      (VERSION_SWITCH, VERSION_HELP)
      (HELP_SWITCH,    HELP_HELP);

    boost::program_options::options_description configuration(CONFIG_OPTIONS);
    configuration.add_options()
      (VERBOSE_SWITCH,                                                                                         VERBOSE_HELP)
      (INCLUDE_SWITCH,               boost::program_options::value< std::vector<std::string> >()->composing(), INCLUDE_HELP)
      (CORE_OUTPUT_SWITCH,           boost::program_options::value< std::string >()->default_value(""),        CORE_OUTPUT_HELP)
      (IMPLEMENTATION_OUTPUT_SWITCH, boost::program_options::value< std::string >()->default_value(""),        IMPLEMENTATION_OUTPUT_HELP)
      (NO_CSE_SWITCH,                                                                                          NO_CSE_HELP)
      (NO_COLOUR_SWITCH,                                                                                       NO_COLOUR_HELP);

    boost::program_options::options_description hidden(HIDDEN_OPTIONS);
    hidden.add_options()
      (INPUT_FILE_SWITCH, boost::program_options::value< std::vector<std::string> >(), INPUT_FILE_HELP)
      (NO_COLOR_SWITCH,                                                                NO_COLOR_HELP);

    boost::program_options::positional_options_description positional_options;
    positional_options.add(INPUT_FILE_SWITCH, -1);

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(configuration).add(hidden);

    boost::program_options::options_description config_file_options;
    config_file_options.add(configuration);

    boost::program_options::options_description visible;
    visible.add(generic).add(configuration);

    boost::program_options::variables_map option_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).positional(positional_options).run(), option_map);
    boost::program_options::notify(option_map);

    bool emitted_version = false;

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

    if(option_map.count(NO_COLOUR_SWITCH) || option_map.count(NO_COLOR_SWITCH)) disable_colour_errors();

    if(option_map.count(INCLUDE_SWITCH_LONG) > 0)
      {
        std::vector<std::string> include_paths = option_map[INCLUDE_SWITCH_LONG].as< std::vector<std::string> >();
        for(std::vector<std::string>::const_iterator t = include_paths.begin(); t != include_paths.end(); ++t)
          {
            path->add(*t);
          }
      }

    unsigned int files_processed = 0;
    unsigned int replacements    = 0;

    if(option_map.count(INPUT_FILE_SWITCH) > 0)
      {
        std::vector<std::string> input_files = option_map[INPUT_FILE_SWITCH].as< std::vector<std::string> >();
        for(std::vector<std::string>::const_iterator t = input_files.begin(); t != input_files.end(); ++t)
          {
            translation_unit unit(*t, *path,
                                  option_map[CORE_OUTPUT_SWITCH].as<std::string>(),
                                  option_map[IMPLEMENTATION_OUTPUT_SWITCH].as<std::string>(),
                                  option_map.count(NO_CSE_SWITCH)==0,
                                  option_map.count(VERBOSE_SWITCH_LONG)>0);

            replacements += unit.apply();
            files_processed++;
          }
      }

    timer.stop();

		if(option_map.count(VERBOSE_SWITCH_LONG))
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << MESSAGE_PROCESSING_COMPLETE_A
			    << " " << files_processed << " "
			    << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
			    << " " << MESSAGE_PROCESSING_COMPLETE_B << " " << format_time(timer.elapsed().wall) << '\n';

//		    std::cout << CPPTRANSPORT_NAME << ": " << replacements << " " << MESSAGE_MACRO_REPLACEMENTS << '\n';
			}

    return 0;
  }
