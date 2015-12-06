//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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

				//! Construct a named task
				task(const std::string& nm);

				//! Deserialization constructor
				task(const std::string& nm, Json::Value& reader);

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
				virtual void serialize(Json::Value& writer) const override;


				// INTERNAL DATA

		  protected:

		    //! Name of this task (left without const qualifier because we want to allow copy assignment)
		    std::string name;

			};


		template <typename number>
		task<number>::task(const std::string& nm)
			: name(nm)
			{
			}


		template <typename number>
		task<number>::task(const std::string& nm, Json::Value& reader)
			: name(nm)
			{
			}


		template <typename number>
		void task<number>::serialize(Json::Value& writer) const
			{
			}


	}   // namespace transport


#endif //__task_H_
