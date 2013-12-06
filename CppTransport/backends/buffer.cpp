//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>

#include "buffer.h"
#include "error.h"

buffer::buffer()
  {
    tag = --buf.end();
  }


void buffer::write_to_end(std::string line)
  {
    this->buf.push_back(line);

//    std::cerr << ":: " << line << std::endl;
  }


void buffer::write_to_tag(std::string line)
  {
    this->buf.insert(this->tag, line);

//    std::cerr << ">> " << line << std::endl;
  }


void buffer::set_tag_to_end()
  {
    this->tag = --this->buf.end();
  }


void buffer::register_closure_handler(buffer_closure_handler handler, void* tag)
  {
    this->closure_handlers.push_back(std::pair<buffer_closure_handler,void*>(handler, tag));
  }


void buffer::deregister_closure_handler(buffer_closure_handler handler, void* tag)
  {
    this->closure_handlers.remove_if([&](std::pair<buffer_closure_handler,void*> item) -> bool { return item.second == tag; } );
  }


void buffer::emit(std::string file)
  {
    // loop through closure handlers, informing them that we intend to finish writing to the buffer
    for(std::list< std::pair<buffer_closure_handler,void*> >::iterator t = this->closure_handlers.begin(); t != this->closure_handlers.end(); t++)
      {
        ((*t).first)();
      }

    std::ofstream out;
    out.open(file);
    if(out.is_open() && !out.fail())
      {
        for(std::list<std::string>::iterator t = this->buf.begin(); t != this->buf.end() && !out.fail(); t++)
          {
            out << *t << std::endl;
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_CPP_BUFFER_WRITE << " '" << file << "'";
        error(msg.str());
      }
  }
