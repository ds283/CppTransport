//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __integration_tasks_H_
#define __integration_tasks_H_


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>
#include <functional>


#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/concepts/range.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE           "time-config-storage-policy"
#define __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN        "n"
#define __CPP_TRANSPORT_NODE_TIME_RANGE                    "time-config-range"

#define __CPP_TRANSPORT_NODE_KSTAR                         "kstar"

#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE         "twopf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN      "n"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG      "bg"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K       "k"

#define __CPP_TRANSPORT_NODE_TWOPF_KRANGE                  "twopf-kconfig-range"

#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE       "threepf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN    "n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN "k1n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN "k2n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN "k3n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG    "bg"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF   "tpf"

#define __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE               "wavenumber-grid"
#define __CPP_TRANSPORT_VAL_THREEPF_CUBIC                  "cubic-lattice"
#define __CPP_TRANSPORT_VAL_THREEPF_FLS                    "fergusson-liguori-shellard"
#define __CPP_TRANSPORT_NODE_THREEPF_KRANGE                "k-range"
#define __CPP_TRANSPORT_NODE_THREEPF_KTRANGE               "kt-range"
#define __CPP_TRANSPORT_NODE_THREEPF_ARANGE                "alpha-range"
#define __CPP_TRANSPORT_NODE_THREEPF_BRANGE                "beta-range"


namespace transport
  {

    // TASK STRUCTURES -- INTEGRATION TASKS

    template <typename number> class integration_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj);

		//! An 'integration_task' is a specialization of 'task'. It contains the basic information
		//! needed to carry out an integration. The more specialized two- and three-pf integration
		//! tasks are derived from it, as is a concrete 'background' task object which is used
		//! internally for integrating the background only.

		//! An 'integration_task' contains information on the initial conditions, horizon-crossing
		//! time and sampling times.
    template <typename number>
    class integration_task: public task<number>
	    {

	      // TIME CONFIGURATION STORAGE POLICIES

      public:

        //! defines a 'time-configuration storage policy' data object, passed to a policy specification
		    //! for the purpose of deciding whether a time configuration will be kept
        class time_config_storage_policy_data
	        {
          public:
		        time_config_storage_policy_data(unsigned int s, double t)
			        : serial(s), time(t)
			        {
			        }

          public:
		        unsigned int serial;
		        double       time;
	        };

        //! defines a 'time-configuration storage policy' object which determines which time steps are retained in the database
        typedef std::function<bool(time_config_storage_policy_data&)> time_config_storage_policy;

		    //! defines an object which computes kstar for a given integration task
        typedef std::function<double(integration_task<number>*)> kconfig_kstar;


      protected:

		    //! default time configuration storage policy - store everything
		    class default_time_config_storage_policy
			    {
		      public:
				    bool operator() (time_config_storage_policy_data&) { return(true); }
			    };


        // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! Construct a named integration task with supplied initial conditions and time-configuration storage policy
		    integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, time_config_storage_policy p);

        //! Construct a named integration task with supplied initial conditions, but no storage policy; delegate to constructor
		    //! with specified storage policy, substituting the default storage policy
        integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
          : integration_task(nm, i, t, default_time_config_storage_policy())
	        {
	        }

        //! Construct an anonymized integration task with supplied initial conditions.
		    //! Anonymized tasks are used for things like constructing initial conditions,
		    //! integrating the background only, finding H at horizon-crossing etc.
		    //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, const range<double>& t)
          : integration_task(random_string(), i, t)
          {
          }

		    //! deserialization constructor
		    integration_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);

        //! Destroy an integration task
        virtual ~integration_task() = default;


        // INTERFACE - EXTRACT INFORMATION ABOUT THE TASK

      public:

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

