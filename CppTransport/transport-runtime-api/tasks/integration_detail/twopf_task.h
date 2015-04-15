//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_task_H_
#define __twopf_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"
#include "transport-runtime-api/tasks/integration_detail/twopf_list_task.h"


namespace transport
	{

    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
	    {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named two-point function task with specified time-configuration storage policy
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   const range<double>& t, const range<double>& ks,
                   typename integration_task<number>::time_config_storage_policy p);

        //! Construct a named two-point function task with default storage policies
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   const range<double>& t, const range<double>& ks)
	        : twopf_task(nm, i, t, ks, typename integration_task<number>::default_time_config_storage_policy())
	        {
	        }

        //! deserialization constructor
        twopf_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        //! Destroy a two-point function task
        ~twopf_task() = default;


        // SERIALIZATION (implements a 'serializable' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual task<number>* clone() const override { return new twopf_task<number>(static_cast<const twopf_task<number>&>(*this)); }

	    };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i,
                                   const range<double>& t, const range<double>& ks,
                                   typename integration_task<number>::time_config_storage_policy p)
	    : twopf_list_task<number>(nm, i, t, p)
	    {
        // the mapping from the provided list of ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); j++)
	        {
            this->push_twopf_klist(ks[j], j==0);
	        }

        this->apply_time_storage_policy();
	    }


    // deserializtaion constructor
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : twopf_list_task<number>(nm, reader, i)
	    {
	    }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE] = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF);

        this->twopf_list_task<number>::serialize(writer);
	    }

	}


#endif //__twopf_task_H_
