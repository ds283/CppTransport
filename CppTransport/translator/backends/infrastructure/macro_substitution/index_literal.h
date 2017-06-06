//
// Created by David Seery on 06/06/2017.
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


#ifndef CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H
#define CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H


#include <memory>
#include <list>

#include "abstract_index.h"


//! enumeration representing variance type (co-, contra-, or unset)
enum class variance { covariant, contravariant, none };


//! index literal represents an instance of an abstract index, and carries context information
//! such as the variance type
class index_literal
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor; defaults to unset variance
    index_literal(abstract_index_list::iterator& i, variance t = variance::none)
      : idx(i),
        type(t)
      {
      }

    //! destructor is default
    ~index_literal() = default;


    // INTERFACE

  public:

    //! get underlying an abstract_index object
    abstract_index& get() { return *this->idx; }

    //! get underlying an abstract_index object (const version)
    const abstract_index& get() const { abstract_index_list::const_iterator t = this->idx; return *t; }

    //! allow conversion to abstract_index
    operator abstract_index() { return this->get(); }

    //! allow conversion to abstract index (const version)
    operator const abstract_index() { return this->get(); }

    //! get index variance
    variance get_variance() const { return this->type; }


    // INTERNAL DATA

  private:

    //! link to abstract index of which this is an instance
    abstract_index_list::iterator idx;

    //! variance
    variance type;

  };


//! typedef for list of index literals
typedef std::list< std::shared_ptr<index_literal> > index_literal_list;


#endif //CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H
