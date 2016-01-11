//
// Created by David Seery on 16/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __threepf_config_database_H_
#define __threepf_config_database_H_


#include <map>

#include <assert.h>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/tasks/task_configurations.h"
#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#include "transport-runtime-api/tasks/configuration-database/generic_record_iterator.h"
#include "transport-runtime-api/tasks/configuration-database/generic_config_iterator.h"
#include "transport-runtime-api/tasks/configuration-database/generic_value_iterator.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/sqlite3_utility.h"

#include "boost/optional.hpp"


namespace transport
  {

    //! database record for a threepf k-configuration
    class threepf_kconfig_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! build a threepf kconfiguration database record
        threepf_kconfig_record(threepf_kconfig& k, bool b, bool k1, bool k2, bool k3);

        //! destructor is default
        ~threepf_kconfig_record() = default;


        // INTERFACE

      public:

        //! store background from this configuration?
        bool is_background_stored() const { return(this->store_background); }

        //! store k1 twopf from this configuration?
        bool is_twopf_k1_stored() const { return(this->store_twopf_k1); }

        //! store k2 twopf from this configuration?
        bool is_twopf_k2_stored() const { return(this->store_twopf_k2); }

        //! store k3 twopf from this configuration?
        bool is_twopf_k3_stored() const { return(this->store_twopf_k3); }

        //! dereference to get k-config object
        threepf_kconfig& operator*() { return(this->record); }
        const threepf_kconfig& operator*() const { return(this->record); }

        //! provide member access into k-config object
        threepf_kconfig* operator->() { return(&this->record); }
        const threepf_kconfig* operator->() const { return(&this->record); }


        // INTERNAL DATA

      private:

        //! record
        threepf_kconfig record;

        //! store the background for this configuration?
        bool store_background;

        //! store the k1 twopf for this configuration?
        bool store_twopf_k1;

        //! store the k2 twopf for this configuration?
        bool store_twopf_k2;

        //! store the k3 twopf for this configuration
        bool store_twopf_k3;

      };


    threepf_kconfig_record::threepf_kconfig_record(threepf_kconfig& k, bool b, bool k1, bool k2, bool k3)
      : record(k),
        store_background(b),
        store_twopf_k1(k1),
        store_twopf_k2(k2),
        store_twopf_k3(k3)
      {
      }


    template <typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const threepf_kconfig_record& obj)
      {
        out << *obj;
        return(out);
      }


    //! database of threepf k-configurations
    class threepf_kconfig_database
      {

      private:

        //! alias for database data structure
        typedef std::map< unsigned int, threepf_kconfig_record > database_type;


        // RECORD VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_record_iterator< database_type::iterator, database_type::const_iterator, threepf_kconfig_record, false > record_iterator;
        typedef configuration_database::generic_record_iterator< database_type::iterator, database_type::const_iterator, threepf_kconfig_record, true >  const_record_iterator;

        typedef configuration_database::generic_record_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, threepf_kconfig_record, false > reverse_record_iterator;
        typedef configuration_database::generic_record_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, threepf_kconfig_record, true >  const_reverse_record_iterator;


        // CONFIG VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_config_iterator< database_type::iterator, database_type::const_iterator, threepf_kconfig, false> config_iterator;
        typedef configuration_database::generic_config_iterator< database_type::iterator, database_type::const_iterator, threepf_kconfig, true>  const_config_iterator;

        typedef configuration_database::generic_config_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, threepf_kconfig, false> reverse_config_iterator;
        typedef configuration_database::generic_config_iterator< database_type::reverse_iterator, database_type::const_reverse_iterator, threepf_kconfig, true>  const_reverse_config_iterator;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        threepf_kconfig_database(double cn);

        //! deserialization constructor
        threepf_kconfig_database(double cn, sqlite3* handle, twopf_kconfig_database& twopf_db);

        //! destructor is default
        ~threepf_kconfig_database() = default;


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
        const_reverse_record_iterator record_rbegin()  const { return const_reverse_record_iterator(this->database.rbegin()); }
        const_reverse_record_iterator record_rend()    const { return const_reverse_record_iterator(this->database.rend()); }

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


        // INTERFACE -- GLOBAL OPERATIONS

      public:

        //! empty database
        void clear();

        //! get number of records in database
        size_t size() const { return(this->database.size()); }

		    //! is the database in a modified (ie. unsaved) state?
		    bool is_modified() const { return(this->modified); }


        // INTERFACE -- ADD AND LOOKUP RECORDS

      public:

        //! add record to the database -- specified by wavenumber on each leg
        template <typename StoragePolicy>
        boost::optional<unsigned int> add_k1k2k3_record(twopf_kconfig_database& twopf_db, double k1_conventional, double k2_conventional, double k3_conventional, StoragePolicy policy);

        //! add record to the database -- specified by Fergusson-Shellard-Liguori parametrization
        template <typename StoragePolicy>
        boost::optional<unsigned int> add_FLS_record(twopf_kconfig_database& twopf_db, double kt_conventional, double alpha, double beta, StoragePolicy policy);

        //! add a record to the database -- directly specified;
        //! returns serial number of stored object
        template <typename StoragePolicy>
        boost::optional<unsigned int> add_record(twopf_kconfig_database& twopf_db, threepf_kconfig config, StoragePolicy policy);

		    //! lookup record with a given serial number -- non const version
		    record_iterator lookup(unsigned int serial);

		    //! lookup record with a given serial number -- const version
		    const_record_iterator lookup(unsigned int serial) const;


        // INTERFACE -- LOOKUP META-INFORMATION

      public:

        //! get largest conventionally-normalized k-number committed to the database
        double get_kmax_conventional() const { return(this->kmax_conventional); }
        double get_kmax_comoving()     const { return(this->kmax_comoving); }

        //! get smallest conventionally-normalized k-number committed to the database
        double get_kmin_conventional() const { return(this->kmin_conventional); }
        double get_kmin_comoving()     const { return(this->kmax_comoving); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void write(sqlite3* handle);


        // INTERNAL DATA


      protected:


        // DATABASE

        //! database of k-configurations
        database_type database;

        //! serial number for next inserted item
        unsigned int serial;

		    //! is the database in a modified (unsaved) state?
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


    threepf_kconfig_database::threepf_kconfig_database(double cn)
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


    threepf_kconfig_database::threepf_kconfig_database(double cn, sqlite3* handle, twopf_kconfig_database& twopf_db)
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
	        << "threepf_kconfig.serial          AS serial, "
	        << "threepf_kconfig.kt_conventional AS kt_conventional, "
	        << "threepf_kconfig.kt_comoving     AS kt_comoving, "
	        << "threepf_kconfig.alpha           AS alpha, "
	        << "threepf_kconfig.beta            AS beta, "
	        << "threepf_kconfig.wavenumber1     AS wavenumber1, "
	        << "threepf_kconfig.wavenumber2     AS wavenumber2, "
	        << "threepf_kconfig.wavenumber3     AS wavenumber3, "
	        << "threepf_kconfig.t_exit          AS t_exit, "
	        << "threepf_kconfig.store_bg        AS store_bg, "
	        << "threepf_kconfig.store_k1        AS store_k1, "
	        << "threepf_kconfig.store_k2        AS store_k2, "
	        << "threepf_kconfig.store_k3        AS store_k3 "
	        << "FROM threepf_kconfig;";

        sqlite3_stmt* stmt;
        sqlite3_operations::check_stmt(handle, sqlite3_prepare_v2(handle, query_stmt.str().c_str(), query_stmt.str().length()+1, &stmt, nullptr));

        database.clear();

        int status;
        while((status = sqlite3_step(stmt)) != SQLITE_DONE)
	        {
            if(status == SQLITE_ROW)
	            {
		            threepf_kconfig config;

                config.serial          = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                config.kt_conventional = sqlite3_column_double(stmt, 1);
                config.kt_comoving     = sqlite3_column_double(stmt, 2);
                config.alpha           = sqlite3_column_double(stmt, 3);
                config.beta            = sqlite3_column_double(stmt, 4);
		            config.t_exit          = sqlite3_column_double(stmt, 8);

		            config.k1_serial = static_cast<unsigned int>(sqlite3_column_int(stmt, 5));
		            config.k2_serial = static_cast<unsigned int>(sqlite3_column_int(stmt, 6));
		            config.k3_serial = static_cast<unsigned int>(sqlite3_column_int(stmt, 7));

				        twopf_kconfig_database::const_record_iterator k1 = twopf_db.lookup(config.k1_serial);
		            twopf_kconfig_database::const_record_iterator k2 = twopf_db.lookup(config.k2_serial);
		            twopf_kconfig_database::const_record_iterator k3 = twopf_db.lookup(config.k3_serial);

		            config.k1_conventional = (*k1)->k_conventional;
		            config.k1_comoving     = (*k1)->k_comoving;

		            config.k2_conventional = (*k2)->k_conventional;
		            config.k2_comoving     = (*k2)->k_comoving;

		            config.k3_conventional = (*k3)->k_conventional;
		            config.k3_comoving     = (*k3)->k_comoving;

		            if(config.serial+1 > serial)                         this->serial            = config.serial+1;
		            if(config.kt_conventional > this->kmax_conventional) this->kmax_conventional = config.kt_conventional;
		            if(config.kt_conventional < this->kmin_conventional) this->kmin_conventional = config.kt_conventional;
		            if(config.kt_comoving > this->kmax_comoving)         this->kmax_comoving     = config.kt_comoving;
		            if(config.kt_comoving < this->kmin_comoving)         this->kmin_comoving     = config.kt_comoving;

		            bool store_bg = (sqlite3_column_int(stmt, 9) > 0);
		            bool store_k1 = (sqlite3_column_int(stmt, 10) > 0);
		            bool store_k2 = (sqlite3_column_int(stmt, 11) > 0);
		            bool store_k3 = (sqlite3_column_int(stmt, 12) > 0);
		            this->database.emplace(config.serial, threepf_kconfig_record(config, store_bg, store_k1, store_k2, store_k3));
	            }
            else
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_THREEPF_DATABASE_READ_FAIL << status << ": " << sqlite3_errmsg(handle) << ")";
                sqlite3_finalize(stmt);
                throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
	            }
          }

        sqlite3_operations::check_stmt(handle, sqlite3_finalize(stmt));
      }


		void threepf_kconfig_database::clear()
			{
		    this->database.clear();

		    this->serial           = 0;
		    this->store_background = true;
			}


    template <typename StoragePolicy>
    boost::optional<unsigned int> threepf_kconfig_database::add_k1k2k3_record(twopf_kconfig_database& twopf_db, double k1_conventional, double k2_conventional, double k3_conventional, StoragePolicy policy)
      {
        // insert a record into the database
        threepf_kconfig config;

        config.serial = 0;
        config.k1_serial = config.k2_serial = config.k3_serial = 0;

        config.k1_conventional  = k1_conventional;
        config.k2_conventional  = k2_conventional;
        config.k3_conventional  = k3_conventional;

        config.k1_comoving      = k1_conventional * this->comoving_normalization;
        config.k2_comoving      = k2_conventional * this->comoving_normalization;
        config.k3_comoving      = k3_conventional * this->comoving_normalization;

        config.kt_conventional  = k1_conventional + k2_conventional + k3_conventional;
        config.kt_comoving = config.kt_conventional * this->comoving_normalization;
        config.beta             = 1.0 - 2.0 * k3_conventional / config.kt_conventional;
        config.alpha            = 4.0 * k2_conventional / config.kt_conventional - 1.0 - config.beta;

        config.t_exit           = 0.0; // this will be updated later, once all k-configurations are known

        return(this->add_record(twopf_db, config, policy));
      }


    template <typename StoragePolicy>
    boost::optional<unsigned int> threepf_kconfig_database::add_FLS_record(twopf_kconfig_database& twopf_db, double kt_conventional, double alpha, double beta, StoragePolicy policy)
      {
        // insert a record into the database
        threepf_kconfig config;

        config.serial = 0;
        config.k1_serial = config.k2_serial = config.k3_serial = 0;

        config.kt_conventional = kt_conventional;
        config.kt_comoving     = kt_conventional * this->comoving_normalization;
        config.alpha           = alpha;
        config.beta            = beta;

        config.k1_conventional  = (kt_conventional / 4.0) * (1.0 + alpha + beta);
        config.k1_comoving      = config.k1_conventional * this->comoving_normalization;

        config.k2_conventional  = (kt_conventional / 4.0) * (1.0 - alpha + beta);
        config.k2_comoving      = config.k2_conventional * this->comoving_normalization;

        config.k3_conventional  = (kt_conventional / 2.0) * (1.0 - beta);
        config.k3_comoving      = config.k3_conventional * this->comoving_normalization;

		    config.t_exit           = 0.0; // this will be updated later, once all k-configurations are known

        return(this->add_record(twopf_db, config, policy));
      }


    template <typename StoragePolicy>
    boost::optional<unsigned int> threepf_kconfig_database::add_record(twopf_kconfig_database& twopf_db, threepf_kconfig config, StoragePolicy policy)
      {
        // populate serial numbers in configuration record before
        // passing to storage policy
        config.serial = this->serial++;

        bool k1_stored;
        bool k2_stored;
        bool k3_stored;

        // perform reverse-lookup to find whether twopf kconfig database records already exist for these k_i
        twopf_kconfig_database::record_iterator rec;
        k1_stored = twopf_db.find(config.k1_conventional, rec);
        if(k1_stored) config.k1_serial = (*rec)->serial;
        else          config.k1_serial = twopf_db.add_record(config.k1_conventional);

        k2_stored = twopf_db.find(config.k2_conventional, rec);
        if(k2_stored) config.k2_serial = (*rec)->serial;
        else          config.k2_serial = twopf_db.add_record(config.k2_conventional);

        k3_stored = twopf_db.find(config.k3_conventional, rec);
        if(k3_stored) config.k3_serial = (*rec)->serial;
        else          config.k3_serial = twopf_db.add_record(config.k3_conventional);

        if(policy(config))   // policy confirms that this configuration should be retained
          {
            if(config.kt_conventional > this->kmax_conventional) this->kmax_conventional = config.kt_conventional;
            if(config.kt_conventional < this->kmin_conventional) this->kmin_conventional = config.kt_conventional;
            if(config.kt_comoving > this->kmax_comoving) this->kmax_comoving = config.kt_comoving;
            if(config.kt_comoving < this->kmin_comoving) this->kmin_comoving = config.kt_comoving;

            this->database.emplace(config.serial,
                                   threepf_kconfig_record(config, this->store_background, !k1_stored, !k2_stored,
                                                          !k3_stored));
            this->store_background = false;

            this->modified = true;

            return(config.serial);
          }

        // policy declined to store this configuration

        // unwind any twopf configurations added
        if(!k1_stored) twopf_db.delete_record(config.k1_serial);
        if(!k2_stored) twopf_db.delete_record(config.k2_serial);
        if(!k3_stored) twopf_db.delete_record(config.k3_serial);

        return boost::optional<unsigned int>();
      }


    void threepf_kconfig_database::write(sqlite3* handle)
      {
        std::ostringstream create_stmt;

		    create_stmt
		      << "CREATE TABLE threepf_kconfig("
		      << "serial          INTEGER PRIMARY KEY, "
		      << "kt_conventional DOUBLE, "
		      << "kt_comoving     DOUBLE, "
		      << "alpha           DOUBLE, "
		      << "beta            DOUBLE, "
		      << "wavenumber1     INTEGER, "
		      << "wavenumber2     INTEGER, "
		      << "wavenumber3     INTEGER, "
		      << "t_exit          DOUBLE, "
		      << "store_bg        INTEGER, "
		      << "store_k1        INTEGER, "
		      << "store_k2        INTEGER, "
		      << "store_k3        INTEGER, "
		      << "FOREIGN KEY(wavenumber1) REFERENCES twopf_kconfig(serial), "
		      << "FOREIGN KEY(wavenumber2) REFERENCES twopf_kconfig(serial), "
		      << "FOREIGN KEY(wavenumber3) REFERENCES twopf_kconfig(serial));";

        sqlite3_operations::exec(handle, create_stmt.str(), CPPTRANSPORT_THREEPF_DATABASE_WRITE_FAIL);

        std::ostringstream insert_stmt;
		    insert_stmt << "INSERT INTO threepf_kconfig VALUES (@serial, @kt_conventional, @kt_comoving, @alpha, @beta, @wavenumber1, @wavenumber2, @wavenumber3, @t_exit, @store_bg, @store_k1, @store_k2, @store_k3);";

        sqlite3_stmt* stmt;
        sqlite3_operations::check_stmt(handle, sqlite3_prepare_v2(handle, insert_stmt.str().c_str(), insert_stmt.str().length()+1, &stmt, nullptr));

        sqlite3_operations::exec(handle, "BEGIN TRANSACTION");

        for(database_type::const_iterator t = this->database.begin(); t != this->database.end(); ++t)
          {
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 1, t->second->serial));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 2, t->second->kt_conventional));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 3, t->second->kt_comoving));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 4, t->second->alpha));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 5, t->second->beta));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 6, t->second->k1_serial));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 7, t->second->k2_serial));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 8, t->second->k3_serial));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_double(stmt, 9, t->second->t_exit));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 10, t->second.is_background_stored()));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 11, t->second.is_twopf_k1_stored()));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 12, t->second.is_twopf_k2_stored()));
            sqlite3_operations::check_stmt(handle, sqlite3_bind_int(stmt, 13, t->second.is_twopf_k3_stored()));

            sqlite3_operations::check_stmt(handle, sqlite3_step(stmt), CPPTRANSPORT_THREEPF_DATABASE_WRITE_FAIL, SQLITE_DONE);

            sqlite3_operations::check_stmt(handle, sqlite3_clear_bindings(stmt));
            sqlite3_operations::check_stmt(handle, sqlite3_reset(stmt));
	        }

        sqlite3_operations::exec(handle, "END TRANSACTION");
        sqlite3_operations::check_stmt(handle, sqlite3_finalize(stmt));

		    this->modified = false;
      }


    threepf_kconfig_database::record_iterator threepf_kconfig_database::lookup(unsigned int serial)
	    {
        database_type::iterator t = this->database.find(serial);          // find has logarithmic complexity

        return threepf_kconfig_database::record_iterator(t);
	    }


    threepf_kconfig_database::const_record_iterator threepf_kconfig_database::lookup(unsigned int serial) const
	    {
        database_type::const_iterator t = this->database.find(serial);    // find has logarithmic complexity

        return threepf_kconfig_database::const_record_iterator(t);
	    }


  }   // namespace transport


#endif //__threepf_config_database_H_
