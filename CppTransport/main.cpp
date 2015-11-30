//
//  main.cpp
//  CppTransport
//
//  Created by David Seery on 12/06/2013.
//  Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include <iostream>

#include "core.h"
#include "local_environment.h"
#include "argument_cache.h"
#include "translation_unit.h"

#include "boost/timer/timer.hpp"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    boost::timer::auto_cpu_timer timer;

    local_environment env;
    argument_cache args(argc, argv);

    set_up_error_environment();

    // set up the initial search path;
    // this should consist of the current working directory, but also
    // any include paths set using environment variables
    finder path;
    path.add(env.search_paths());
    path.add(args.search_paths());

    unsigned int files_processed = 0;
    unsigned int replacements    = 0;

    const std::list<std::string> input_files = args.input_files();
    for(std::list<std::string>::const_iterator t = input_files.begin(); t != input_files.end(); ++t)
      {
        translation_unit unit(*t, path, args, env);
        replacements += unit.apply();
        ++files_processed;
      }

    timer.stop();

		if(args.verbose())
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << MESSAGE_PROCESSING_COMPLETE_A
			    << " " << files_processed << " "
			    << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
			    << " " << MESSAGE_PROCESSING_COMPLETE_B << " " << format_time(timer.elapsed().wall) << '\n';
			}

    return(EXIT_SUCCESS);
  }
