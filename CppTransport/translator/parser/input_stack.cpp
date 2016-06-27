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
    this->inclusions.emplace_front(name, 1);
  }


void input_stack::set_line(unsigned int line)
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    this->inclusions.front().line = line;
  }


unsigned int input_stack::increment_line()
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    return ++this->inclusions.front().line;
  }


unsigned int input_stack::get_line() const
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_EMPTY);

    return this->inclusions.front().line;
  }


void input_stack::pop()
  {
    if(inclusions.size() == 0) throw std::runtime_error(ERROR_FILESTACK_TOO_SHORT);

    this->inclusions.pop_front();
  }


std::string input_stack::write() const
  {
    return this->write(this->inclusions.size());
  }


std::string input_stack::write(size_t level) const
  {
    std::ostringstream out;

    if(this->inclusions.size() < level) level = static_cast<unsigned int>(this->inclusions.size());

    if(level >= 1)
      {
        const inclusion& item = this->inclusions.front();
        out << item.line << " " << OUTPUT_STACK_OF << " " << item.name;
      }

    unsigned int level_counter = 1;
    for(inclusion_stack::const_iterator t = ++this->inclusions.begin(); t != this->inclusions.end() && level_counter < level; ++t)
      {
        const inclusion& item = *t;
        out << '\n'
            << OUTPUT_STACK_WRAP_PAD << OUTPUT_STACK_INCLUDED_FROM << " " << item.line
            << " " << OUTPUT_STACK_OF_FILE << " " << item.name;

        ++level_counter;
      }

    return(out.str());
  }
