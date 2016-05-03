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


#ifndef CPPTRANSPORT_TASK_H
#define CPPTRANSPORT_TASK_H


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>


#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/tasks/task_types.h"


#define CPPTRANSPORT_NODE_TASK_DESCRIPTION        "description"

#define CPPTRANSPORT_NODE_TASK_TYPE               "task-type"

#define CPPTRANSPORT_NODE_TASK_TYPE_TWOPF         "twopf-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC "threepf-cubic-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_FLS   "threepf-fls-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_OUTPUT        "output-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF    "zeta-twopf-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF  "zeta-threepf-task"
#define CPPTRANSPORT_NODE_TASK_TYPE_FNL           "fNL-task"


namespace transport
  {

		// TASK STRUCTURES -- GENERIC

		//! A 'task' is the basic, abstract task from which all other types of task are derived.
		template <typename number>
		class task: public serializable
			{

		  public:

				// CONSTRUCTOR, DESTRUCTOR

				//! Construct a named task with given serializable status
				task(std::string nm, bool s=true);

				//! Deserialization constructor
				task(std::string nm, Json::Value& reader);

				//! Destroy a task
				virtual ~task() = default;


				// INTERFACE

		  public:

		    //! Get name
		    const std::string& get_name() const { return(this->name); }

        //! Reset name; if name is non-zero, assume this
        void set_name(std::string s) { this->name = std::move(s); }

        //! Set description
        void set_description(std::string d) { this->description = std::move(d); }

        //! Get description
        const std::string& get_description() const { return(this->description); }

        //! Get serializable status
        bool is_serializable() const { return(this->serializable); }


				// CLONE

		  public:

				//! Clone this task object using a virtual copy idiom.
				//! Given a pointer just to the base class task<>, it isn't possible
				//! to perform a deep copy. This method avoids that problem.
				virtual task<number>* clone() const = 0;


				// SERIALIZE -- implements a 'serializable' interface

		  public:

		    //! serialize this object
				virtual void serialize(Json::Value& writer) const override;


				// INTERNAL DATA

		  protected:

		    //! Name of this task (left without const qualifier because we want to allow copy assignment)
		    std::string name;

        //! Description for this task
        std::string description;

        //! Is this task serializable?
        bool serializable;

			};


		template <typename number>
		task<number>::task(std::string nm, bool s)
			: name(std::move(nm)),
        serializable(s)
			{
			}


		template <typename number>
		task<number>::task(std::string nm, Json::Value& reader)
			: name(std::move(nm)),
        description(reader[CPPTRANSPORT_NODE_TASK_DESCRIPTION].asString()),
        serializable(true)
			{
			}


		template <typename number>
		void task<number>::serialize(Json::Value& writer) const
			{
        writer[CPPTRANSPORT_NODE_TASK_DESCRIPTION] = this->description;
			}


	}   // namespace transport


#endif //CPPTRANSPORT_TASK_H
