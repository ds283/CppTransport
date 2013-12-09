//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <sstream>

#include "filestack.h"

#include "msg_en.h"
#include "basic_error.h"


filestack::filestack()
  : locked(false)
  {
  }


void filestack::push(const std::string name)
  {
    if(!locked)
      {
        struct inclusion incl;

        incl.line = 1;    // line number doesn't matter; // it will get overwritten later
        incl.name = name;

        this->inclusions.push_front(incl);
      }
    else
      {
        basic_error(ERROR_FILESTACK_LOCKED);
      }
  }


void filestack::set_line(unsigned int line)
  {
    if(!locked)
      {
        if(inclusions.size() == 0)
          {
            basic_error(ERROR_FILESTACK_EMPTY);
          }
        else
          {
            this->inclusions[0].line = line;
          }
      }
    else
      {
        basic_error(ERROR_FILESTACK_LOCKED);
      }
  }


void filestack::pop()
  {
    if(!locked)
      {
        if(inclusions.size() > 0)
          {
            this->inclusions.pop_front();
          }
        else
          {
            basic_error(ERROR_FILESTACK_TOO_SHORT);
          }
      }
    else
      {
        basic_error(ERROR_FILESTACK_LOCKED);
      }
  }


std::string filestack::write(unsigned int level)
  {
    std::ostringstream out;

    if(this->inclusions.size() < level)
      {
        level = (unsigned int)this->inclusions.size();
      }

    if(level >= 1)
      {
        out << " " << ERROR_MESSAGE_OF << " '" << this->inclusions[0].name << "'";
      }
    out << std::endl;

    for(int i = 1; i < level; i++)
      {
        out << ERROR_MESSAGE_WRAP_PAD << ERROR_MESSAGE_INCLUDED_FROM << " " << this->inclusions[i].line
      << " " << ERROR_MESSAGE_OF_FILE << " '" << this->inclusions[i].name << "'" << std::endl;
      }

    return(out.msg());
  }


void filestack::lock()
  {
    this->locked = true;
  }