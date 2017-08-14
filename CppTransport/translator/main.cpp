//
// Created by David Seery on 12/06/2013.
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

#include "core.h"
#include "local_environment.h"
#include "argument_cache.h"
#include "translation_unit.h"
#include "version_policy.h"
#include "error.h"

#include "ginac_print_indexed.h"

#include "boost/timer/timer.hpp"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    boost::timer::auto_cpu_timer timer;

    // we use Kronecker deltas in some symbolic expressions, and GiNaC's C-source output
    // has to be adjusted to print these correctly
    set_up_ginac_printing();

    // construct local environment and argument cache
    local_environment env;
    argument_cache args(argc, argv, env);

		// emit any messages generated during argument parsing
		auto& messages = args.get_messages();
		for(const auto& m : messages)
			{
        // first component of pair is a flag that indicates whether this is an error or a warning
				if(m.first) error(m.second, args, env);
				if(!m.first) warn(m.second, args, env);
			}

    // set up the initial search path;
    // this should consist of the current working directory, but also
    // any include paths set using environment variables
    finder path;
    path.add_cwd().add(args.search_paths());
    if(args.search_environment()) path.add_environment_variable(CPPTRANSPORT_PATH_ENV, CPPTRANSPORT_TEMPLATE_PATH);
    
    // set up version policy registry
    version_policy policy;

    // process specified files
    unsigned int files_processed = 0;
    unsigned int replacements    = 0;

    bool errors = false;

    const auto& input_files = args.input_files();
    for(const auto& f : input_files)
      {
        translation_unit unit(f, path, args, env, policy);
        replacements += unit.apply();
        ++files_processed;

        errors = errors || unit.fail();
      }

    timer.stop();

    // issue summary statistics
		if(args.verbose())
			{
        std::cout << CPPTRANSPORT_NAME << ": " << MESSAGE_PROCESSING_COMPLETE_A
                  << " " << files_processed << " "
                  << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
                  << " " << MESSAGE_PROCESSING_COMPLETE_B << " " << format_time(timer.elapsed().wall) << '\n';
			}

    if(errors) return (EXIT_FAILURE);
    return(EXIT_SUCCESS);
  }
