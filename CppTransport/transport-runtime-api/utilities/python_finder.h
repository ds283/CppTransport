//
// Created by David Seery on 24/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __python_finder_H_
#define __python_finder_H_

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "transport-runtime-api/defaults.h"

#include <boost/algorithm/string.hpp>


namespace transport
	{

		std::string find_python()
			{
		    std::string path;
		    FILE* f = popen("which python", "r");

		    if(!f)
			    {
		        path = CPPTRANSPORT_DEFAULT_PYTHON_PATH;
			    }
		    else
			    {
		        char buffer[1024];
		        char* line = fgets(buffer, sizeof(buffer), f);
		        pclose(f);

            if(line != nullptr)
              {
                path = std::string(line);
                boost::algorithm::trim_right(path);
              }
            else
              {
                path = CPPTRANSPORT_DEFAULT_PYTHON_PATH;
              }
			    }

				return(path);
			}

	}


#endif //__python_finder_H_
