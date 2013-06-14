//
//  main.cpp
//  CppTransport
//
//  Created by David Seery on 12/06/2013.
//  Copyright (c) 2013 University of Sussex. All rights reserved.
//

#include <iostream>
#include <deque>

#include "core.h"
#include "finder.h"
#include "lexstream.h"

struct input
  {
    lexstream*  stream;
    std::string name;
  };

int main(int argc, const char *argv[])
  {
    std::cout << CPPTRANSPORT_NAME << " " << CPPTRANSPORT_VERSION << " " << CPPTRANSPORT_COPYRIGHT << "\n";

    // set up the initial search path to consist only of CWD
    finder path("~");

    std::deque<struct input> inputs;

    int i;
    for(i = 1; i < argc; i++)
      {
        if(strcmp(argv[i], "-I") == 0)
          {
            path.add(std::string(argv[++i]));
          }
        else  // assume to be an input file we are processing
          {
            struct input in;

            in.name   = (std::string)argv[i];
            in.stream = new lexstream((std::string)argv[i], &path);

            inputs.push_back(in);
          }
      }

    return 0;
  }
