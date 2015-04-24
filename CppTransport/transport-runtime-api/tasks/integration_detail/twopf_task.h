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

        //! Construct a named two-point function task
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   const range<double>& t, const range<double>& ks, bool ff=true);

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
        virtual twopf_task<number>* clone() const override { return new twopf_task<number>(static_cast<const twopf_task<number>&>(*this)); }

	    };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i,
                                   const range<double>& t, const range<double>& ks, bool ff)
	    : twopf_list_task<number>(nm, i, t, ff)
	    {
        // the mapping from the provided list of ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); ++j)
	        {
            this->twopf_list_task<number>::twopf_db.add_record(ks[j]);
	        }

        std::cout << "'" << this->get_name() << "': " << __CPP_TRANSPORT_TASK_TWOPF_ELEMENTS_A << " " << this->twopf_db.size() << " "
          <<__CPP_TRANSPORT_TASK_TWOPF_ELEMENTS_B << std::endl;

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details();
        this->cache_stored_time_config_database();
	    }


    // deserialization constructor
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : twopf_list_task<number>(nm, reader, i)
	    {
        // reconstruct horizon-exit times; these aren't stored in the repository record to save space
        this->compute_horizon_exit_times();

        this->cache_stored_time_config_database();
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
