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

    // TIME CONFIGURATION STORAGE POLICIES

    //! defines a 'time-configuration storage policy' data object, passed to a policy specification
    //! for the purpose of deciding whether a time configuration will be kept
    class time_config_storage_policy_data
      {
      public:
        time_config_storage_policy_data(double t, unsigned int s)
          : serial(s), time(t)
          {
          }

      public:
        unsigned int serial;
        double       time;
      };

    //! defines a 'time-configuration storage policy' object which determines which time steps are retained in the database
    typedef std::function<bool(time_config_storage_policy_data&)> time_config_storage_policy;

    class time_storage_record
      {
      public:
        time_storage_record(bool s, unsigned int n, double t)
          : store(s),
            tserial(n),
            time(t)
          {
            assert(s == true || (s== false && n == 0));
          }

        bool store;
        unsigned int tserial;
        double time;
      };

    //! default time configuration storage policy - store everything
    class default_time_config_storage_policy
      {
      public:
        bool operator() (time_config_storage_policy_data&) { return(true); }
      };
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

		    record_iterator       record_begin()       { return record_iterator(this->database.begin()); }
		    record_iterator       record_end()         { return record_iterator(this->database.end()); }
		    const_record_iterator record_begin() const { return const_record_iterator(this->database.begin()); }
		    const_record_iterator record_end()   const { return const_record_iterator(this->database.end()); }

		    const_record_iterator crecord_begin()      { return const_record_iterator(this->database.cbegin()); }
		    const_record_iterator crecord_end()        { return const_record_iterator(this->database.cend()); }


		    // CONFIG ITERATORS

		  public:

		    config_iterator       config_begin()       { return config_iterator(this->database.begin()); }
		    config_iterator       config_end()         { return config_iterator(this->database.end()); }
		    const_config_iterator config_begin() const { return const_config_iterator(this->database.begin()); }
		    const_config_iterator config_end()   const { return const_config_iterator(this->database.end()); }

		    const_config_iterator cconfig_begin()      { return const_config_iterator(this->database.cbegin()); }
		    const_config_iterator cconfig_end()        { return const_config_iterator(this->database.cend()); }


		    // VALUE ITERATORS

		  public:

		    value_iterator       value_begin()       { return value_iterator(this->database.begin()); }
		    value_iterator       value_end()         { return value_iterator(this->database.end()); }
		    const_value_iterator value_begin() const { return const_value_iterator(this->database.begin()); }
		    const_value_iterator value_end()   const { return const_value_iterator(this->database.end()); }

		    const_value_iterator cvalue_begin()      { return const_value_iterator(this->database.cbegin()); }
		    const_value_iterator cvalue_end()        { return const_value_iterator(this->database.cend()); }


				// INTERFACE -- ADD AND LOOKUP RECORDS

		  public:

				//! add record to the database
				unsigned int add_record(double t, bool store=true);


		    // INTERNAL DATA


		  protected:

		    // DATABASE

		    //! database of k-configurations
		    database_type database;

		    //! serial number for next inserted item
		    unsigned int serial;

			};


		time_config_database::time_config_database()
			: serial(0)
			{
			}


		unsigned int time_config_database::add_record(double t, bool store)
			{
				time_config config;

				config.serial = this->serial++;
				config.t = t;

				this->database.emplace(config.serial, time_config_record(config, store));

				return(config.serial);
			}


  }   // namespace transport


#endif //__time_config_database_H_
