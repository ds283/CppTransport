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
    struct inclusion
      {
        boost::filesystem::path  name;
        unsigned int             line;
      };

    virtual ~input_stack() = default;

    void         push                  (const boost::filesystem::path name);

    virtual void         set_line      (unsigned int line) override;
    virtual unsigned int increment_line() override;
    virtual unsigned int get_line      () const override;

    virtual void         pop           () override;

    virtual std::string  write         (size_t level) const override;
    virtual std::string  write         () const override;

  protected:

    std::deque<struct inclusion> inclusions;

  };


#endif //CPPTRANSPORT_INPUT_STACK_H
