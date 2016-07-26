//
// Created by David Seery on 09/12/2013.
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


#ifndef CPPTRANSPORT_INPUT_STACK_H
#define CPPTRANSPORT_INPUT_STACK_H


#include <string>
#include <deque>

#include "filestack.h"

#include "boost/filesystem/operations.hpp"


class input_stack: public filestack_derivation_helper<input_stack>
  {

  public:

    // data structure for tracking the source of any lexeme
    class inclusion
      {

      public:
        inclusion(boost::filesystem::path p, unsigned int l)
          : name(std::move(p)),
            line(l)
          {
          }

      public:

        const boost::filesystem::path name;
        unsigned int                  line;

      };

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    input_stack() = default;

    //! destructor is default
    virtual ~input_stack() = default;


    // STACK MANAGEMENT -- implements a 'filestack' interface

  public:

    //! push item on to stack
    void push(const boost::filesystem::path name);

    //! pop item from stack
    void pop() override;

    //! get size of inclusion stack
    size_t size() const override { return this->inclusions.size(); }


    // CONTEXT TRACKING

  public:

    //! set current line number within file
    virtual void set_line(unsigned int line) override;

    //! increment current line number
    virtual unsigned int increment_line() override;

    //! return current line number
    virtual unsigned int get_line() const override;


    // CONTEXT PRINTING

  public:

    //! print context up to given level
    virtual std::string write(size_t level) const override;

    //! print context, no maximum level
    virtual std::string write() const override;


    // INTERNAL DATA

  protected:

    //! inclusion stack
    //! can't use a real stack data type because for context management we need to walk through it,
    //! and a real stack only exposes the top element
    typedef std::deque<inclusion> inclusion_stack;
    inclusion_stack inclusions;

  };


#endif //CPPTRANSPORT_INPUT_STACK_H
