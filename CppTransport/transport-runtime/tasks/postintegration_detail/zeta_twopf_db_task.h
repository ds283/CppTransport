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


#ifndef CPPTRANSPORT_ZETA_TWOPF_DB_TASK_H
#define CPPTRANSPORT_ZETA_TWOPF_DB_TASK_H


#include "transport-runtime/tasks/postintegration_detail/common.h"
#include "transport-runtime/tasks/postintegration_detail/abstract.h"


namespace transport
	{

    // ZETA TWOPF DATABASE TASK -- COMMON ANCESTOR FOR ZETA_TWOPF_TASK AND ZETA_THREEPF_TASK

    template <typename number>
    class zeta_twopf_db_task: public postintegration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a zeta_twopf_database task
        zeta_twopf_db_task(const std::string& nm, const twopf_db_task<number>& t);

        //! deserialization constructor
        zeta_twopf_db_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder);

        //! override copy constructor to reset ptk_as_twopf_db
        zeta_twopf_db_task(const zeta_twopf_db_task<number>& obj);

        //! destructor is default
        ~zeta_twopf_db_task() = default;


        // INTERFACE

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(this->ptk_as_twopf_db->get_twopf_database()); }


        // SERIALIZATION

      public:

        virtual void serialize(Json::Value& writer) const override;


        // PRIVATE DATA

      protected:

        //! cast-up version of parent task
        //! TODO: it would be preferable to avoid this somehow
        twopf_db_task<number>* ptk_as_twopf_db;

	    };


    template <typename number>
    zeta_twopf_db_task<number>::zeta_twopf_db_task(const std::string& nm, const twopf_db_task<number>& t)
	    : postintegration_task<number>(nm, t),
	      ptk_as_twopf_db(nullptr)
	    {
        ptk_as_twopf_db = dynamic_cast< twopf_db_task<number>* >(this->ptk);
        assert(ptk_as_twopf_db != nullptr);

        if(ptk_as_twopf_db == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    zeta_twopf_db_task<number>::zeta_twopf_db_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder)
	    : postintegration_task<number>(nm, reader, finder),
	      ptk_as_twopf_db(nullptr)
	    {
        ptk_as_twopf_db = dynamic_cast< twopf_db_task<number>* >(this->ptk);
        assert(ptk_as_twopf_db != nullptr);

        if(ptk_as_twopf_db == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    zeta_twopf_db_task<number>::zeta_twopf_db_task(const zeta_twopf_db_task<number>& obj)
	    : postintegration_task<number>(obj),
	      ptk_as_twopf_db(nullptr)
	    {
        ptk_as_twopf_db = dynamic_cast< twopf_db_task<number>* >(this->ptk);
        assert(ptk_as_twopf_db != nullptr);

        if(ptk_as_twopf_db == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    void zeta_twopf_db_task<number>::serialize(Json::Value& writer) const
	    {
        this->postintegration_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_ZETA_TWOPF_DB_TASK_H
