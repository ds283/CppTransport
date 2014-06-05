//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_H_
#define __task_H_


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


#define __CPP_TRANSPORT_NODE_TASK_TYPE               "task-type"
#define __CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF         "twopf-task"
#define __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC "threepf-cubic-task"
#define __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_FLS   "threepf-cubic-fls"
#define __CPP_TRANSPORT_NODE_TASK_TYPE_OUTPUT        "output-task"


namespace transport
  {

		// TASK STRUCTURES -- GENERIC

		//! A 'task' is the basic, abstract task from which all other types of task are derived.
		template <typename number>
		class task: public serializable
			{

		  public:

				// CONSTRUCTOR, DESTRUCTOR

				//! Construct a named task
				task(const std::string& nm);

				//! Deserialization constructor
				task(const std::string& nm, serialization_reader* reader);

				//! Destroy a task
				virtual ~task() = default;


				// INTERFACE

		  public:

		    //! Get name
		    const std::string& get_name() const { return(this->name); }


				// CLONE

		  public:

				//! Clone this task object using a virtual copy idiom.
				//! Given a pointer just to the base class task<>, it isn't possible
				//! to perform a deepy copy. This method avoids that problem.
				virtual task<number>* clone() const = 0;


				// SERIALIZE -- implements a 'serializable' interface

		  public:

		    //! serialize this object
				virtual void serialize(serialization_writer& writer) const override;


				// INTERNAL DATA

		  protected:

		    //! Name of this task
		    const std::string name;

			};


		template <typename number>
		task<number>::task(const std::string& nm)
			: name(nm)
			{
			}


		template <typename number>
		task<number>::task(const std::string& nm, serialization_reader* reader)
			: name(nm)
			{
				assert(reader != nullptr);
				if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);
				// Currently no deserialization required
			}


		template <typename number>
		void task<number>::serialize(serialization_writer& writer) const
			{
				// Currently no serialization required
			}


	}   // namespace transport


#endif //__task_H_
