//
// Created by David Seery on 23/06/2017.
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

#ifndef CPPTRANSPORT_TENSOR_EXCEPTION_H
#define CPPTRANSPORT_TENSOR_EXCEPTION_H


//! exception class thrown when errors occur during tensor evaluation
class tensor_exception: std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    tensor_exception(std::string x)
      : std::runtime_error(std::move(x))
      {
      }

    //! destructor is default
    ~tensor_exception() = default;

  };


#endif //CPPTRANSPORT_TENSOR_EXCEPTION_H
