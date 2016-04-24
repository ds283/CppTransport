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


#ifndef CPPTRANSPORT_ZETA_THREEPF_TASK_H
#define CPPTRANSPORT_ZETA_THREEPF_TASK_H


#include "transport-runtime/tasks/postintegration_detail/common.h"
#include "transport-runtime/tasks/postintegration_detail/abstract.h"
#include "transport-runtime/tasks/postintegration_detail/zeta_twopf_list_task.h"


namespace transport
	{

    // ZETA THREEPF TASK

    //! A 'zeta_threepf_task' task is a postintegration task which produces the zeta three-point
    //! function and associated derived quantities (the reduced bispectrum at the moment)
    template <typename number>
    class zeta_threepf_task: public zeta_twopf_list_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a zeta_threepf task
        zeta_threepf_task(const std::string& nm, const threepf_task<number>& t);

        //! deserialization constructor
        zeta_threepf_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder);

        //! override copy constructor to reset ptk_as_threepf
        zeta_threepf_task(const zeta_threepf_task<number>& obj);

        //! destructor is default
        virtual ~zeta_threepf_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::postintegration; }

        //! respond to task type query
        postintegration_task_type get_task_type() const override final { return postintegration_task_type::threepf; }


        // INTERFACE

      public:

        //! get pairing status
        bool is_paired() const { return(this->paired); }

        //! set pairing status
        void set_paired(bool g) { this->paired = g; }

        //! Determine whether this task is integrable; inherited from parent threepf_task
        bool is_integrable() const { return(this->ptk_as_threepf->is_integrable()); }

        //! Provide access to threepf k-configuration database
        const threepf_kconfig_database& get_threepf_database() const { return(this->ptk_as_threepf->get_threepf_database()); }

        //! Get measure at a particular k-point
        number measure(const threepf_kconfig& config) const { return(this->ptk_as_threepf->measure(config)); }


        // SERIALIZATION

      public:

        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        virtual zeta_threepf_task<number>* clone() const override { return new zeta_threepf_task<number>(static_cast<const zeta_threepf_task<number>&>(*this)); }


        // PRIVATE DATA

      protected:

        //! cast-up version of parent task
        //! TODO: it would be preferable to avoid this somehow
        threepf_task<number>* ptk_as_threepf;

        //! is this task paired to its parent integration task? ie., both tasks are performed simultaneously
        bool paired;

	    };


    template <typename number>
    struct postintegration_task_traits<number, postintegration_task_type::threepf>
      {
        typedef zeta_threepf_task<number> task_type;
      };


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, const threepf_task<number>& t)
	    : zeta_twopf_list_task<number>(nm, t),
	      ptk_as_threepf(nullptr),
	      paired(false)
	    {
        ptk_as_threepf = dynamic_cast< threepf_task<number>* >(this->ptk);
        assert(ptk_as_threepf != nullptr);

        if(ptk_as_threepf == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_THREEPF_CAST_FAIL);
	    }


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder)
	    : zeta_twopf_list_task<number>(nm, reader, finder),
	      ptk_as_threepf(nullptr)
	    {
        ptk_as_threepf = dynamic_cast< threepf_task<number>* >(this->ptk);
        assert(ptk_as_threepf != nullptr);

        if(ptk_as_threepf == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_THREEPF_CAST_FAIL);

        this->paired = reader[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED].asBool();
	    }


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const zeta_threepf_task<number>& obj)
	    : zeta_twopf_list_task<number>(obj),
	      ptk_as_threepf(nullptr),
	      paired(obj.paired)
	    {
        ptk_as_threepf = dynamic_cast< threepf_task<number>* >(this->ptk);
        assert(ptk_as_threepf != nullptr);

        if(ptk_as_threepf == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_THREEPF_CAST_FAIL);
	    }


    template <typename number>
    void zeta_threepf_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE]                   = std::string(CPPTRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF);
        writer[CPPTRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED] = this->paired;

        this->zeta_twopf_list_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_ZETA_THREEPF_TASK_H
