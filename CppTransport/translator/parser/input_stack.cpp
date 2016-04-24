//
// Created by David Seery on 09/12/2013.
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


#include <string>
#include <sstream>
#include <stdexcept>

#include "msg_en.h"
#include "input_stack.h"


void input_stack::push(const boost::filesystem::path name)
  {
    struct inclusion incl;

    incl.line = 1;
    incl.name = name;

    this->inclusions.push_front(incl);
  }


void input_stack::set_line(unsigned int line)
  {
    if(inclusions.size() == 0)
      {
        throw std::runtime_error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        this->inclusions[0].line = line;
      }
  }


unsigned int input_stack::increment_line()
  {
    unsigned int rval = 0;

    if(inclusions.size() == 0)
      {
        throw std::runtime_error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        rval = ++this->inclusions[0].line;
      }
    return(rval);
  }


unsigned int input_stack::get_line() const
  {
    unsigned int rval = 0;

    if(inclusions.size() == 0)
      {
        throw std::runtime_error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        rval = this->inclusions[0].line;
      }
    return(rval);
  }


void input_stack::pop()
  {
    if(inclusions.size() > 0)
      {
        this->inclusions.pop_front();
      }
    else
      {
        throw std::runtime_error(ERROR_FILESTACK_TOO_SHORT);
      }
  }


std::string input_stack::write() const
  {
    return this->write(this->inclusions.size());
  }


std::string input_stack::write(size_t level) const
  {
    std::ostringstream out;

    if(this->inclusions.size() < level)
      {
        level = (unsigned int)this->inclusions.size();
      }

    if(level >= 1)
      {
        out << this->inclusions[0].line << " " << OUTPUT_STACK_OF << " " << this->inclusions[0].name;
      }

    for(int i = 1; i < level; ++i)
      {
        out << '\n'
            << OUTPUT_STACK_WRAP_PAD << OUTPUT_STACK_INCLUDED_FROM << " " << this->inclusions[i].line
            << " " << OUTPUT_STACK_OF_FILE << " " << this->inclusions[i].name;
      }

    return(out.str());
  }
