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
#include "formatter.h"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    boost::timer::auto_cpu_timer timer;

    // set up the initial search path to consist only of CWD
    std::shared_ptr<finder> path = std::make_shared<finder>();

    // set up Boost::program_options descriptors for command-line arguments
    boost::program_options::options_description generic("Generic options");
    generic.add_options()
      ("version", "output version information")
      ("help",    "obtain brief descrption of command line options");

    boost::program_options::options_description configuration("Configuration options");
    configuration.add_options()
      ("verbose,v",                                                                                       "enable verbose output")
      ("include,I",             boost::program_options::value< std::vector<std::string> >()->composing(), "add specified path to search list")
      ("core-output",           boost::program_options::value< std::string >()->default_value(""),        "specify name of core header")
      ("implementation-output", boost::program_options::value< std::string >()->default_value(""),        "specify name of implementation header")
      ("no-cse",                                                                                          "disable common sub-expression elimination");

    boost::program_options::options_description hidden("Hidden options");
    hidden.add_options()
      ("input-file", boost::program_options::value< std::vector<std::string> >(), "input file");

    boost::program_options::positional_options_description positional_options;
    positional_options.add("input-file", -1);

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

    if(option_map.count("version"))
      {
        std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << std::endl;
        emitted_version = true;
      }

    if(option_map.count("help"))
      {
        if(!emitted_version) std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << std::endl;
        std::cout << visible << std::endl;
      }

    if(option_map.count("include") > 0)
      {
        std::vector<std::string> include_paths = option_map["include"].as< std::vector<std::string> >();
        for(std::vector<std::string>::const_iterator t = include_paths.begin(); t != include_paths.end(); ++t)
          {
            path->add(*t);
          }
      }

    unsigned int files_processed = 0;
    unsigned int replacements    = 0;

    if(option_map.count("input-file") > 0)
      {
        std::vector<std::string> input_files = option_map["input-file"].as< std::vector<std::string> >();
        for(std::vector<std::string>::const_iterator t = input_files.begin(); t != input_files.end(); ++t)
          {
            translation_unit unit(*t, path,
                                  option_map["core-output"].as<std::string>(),
                                  option_map["implementation-output"].as<std::string>(),
                                  option_map.count("no-cse")==0,
                                  option_map.count("verbose")>0);

            replacements += unit.apply();
            files_processed++;
          }
      }

    timer.stop();

		if(option_map.count("verbose"))
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << MESSAGE_PROCESSING_COMPLETE_A
			    << " " << files_processed << " "
			    << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
			    << " " << MESSAGE_PROCESSING_COMPLETE_B << " " << format_time(timer.elapsed().wall) << std::endl;

//		    std::cout << CPPTRANSPORT_NAME << ": " << replacements << " " << MESSAGE_MACRO_REPLACEMENTS << std::endl;
			}

    return 0;
  }
