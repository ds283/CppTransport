//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_POSTINTEGRATION_ABSTRACT_TASK_H
#define CPPTRANSPORT_POSTINTEGRATION_ABSTRACT_TASK_H


#include "transport-runtime/tasks/postintegration_detail/common.h"

#include "boost/log/utility/formatting_ostream.hpp"


namespace transport
	{

	  constexpr auto CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT     = "parent-task";
    constexpr auto CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED     = "paired";

    // TASK STRUCTURES -- POST-PROCESSING TASKS

    //! A 'postintegration_task' is a specialization of 'task', used to post-process the output of
    //! an integration to produce zeta correlation functions and other derived products.
    //! The more specialized two- and three-pf zeta tasks are derived from it.
    template <typename number>
    class postintegration_task: public derivable_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named postintegration_task with supplied parent derivable_task
        postintegration_task(const std::string& nm, const derivable_task<number>& t);

        //! deserialization constructor
        postintegration_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder);

        //! override copy constructor to perform a deep copy
        postintegration_task(const postintegration_task<number>& obj);

        //! destroy a postintegration_task
        virtual ~postintegration_task();


        // INTERFACE

      public:

        //! identify integration task type
        virtual postintegration_task_type get_task_type() const = 0;


        // INTERFACE - implements a 'derivable task' interface

      public:

        //! Get database of stored time configurations
        virtual const time_config_database& get_stored_time_config_database() const override { return(this->ptk->get_stored_time_config_database()); }


        // INTERFACE

      public:

        //! Get parent integration task
        derivable_task<number>* get_parent_task() const { return(this->ptk); }


        // SERIALIZATION (implements a 'serializable' interface)

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! write to stream
        template <typename Stream> void write(Stream& out) const;


        // INTERNAL DATA

      protected:

        //! Parent task, which must be of derivable type
        derivable_task<number>* ptk;

	    };


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const postintegration_task<number>& obj)
	    {
        obj.write(out);
        return(out);
	    }


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const postintegration_task<number>& obj)
      {
        obj.write(out);
        return(out);
      }


    template <typename number>
    postintegration_task<number>::postintegration_task(const std::string& nm, const derivable_task<number>& t)
	    : derivable_task<number>(nm),
	      ptk(dynamic_cast<derivable_task<number>*>(t.clone()))
	    {
        assert(ptk != nullptr);
	    }


    template <typename number>
    postintegration_task<number>::postintegration_task(const postintegration_task<number>& obj)
	    : derivable_task<number>(obj),
	      ptk(dynamic_cast<derivable_task<number>*>(obj.ptk->clone()))
	    {
        assert(ptk != nullptr);
	    }


    template <typename number>
    postintegration_task<number>::postintegration_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder)
	    : derivable_task<number>(nm, reader),
	      ptk(nullptr)
	    {
        // deserialize and reconstruct parent integration task
        std::string tk_name = reader[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT].asString();

        std::unique_ptr< task_record<number> > record = finder(tk_name);
        assert(record.get() != nullptr);

        ptk = dynamic_cast< derivable_task<number>* >(record->get_abstract_task()->clone());
        if(ptk == nullptr)
	        {
            std::stringstream msg;
            msg << CPPTRANSPORT_REPO_ZETA_TASK_NOT_DERIVABLE << " '" << tk_name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    postintegration_task<number>::~postintegration_task()
	    {
        delete this->ptk;
	    }


    template <typename number>
    void postintegration_task<number>::serialize(Json::Value& writer) const
	    {
        // serialize parent integration task
        writer[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT] = this->ptk->get_name();
        this->derivable_task<number>::serialize(writer);
	    }


    template <typename number>
    template <typename Stream>
    void postintegration_task<number>::write(Stream& out) const
	    {
        out << CPPTRANSPORT_PARENT_TASK << ": '" << this->ptk->get_name() << "'" << '\n';
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_POSTINTEGRATION_ABSTRACT_TASK_H
