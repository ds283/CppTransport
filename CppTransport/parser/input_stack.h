//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __input_stack_H_
#define __input_stack_H_


#include <string>
#include <deque>

#include "filestack.h"


class input_stack: public filestack
  {
  public:
    // data structure for tracking the source of any lexeme
    struct inclusion
      {
        std::string  name;
        unsigned int line;
      };

    input_stack();
    ~input_stack();

    void        push    (const std::string name);

    void        set_line(unsigned int line);

    void        pop     ();

    std::string write   (unsigned int level);
    std::string write   ();

  protected:
    std::deque<struct inclusion> inclusions;
  };


#endif //__input_stack_H_
