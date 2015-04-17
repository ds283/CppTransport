//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_list_task_H_
#define __twopf_list_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"

#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#define __CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE "twopf-database"


namespace transport
	{

    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a flat vector of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public integration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a twopf-list-task object
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                        time_config_storage_policy p);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE

      public:

        //! Provide access to twopf k-configuration database
        const twopf_kconfig_database& get_twopf_database() const { return(this->twopf_db); }


        // FAST-FORWARD INTEGRATION

      public:

        //! get largest k-mode included in the integration
        virtual double get_kmax() const override { return(this->twopf_db.get_kmax_conventional()); }

        //! get smallest k-mode included in the integration
        virtual double get_kmin() const override { return(this->twopf_db.get_kmin_conventional()); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! database of twopf k-configurations
        twopf_kconfig_database twopf_db;

	    };


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                             time_config_storage_policy p)
	    : integration_task<number>(nm, i, t, p),
        twopf_db(i.get_model()->compute_kstar(this))
	    {
	    }


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : integration_task<number>(nm, reader, i),
        twopf_db(reader[__CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE])
	    {
	    }


    template <typename number>
    void twopf_list_task<number>::serialize(Json::Value& writer) const
	    {
        Json::Value db;
        this->twopf_db.serialize(db);
        writer[__CPP_TRANSPORT_NODE_TWOPF_LIST_DATABASE] = db;

        this->integration_task<number>::serialize(writer);
	    }


	}   // namespace transport


#endif //__twopf_list_task_H_
