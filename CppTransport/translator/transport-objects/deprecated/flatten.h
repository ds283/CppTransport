//
// Created by David Seery on 14/11/2013.
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


#ifndef CPPTRANSPORT_FLATTEN_H
#define CPPTRANSPORT_FLATTEN_H


class abstract_flattener
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    abstract_flattener(unsigned int s)
      : size(s)
      {
      }


    // INTERFACE

  public:

    //! get size of a flattened d-dimensional array
    unsigned int get_flattened_size(unsigned int d)
      {
        unsigned int s = 1;
        while(d > 0)
          {
            s *= size;
            --d;
          }
        return(s);
      }

    //! flatten 1-index object
    virtual unsigned int flatten(unsigned int a) = 0;

    //! flatten 2-index object
    virtual unsigned int flatten(unsigned int a, unsigned int b) = 0;

    //! flatten 3-index object
    virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) = 0;


    // INTERNAL DATA

  protected:

    //! size of each dimension
    unsigned int size;

  };


// notice that our flattening scheme is never exposed to the external environment;
// it's purely internal to the translator
// So there is no need to offer different flattening schemes
class right_order_flattener : public abstract_flattener
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    right_order_flattener(unsigned int s)
      : abstract_flattener(s)
      {
      }


    // INTERFACE

  public:

    //! flatten 1-index object
    unsigned int flatten(unsigned int a)                                 { return(a); }

    //! flatten 2-index object
    unsigned int flatten(unsigned int a, unsigned int b)                 { return(this->size*a + b); }

    //! flatten 3-index object
    unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(this->size*this->size*a + this->size*b + c); }

  };


#endif // CPPTRANSPORT_FLATTEN_H