//
// Created by David Seery on 20/06/2017.
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

#ifndef CPPTRANSPORT_SIMPLE_RESOURCE_H
#define CPPTRANSPORT_SIMPLE_RESOURCE_H

#include <string>
#include <limits>

#include "contexted_value.h"
#include "index_literal.h"

#include "msg_en.h"

#include "boost/optional.hpp"


template <typename DataType = std::string>
class simple_resource
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor should set useage flag
    simple_resource(bool dw);

    //! destructor checks whether label is unused
    ~simple_resource();
    
    
    // INTERFACE
    
  public:
    
    //! assign a value
    simple_resource& assign(const contexted_value<DataType>& d);
    
    //! lookup value
    const boost::optional< contexted_value<DataType> >& find();
    
    //! clear any stored values
    simple_resource& reset();
    
  protected:
    
    //! check whether the label is unused, and if so issue an appropriate warning
    void warn_unused() const;
    
    
    // INTERNAL DATA
    
  private:
    
    //! resource label, if set
    boost::optional< contexted_value<DataType> > label;
    
    //! usage flag -- to track whether unused resources are being declared
    //! (if so then they are presumably also being computed, so should be eliminated)
    bool used;

    //! show developer warnings?
    bool dev_warn;
    
  };


template <typename DataType>
simple_resource<DataType>::simple_resource(bool dw)
  : used(false),
    dev_warn(dw)
  {
  }


template <typename DataType>
simple_resource<DataType>::~simple_resource()
  {
    this->warn_unused();
  }


template <typename DataType>
simple_resource<DataType>& simple_resource<DataType>::assign(const contexted_value<DataType>& d)
  {
    // if there is no existing value, assign and return
    if(!this->label)
      {
        this->label = d;
        return *this;
      }
    
    // otherwise, issue a warning that we are overwriting an unreleased value
    if(this->dev_warn)
      {
        const error_context& ctx = d.get_declaration_point();
        ctx.warn(NOTIFY_RESOURCE_REDECLARATION);

        const error_context& p_ctx = this->label.get().get_declaration_point();
        p_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
      }

    this->label = d;
    
    return *this;
  }


template <typename DataType>
const boost::optional<contexted_value<DataType> >& simple_resource<DataType>::find()
  {
    this->used = true;
    return this->label;
  }


template <typename DataType>
simple_resource<DataType>& simple_resource<DataType>::reset()
  {
    this->warn_unused();
    this->label.reset();
    return *this;
  }


template <typename DataType>
void simple_resource<DataType>::warn_unused() const
  {
    if(this->dev_warn && !this->used && this->label)
      {
        const error_context& ctx = this->label->get_declaration_point();
        ctx.warn(NOTIFY_RESOURCE_DECLARED_NOT_USED);
      }
  }


#endif //CPPTRANSPORT_SIMPLE_RESOURCE_H
