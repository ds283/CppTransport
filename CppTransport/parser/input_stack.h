//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __input_stack_H_
#define __input_stack_H_


#include <string>
#include <deque>

#include "filestack.h"


class input_stack: public filestack_derivation_helper<input_stack>
  {
  public:
    // data structure for tracking the source of any lexeme
    struct inclusion
      {
        std::string  name;
        unsigned int line;
      };

    ~input_stack();

    void         push          (const std::string name);

    void         set_line      (unsigned int line);
    unsigned int increment_line();
    unsigned int get_line      () const;

    void         pop           ();

    std::string  write         (size_t level) const;
    std::string  write         () const;

  protected:
    std::deque<struct inclusion> inclusions;
  };


#endif //__input_stack_H_
