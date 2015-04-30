//
// Created by David Seery on 16/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __threepf_config_database_H_
#define __threepf_config_database_H_


#include <map>

#include <assert.h>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/tasks/task_configurations.h"
#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"

#include "transport-runtime-api/tasks/configuration-database/generic_iterator.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_KSTAR            "kstar"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE            "database"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_K1_SERIAL        "1"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_K2_SERIAL        "2"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_K3_SERIAL        "3"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_BACKGROUND "b"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K1   "t1"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K2   "t2"
#define __CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K3   "t3"


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

        friend std::ostream& operator<<(std::ostream& out, const threepf_kconfig_record& obj);


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


    std::ostream& operator<<(std::ostream& out, const threepf_kconfig_record& obj)
      {
        out << *obj;
        return(out);
      }


    //! database of threepf k-configurations
    class threepf_kconfig_database: public serializable
      {

      private:

        //! alias for database data structure
        typedef std::map< unsigned int, threepf_kconfig_record > database_type;


        // RECORD VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_record_iterator<database_type, threepf_kconfig_record, false > record_iterator;
        typedef configuration_database::generic_record_iterator<database_type, threepf_kconfig_record, true >  const_record_iterator;


        // CONFIG VALUED ITERATOR

      public:

        // specialize to obtain intended iterators
        typedef configuration_database::generic_config_iterator<database_type, threepf_kconfig, false> config_iterator;
        typedef configuration_database::generic_config_iterator<database_type, threepf_kconfig, true>  const_config_iterator;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        threepf_kconfig_database(double cn);

        //! deserialization constructor
        threepf_kconfig_database(Json::Value& reader, twopf_kconfig_database& twopf_db);

        //! destructor is default
        ~threepf_kconfig_database() = default;


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

        //! add record to the database -- specified by wavenumber on each leg
        template <typename StoragePolicy>
        int add_k1k2k3_record(twopf_kconfig_database& twopf_db, double k1_conventional, double k2_conventional, double k3_conventional, StoragePolicy policy);

        //! add record to the database -- specified by Fergusson-Shellard-Liguori parametrization
        template <typename StoragePolicy>
        int add_FLS_record(twopf_kconfig_database& twopf_db, double kt_conventional, double alpha, double beta, StoragePolicy policy);

        //! add a record to the database -- directly specified
        template <typename StoragePolicy>
        int add_record(twopf_kconfig_database& twopf_db, threepf_kconfig config, StoragePolicy policy);

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
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA


      protected:


        // DATABASE

        //! database of k-configurations
        database_type database;

        //! serial number for next inserted item
        unsigned int serial;

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
        store_background(true)
      {
      }


    threepf_kconfig_database::threepf_kconfig_database(Json::Value& reader, twopf_kconfig_database& twopf_db)
      : serial(0),
        kmax_conventional(-std::numeric_limits<double>::max()),
        kmin_conventional(std::numeric_limits<double>::max()),
        kmax_comoving(-std::numeric_limits<double>::max()),
        kmin_comoving(std::numeric_limits<double>::max()),
        store_background(false)
      {
        // deserialize comoving normalization constant
        comoving_normalization = reader[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_KSTAR].asDouble();

        // deserialize database of threepf k-configurations
        Json::Value& db_array = reader[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE];
        assert(db_array.isArray());

        database.clear();
        for(Json::Value::iterator t = db_array.begin(); t != db_array.end(); ++t)
          {
            threepf_kconfig config;

            config.serial = serial++;
            config.k1_serial = (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K1_SERIAL].asUInt();
            config.k2_serial = (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K2_SERIAL].asUInt();
            config.k3_serial = (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K3_SERIAL].asUInt();

		        twopf_kconfig_database::const_record_iterator k1 = twopf_db.lookup(config.k1_serial);
            twopf_kconfig_database::const_record_iterator k2 = twopf_db.lookup(config.k2_serial);
            twopf_kconfig_database::const_record_iterator k3 = twopf_db.lookup(config.k3_serial);

            config.k1_conventional = (*k1)->k_conventional;
            config.k1_comoving     = (*k1)->k_comoving;

            config.k2_conventional = (*k2)->k_conventional;
            config.k2_comoving     = (*k2)->k_comoving;

            config.k3_conventional = (*k3)->k_conventional;
            config.k3_comoving     = (*k3)->k_comoving;

            config.kt_conventional = (*k1)->k_conventional + (*k2)->k_conventional + (*k3)->k_conventional;
            config.kt_comoving     = (*k1)->k_comoving + (*k2)->k_comoving + (*k3)->k_comoving;

            config.beta  = 1.0 - 2.0 * config.k3_conventional / config.kt_conventional;
            config.alpha = 4.0 * (*k2)->k_conventional / config.kt_conventional - 1.0 - config.beta;

		        config.t_exit = 0.0;  // will be updated later

            if(config.kt_conventional > this->kmax_conventional) this->kmax_conventional = config.kt_conventional;
            if(config.kt_conventional < this->kmin_conventional) this->kmin_conventional = config.kt_conventional;
            if(config.kt_comoving > this->kmax_comoving)         this->kmax_comoving     = config.kt_comoving;
            if(config.kt_comoving < this->kmin_comoving)         this->kmin_comoving     = config.kt_comoving;

            this->database.emplace(config.serial, threepf_kconfig_record(config,
                                                                           (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_BACKGROUND].asBool(),
                                                                           (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K1].asBool(),
                                                                           (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K2].asBool(),
                                                                           (*t)[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K3].asBool()));
          }
      }


		void threepf_kconfig_database::clear()
			{
		    this->database.clear();

		    this->serial           = 0;
		    this->store_background = true;
			}


    template <typename StoragePolicy>
    int threepf_kconfig_database::add_k1k2k3_record(twopf_kconfig_database& twopf_db, double k1_conventional, double k2_conventional, double k3_conventional, StoragePolicy policy)
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
    int threepf_kconfig_database::add_FLS_record(twopf_kconfig_database& twopf_db, double kt_conventional, double alpha, double beta, StoragePolicy policy)
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
    int threepf_kconfig_database::add_record(twopf_kconfig_database& twopf_db, threepf_kconfig config, StoragePolicy policy)
      {
        if(policy(config))
          {
            config.serial = this->serial++;

            bool k1_stored;
            bool k2_stored;
            bool k3_stored;

            k1_stored = twopf_db.find(config.k1_conventional, config.k1_serial);
            if(!k1_stored) config.k1_serial = twopf_db.add_record(config.k1_conventional);

            k2_stored = twopf_db.find(config.k2_conventional, config.k2_serial);
            if(!k2_stored) config.k2_serial = twopf_db.add_record(config.k2_conventional);

            k3_stored = twopf_db.find(config.k3_conventional, config.k3_serial);
            if(!k3_stored) config.k3_serial = twopf_db.add_record(config.k3_conventional);

            if(config.kt_conventional > this->kmax_conventional) this->kmax_conventional = config.kt_conventional;
            if(config.kt_conventional < this->kmin_conventional) this->kmin_conventional = config.kt_conventional;
            if(config.kt_comoving > this->kmax_comoving)         this->kmax_comoving     = config.kt_comoving;
            if(config.kt_comoving < this->kmin_comoving)         this->kmin_comoving     = config.kt_comoving;

            this->database.emplace(config.serial, threepf_kconfig_record(config, this->store_background, !k1_stored, !k2_stored, !k3_stored));
            this->store_background = false;
            return(config.serial);
          }

        return(-1);
      }


    void threepf_kconfig_database::serialize(Json::Value& writer) const
      {
        // serialize comoving normalization constant
        writer[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_KSTAR] = this->comoving_normalization;

        // serialize database of threepf configurations
        Json::Value db_array(Json::arrayValue);

        unsigned int count = 0;
        for(database_type::const_iterator t = this->database.begin(); t != this->database.end(); ++t, ++count)
          {
            assert(count == t->first);
            if(count != t->first) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_THREEPF_DATABASE_OUT_OF_ORDER);

            Json::Value record(Json::objectValue);
            record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K1_SERIAL] = t->second->k1_serial;
            record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K2_SERIAL] = t->second->k2_serial;
            record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_K3_SERIAL] = t->second->k3_serial;

            if(t->second.is_background_stored()) record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_BACKGROUND] = true;
            if(t->second.is_twopf_k1_stored())   record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K1]   = true;
            if(t->second.is_twopf_k2_stored())   record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K2]   = true;
            if(t->second.is_twopf_k3_stored())   record[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE_TWOPF_K3]   = true;

            db_array.append(record);
          }
        writer[__CPP_TRANSPORT_NODE_THREEPF_DATABASE_STORE] = db_array;
      }


		namespace threepf_kconfig_database_impl
			{

		    class FindBySerial
			    {
		      public:
		        FindBySerial(unsigned int s)
			        : serial(s)
			        {
			        }

		        bool operator()(const std::pair<unsigned int, threepf_kconfig_record>& a)
			        {
		            return(this->serial == a.second->serial);
			        }

		      private:
		        unsigned int serial;
			    };

			}


    threepf_kconfig_database::record_iterator threepf_kconfig_database::lookup(unsigned int serial)
	    {
        database_type::iterator t = std::find_if(this->database.begin(), this->database.end(), threepf_kconfig_database_impl::FindBySerial(serial));

        return threepf_kconfig_database::record_iterator(t);
	    }


    threepf_kconfig_database::const_record_iterator threepf_kconfig_database::lookup(unsigned int serial) const
	    {
        database_type::const_iterator t = std::find_if(this->database.begin(), this->database.end(), threepf_kconfig_database_impl::FindBySerial(serial));

        return threepf_kconfig_database::const_record_iterator(t);
	    }


  }   // namespace transport


#endif //__threepf_config_database_H_
