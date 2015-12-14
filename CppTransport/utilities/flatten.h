//
// Created by David Seery on 14/11/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_FLATTEN_H
#define CPPTRANSPORT_FLATTEN_H

#include <math.h>


class abstract_flattener
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    abstract_flattener(unsigned int s) : size(s) {}


    // INTERFACE

  public:

    unsigned int get_size()                         { return(this->size); }
    void         set_size(unsigned int s)           { this->size = s; }
    unsigned int get_flattened_size(unsigned int d) { return(d > 0 ? this->size*this->get_flattened_size(d-1) : 1); }

    virtual unsigned int flatten(unsigned int a) = 0;
    virtual unsigned int flatten(unsigned int a, unsigned int b) = 0;
    virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) = 0;


    // INTERNAL DATA

  protected:

    unsigned int size;

  };


class flattener : public abstract_flattener
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    flattener(unsigned int s) : abstract_flattener(s) {}


    // INTERFACE

  public:

    unsigned int flatten(unsigned int a)                                 { return(a); }
    unsigned int flatten(unsigned int a, unsigned int b)                 { return(this->size*a + b); }
    unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(this->size*this->size*a + this->size*b + c); }

  };


#endif // CPPTRANSPORT_FLATTEN_H