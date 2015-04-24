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


#define __CPP_TRANSPORT_NODE_TWOPF_DATABASE_KSTAR            "kstar"
#define __CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE            "database"
#define __CPP_TRANSPORT_NODE_TWOPF_DATABASE_K                "k"
#define __CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE_BACKGROUND "b"


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


    //! database of twopf k-configurations
    class twopf_kconfig_database: public serializable
      {

      private:

        //! alias for database data structure
        typedef std::map< unsigned int, twopf_kconfig_record > database_type;


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
        twopf_kconfig_database(Json::Value& reader);

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


        // INTERFACE -- ADD AND LOOKUP RECORDS

      public:

        //! add record to the database
        unsigned int add_record(double k_conventional);

        //! lookup record with a given serial number -- non const version
        record_iterator lookup(unsigned int serial);

        //! lookup record with a given serial number -- const version
        const_record_iterator lookup(unsigned int serial) const;

        //! check for existence of a record with a given conventionally-normalized k
        bool find(double k_conventional, unsigned int& serial) const;


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
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA


      protected:

        // DATABASE

        //! database of k-configurations
        database_type database;

        //! serial number for next inserted item
        unsigned int serial;


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

        //! keep track of whether the background has been stored
        bool store_background;

      };


    twopf_kconfig_database::twopf_kconfig_database(double cn)
      : comoving_normalization(cn),
        serial(0),
        kmax_conventional(-std::numeric_limits<double>::max()),
        kmin_conventional(std::numeric_limits<double>::max()),
        kmax_comoving(-std::numeric_limits<double>::max()),
        kmin_comoving(std::numeric_limits<double>::max()),
        store_background(true)
      {
      }


    twopf_kconfig_database::twopf_kconfig_database(Json::Value& reader)
      : serial(0),
        kmax_conventional(-std::numeric_limits<double>::max()),
        kmin_conventional(std::numeric_limits<double>::max()),
        kmax_comoving(-std::numeric_limits<double>::max()),
        kmin_comoving(std::numeric_limits<double>::max()),
        store_background(false)
      {
        // deserialize comoving normalization constant
        comoving_normalization = reader[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_KSTAR].asDouble();

        // deserialize database of twopf k-configurations
        Json::Value& db_array = reader[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE];
        assert(db_array.isArray());

        database.clear();
        for(Json::Value::iterator t = db_array.begin(); t != db_array.end(); ++t)
          {
            twopf_kconfig config;

            config.serial = serial++;
            config.k_conventional = (*t)[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_K].asDouble();
            config.k_comoving = config.k_conventional * this->comoving_normalization;

            if(config.k_conventional > this->kmax_conventional) this->kmax_conventional = config.k_conventional;
            if(config.k_conventional < this->kmin_conventional) this->kmin_conventional = config.k_conventional;
		        if(config.k_comoving > this->kmax_comoving)         this->kmax_comoving     = config.k_comoving;
		        if(config.k_comoving < this->kmin_comoving)         this->kmin_comoving     = config.k_comoving;

            this->database.emplace(config.serial, twopf_kconfig_record(config, (*t)[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE].asBool()));
          }
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
		    config.t_exit         = 0.0;    // will be reset later

        if(config.k_conventional > this->kmax_conventional) this->kmax_conventional = config.k_conventional;
        if(config.k_conventional < this->kmin_conventional) this->kmin_conventional = config.k_conventional;
        if(config.k_comoving > this->kmax_comoving)         this->kmax_comoving     = config.k_comoving;
        if(config.k_comoving < this->kmin_comoving)         this->kmin_comoving     = config.k_comoving;

        this->database.emplace(config.serial, twopf_kconfig_record(config, this->store_background));
        this->store_background = false;

        return(config.serial);
      }


    class FindBySerial
	    {
      public:
        FindBySerial(unsigned int s)
	        : serial(s)
	        {
	        }

        bool operator()(const std::pair<unsigned int, twopf_kconfig_record>& a)
	        {
            return(this->serial == a.second->serial);
	        }

      private:
        unsigned int serial;
	    };


    twopf_kconfig_database::record_iterator twopf_kconfig_database::lookup(unsigned int serial)
      {
        database_type::iterator t = std::find_if(this->database.begin(), this->database.end(), FindBySerial(serial));

		    return twopf_kconfig_database::record_iterator(t);
      }


    twopf_kconfig_database::const_record_iterator twopf_kconfig_database::lookup(unsigned int serial) const
	    {
        database_type::const_iterator t = std::find_if(this->database.begin(), this->database.end(), FindBySerial(serial));

        return twopf_kconfig_database::const_record_iterator(t);
	    }


    class FindByKConventional
	    {
      public:
        FindByKConventional(double k)
	        : k_conventional(k)
	        {
	        }

        bool operator()(const std::pair<unsigned int, twopf_kconfig_record>& a)
	        {
            return(fabs(this->k_conventional - a.second->k_conventional) < __CPP_TRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION);
	        }

      private:
        double k_conventional;
	    };


    bool twopf_kconfig_database::find(double k_conventional, unsigned int& serial) const
      {
        database_type::const_iterator t = std::find_if(this->database.begin(), this->database.end(), FindByKConventional(k_conventional));

        if(t != this->database.end())
          {
            serial = t->first;
            return(true);
          }

        return(false);
      }


    void twopf_kconfig_database::serialize(Json::Value& writer) const
      {
        // serialize comoving normalization constant
        writer[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_KSTAR] = this->comoving_normalization;

        // serialize database of twopf configurations
        Json::Value db_array(Json::arrayValue);

        unsigned int count = 0;
        for(database_type::const_iterator t = this->database.begin(); t != this->database.end(); ++t, ++count)
          {
            assert(count == t->first);
            if(count != t->first) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TWOPF_DATABASE_OUT_OF_ORDER);

            Json::Value record(Json::objectValue);
            record[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_K] = t->second->k_conventional;

            if(t->second.is_background_stored()) record[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE_BACKGROUND] = true;

            db_array.append(record);
          }
        writer[__CPP_TRANSPORT_NODE_TWOPF_DATABASE_STORE] = db_array;
      }


  }   // namespace transport


#endif //__twopf_config_database_H_
