//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __zeta_twopf_task_H_
#define __zeta_twopf_task_H_


#include "transport-runtime-api/tasks/postintegration_detail/common.h"
#include "transport-runtime-api/tasks/postintegration_detail/abstract.h"
#include "transport-runtime-api/tasks/postintegration_detail/zeta_twopf_list_task.h"


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
        zeta_twopf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

        //! destructor is default
        virtual ~zeta_twopf_task() = default;


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
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, const twopf_task<number>& t)
	    : zeta_twopf_list_task<number>(nm, t),
	      paired(false)
	    {
	    }


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
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


#endif //__zeta_twopf_task_H_
