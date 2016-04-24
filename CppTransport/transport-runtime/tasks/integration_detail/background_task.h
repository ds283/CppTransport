//
// Created by David Seery on 15/04/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_BACKGROUND_TASK_H
#define CPPTRANSPORT_BACKGROUND_TASK_H


#include "transport-runtime/tasks/integration_detail/common.h"
#include "transport-runtime/tasks/integration_detail/abstract.h"


namespace transport
	{

    //! A background task is a specialization of an integration task which is concrete.
    //! (The integration task class itself is abstract, because it delegates cloning and serialization
    //! to a concrete derived class.)
    //! It is used for integrating the background only, eg. when finding k* = H*
    //! or processing initial conditions
    template <typename number>
    class background_task: public integration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a background task
        background_task(const initial_conditions<number>& i, range<double>& t);

        virtual ~background_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::integration; }

        //! does not accept task_type queries
        integration_task_type get_task_type() const override final { throw std::runtime_error(CPPTRANSPORT_QUERY_BACKGROUND_TASK); }


        // DISABLE SERIALIZATION

      public:

        //! Throw an exception if any attempt is made to serialize a background_task.
        //! Only twopf and threepf integration tasks can be serialized.
        virtual void serialize(Json::Value& writer) const override { throw std::runtime_error(CPPTRANSPORT_SERIALIZE_BACKGROUND_TASK); }


        // DISABLE K-CONFIGURATION DATABASE

      public:

		    //! Throw an exception if an attempt is made to write a background k-configuration database
		    virtual void write_kconfig_database(sqlite3* handle) override { throw std::runtime_error(CPPTRANSPORT_KCONFIG_BACKGROUND_TASK); }

        //! Throw an exception if an attempt is made to write a background k-configuration database
		    virtual bool is_kconfig_database_modified() const override { throw std::runtime_error(CPPTRANSPORT_KCONFIG_BACKGROUND_TASK); }


        // CLONE

      public:

        //! Virtual copy
        virtual background_task<number>* clone() const override { return new background_task<number>(static_cast<const background_task<number>&>(*this)); }

	    };


    template <typename number>
    background_task<number>::background_task(const initial_conditions<number>& i, range<double>& t)
	    : integration_task<number>(i, t)
	    {
        this->build_time_config_database();
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_BACKGROUND_TASK_H
