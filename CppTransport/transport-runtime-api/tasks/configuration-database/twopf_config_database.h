//
// Created by David Seery on 16/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __twopf_config_database_H_
#define __twopf_config_database_H_


#include <limits>
#include <map>

#include <math.h>
#include <assert.h>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/tasks/configuration-database/generic_iterator.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/sqlite3_utility.h"


namespace transport
  {

    //! database record for a twopf k-configuration
    class twopf_kconfig_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! build a twopf kconfiguration database record
        twopf_kconfig_record(twopf_kconfig& k, bool s);

        //! destructor is default
        ~twopf_kconfig_record() = default;


        // INTERFACE

      public:

        //! store background from this configuration?
        bool is_background_stored() const { return(this->store_background); }

        //! dereference to get k-config object
        twopf_kconfig& operator*() { return(this->record); }
        const twopf_kconfig& operator*() const { return(this->record); }

        //! provide member access into k-config object
        twopf_kconfig* operator->() { return(&this->record); }
        const twopf_kconfig* operator->() const { return(&this->record); }

        friend std::ostream& operator<<(std::ostream& out, const twopf_kconfig_record& obj);


        // INTERNAL DATA

      private:

        //! record
        twopf_kconfig record;

        //! store the background for this configuration?
        bool store_background;

      };


    twopf_kconfig_record::twopf_kconfig_record(twopf_kconfig& k, bool s)
      : record(k),
        store_background(s)
      {
      }


    std::ostream& operator<<(std::ostream& out, const twopf_kconfig_record& obj)
      {
        out << *obj;
        return(out);
      }


    namespace twopf_config_database_impl
	    {

        class ReverseKLookupComparator
	        {

          public:

		        // should return true if a < b
		        // elements considered equal if !(a<b) and !(b<a)
		        bool operator()(const double& a, const double& b) const
			        {
				        return((b-a)/std::abs(a) > CPPTRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION);
			        }

	        };

	    }


    //! database of twopf k-configurations
    class twopf_kconfig_database
      {

      private:

        //! alias for database data structure
        typedef std::map< unsigned int, twopf_kconfig_record > database_type;

		    //! alias for reverse index -- we want to be able to lookup elements in the database
		    //! by their k-value efficiently
		    //! To do that we use a map with a custom comparator, and use the k-value as a key
		    typedef std::map< double, database_type::iterator, twopf_config_database_impl::ReverseKLookupComparator > index_type;


        // RECORD VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_record_iterator<database_type, twopf_kconfig_record, false > record_iterator;
        typedef configuration_database::generic_record_iterator<database_type, twopf_kconfig_record, true >  const_record_iterator;


        // CONFIG VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_config_iterator<database_type, twopf_kconfig, false> config_iterator;
        typedef configuration_database::generic_config_iterator<database_type, twopf_kconfig, true>  const_config_iterator;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        twopf_kconfig_database(double cn);

        //! deserialization constructor
        twopf_kconfig_database(double cn, sqlite3* handle);

        //! destructor is default
        ~twopf_kconfig_database() = default;


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


        // INTERFACE -- GLOBAL OPERATIONS

      public:

        //! empty database
        void clear();

        //! get number of records in database
        size_t size() const { return(this->database.size()); }

		    //! is the database modified (in an unsaved state)?
		    bool is_modified() const { return(this->modified); }


        // INTERFACE -- ADD AND LOOKUP RECORDS

      public:

        //! add record to the database

		    //! The record shouldn't already exist. No checks are made to test for duplicates
        unsigned int add_record(double k_conventional);

        //! lookup record with a given serial number -- non const version
        record_iterator lookup(unsigned int serial);

        //! lookup record with a given serial number -- const version
        const_record_iterator lookup(unsigned int serial) const;

        //! check for existence of a record with a given conventionally-normalized k
        bool find(double k_conventional, twopf_kconfig_database::record_iterator&) const;


        // INTERFACE -- LOOKUP META-INFORMATION

      public:

        //! get largest conventionally-normalized k-number committed to the database
        double get_kmax_conventional() const { return(this->kmax_conventional); }
		    double get_kmax_comoving()     const { return(this->kmax_comoving); }

        //! get smallest conventionally-normalized k-number committed to the database
        double get_kmin_conventional() const { return(this->kmin_conventional); }
		    double get_kmin_comoving()     const { return(this->kmax_comoving); }


        // SERIALIZATION

      public:

        //! Write database out to SQLite
        void write(sqlite3* handle);


        // INTERNAL DATA


      protected:

        // DATABASE

        //! database of k-configurations
        database_type database;

		    //! index database by k-value
		    index_type index_on_k;

        //! serial number for next inserted item
        unsigned int serial;

		    //! is the database in a modified, unsaved state?
		    bool modified;

        //! keep track of whether the background has been stored
        bool store_background;


        // COMOVING NORMALIZATION

        //! normalization from conventional to comoving wavenumbers
        double comoving_normalization;


        // META-INFORMATION

        //! cache maximum stored wavenumber
        double kmax_conventional;
		    double kmax_comoving;

        //! cache minimum stored wavenumber
        double kmin_conventional;
				double kmin_comoving;

      };


    twopf_kconfig_database::twopf_kconfig_database(double cn)
      : comoving_normalization(cn),
        serial(0),
        kmax_conventional(-std::numeric_limits<double>::max()),
        kmin_conventional(std::numeric_limits<double>::max()),
        kmax_comoving(-std::numeric_limits<double>::max()),
        kmin_comoving(std::numeric_limits<double>::max()),
        store_background(true),
        modified(true)
      {
      }


    twopf_kconfig_database::twopf_kconfig_database(double cn, sqlite3* handle)
      : comoving_normalization(cn),
        serial(0),
        kmax_conventional(-std::numeric_limits<double>::max()),
        kmin_conventional(std::numeric_limits<double>::max()),
        kmax_comoving(-std::numeric_limits<double>::max()),
        kmin_comoving(std::numeric_limits<double>::max()),
        store_background(false),
        modified(false)
      {
        std::ostringstream query_stmt;

		    query_stmt
		      << "SELECT "
          << "twopf_kconfig.serial       AS serial, "
		      << "twopf_kconfig.conventional AS conventional, "
		      << "twopf_kconfig.comoving     AS comoving, "
		      << "twopf_kconfig.t_exit       AS t_exit, "
		      << "twopf_kconfig.store_bg     AS store_bg "
		      << "FROM twopf_kconfig;";

		    sqlite3_stmt* stmt;
        sqlite3_operations::check_stmt(handle, sqlite3_prepare_v2(handle, query_stmt.str().c_str(), query_stmt.str().length()+1, &stmt, nullptr));

        database.clear();

        int status;
		    while((status = sqlite3_step(stmt)) != SQLITE_DONE)
			    {
				    if(status == SQLITE_ROW)
					    {
				        twopf_kconfig config;

				        config.serial         = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
				        config.k_conventional = sqlite3_column_double(stmt, 1);
				        config.k_comoving     = sqlite3_column_double(stmt, 2);
						    config.t_exit         = sqlite3_column_double(stmt, 3);

				        if(config.serial+1 > serial)                        this->serial            = config.serial+1;
				        if(config.k_conventional > this->kmax_conventional) this->kmax_conventional = config.k_conventional;
				        if(config.k_conventional < this->kmin_conventional) this->kmin_conventional = config.k_conventional;
				        if(config.k_comoving > this->kmax_comoving)         this->kmax_comoving     = config.k_comoving;
				        if(config.k_comoving < this->kmin_comoving)         this->kmin_comoving     = config.k_comoving;

						    bool store = (sqlite3_column_int(stmt, 4) != 0);

				        std::pair<database_type::iterator, bool> emplaced_value = this->database.emplace(config.serial, twopf_kconfig_record(config, store));
						    assert(emplaced_value.second);

						    // insert index record
						    this->index_on_k.emplace(config.k_conventional, emplaced_value.first);
					    }
				    else
					    {
				        std::ostringstream msg;
				        msg << CPPTRANSPORT_TWOPF_DATABASE_READ_FAIL << status << ": " << sqlite3_errmsg(handle) << ")";
				        sqlite3_finalize(stmt);
				        throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
					    }
			    }

        sqlite3_operations::check_stmt(handle, sqlite3_finalize(stmt));
      }


    void twopf_kconfig_database::clear()
	    {
        this->database.clear();

        this->serial           = 0;
        this->store_background = true;
	    }


    unsigned int twopf_kconfig_database::add_record(double k_conventional)
      {
        // insert a record into the database
        twopf_kconfig config;

        config.serial         = this->serial++;
        config.k_conventional = k_conventional;
        config.k_comoving     = k_conventional * this->comoving_normalization;
		    config.t_exit         = 0.0;    // will be updated later

        if(config.k_conventional > this->kmax_conventional) this->kmax_conventional = config.k_conventional;
        if(config.k_conventional < this->kmin_conventional) this->kmin_conventional = config.k_conventional;
        if(config.k_comoving > this->kmax_comoving)         this->kmax_comoving     = config.k_comoving;
        if(config.k_comoving < this->kmin_comoving)         this->kmin_comoving     = config.k_comoving;

        std::pair<database_type::iterator, bool> emplaced_value = this->database.emplace(config.serial, twopf_kconfig_record(config, this->store_background));
        assert(emplaced_value.second);

        // insert index record
        this->index_on_k.emplace(config.k_conventional, emplaced_value.first);

        this->store_background = false;
		    this->modified = true;

        return(config.serial);
      }


    twopf_kconfig_database::record_iterator twopf_kconfig_database::lookup(unsigned int serial)
      {
        database_type::iterator t = this->database.find(serial);        // find has logarithmic complexity

		    return twopf_kconfig_database::record_iterator(t);
      }


    twopf_kconfig_database::const_record_iterator twopf_kconfig_database::lookup(unsigned int serial) const
	    {
        database_type::const_iterator t = this->database.find(serial);  // find has logarithmic complexity

        return twopf_kconfig_database::const_record_iterator(t);
	    }


    bool twopf_kconfig_database::find(double k_conventional, twopf_kconfig_database::record_iterator& rec) const
      {
        index_type::const_iterator t = this->index_on_k.find(k_conventional);   // find has logarithmic complexity

		    if(t != this->index_on_k.end())
			    {
				    rec = twopf_kconfig_database::record_iterator(t->second);
				    return(true);
			    }

		    return(false);
      }


    void twopf_kconfig_database::write(sqlite3* handle)
      {
        std::ostringstream create_stmt;

		    create_stmt
		      << "CREATE TABLE twopf_kconfig("
		      << "serial       INTEGER PRIMARY KEY, "
		      << "conventional DOUBLE, "
			    << "comoving     DOUBLE, "
          << "t_exit       DOUBLE, "
          << "store_bg     INTEGER);";

        sqlite3_operations::exec(handle, create_stmt.str(), CPPTRANSPORT_TWOPF_DATABASE_WRITE_FAIL);

        std::ostringstream insert_stmt;
		    insert_stmt << "INSERT INTO twopf_kconfig VALUES (@serial, @conventional, @comoving, @t_exit, @store_bg);";

		    sqlite3_stmt* stmt;
        sqlite3_operations::check_stmt(handle, sqlite3_prepare_v2(handle, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

        sqlite3_operations::exec(handle, "BEGIN TRANSACTION");

		    unsigned int count = 0;
        for(database_type::const_iterator t = this->database.begin(); t != this->database.end(); ++t, ++count)
          {
            assert(count == t->first);
            if(count != t->first) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, CPPTRANSPORT_TWOPF_DATABASE_OUT_OF_ORDER);

            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 1, t->second->serial));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 2, t->second->k_conventional));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 3, t->second->k_comoving));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 4, t->second->t_exit));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 5, t->second.is_background_stored()));

            sqlite3_operations::check_stmt(handle, sqlite3_step(stmt), CPPTRANSPORT_TWOPF_DATABASE_WRITE_FAIL, SQLITE_DONE);

            sqlite3_operations::check_stmt(handle, sqlite3_clear_bindings(stmt));
            sqlite3_operations::check_stmt(handle, sqlite3_reset(stmt));
          }

        sqlite3_operations::exec(handle, "END TRANSACTION");
        sqlite3_operations::check_stmt(handle, sqlite3_finalize(stmt));

		    this->modified = false;
      }


  }   // namespace transport


#endif //__twopf_config_database_H_
