//
// Created by David Seery on 16/04/15.
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


#ifndef CPPTRANSPORT_TIME_CONFIG_DATABASE_H
#define CPPTRANSPORT_TIME_CONFIG_DATABASE_H


#include <functional>
#include <map>

#include <assert.h>

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/tasks/task_configurations.h"

#include "transport-runtime/tasks/configuration-database/generic_record_iterator.h"
#include "transport-runtime/tasks/configuration-database/generic_config_iterator.h"
#include "transport-runtime/tasks/configuration-database/generic_value_iterator.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


#define CPPTRANSPORT_TIME_DATABASE_LOWEST_SERIAL  (1)
#define CPPTRANSPORT_TIME_DATABASE_SPECIAL_SERIAL (0)


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


    template <typename Char, typename Traits>
		std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const time_config_record& obj)
			{
				out << *obj;
        return(out);
			}


		class time_config_database
			{

		  private:

				//! alias for database data structure
				using database_type = std::map< unsigned int, time_config_record >;


				// RECORD VALUED ITERATORS

		  public:

		    // specialize to obtain intended iterators
		    using record_iterator = configuration_database::generic_record_iterator< database_type::iterator, database_type::const_iterator, time_config_record, false>;
		    using const_record_iterator = configuration_database::generic_record_iterator< database_type::iterator, database_type::const_iterator, time_config_record, true>;

        using reverse_record_iterator = configuration_database::generic_record_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, time_config_record, false>;
        using const_reverse_record_iterator = configuration_database::generic_record_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, time_config_record, true>;


		    // CONFIG VALUED ITERATOR

		  public:

		    // specialize to obtain intended iterators
		    using config_iterator = configuration_database::generic_config_iterator< database_type::iterator, database_type::const_iterator, time_config, false>;
		    using const_config_iterator = configuration_database::generic_config_iterator< database_type::iterator, database_type::const_iterator, time_config, true>;

        using reverse_config_iterator = configuration_database::generic_config_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, time_config, false>;
        using const_reverse_config_iterator = configuration_database::generic_config_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, time_config, true>;


            // VALUE ITERATOR

		  public:

		    // specialize to obtain intended iterators
        using value_iterator = configuration_database::generic_value_iterator< database_type::iterator, database_type::const_iterator, double, false>;
        using const_value_iterator = configuration_database::generic_value_iterator< database_type::iterator, database_type::const_iterator, double, true>;

        using reverse_value_iterator = configuration_database::generic_value_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, double, false>;
        using const_reverse_value_iterator = configuration_database::generic_value_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, double, true>;


		    // CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor
				time_config_database();

				//! destructor
				~time_config_database() = default;


		    // RECORD ITERATORS

		  public:

		    record_iterator               record_begin()         { return record_iterator(this->database.begin()); }
		    record_iterator               record_end()           { return record_iterator(this->database.end()); }
		    const_record_iterator         record_begin()   const { return const_record_iterator(this->database.begin()); }
		    const_record_iterator         record_end()     const { return const_record_iterator(this->database.end()); }

		    const_record_iterator         record_cbegin()  const { return const_record_iterator(this->database.cbegin()); }
		    const_record_iterator         record_cend()    const { return const_record_iterator(this->database.cend()); }

        reverse_record_iterator       record_rbegin()        { return reverse_record_iterator(this->database.rbegin()); }
        reverse_record_iterator       record_rend()          { return reverse_record_iterator(this->database.rend()); }
        const_reverse_record_iterator record_rbegin()  const { return const_reverse_record_iterator(this->database.crbegin()); }
        const_reverse_record_iterator record_rend()    const { return const_reverse_record_iterator(this->database.crend()); }

        const_reverse_record_iterator record_crbegin() const { return const_reverse_record_iterator(this->database.crbegin()); }
        const_reverse_record_iterator record_crend()   const { return const_reverse_record_iterator(this->database.crend()); }


        // CONFIG ITERATORS

		  public:

		    config_iterator               config_begin()         { return config_iterator(this->database.begin()); }
		    config_iterator               config_end()           { return config_iterator(this->database.end()); }
		    const_config_iterator         config_begin()   const { return const_config_iterator(this->database.begin()); }
		    const_config_iterator         config_end()     const { return const_config_iterator(this->database.end()); }

		    const_config_iterator         config_cbegin()  const { return const_config_iterator(this->database.cbegin()); }
		    const_config_iterator         config_cend()    const { return const_config_iterator(this->database.cend()); }

        reverse_config_iterator       config_rbegin()        { return reverse_config_iterator(this->database.rbegin()); }
        reverse_config_iterator       config_rend()          { return reverse_config_iterator(this->database.rend()); }
        const_reverse_config_iterator config_rbegin()  const { return const_reverse_config_iterator(this->database.crbegin()); }
        const_reverse_config_iterator config_rend()    const { return const_reverse_config_iterator(this->database.crend()); }

        const_reverse_config_iterator config_crbegin() const { return const_reverse_config_iterator(this->database.crbegin()); }
        const_reverse_config_iterator config_crend()   const { return const_reverse_config_iterator(this->database.crend()); }


        // VALUE ITERATORS

		  public:

		    value_iterator               value_begin(double offset=0.0)        { return value_iterator(this->database.begin(), offset); }
		    value_iterator               value_end(double offset=0.0)          { return value_iterator(this->database.end(), offset); }
		    const_value_iterator         value_begin(double offset=0.0)  const { return const_value_iterator(this->database.begin(), offset); }
		    const_value_iterator         value_end(double offset=0.0)    const { return const_value_iterator(this->database.end(), offset); }

		    const_value_iterator         value_cbegin(double offset=0.0)       { return const_value_iterator(this->database.cbegin(), offset); }
		    const_value_iterator         value_cend(double offset=0.0)         { return const_value_iterator(this->database.cend(), offset); }

        reverse_value_iterator       value_rbegin(double offset=0.0)       { return reverse_value_iterator(this->database.rbegin(), offset); }
        reverse_value_iterator       value_rend(double offset=0.0)         { return reverse_value_iterator(this->database.rend(), offset); }
        const_reverse_value_iterator value_rbegin(double offset=0.0) const { return const_reverse_value_iterator(this->database.crbegin(), offset); }
        const_reverse_value_iterator value_rend(double offset=0.0)   const { return const_reverse_value_iterator(this->database.crend(), offset); }

        const_reverse_value_iterator value_crbegin(double offset=0.0)      { return const_reverse_value_iterator(this->database.crbegin(), offset); }
        const_reverse_value_iterator value_crend(double offset=0.0)        { return const_reverse_value_iterator(this->database.crend(), offset); }


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


#endif //CPPTRANSPORT_TIME_CONFIG_DATABASE_H
