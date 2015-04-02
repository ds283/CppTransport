//
// Created by David Seery on 02/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __derivable_task_H_
#define __derivable_task_H_


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/task_configurations.h"

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

        //! Get vector of time configurations to store
        virtual const std::vector<time_config>& get_time_config_list() const = 0;


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


#endif //__derivable_task_H_
