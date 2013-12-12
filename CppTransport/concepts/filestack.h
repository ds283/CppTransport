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
    virtual ~filestack();

    // push a new filename to the top of the stack, included from line current_line
    // of the file which is *currently* on top of the stack
    virtual void push(const std::string name) = 0;

    // set line number information for the item on top of the stack
    virtual void set_line(unsigned int line) = 0;

    // increment line number information for the item on top of the stack
    virtual unsigned int increment_line() = 0;

    // get line number information for the item on top of the stack
    virtual unsigned int get_line() const = 0;

    // pop a filename from the top of the stack
    virtual void pop() = 0;

    // generate a list of included files, suitable for producing an error report
    // the list is truncated at depth 'level' to avoid excess verbosity
    virtual std::string write(unsigned int level) const = 0;
    virtual std::string write()                   const = 0;

    virtual filestack* clone() const = 0;

  protected:
    bool                         locked;
  };


// if we need to copy a derived class from a polymorphic pointer to the base class,
// then some extra magic is needed to make that happen
// here, this is done by inherited from an auxiliary class that implements
// a suitable clone() method calling the copy constructor of the derived class
template <class Derived>
class filestack_derivation_helper : public filestack
  {
  public:
    virtual filestack* clone() const
      {
        return new Derived(static_cast<const Derived&>(*this)); // call the copy ctor.
      }
  };


#endif //__filestack_H_
