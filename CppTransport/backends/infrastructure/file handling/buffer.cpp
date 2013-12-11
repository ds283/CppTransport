//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>

#include "buffer.h"
#include "error.h"

buffer::buffer()
  {
    tag = --buf.end();
  }


void buffer::write_to_end(std::string line)
  {
    bool write = true;
    if(this->skips.size() > 0)
      {
        if(this->skips[0] && line == "") write = false;
      }

    if(write)
      {
        this->delimit_line(line);
        this->buf.push_back(line);
//    std::cerr << ":: " << line << std::endl;
      }
  }


void buffer::write_to_tag(std::string line)
  {
    bool write = true;
    if(this->skips.size() > 0)
      {
        if(this->skips[0] && line == "") write = false;
      }

    if(write)
      {
        this->delimit_line(line);
        this->buf.insert(this->tag, line);

//    std::cerr << ">> " << line << std::endl;
      }
  }


void buffer::delimit_line(std::string& line)
  {
    for(std::deque<struct delimiter>::iterator t = this->delimiters.begin(); t != this->delimiters.end(); t++)
      {
        line.insert(0, (*t).left);
        line.append((*t).right);
      }
  }


void buffer::set_tag_to_end()
  {
    this->tag = --this->buf.end();
  }


void buffer::register_closure_handler(buffer_flush_handler handler, void* tag)
  {
    this->flush_handlers.push_back(std::pair<buffer_flush_handler,void*>(handler, tag));
  }


void buffer::deregister_closure_handler(buffer_flush_handler handler, void* tag)
  {
    this->flush_handlers.remove_if([&](std::pair<buffer_flush_handler,void*> item) -> bool { return item.second == tag; } );
  }


void buffer::emit(std::string file)
  {
    // loop through closure handlers, asking them to flush anything waiting to be written to the output
    this->flush();

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


void buffer::flush()
  {
    for(std::list< std::pair<buffer_flush_handler,void*> >::iterator t = this->flush_handlers.begin(); t != this->flush_handlers.end(); t++)
      {
        ((*t).first)();
      }
  }


void buffer::push_delimiter(std::string left, std::string right)
  {
    struct delimiter d;

    d.left = left;
    d.right = right;

    this->delimiters.push_back(d);
  }


void buffer::pop_delimiter()
  {
    if(this->delimiters.size() > 0)
      {
        this->delimiters.pop_back();
      }
  }


void buffer::push_skip_blank(bool skip)
  {
    this->skips.push_front(skip);
  }


void buffer::pop_skip_blank()
  {
    if(this->skips.size() > 0)
      {
        this->skips.pop_front();
      }
  }
