//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <string>
#include <sstream>


#include "output_stack.h"
#include "msg_en.h"
#include "macro.h"
#include "package_group.h"
#include "error.h"


void output_stack::push(const std::string& in, buffer& buf, macro_agent& agent, enum process_type type)
  {
		// NOTE ms and package elements should be updated later with push_top_data()
    this->inclusions.push_front(inclusion(in, 1, buf, agent, type));
  }


void output_stack::set_line(unsigned int line)
  {
    if(inclusions.size() == 0)
      {
        error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        this->inclusions[0].line = line;
      }
  }


unsigned int output_stack::increment_line()
  {
    unsigned int rval = 0;

    if(inclusions.size() == 0)
      {
        error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        rval = ++this->inclusions[0].line;
      }
    return(rval);
  }


unsigned int output_stack::get_line() const
  {
    unsigned int rval = 0;

    if(inclusions.size() == 0)
      {
        error(ERROR_FILESTACK_EMPTY);
      }
    else
      {
        rval = this->inclusions[0].line;
      }
    return(rval);
  }


void output_stack::pop()
  {
    if(inclusions.size() > 0)
      {
        this->inclusions.pop_front();
      }
    else
      {
        error(ERROR_FILESTACK_TOO_SHORT);
      }
  }


std::string output_stack::write() const
  {
    return this->write(this->inclusions.size());
  }


std::string output_stack::write(size_t level) const
  {
    std::ostringstream out;

    if(this->inclusions.size() < level)
      {
        level = (unsigned int)this->inclusions.size();
      }

    if(level >= 1)
      {
        out << this->inclusions[0].line << " " << OUTPUT_STACK_OF << " '" << this->inclusions[0].in << "'";
      }

    for(int i = 1; i < level; ++i)
      {
        out << std::endl
            << OUTPUT_STACK_WRAP_PAD << OUTPUT_STACK_INCLUDED_FROM << " " << this->inclusions[i].line
            << " " << OUTPUT_STACK_OF_FILE << " '" << this->inclusions[i].in << "'";
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
