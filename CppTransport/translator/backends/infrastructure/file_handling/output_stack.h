//
// Created by David Seery on 10/12/2013.
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


#ifndef CPPTRANSPORT_OUTPUT_STACK_H
#define CPPTRANSPORT_OUTPUT_STACK_H


#include <string>
#include <deque>

#include "filestack.h"
#include "buffer.h"

#include "boost/filesystem/operations.hpp"


// forward reference to avoid circularity
class macro_agent;


enum class process_type
  {
    process_core, process_implementation
  };


class output_stack : public filestack_derivation_helper<output_stack>
  {

  public:

    class inclusion
      {

      public:

        inclusion(boost::filesystem::path i, unsigned int l, buffer& b, macro_agent& a, enum process_type t)
          : in(std::move(i)),
            line(l),
            buf(b),
            agent(a),
            type(t)
          {
          }


      public:

        const boost::filesystem::path in;
        unsigned int line;
        buffer& buf;
        macro_agent& agent;
        const enum process_type type;

      };


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    output_stack() = default;

    //! destructor is default
    virtual ~output_stack() = default;


    // STACK MANAGEMENT -- implements a 'filestack' interface

  public:

    // push object to the top of the stack
    void push(const boost::filesystem::path in, buffer& buf, macro_agent& agent, enum process_type type);

    //! pop object from top of tstack
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


    // INTERFACE - specific to output_stack

  public:

    //! return current top-of-stack buffer
    buffer& top_buffer();

    //! return current top-of-stack macro package
    macro_agent& top_macro_package();

    //! return current top-of-stack process type (core, implementation)
    enum process_type top_process_type() const;


    // INTERNAL DATA

  protected:

    //! inclusion stack
    //! can't use a real stack data type because for context management we need to walk through it,
    //! and a real stack only exposes the top element
    typedef std::deque<inclusion> inclusion_stack;
    inclusion_stack inclusions;

  };


#endif //CPPTRANSPORT_OUTPUT_STACK_H
