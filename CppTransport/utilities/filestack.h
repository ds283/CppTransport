//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __filestack_H_
#define __filestack_H_


#include <string>
#include <deque>


class filestack
  {
  public:
    // data structure for tracking the source of any lexeme
    struct inclusion
      {
        std::string  name;
        unsigned int line;
      };

    filestack();

    // push a new filename to the top of the stack, included from line current_line
    // of the file which is *currently* on top of the stack
    void push(const std::string name);

    // set line number information for the item on top of the stack
    void set_line(unsigned int line);

    // pop a filename from the top of the stack
    void pop();

    // generate a list of included files, suitable for producing an error report
    // the list is truncated at depth 'level' to avoid excess verbosity
    std::string write(unsigned int level);

    // lock the filestack, preventing future changes
    // there is no unlock API, so this puts to filestack into a state suitable
    // for archiving, eg. as part of the lexeme list
    void lock();

  protected:
    std::deque<struct inclusion> inclusions;
    bool                         locked;
  };


#endif //__filestack_H_
