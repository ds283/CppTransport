//
// Created by David Seery on 25/04/2014.
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


#ifndef CPPTRANSPORT_SERIALIZABLE_H
#define CPPTRANSPORT_SERIALIZABLE_H


#include <list>
#include <string>
#include <functional>
#include <utility>

#include "json/json.h"


namespace transport
	{


		// SERIALIZABLE OBJECT INTERFACE

		//! an abstract class which defines the serialization interface

		//! Serialization is used when writing tasks and models to the repository.
		//! We use libjsoncpp to provide a rich interface for serialization and
		//! deserialization

    class serializable
      {

      public:

				// define virtual destructor so that all derived destructors get called correctly
	      virtual ~serializable() = default;


	      // SERIALIZATION INTERFACE

	      //! Serialize this object to the repository. Should be implemented by the derived class
	      virtual void serialize(Json::Value& writer) const = 0;

      };  // class serializable


	}   // namespace transport


#endif //CPPTRANSPORT_SERIALIZABLE_H
