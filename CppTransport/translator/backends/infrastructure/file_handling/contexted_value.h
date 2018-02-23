//
// Created by David Seery on 02/12/2015.
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

#ifndef CPPTRANSPORT_CONTEXTED_VALUE_H
#define CPPTRANSPORT_CONTEXTED_VALUE_H


#include "error_context.h"
#include "parse_error.h"
#include "msg_en.h"


template <typename ValueType>
class contexted_value
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    contexted_value(ValueType v, const error_context& l)
      : value(std::move(v)),
        declaration_point(std::make_shared<error_context>(l))
      {
      }

    //! destructor is default
    ~contexted_value() = default;


    // INTERFACE

  public:

    //! allow implicit conversion to value
    operator ValueType() const { return this->value; }
    
    //! allow dereferencing
    const ValueType& operator*() const { return this->value; }
    const ValueType& get() const { return this->value; }

    //! get declaration point
    const error_context& get_declaration_point() const { return *this->declaration_point; }


    // INTERNAL DATA

  private:

    //! value stored
    ValueType value;

    //! link to declaration point; we take a copy of the
    //! error context provided to us, so that it does not itself
    //! have to be long-lived. We used std::shared_ptr<> since
    //! we share ownership with any copies of ourselves
    std::shared_ptr<error_context> declaration_point;

  };


// set a contexted value for a single data element, but prevent overwriting
template <typename DataType, typename ValueType, typename LexemeType>
bool SetContextedValue(DataType& data, const ValueType& value, const LexemeType& l,
                       std::string err_msg, std::string duplicate_msg = NOTIFY_DUPLICATION_DECLARATION_WAS)
  {
    if(data)   // has a value already been set? if so, report an error
      {
        l.error(err_msg);
        data->get_declaration_point().warn(duplicate_msg);
        throw parse_error(err_msg);
      }
    
    data = std::make_unique< contexted_value<ValueType> >(value, l.get_error_context());
    return true;
  };


#endif //CPPTRANSPORT_CONTEXTED_VALUE_H
