//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __filestack_H_
#define __filestack_H_


#include <string>


// this is an abstract class whose interface can be implemented by
// subsequent, more specialized derived classes

class filestack
  {
  public:
    filestack();
    virtual ~filestack();

    // push a new filename to the top of the stack, included from line current_line
    // of the file which is *currently* on top of the stack
    virtual void push(const std::string name) = 0;

    // set line number information for the item on top of the stack
    virtual void set_line(unsigned int line) = 0;

    // pop a filename from the top of the stack
    virtual void pop() = 0;

    // generate a list of included files, suitable for producing an error report
    // the list is truncated at depth 'level' to avoid excess verbosity
    virtual std::string write(unsigned int level) = 0;
    virtual std::string write()                   = 0;

    // lock the filestack, preventing future changes
    // there is no unlock API, so this puts the filestack into a state suitable
    // for archiving, eg. as part of the lexeme list
    void lock();

  protected:
    bool                         locked;
  };


#endif //__filestack_H_
