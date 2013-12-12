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

#include "core.h"
#include "buffer.h"
#include "error.h"

#include "boost/algorithm/string.hpp"


#define BUFFER_MAGIC_TAG "MAGIC_TAG"


buffer::buffer()
  {
    tag = --buf.end();
    buf.insert(buf.end(), BUFFER_MAGIC_TAG);
    ++tag;
  }


void buffer::write(std::string& line, std::list<std::string>::iterator insertion_point)
  {
    // break up the string to be written into individual lines
    std::vector<std::string> lines;
    boost::split(lines, line, boost::is_any_of(NEWLINE_CHAR));

    for(std::vector<std::string>::const_iterator t = lines.begin(); t != lines.end(); t++)
      {
        bool write = true;
        if(this->skips.size() > 0)
          {
            if(this->skips[0] && *t == "") write = false;
          }

        if(write)
          {
            std::string item = *t;
            this->delimit_line(item);
            this->buf.insert(insertion_point, item);
//    std::cerr << ":: " << line << std::endl;
          }
      }
  }


void buffer::write_to_end(std::string line)
  {
    this->write(line, this->buf.end());
  }


void buffer::write_to_tag(std::string line)
  {
    this->write(line, this->tag);
  }


void buffer::delimit_line(std::string& item)
  {
    for(std::deque<struct delimiter>::iterator t = this->delimiters.begin(); t != this->delimiters.end(); t++)
      {
        item.insert(0, (*t).left);
        item.append((*t).right);
      }
  }


void buffer::set_tag_to_end()
  {
    // remove blank placeholder line if it is safe to do so (should always be true)
    if(*(this->tag) == BUFFER_MAGIC_TAG) this->buf.erase(this->tag);

    // magic to keep track of tagged line position
    // (it's hard to make sure tagged content appears at the right place, because
    // tags will usually be set *before* output of the line they correpond to--that is,
    // macro replacement functions which set the tag location have to return a string,
    // and the string is sent to the buffer *afterwards*)
    // this magic uses a special tagged line to keep track of where
    // the insertion should really happen
    this->tag = --this->buf.end();
    this->buf.insert(this->buf.end(), BUFFER_MAGIC_TAG);
    ++this->tag;
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

    if(*(this->tag) == BUFFER_MAGIC_TAG) this->buf.erase(this->tag);

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

    this->tag = --this->buf.end();
    this->buf.insert(this->buf.end(), BUFFER_MAGIC_TAG);
    ++this->tag;
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
