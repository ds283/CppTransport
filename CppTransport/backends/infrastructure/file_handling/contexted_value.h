//
// Created by David Seery on 02/12/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTEXTED_VALUE_H
#define CPPTRANSPORT_CONTEXTED_VALUE_H


#include "error_context.h"


template <typename ValueType>
class contexted_value
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    contexted_value(ValueType v, const error_context& l)
      : value(std::move(v)),
        declaration_point(l)
      {
      }

    //! destructor is default
    ~contexted_value() = default;


    // INTERFACE

  public:

    //! allow implicit conversion to value
    operator ValueType() const { return(this->value); }

    //! get declaration point
    const error_context& get_declaration_point() const { return(this->declaration_point); }


    // INTERNAL DATA

  private:

    //! value stored
    ValueType value;

    //! reference to declaration point
    const error_context& declaration_point;

  };


#endif //CPPTRANSPORT_CONTEXTED_VALUE_H
