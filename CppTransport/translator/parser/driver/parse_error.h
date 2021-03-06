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

#ifndef CPPTRANSPORT_PARSE_ERROR_H
#define CPPTRANSPORT_PARSE_ERROR_H


#include <stdexcept>


class parse_error : public std::runtime_error
  {
  
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    parse_error(std::string m)
      : std::runtime_error(m),
        msg(std::move(m))
      {
      }
    
    //! destructor is default
    ~parse_error() = default;
    
    
    // INTERNAL DATA
    
  protected:
    
    //! store message
    std::string msg;
  
  };


#endif //CPPTRANSPORT_PARSE_ERROR_H
