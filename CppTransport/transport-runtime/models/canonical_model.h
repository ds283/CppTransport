//
// Created by David Seery on 22/12/2013.
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


#ifndef __canonical_model_H_
#define __canonical_model_H_


#include <string>
#include <vector>
#include <memory>

#include "transport-runtime/models/model.h"


namespace transport
  {

    // a canonical model: allows an arbitrary number of fields, but flat field-space metric.
    // has a potential but no other structure

    template <typename number>
    class canonical_model : public model<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_model(const std::string& uid, unsigned int tver, error_handler e, warning_handler w, message_handler m)
          : model<number>(uid, tver, std::move(e), std::move(w), std::move(m))
          {
          }

        //! destructor is default
        virtual ~canonical_model() = default;


        // INTERFACE

      public:

        // calculate potential, given a field configuration. Pure virtual, so must be overridden by derived class
        virtual number V(const parameters<number>& p, const std::vector<number>& coords) const = 0;

      };

  }


#endif //__canonical_model_H_
