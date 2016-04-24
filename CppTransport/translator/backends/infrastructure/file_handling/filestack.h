//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_FILESTACK_H
#define CPPTRANSPORT_FILESTACK_H


#include <string>


// this is an abstract class whose interface can be implemented by
// subsequent, more specialized derived classes

class filestack
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    // TODO: intended to be explicitly defaulted, but Intel compiler prior to v16 complains
    virtual ~filestack()
      {
      }


		// INTERFACE

  public:

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
    virtual std::string write(size_t level) const = 0;
    virtual std::string write()             const = 0;

    virtual filestack* clone() const = 0;


		// INTERNAL DATA

  protected:

    bool locked;

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


#endif //CPPTRANSPORT_FILESTACK_H
