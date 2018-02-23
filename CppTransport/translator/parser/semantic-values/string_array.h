//
// Created by David Seery on 26/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_STRING_ARRAY_H
#define CPPTRANSPORT_STRING_ARRAY_H


#include <iostream>
#include <string>
#include <memory>

#include "filestack.h"
#include "lexeme.h"
#include "contexted_value.h"
#include "y_common.h"
#include "msg_en.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


class string_array
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    string_array() = default;

    //! destructor
    ~string_array() = default;


    // INTERFACE

  public:

    //! get array of contexted values representing string literals
    const std::vector< contexted_value<std::string> >& get_array() const { return(this->array); }

    //! push element to the array
    void push_element(const std::string& s, const y::lexeme_type& l) { this->array.emplace_back(s, l.get_error_context()); }


    // INTERNAL DATA

  private:

    //! array of contexted string literals
    std::vector< contexted_value<std::string> > array;

  };


#endif //CPPTRANSPORT_STRING_ARRAY_H
