//
//  main.cpp
//  CppTransport
//
//  Created by David Seery on 12/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#include <iostream>
#include <deque>

#include <boost/timer/timer.hpp>

#include "core.h"
#include "translation_unit.h"


// ******************************************************************


int main(int argc, const char *argv[])
  {
    std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << std::endl;

    boost::timer::auto_cpu_timer timer;

    // set up the initial search path to consist only of CWD
    finder path;

    std::string                         current_core = "";
    std::string                         current_implementation = "";
    bool                                cse = true;

    unsigned int                        files_processed = 0;
    unsigned int                        replacements = 0;

    for(int i = 1; i < argc; i++)
      {
        if(strcmp(argv[i], "-I") == 0)
          {
            if(i + 1 < argc) path.add(std::string(argv[++i]));
            else
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_PATHNAME << " -I";
                error(msg.str());
              }
          }
        else if(strcmp(argv[i], "--core-output") == 0)
          {
            if(i + 1 < argc) current_core = (std::string)argv[++i];
            else
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_PATHNAME << " --core-output";
                error(msg.str());
              }
          }
        else if(strcmp(argv[i], "--implementation-output") == 0)
          {
            if(i + 1 < argc) current_implementation = (std::string)argv[++i];
            else
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_PATHNAME << " --implementation-output";
                error(msg.str());
              }
          }
        else if(strcmp(argv[i], "--cse") == 0)    cse = true;
        else if(strcmp(argv[i], "--no-cse") == 0) cse = false;
        else if((argv[i])[0] == '-')  // assume to be a switch we don't know about
          {
            std::ostringstream msg;
            msg << WARNING_UNKNOWN_SWITCH << " " << argv[i];
            warn(msg.str());
          }
        else // assume to be an input file we are processing
          {
            translation_unit unit((std::string)argv[i], &path, current_core, current_implementation, cse);

            replacements += unit.do_replacement();
            files_processed++;
          }
      }

    timer.stop();
    std::cout << MESSAGE_PROCESSING_COMPLETE_A
              << " " << files_processed << " "
              << (files_processed != 1 ? MESSAGE_PROCESSING_PLURAL : MESSAGE_PROCESSING_SINGULAR)
              << " " << MESSAGE_PROCESSING_COMPLETE_B;
    timer.report();
    std::cout << replacements << " macro replacements" << std::endl;

    return 0;
  }