//        //! Get 'range' object representing sample times
//        const range<double>& get_times() const { return(this->times); }

        //! Get initial times
        double get_Ninit() const { return(this->times.get_min()); }

        //! Get horizon-crossing time
        double get_Nstar() const { return(this->ics.get_Nstar()); }

        //! Get std::vector of sample times
        const std::vector<double>& get_time_config_sample() const { return(this->times.get_grid()); }

        //! Get std::vector of initial conditions
        const std::vector<number>& get_ics_vector() const { return(this->ics.get_vector()); }

        //! Get number of samples
        unsigned int get_num_time_config_samples() const { return(this->times.size()); }


				// SERIALIZE - implements a 'serializable' interface

		  public:

				//! serialize this object
				virtual void serialize(serialization_writer& writer) const override;



		    // WRITE TO STREAM

      public:

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const integration_task<number>& obj);


		    // INTERNAL DATA

      protected:

        //! Initial conditions for this task (including parameter choices)
        const initial_conditions<number> ics;

        //! Range of times at which to sample for this task;
				//! really kept only for serialization purposes
        range<double>                    times;

		    //! Time configuration storage policy for this task
		    const time_config_storage_policy time_storage_policy;

				//! Unfiltered list of time-vales
				std::vector<double>              raw_time_list;

				//! Filtered list of time-values (corresponding to values stored in the database)
				std::vector<double>              filtered_time_list;

				//! Filtered list of time-configurations (corresponding to values stored in the database)
				std::vector<time_config>         time_config_list;
      };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i,
                                               const range<double>& t, time_config_storage_policy p)
      : ics(i), times(t), task<number>(nm), time_storage_policy(p)
      {
        // validate relation between Nstar and the sampling time
        assert(times.get_steps() > 0);
        assert(i.get_Nstar() > times.get_min());
        assert(i.get_Nstar() < times.get_max());

        if(times.get_steps() == 0)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NO_TIMES;
            throw std::logic_error(msg.str());
          }

        if(times.get_min() >= ics.get_Nstar())
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_EARLY << " (Ninit = " << times.get_min() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
          }

        if(times.get_max() <= ics.get_Nstar())
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_LATE << " (Nend = " << times.get_max() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
          }

		    // get raw grid of sampling times
        raw_time_list = times.get_grid();

		    // filter sampling times according to our storage policy
		    for(unsigned int i = 0; i < raw_time_list.size(); i++)
			    {
				    time_config tc;
				    tc.t = raw_time_list[i];
				    tc.serial = i;

				    time_config_storage_policy_data data(tc.t, tc.serial);
				    if(time_storage_policy(data))
					    {
						    filtered_time_list.push_back(tc.t);
						    time_config_list.push_back(tc);
					    }
			    }
      }


		template <typename number>
		integration_task<number>::integration_task(const std::string& nm, serialization_reader* reader,
                                               const initial_conditions<number>& i)
			: time_storage_policy(integration_task<number>::default_time_config_storage_policy()),
        ics(i),
        task<number>(nm, reader)
			{
				assert(reader != nullptr);
		    if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        // deserialize range of sampling times
        reader->start_node(__CPP_TRANSPORT_NODE_TIME_RANGE);
        times = range<double>(reader);
        reader->end_element(__CPP_TRANSPORT_NODE_TIME_RANGE);

				// raw grid of sampling times can be reconstructed from the range object
				raw_time_list = times.get_grid();

				// filtered times have to be reconstructed from the database
				unsigned int sample_times = reader->start_array(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);
				for(unsigned int i = 0; i < sample_times; i++)
					{
						reader->start_array_element();

						unsigned int sn;
						reader->read_value(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN, sn);
						if(sn >= raw_time_list.size()) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_TIME_CONFIG_SN_TOO_BIG);

						time_config tc;
						tc.t = raw_time_list[sn];
						tc.serial = sn;
						filtered_time_list.push_back(tc.t);
						time_config_list.push_back(tc);
					}
				reader->end_element(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);
			}


		template <typename number>
		void integration_task<number>::serialize(serialization_writer& writer) const
			{
        // serialize range of sampling times
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TIME_RANGE, false);
        this->times.serialize(writer);
        this->end_element(writer, __CPP_TRANSPORT_NODE_TIME_RANGE);

        // serialize filtered range of times, those which will be stored in the database
				this->begin_array(writer, __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE, this->time_config_list.size() == 0);
				for(std::vector<time_config>::const_iterator t = this->time_config_list.begin(); t != this->time_config_list.end(); t++)
					{
						this->begin_node(writer, "arrayelt", false);    // node name ignored in arrays
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN, (*t).serial);
						this->end_element(writer, "arrayelt");
					}
				this->end_element(writer, __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);

        // note that we DO NOT serialize the initial conditions;
        // these are handled separately by the repository layer

				// call next serializers in the queue
				this->task<number>::serialize(writer);
			}


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj)
      {
        out << obj.ics << std::endl;
        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
        return(out);
      }


		//! A background task is a specialization of an integration task which is concrete.
		//! (The integration task class itself is abstract, because it delegates cloning and serialization
		//! to a concrete derived class.)
		//! It is used for integrating the background only, eg. when finding k* = H*
		//! or processing initial conditions
		template <typename number>
		class background_task: public integration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a background task
				background_task(const initial_conditions<number>& i, const range<double>& t)
		      : integration_task<number>(i, t)
					{
					}

				virtual ~background_task() = default;


		    // DISABLE SERIALIZATION

		  public:

		    //! Throw an exception if any attempt is made to serialize a background_task.
		    //! Only twopf and threepf integration tasks can be serialized.
        virtual void serialize(serialization_writer& writer) const override { throw std::runtime_error(__CPP_TRANSPORT_SERIALIZE_BACKGROUND_TASK); }


		    // CLONE

		  public:

		    //! Virtual copy
        virtual task<number>* clone() const override { return new background_task<number>(static_cast<const background_task<number>&>(*this)); }

			};


    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a flat vector of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public integration_task<number>
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a twopf-list-task object
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, typename integration_task<number>::kconfig_kstar kstar);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE

      public:

        //! Add a wavenumber to the list. The wavenumber should be conventionally normalized.
        void push_twopf_klist(double k, bool store=false);

        //! Convert a conventionally-normalized wavenumber to a comoving wavenumber
        double comoving_normalize(double k);

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<twopf_kconfig>& get_twopf_kconfig_list() const { return(this->twopf_config_list); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(serialization_writer& writer) const override;


        // INTERNAL DATA

      private:

        //! Normalization constant for comoving ks
        double comoving_normalization;

        //! List of twopf k-configurations associated with this task
        std::vector<twopf_kconfig> twopf_config_list;

        //! current serial number
        unsigned int serial;

      };


    template <typename number>
    twopf_list_task::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, typename integration_task<number>::kconfig_kstar kstar)
      : integration_task<number>(nm, i, t),
        serial(0)
      {
        // we can use 'this' here, because the integration-task components are guaranteed to be initialized
        // by the time the body of the constructor is executed
        comoving_normalization = kstar(this);
      }


    template <typename number>
    twopf_list_task<number>::twopf_list_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : integration_task<number>(nm, reader, i),
        serial(0)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        // deserialize comoving normalization constant
        reader->read_value(__CPP_TRANSPORT_NODE_KSTAR, comoving_normalization);

        // deserialize list of twopf k-configurations
        unsigned int configs = reader->start_array(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE);
        for(unsigned int i = 0; i < configs; i++)
          {
            reader->start_array_element();

            twopf_kconfig c;
            reader->read_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN, c.serial);
            reader->read_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K, c.k_conventional);
            c.k = c.k_conventional*comoving_normalization;
            reader->read_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG, c.store_background);

            twopf_config_list.push_back(c);
            serial++;

            reader->end_array_element();
          }
        reader->end_element(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE);

        // sort twopf_config_list into ascending serial-number order
        // this isn't absolutely required, because nothing in the integration step depends on
        // the twopf_kconfig list being in serial-number order.
        struct KConfigSorter
          {
            bool operator() (const twopf_kconfig& a, const twopf_kconfig& b) { return(a.serial < b.serial); }
          };

        std::sort(twopf_config_list.begin(), twopf_config_list.end(), KConfigSorter());
      }


    template <typename number>
    void twopf_list_task<number>::serialize(serialization_writer& writer) const
      {
        // serialize comoving normalization constant
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_KSTAR, this->comoving_normalization);

        // serialize list of twopf kconfigurations
        // note we store only k_conventional to save space
        // k_comoving can be reconstructed on deserialization
        this->begin_array(writer, __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE, this->twopf_config_list.size()==0);
        for(std::vector<twopf_kconfig>::const_iterator t = this->twopf_config_list.begin(); t != this->twopf_config_list.end(); t++)
          {
            this->begin_node(writer, "arrayelt", false);    // node name ignored in arrays
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN, (*t).serial);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K, (*t).k_conventional);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG, (*t).store_background);
            this->end_element(writer, "arrayelt");
          }
        this->end_element(writer, __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE);

        this->integration_task<number>::serialize(writer);
      }


    template <typename number>
    void twopf_list_task<number>::push_twopf_klist(double k, bool store)
      {
        twopf_kconfig c;

        c.serial = this->serial++;
        c.k = k*this->comoving_normalization;
        c.k_conventional = k;

        c.store_background = store;

        this->twopf_config_list.push_back(c);
      }


    template <typename number>
    double twopf_list_task<number>::comoving_normalize(double k)
      {
        return(k*this->comoving_normalization);
      }


    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
      {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named two-point function task
        twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized two-point function task
        twopf_task(const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
          : twopf_task(random_string(), i, t, ks, kstar)
          {
          }

        //! deserialization constructor
        twopf_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);

        //! Destroy a two-point function task
        ~twopf_task() = default;


        // INTERFACE

      public:

        //! Get list of k-configurations at which this task will sample the twopf
        const std::vector<twopf_kconfig>& get_sample() const { return(this->twopf_config_list); }

        //! Get the number of k-configurations at which this task will sample the twopf
        unsigned int get_number_kconfigs() const { return(this->twopf_config_list.size()); }


        // SERIALIZATION (implements a 'serializable' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual task<number>* clone() const override { return new twopf_task<number>(static_cast<const twopf_task<number>&>(*this)); }


		    // INTERNAL DATA

      protected:

        //! Original range of wavenumber ks used to produce this task. Retained for serialization to the repository
        range<double>              original_ks;

      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
      : original_ks(ks), twopf_list_task<number>(nm, i, t, kstar)
      {
        // the mapping from the provided list of ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            this->push_twopf_klist(ks[j], j==0);
          }
      }


    // deserializtaion constructor
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : twopf_list_task<number>(nm, reader, i)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        // deserialize range of ks
        reader->start_node(__CPP_TRANSPORT_NODE_TWOPF_KRANGE);
        original_ks = range<double>(reader);
        reader->end_element(__CPP_TRANSPORT_NODE_TWOPF_KRANGE);
      }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize(serialization_writer& writer) const
      {
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF));

        // serialize range of ks
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KRANGE, false);
        this->original_ks.serialize(writer);
        this->end_element(writer, __CPP_TRANSPORT_NODE_TWOPF_KRANGE);

        this->twopf_list_task<number>::serialize(writer);
      }


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {
      public:

        virtual ~threepf_task() = default;

        //! Construct a threepf-task
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     typename integration_task<number>::kconfig_kstar kstar);

        //! deserialization constructor
        threepf_task(const std::string& n, serialization_reader* reader, const initial_conditions<number>& i);

        //! Destroy a three-point function task
        virtual ~threepf_task() = default;


        // INTERFACE

      public:

        //! Get list of k-configurations at which this task will sample the threepf
        const std::vector<threepf_kconfig>& get_threepf_kconfig_list() const { return(this->threepf_config_list); }


        // SERIALIZATION -- implements a 'serialiazble' interface

      public:

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


		    // INTERNAL DATA

      protected:

        //! List of k-configurations associated with this task
        std::vector<threepf_kconfig> threepf_config_list;

        //! current serial number
        unsigned int serial;

      };


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       typename integration_task<number>::kconfig_kstar kstar)
      : twopf_list_task(nm, i, t, kstar),
        serial(0)
      {
      }


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : twopf_list_task(nm, reader, i),
        serial(0)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        unsigned int configs = reader->start_array(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE);
        for(unsigned int i = 0; i < configs; i++)
          {
            reader->start_array_element();

            threepf_kconfig c;
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN, c.serial);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN, c.index[0]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN, c.index[1]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN, c.index[2]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG, c.store_background);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF, c.store_twopf);

            threepf_config_list.push_back(c);
            serial++;

            reader->end_array_element();
          }
        reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE);

        // sort threepf_config_list into ascending serial-number order
        // this isn't absolutely required, because nothing in the integration step depends on it
        struct KConfigSorter
          {
            bool operator() (const threepf_kconfig& a, const threepf_kconfig& b) { return(a.serial < b.serial); }
          };

        std::sort(threepf_config_list.begin(), threepf_config_list.end(), KConfigSorter());
      }


    template <typename number>
    void threepf_task<number>::serialize(serialization_writer& writer) const
      {
        this->begin_array(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE, this->threepf_config_list.size()==0);
        for(std::vector<threepf_kconfig>::const_iterator t = this->threepf_config_list.begin(); t != this->threepf_config_list.end(); t++)
          {
            this->begin_node(writer, "arrayelt", false);    // node name is ignored in arrays
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN, (*t).serial);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN, (*t).index[0]);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN, (*t).index[1]);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN, (*t).index[2]);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG, (*t).store_background);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF, (*t).store_twopf);
            this->end_element(writer, "arrayelt");
          }
        this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE);
      }


    template <typename number>
    class threepf_cubic_task: public threepf_task<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling from a cubic lattice of ks
        threepf_cubic_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
          : threepf_cubic_task(random_string(), i, t, ks, kstar)
          {
          }


        // SERIALIZATION (implements a 'serialiazble' interface)

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! Original lattice of wavenumbers, retained for serialization
        const range<double> original_ks;

      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
      : original_ks(ks),
        threepf_task<number>(nm, i, t, kstar)
      {
        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        bool stored_background = false;

        unsigned int serial = 0;
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            bool stored_twopf = false;

            for(unsigned int k = 0; k <= j; k++)
              {
                for(unsigned int l = 0; l <= k; l++)
                  {
                    threepf_kconfig kconfig;

                    auto maxij  = (ks[j] > ks[k] ? ks[j] : ks[k]);
                    auto maxijk = (maxij > ks[l] ? maxij : ks[l]);

                    if(ks[j] + ks[k] + ks[l] >= 2.0 * maxijk)   // impose the triangle conditions
                      {
                        kconfig.k1  = this->comoving_normalize(ks[j]);
                        kconfig.k2  = this->comoving_normalize(ks[k]);
                        kconfig.k3  = this->comoving_normalize(ks[l]);

                        kconfig.k_t = this->comoving_normalize(ks[j] + ks[k] + ks[l]);
                        kconfig.beta  = 1.0 - 2.0 * ks[l] / (ks[j] + ks[k] + ks[l]);
                        kconfig.alpha = 4.0 * ks[j] / (ks[j] + ks[k] + ks[l]) - 1.0 - kconfig.beta;

                        kconfig.k_t_conventional = ks[j] + ks[k] + ks[l];

                        kconfig.index[0] = j;
                        kconfig.index[1] = k;
                        kconfig.index[2] = l;

                        kconfig.store_background = stored_background ? false : (stored_background = true);
                        kconfig.store_twopf      = stored_twopf      ? false : (stored_twopf = true);

                        kconfig.serial = serial++;

                        threepf_config_list.push_back(kconfig);
                      }
                  }
              }

            if(!stored_twopf) throw std::logic_error(__CPP_TRANSPORT_TWOPF_STORE);

            this->push_twopf_klist(ks[j]);
          }

        if(!stored_background) throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
      }


    template <typename number>
    class threepf_fls_task: public threepf_task<number>
      {

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling in FLS parameters
        threepf_fls_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename integration_task<number>::kconfig_kstar kstar)
          : threepf_fls_task(random_string(), i, t, kts, alphas, betas, kstar)
          {
          }


        // SERIALIZATION (implements a 'serialiazble' interface)

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:


        //! Original kt-grid, if used, retained for serialization
        const range<double> original_kts;

        //! Original alpha-grid, if used, retained for serialization
        const range<double> original_alphas;

        //! Original beta-grid, if used, retained for serialization
        const range<double> original_betas;

      };


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, const range<double>& kts, const range<double>& alphas, const range<double>& betas, typename integration_task<number>::kconfig_kstar kstar)
      : original_kts(kts), original_alphas(alphas), original_betas(betas),
        threepf_task<number>(nm, i, t, kstar)
      {
        assert(false);
      }


    // serialize a threepf task to the repository
    template <typename number>
    void threepf_task<number>::serialize(serialization_writer& writer) const
      {
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF));

        this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE, false);
        this->times.serialize(writer);
        this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE);

       switch(this->original_lattice)
          {
            case cubic_lattice:
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE, false,
                               __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE, __CPP_TRANSPORT_VAL_THREEPF_CUBIC);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KRANGE, false);
              this->original_ks.serialize(writer);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_KRANGE);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);
              break;

            case fergusson_liguori_shellard:
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE, false,
                               __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE, __CPP_TRANSPORT_VAL_THREEPF_FLS);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_KTRANGE, false);
              this->original_kts.serialize(writer);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_KTRANGE);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_ARANGE, false);
              this->original_alphas.serialize(writer);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_ARANGE);
              this->begin_node(writer, __CPP_TRANSPORT_NODE_THREEPF_BRANGE, false);
              this->original_betas.serialize(writer);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_BRANGE);
              this->end_element(writer, __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);
            break;

            default:
	            assert(false);
              throw std::runtime_error(__CPP_TRANSPORT_TASK_THREEPF_TYPE);
          }

        this->integration_task<number>::serialize(writer);
      }


    namespace
	    {

        namespace threepf_task_helper
	        {

            template <typename number>
            transport::threepf_task<number> deserialize(serialization_reader* reader, const std::string& name,
                                                        const transport::initial_conditions<number>& ics, typename transport::integration_task<number>::kconfig_kstar kstar)
	            {
                reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_TSAMPLE);
                reader->push_bookmark();
                transport::range<double> times = range::deserialize<double>(reader);
                reader->pop_bookmark();
                reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_TSAMPLE);

                reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);
                std::string sample_type;
		            reader->read_attribute(__CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE, sample_type);

								if(sample_type == __CPP_TRANSPORT_VAL_THREEPF_CUBIC)
									{
										reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_KRANGE);
										reader->push_bookmark();
								    transport::range<double> ks = range::deserialize<double>(reader);
								    reader->pop_bookmark();
								    reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_KRANGE);

										reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);

										return(transport::threepf_task<number>(name, ics, times, ks, kstar));
									}
		            else if(sample_type == __CPP_TRANSPORT_VAL_THREEPF_FLS)
									{
										reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_KTRANGE);
										reader->push_bookmark();
								    transport::range<double> kt = range::deserialize<double>(reader);
										reader->pop_bookmark();
										reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_KTRANGE);

										reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_ARANGE);
										reader->push_bookmark();
								    transport::range<double> alpha = range::deserialize<double>(reader);
										reader->pop_bookmark();
										reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_ARANGE);

										reader->start_node(__CPP_TRANSPORT_NODE_THREEPF_BRANGE);
										reader->push_bookmark();
								    transport::range<double> beta = range::deserialize<double>(reader);
										reader->pop_bookmark();
										reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_BRANGE);

										reader->end_element(__CPP_TRANSPORT_NODE_THREEPF_KSAMPLE);

										return(transport::threepf_task<number>(name, ics, times, kt, alpha, beta, kstar));
									}

		            assert(false);
								throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);
	            }

	        }

      }

	}   // namespace transport


#endif //__integration_tasks_H_
