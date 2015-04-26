//
// Created by David Seery on 16/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __time_config_database_H_
#define __time_config_database_H_


#include <functional>
#include <map>

#include <assert.h>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/tasks/configuration-database/generic_iterator.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


namespace transport
  {

		class time_config_record
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! build a time configuration database record
				time_config_record(time_config& t, bool s);

				//! destructor is default
				~time_config_record() = default;


				// INTERFACE

		  public:

				//! store this configuration
				bool is_stored() const { return(this->store); }

				//! dereference to get time
				time_config& operator*() { return(this->record); }
				const time_config& operator*() const { return(this->record); }

        //! provide member access into the time_config record
        time_config* operator->() { return(&this->record); }
        const time_config* operator->() const { return(&this->record); }

				friend std::ostream& operator<<(std::ostream& out, const time_config_record& obj);


				// INTERNAL DATA

		  private:

				//! time
				time_config record;

				//! store this configuration in the database?
				bool store;

			};


		time_config_record::time_config_record(time_config& t, bool s)
			: record(t),
		    store(s)
			{
			}


		std::ostream& operator<<(std::ostream& out, const time_config_record& obj)
			{
				out << *obj;
        return(out);
			}


		class time_config_database
			{

		  private:

				//! alias for database data structure
				typedef std::map< unsigned int, time_config_record > database_type;


				// RECORD VALUED ITERATORS

		  public:

		    // specialize to obtain intended iterators
		    typedef configuration_database::generic_record_iterator<database_type, time_config_record, false > record_iterator;
		    typedef configuration_database::generic_record_iterator<database_type, time_config_record, true >  const_record_iterator;


		    // CONFIG VALUED ITERATOR

		  public:

		    // specialize to obtain intended iterators
		    typedef configuration_database::generic_config_iterator<database_type, time_config, false> config_iterator;
		    typedef configuration_database::generic_config_iterator<database_type, time_config, true>  const_config_iterator;


		    // VALUE ITERATOR

		  public:

		    // specialize to obtain intended iterators
		    typedef configuration_database::generic_value_iterator<database_type, double, false> value_iterator;
		    typedef configuration_database::generic_value_iterator<database_type, double, true>  const_value_iterator;


		    // CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor
				time_config_database();

				//! destructor
				~time_config_database() = default;


		    // RECORD ITERATORS

		  public:

		    record_iterator       record_begin()        { return record_iterator(this->database.begin()); }
		    record_iterator       record_end()          { return record_iterator(this->database.end()); }
		    const_record_iterator record_begin()  const { return const_record_iterator(this->database.begin()); }
		    const_record_iterator record_end()    const { return const_record_iterator(this->database.end()); }

		    const_record_iterator crecord_begin() const { return const_record_iterator(this->database.cbegin()); }
		    const_record_iterator crecord_end()   const { return const_record_iterator(this->database.cend()); }


		    // CONFIG ITERATORS

		  public:

		    config_iterator       config_begin()        { return config_iterator(this->database.begin()); }
		    config_iterator       config_end()          { return config_iterator(this->database.end()); }
		    const_config_iterator config_begin()  const { return const_config_iterator(this->database.begin()); }
		    const_config_iterator config_end()    const { return const_config_iterator(this->database.end()); }

		    const_config_iterator cconfig_begin() const { return const_config_iterator(this->database.cbegin()); }
		    const_config_iterator cconfig_end()   const { return const_config_iterator(this->database.cend()); }


		    // VALUE ITERATORS

		  public:

		    value_iterator       value_begin(double offset=0.0)       { return value_iterator(this->database.begin(), offset); }
		    value_iterator       value_end(double offset=0.0)         { return value_iterator(this->database.end(), offset); }
		    const_value_iterator value_begin(double offset=0.0) const { return const_value_iterator(this->database.begin(), offset); }
		    const_value_iterator value_end(double offset=0.0)   const { return const_value_iterator(this->database.end(), offset); }

		    const_value_iterator cvalue_begin(double offset=0.0)      { return const_value_iterator(this->database.cbegin(), offset); }
		    const_value_iterator cvalue_end(double offset=0.0)        { return const_value_iterator(this->database.cend(), offset); }


				// INTERFACE -- GLOBAL OPERATIONS

		  public:

				//! empty database
				void clear();

				//! get number of records in database
				size_t size() const { return(this->database.size()); }


				// INTERFACE -- ADD AND LOOKUP RECORDS

		  public:

				//! add record to the database
				void add_record(double t, bool store, unsigned int serial);


		    // INTERNAL DATA


		  protected:

		    // DATABASE

		    //! database of k-configurations
		    database_type database;

			};


		time_config_database::time_config_database()
			{
			}


    void time_config_database::clear()
	    {
        this->database.clear();
	    }


		void time_config_database::add_record(double t, bool store, unsigned int serial)
			{
				time_config config;

				config.serial = serial;
				config.t = t;

				this->database.emplace(config.serial, time_config_record(config, store));
			}


  }   // namespace transport


#endif //__time_config_database_H_
