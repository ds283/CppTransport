//
// Created by David Seery on 10/12/2013.
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

#include "output_stack.h"
#include "msg_en.h"
#include "macro.h"
#include "package_group.h"


void output_stack::push(boost::filesystem::path in, buffer& buf, macro_agent& agent, enum process_type type)
  {
    // starts at line 2, because first line of template is a header that defines output language, minimum version, etc.
    this->inclusions.emplace_front(in, 2, buf, agent, type);
  }


void output_stack::set_line(unsigned int line)
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    this->inclusions.front().line = line;
  }


unsigned int output_stack::increment_line()
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    return ++this->inclusions.front().line;
  }


unsigned int output_stack::get_line() const
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    return this->inclusions.front().line;
  }


void output_stack::pop()
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_TOO_SHORT);

    this->inclusions.pop_front();
  }


std::string output_stack::write() const
  {
    return this->write(this->inclusions.size());
  }


std::string output_stack::write(size_t level) const
  {
    std::ostringstream out;

    if(this->inclusions.size() < level) level = static_cast<unsigned int>(this->inclusions.size());

    if(level >= 1)
      {
        const inclusion& item = this->inclusions.front();
        out << item.line << " " << OUTPUT_STACK_OF << " " << item.in;
      }

    unsigned int level_counter = 1;
    for(inclusion_stack::const_iterator t = ++this->inclusions.begin(); t != this->inclusions.end() && level_counter < level; ++t)
      {
        const inclusion& item = *t;
        out << '\n'
            << OUTPUT_STACK_WRAP_PAD << OUTPUT_STACK_INCLUDED_FROM << " " << item.line
            << " " << OUTPUT_STACK_OF_FILE << " " << item.in;

        ++level_counter;
      }

    return(out.str());
  }


buffer& output_stack::top_buffer()
  {
    if(inclusions.size() > 0)
      {
        return this->inclusions[0].buf;
      }

    throw std::runtime_error(ERROR_FILESTACK_EMPTY);
  }


macro_agent& output_stack::top_macro_package()
  {
    if(inclusions.size() > 0)
      {
        return this->inclusions[0].agent;
      }

    throw std::runtime_error(ERROR_FILESTACK_EMPTY);
  }


enum process_type output_stack::top_process_type() const
  {
    if(inclusions.size() > 0)
      {
        return this->inclusions[0].type;
      }

    throw std::runtime_error(ERROR_FILESTACK_EMPTY);
  }
