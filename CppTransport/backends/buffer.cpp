//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <sstream>

#include "error.h"
#include "buffer.h"


void buffer::emit(std::string file)
  {
    for(std::list<buffer_closure_handler>::iterator t = this->closure_handlers.begin(); t != this->closure_handlers.end(); t++)
      {
        (*t)();
      }

    std::ofstream out;
    out.open(file.c_str());

    if(out.fail())
      {
        std::ostringstream msg;
        msg << ERROR_BACKEND_OUTPUT << " '" << file << "'";
        error(msg.str());
      }
    else
      {
        for(std::list<std::string>::iterator t = this->buf.begin(); t != this->buf.end() && out.fail() == false; t++)
          {
            out << *t << std::endl;
          }

        if(out.fail())
          {
            std::ostringstream msg;
            msg << ERROR_CPP_BACKEND_WRITE << "'" << file << "'";
            error(msg.str());
          }
      }
  }
