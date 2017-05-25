//
// Created by David Seery on 14/06/2013.
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
#include <fstream>
#include <assert.h>

#include "lexfile.h"
#include "msg_en.h"


// ******************************************************************


lexfile::lexfile(const boost::filesystem::path& fnam, filestack& s)
  : file(fnam),
    stack(s),
    state(internal_state::unready),
    char_pos(0)
  {
    stream.open(fnam.string());    // when building with GCC LLVM 4.2, stream.open() doesn't accept std::string

    // read in lines from the file and store them in the line array
    std::string line;
    while(std::getline(stream, line))
      {
        line_array.push_back(std::make_shared<std::string>(line));
      }

    // reset to the beginning of the stream
    stream.clear();
    stream.seekg(0, std::ios::beg);

    assert(stream.is_open());
  }

lexfile::~lexfile()
  {
    if(this->stream.is_open())
      {
        this->stream.close();
      }
  }


// ******************************************************************


void lexfile::make_ready()
  {
    // detect EOF condition
    if(this->stream.eof())
      {
        this->state = internal_state::eof;
        this->c = 0;
        return;
      }

    while(this->state == internal_state::unready)
      {
        assert(this->stream.is_open());
        assert(!(this->stream.fail()));

        this->stream.get(this->c);

        if(this->stream.eof())                // reached end-of-file
          {
            this->state = internal_state::eof;
            this->c = 0;
          }
        else if(this->stream.fail())          // there was an error
          {
            this->state = internal_state::error;
            this->c = 0;
          }
        else                                  // can assume this character is valid
          {
            this->state = internal_state::ready;
          }
      }
  }


lexfile::value_type lexfile::get()
  {
    switch(this->state)
      {
        case internal_state::unready:                  // this will be the most common case; we need to read a new char
          {
            this->make_ready();
            break;
          }

        case internal_state::ready:
          {
            break;                                    // don't need to do anything if we are already 'ready'
          }

        case internal_state::eof:
        case internal_state::error:
          {
            break;                                    // do nothing if there is already an error or eof condition
          }
      }

    if(this->state == internal_state::eof)
      {
        return std::make_tuple(c, status::eof);
      }
    else if(this->state == internal_state::error)
      {
        return std::make_tuple(c, status::error);
      }

    return std::make_tuple(c, status::ok);
  }


lexfile& lexfile::operator++()
  {
    assert(this->stream.is_open());
    
    if(this->state == internal_state::ready && this->c == '\n')
      {
        this->stack.increment_line();
        this->char_pos = 0;
      }
    else
      {
        ++this->char_pos;
      }

    this->state = internal_state::unready;

    return *this;
  }


lexfile::status lexfile::get_state() const
  {
    if(this->state == internal_state::eof)
      {
        return status::eof;
      }
    else if(this->state == internal_state::error)
      {
        return status::error;
      }

    return status::ok;
  }


const std::shared_ptr<std::string>& lexfile::get_current_line() const
  {
    unsigned int cline = this->stack.get_line();

    if(cline <= this->line_array.size())
      {
        return this->line_array[cline-1];
      }

    throw std::runtime_error(ERROR_CURRENT_LINE_EMPTY);
  }
