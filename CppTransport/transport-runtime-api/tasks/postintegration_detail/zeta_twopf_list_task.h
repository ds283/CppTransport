//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __zeta_twopf_list_task_H_
#define __zeta_twopf_list_task_H_


#include "transport-runtime-api/tasks/postintegration_detail/common.h"
#include "transport-runtime-api/tasks/postintegration_detail/abstract.h"


namespace transport
	{

    // ZETA TWOPF LIST TASK -- COMMON ANCESTOR FOR ZETA_TWOPF_TASK AND ZETA_THREEPF_TASK

    template <typename number>
    class zeta_twopf_list_task: public postintegration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a zeta_twopf_list task
        zeta_twopf_list_task(const std::string& nm, const twopf_list_task<number>& t);

        //! deserialization constructor
        zeta_twopf_list_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

        //! override copy constructor to reset ptk_as_twopf_list
        zeta_twopf_list_task(const zeta_twopf_list_task<number>& obj);

        //! destructor is default
        ~zeta_twopf_list_task() = default;


        // INTERFACE

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(this->ptk_as_twopf_list->get_twopf_database()); }


        // SERIALIZATION

      public:

        virtual void serialize(Json::Value& writer) const override;


        // PRIVATE DATA

      protected:

        //! cast-up version of parent task
        //! TODO: it would be preferable to avoid this somehow
        twopf_list_task<number>* ptk_as_twopf_list;

	    };


    template <typename number>
    zeta_twopf_list_task<number>::zeta_twopf_list_task(const std::string& nm, const twopf_list_task<number>& t)
	    : postintegration_task<number>(nm, t),
	      ptk_as_twopf_list(nullptr)
	    {
        ptk_as_twopf_list = dynamic_cast< twopf_list_task<number>* >(this->ptk);
        assert(ptk_as_twopf_list != nullptr);

        if(ptk_as_twopf_list == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    zeta_twopf_list_task<number>::zeta_twopf_list_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
	    : postintegration_task<number>(nm, reader, finder),
	      ptk_as_twopf_list(nullptr)
	    {
        ptk_as_twopf_list = dynamic_cast< twopf_list_task<number>* >(this->ptk);
        assert(ptk_as_twopf_list != nullptr);

        if(ptk_as_twopf_list == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    zeta_twopf_list_task<number>::zeta_twopf_list_task(const zeta_twopf_list_task<number>& obj)
	    : postintegration_task<number>(obj),
	      ptk_as_twopf_list(nullptr)
	    {
        ptk_as_twopf_list = dynamic_cast< twopf_list_task<number>* >(this->ptk);
        assert(ptk_as_twopf_list != nullptr);

        if(ptk_as_twopf_list == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
	    }


    template <typename number>
    void zeta_twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        this->postintegration_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //__zeta_twopf_list_task_H_
