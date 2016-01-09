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

#include "ginac_print_indexed.h"

#include "boost/timer/timer.hpp"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    boost::timer::auto_cpu_timer timer;

    // we use Kroncecker deltas in some symbolic expressions, and GiNaC's C-source output
    // has to be adjusted to print these correctly
    set_up_ginac_printing();

    // construct local environment and argument cache
    local_environment env;
    argument_cache args(argc, argv, env);

    // set up the initial search path;
    // this should consist of the current working directory, but also
    // any include paths set using environment variables
    finder path;
    if(args.search_environment()) path.add(env.search_paths());
    path.add(args.search_paths());

    unsigned int files_processed = 0;
    unsigned int replacements    = 0;

    bool errors = false;

    const std::list<boost::filesystem::path> input_files = args.input_files();
    for(const boost::filesystem::path& f : input_files)
      {
        translation_unit unit(f, path, args, env);
        replacements += unit.apply();
        ++files_processed;

        errors = errors | unit.fail();
      }

    timer.stop();

		if(args.verbose())
			{
		    std::cout << CPPTRANSPORT_NAME << ": " << MESSAGE_PROCESSING_COMPLETE_A
			    << " " << files_processed << " "
			    << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
			    << " " << MESSAGE_PROCESSING_COMPLETE_B << " " << format_time(timer.elapsed().wall) << '\n';
			}

    if(errors) return (EXIT_FAILURE);
    else return(EXIT_SUCCESS);
  }
