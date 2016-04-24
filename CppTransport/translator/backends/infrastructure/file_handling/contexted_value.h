//
// Created by David Seery on 02/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
    contexted_value(ValueType v, const error_context l)
      : value(std::move(v)),
        declaration_point(std::make_shared<error_context>(l))
      {
      }

    //! destructor is default
    ~contexted_value() = default;


    // INTERFACE

  public:

    //! allow implicit conversion to value
    operator ValueType() const { return(this->value); }

    //! get declaration point
    const error_context& get_declaration_point() const { return(*this->declaration_point); }


    // INTERNAL DATA

  private:

    //! value stored
    ValueType value;

    //! link to declaration point; we take a copy of the
    //! error context provided to us, so that it does not itself
    //! have to be long-lived. We share ownership with any
    //! copies of ourselves
    std::shared_ptr<error_context> declaration_point;

  };


#endif //CPPTRANSPORT_CONTEXTED_VALUE_H
