//
// Created by David Seery on 02/04/15.
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


#ifndef CPPTRANSPORT_DERIVABLE_TASK_H
#define CPPTRANSPORT_DERIVABLE_TASK_H


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

#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/task_configurations.h"
#include "transport-runtime/tasks/configuration-database/time_config_database.h"


namespace transport
	{

    //! A derivable_task is a task which produces output which can be consumed
    //! by another task or an item of derived content
    template <typename number>
    class derivable_task: public task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a derivable task
        derivable_task(const std::string& nm);

        //! deserialization constructor
        derivable_task(const std::string& nm, Json::Value& reader);

        //! destructor
        virtual ~derivable_task() = default;


        // INTERFACE

      public:

        //! Get database of stored time configurations
        virtual const time_config_database& get_stored_time_config_database() const = 0;

        //! Identify task type
        virtual task_type get_type() const = 0;


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;

	    };


    template <typename number>
    derivable_task<number>::derivable_task(const std::string& nm)
	    : task<number>(nm)
	    {
	    }


    template <typename number>
    derivable_task<number>::derivable_task(const std::string& nm, Json::Value& reader)
	    : task<number>(nm, reader)
	    {
	    }


    template <typename number>
    void derivable_task<number>::serialize(Json::Value& writer) const
	    {
        this->task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVABLE_TASK_H
