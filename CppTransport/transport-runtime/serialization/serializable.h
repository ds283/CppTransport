//
// Created by David Seery on 25/04/2014.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
