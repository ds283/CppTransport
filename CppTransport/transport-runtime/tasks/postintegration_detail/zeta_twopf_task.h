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


#ifndef CPPTRANSPORT_ZETA_TWOPF_TASK_H
#define CPPTRANSPORT_ZETA_TWOPF_TASK_H


#include "transport-runtime/tasks/postintegration_detail/common.h"
#include "transport-runtime/tasks/postintegration_detail/abstract.h"
#include "transport-runtime/tasks/postintegration_detail/zeta_twopf_list_task.h"


namespace transport
	{

    // ZETA TWOPF TASK

    //! A 'zeta_twopf_task' task is a postintegration task which produces the zeta two-point function
    template <typename number>
    class zeta_twopf_task: public zeta_twopf_list_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a zeta_twopf task
        zeta_twopf_task(const std::string& nm, const twopf_task<number>& t);

        //! deserialization constructor
        zeta_twopf_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder);

        //! destructor is default
        virtual ~zeta_twopf_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::postintegration; }

        //! respond to task type query
        postintegration_task_type get_task_type() const override final { return postintegration_task_type::twopf; }


        // INTERFACE

      public:

        //! get pairing status
        bool is_paired() const { return(this->paired); }

        //! set pairing status
        void set_paired(bool g) { this->paired = g; }


        // SERIALIZATION

      public:

        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        virtual zeta_twopf_task<number>* clone() const override { return new zeta_twopf_task<number>(static_cast<const zeta_twopf_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! is this task paired to its parent integration task? ie., both tasks are performed simultaneously
        bool paired;

	    };


    template <typename number>
    struct postintegration_task_traits<number, postintegration_task_type::twopf>
      {
        typedef zeta_twopf_task<number> task_type;
      };


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, const twopf_task<number>& t)
	    : zeta_twopf_list_task<number>(nm, t),
	      paired(false)
	    {
	    }


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder)
	    : zeta_twopf_list_task<number>(nm, reader, finder)
	    {
        this->paired = reader[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED].asBool();
	    }


    template <typename number>
    void zeta_twopf_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE]                   = std::string(CPPTRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF);
        writer[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED] = this->paired;

        this->zeta_twopf_list_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_ZETA_TWOPF_TASK_H
