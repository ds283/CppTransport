//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_TWOPF_TASK_H
#define CPPTRANSPORT_TWOPF_TASK_H


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"
#include "transport-runtime-api/tasks/integration_detail/twopf_db_task.h"


namespace transport
	{

    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_db_task<number>
	    {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named two-point function task
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   range<double>& t, range<double>& ks, bool ff=false);

        //! deserialization constructor
        twopf_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);

        //! Destroy a two-point function task
        ~twopf_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::integration; }

        //! respond to task type query
        integration_task_type get_task_type() const override final { return integration_task_type::twopf; }


        // SERIALIZATION (implements a 'serializable' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual twopf_task<number>* clone() const override { return new twopf_task<number>(static_cast<const twopf_task<number>&>(*this)); }

	    };


    template <typename number>
    struct integration_task_traits<number, integration_task_type::twopf>
      {
        typedef twopf_task<number> task_type;
      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i,
                                   range<double>& t, range<double>& ks, bool ff)
	    : twopf_db_task<number>(nm, i, t, ff)
	    {
        // the mapping from the provided list of ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); ++j)
	        {
            this->twopf_db_task<number>::twopf_db->add_record(ks[j]);
	        }

        std::ostringstream msg;
        msg << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_TWOPF_ELEMENTS_A << " " << this->twopf_db->size() << " " << CPPTRANSPORT_TASK_TWOPF_ELEMENTS_B;
        this->get_model()->message(msg.str());

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details();
        this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional());
	    }


    // deserialization constructor
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : twopf_db_task<number>(nm, reader, handle, i)
	    {
		    // rebuild database of stored times; this isn't serialized but recomputed on-the-fly
        this->cache_stored_time_config_database(this->twopf_db->get_kmax_conventional());
	    }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_TWOPF);

        this->twopf_db_task<number>::serialize(writer);
	    }

	}


#endif //CPPTRANSPORT_TWOPF_TASK_H
