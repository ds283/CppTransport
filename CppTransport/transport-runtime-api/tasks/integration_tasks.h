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


// need repository to get repository::package_finder
#include "transport-runtime-api/manager/repository.h"

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/concepts/range.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_FAST_FORWARD                  "fast-forward"
#define __CPP_TRANSPORT_NODE_MESH_REFINEMENTS              "mesh-refinements"

#define __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE           "time-config-storage-policy"
#define __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN        "n"
#define __CPP_TRANSPORT_NODE_TIME_RANGE                    "time-config-range"

#define __CPP_TRANSPORT_NODE_KSTAR                         "kstar"
#define __CPP_TRANSPORT_NODE_PACKAGE_NAME                  "package"

#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE         "twopf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN      "n"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG      "bg"
#define __CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K       "k"

#define __CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE            "integrable"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE       "threepf-kconfig-storage-policy"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN    "n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN "k1n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN "k2n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN "k3n"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG    "bg"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1  "tpf1"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2  "tpf2"
#define __CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3  "tpf3"

#define __CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING         "k-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING        "kt-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING     "alpha-spacing"
#define __CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING      "beta-spacing"


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

        //! Construct an anonymized integration task with supplied initial conditions.
		    //! Anonymized tasks are used for things like constructing initial conditions,
		    //! integrating the background only, finding H at horizon-crossing etc.
		    //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, const range<double>& t)
          : integration_task(random_string(), i, t, default_time_config_storage_policy())
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

        //! Get model associated with this task
        model<number>* get_model() const { return(this->ics.get_model()); }

        //! Get initial times
        double get_Ninit() const { return(this->times.get_min()); }

        //! Get horizon-crossing time
        double get_Nstar() const { return(this->ics.get_Nstar()); }

        //! Get std::vector of integration steps
        const std::vector<double>& get_integration_step_times() const { return(this->raw_time_list); }

        //! Get vector of time configurations to store
        const std::vector<time_config>& get_time_config_list() const { return(this->time_config_list); }

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
        initial_conditions<number>       ics;

        //! Range of times at which to sample for this task;
				//! really kept only for serialization purposes
        range<double>                    times;

		    //! Time configuration storage policy for this task
		    const time_config_storage_policy time_storage_policy;

				//! Unfiltered list of time-vales
				std::vector<double>              raw_time_list;

				//! Filtered list of time-configurations (corresponding to values stored in the database)
				std::vector<time_config>         time_config_list;

		    //! Whether to use fast-forward integration
		    bool                             fast_forward;

		    //! How many mesh refinements to allow per triangle
		    unsigned int                     max_refinements;

      };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i,
                                               const range<double>& t, time_config_storage_policy p)
      : ics(i),
        times(t),
        task<number>(nm),
        time_storage_policy(p),
        fast_forward(true),
        max_refinements(__CPP_TRANSPORT_DEFAULT_MESH_REFINEMENTS)
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
				    if(time_storage_policy(data)) time_config_list.push_back(tc);
			    }
      }


		template <typename number>
		integration_task<number>::integration_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
			: time_storage_policy(integration_task<number>::default_time_config_storage_policy()),
        ics(i),
        task<number>(nm, reader)
			{
				assert(reader != nullptr);
		    if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

				// deserialize fast-forward integration setting
				reader->read_value(__CPP_TRANSPORT_NODE_FAST_FORWARD, fast_forward);

				// deserialize max number of mesh refinements
				reader->read_value(__CPP_TRANSPORT_NODE_MESH_REFINEMENTS, max_refinements);

        // deserialize range of sampling times
        reader->start_node(__CPP_TRANSPORT_NODE_TIME_RANGE);
        times = range<double>(reader);
        reader->end_element(__CPP_TRANSPORT_NODE_TIME_RANGE);

				// raw grid of sampling times can be reconstructed from the range object
				raw_time_list = times.get_grid();

				// filtered times have to be reconstructed from the database
				unsigned int num_configs = reader->start_array(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);
        time_config_list.clear();
        time_config_list.reserve(num_configs);

				for(unsigned int i = 0; i < num_configs; i++)
					{
						reader->start_array_element();

						unsigned int sn;
						reader->read_value(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN, sn);
						if(sn >= raw_time_list.size()) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_TIME_CONFIG_SN_TOO_BIG);

						time_config tc;
						tc.t = raw_time_list[sn];
						tc.serial = sn;
						time_config_list.push_back(tc);

            reader->end_array_element();
					}
				reader->end_element(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);

        // the integrator relies on the list of time configurations being in order
        struct TimeConfigSorter
          {
          public:
            bool operator() (const time_config& a, const time_config& b) { return(a.serial < b.serial); }
          };

        std::sort(time_config_list.begin(), time_config_list.end(), TimeConfigSorter());
			}


		template <typename number>
		void integration_task<number>::serialize(serialization_writer& writer) const
			{
        // store name of package
        writer.write_value(__CPP_TRANSPORT_NODE_PACKAGE_NAME, this->ics.get_name());

				// store fast-forward integration setting
				writer.write_value(__CPP_TRANSPORT_NODE_FAST_FORWARD, this->fast_forward);

				// store max number of mesh refinements
				writer.write_value(__CPP_TRANSPORT_NODE_MESH_REFINEMENTS, this->max_refinements);

        // serialize range of sampling times
        writer.start_node(__CPP_TRANSPORT_NODE_TIME_RANGE, false);
        this->times.serialize(writer);
        writer.end_element(__CPP_TRANSPORT_NODE_TIME_RANGE);

        // serialize filtered range of times, those which will be stored in the database
				writer.start_array(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE, this->time_config_list.size() == 0);
				for(std::vector<time_config>::const_iterator t = this->time_config_list.begin(); t != this->time_config_list.end(); t++)
					{
						writer.start_node("arrayelt", false);    // node name ignored in arrays
						writer.write_value(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE_SN, (*t).serial);
						writer.end_element("arrayelt");
					}
				writer.end_element(__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE);

        // note that we DO NOT serialize the initial conditions;
        // these are handled separately by the repository layer

				// call next serializers in the queue
				this->task<number>::serialize(writer);
			}


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj)
      {
        out << __CPP_TRANSPORT_FAST_FORWARD     << ": " << (obj.fast_forward ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << ", ";
        out << __CPP_TRANSPORT_MESH_REFINEMENTS << ": " << obj.max_refinements << std::endl;
        out << obj.ics << std::endl;
//        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
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
        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                        typename integration_task<number>::time_config_storage_policy p);

        //! deserialization constructor
        twopf_list_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);

        virtual ~twopf_list_task() = default;


        // INTERFACE

      public:

        //! Add a wavenumber to the list. The wavenumber should be conventionally normalized.
        //! Returns the serial number of the new configuration.
        unsigned int push_twopf_klist(double k, bool store=false);

        //! Convert a conventionally-normalized wavenumber to a comoving wavenumber
        double comoving_normalize(double k) const;

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<twopf_kconfig>& get_twopf_kconfig_list() const { return(this->twopf_config_list); }

        //! Search for a twopf kconfiguration by conventionally-normalized wavenumber.
        bool find_comoving_k(double k, unsigned int& serial) const;

        //! Look up a twopf k-configuration by serial number
        const twopf_kconfig& lookup_twopf_kconfig(unsigned int serial);


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
    twopf_list_task<number>::twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                             typename integration_task<number>::time_config_storage_policy p)
      : integration_task<number>(nm, i, t, p),
        serial(0)
      {
        // we can use 'this' here, because the integration-task components are guaranteed to be initialized
        // by the time the body of the constructor is executed
        model<number>* m = i.get_model();

        comoving_normalization = m->compute_kstar(this);
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
        twopf_config_list.clear();
        twopf_config_list.reserve(configs);

        for(unsigned int i = 0; i < configs; i++)
          {
            reader->start_array_element();

            twopf_kconfig c;
            reader->read_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN, c.serial);
            reader->read_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K, c.k_conventional);

            assert(c.k_conventional > 0.0);
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
        writer.write_value(__CPP_TRANSPORT_NODE_KSTAR, this->comoving_normalization);

        // serialize list of twopf kconfigurations
        // note we store only k_conventional to save space
        // k_comoving can be reconstructed on deserialization
        writer.start_array(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE, this->twopf_config_list.size()==0);
        for(std::vector<twopf_kconfig>::const_iterator t = this->twopf_config_list.begin(); t != this->twopf_config_list.end(); t++)
          {
            writer.start_node("arrayelt", false);    // node name ignored in arrays
            writer.write_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_SN, (*t).serial);
            writer.write_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_K, (*t).k_conventional);
            writer.write_value(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE_BG, (*t).store_background);
            writer.end_element("arrayelt");
          }
        writer.end_element(__CPP_TRANSPORT_NODE_TWOPF_KCONFIG_STORAGE);

        this->integration_task<number>::serialize(writer);
      }


    template <typename number>
    unsigned int twopf_list_task<number>::push_twopf_klist(double k, bool store)
      {
        assert(k > 0.0);

        twopf_kconfig c;

        c.serial = this->serial++;
        c.k = k*this->comoving_normalization;
        c.k_conventional = k;

        c.store_background = store;

        this->twopf_config_list.push_back(c);

        return(c.serial);
      }


    template <typename number>
    double twopf_list_task<number>::comoving_normalize(double k) const
      {
        return(k*this->comoving_normalization);
      }


    template <typename number>
    bool twopf_list_task<number>::find_comoving_k(double k, unsigned int& serial) const
      {
        bool rval = false;
        for(std::vector<twopf_kconfig>::const_iterator t = this->twopf_config_list.begin(); !rval && t != this->twopf_config_list.end(); t++)
          {
            if(fabs((*t).k_conventional - k) < __CPP_TRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION)
              {
                rval = true;
                serial = (*t).serial;
              }
          }

        return(rval);
      }


    template <typename number>
    const twopf_kconfig& twopf_list_task<number>::lookup_twopf_kconfig(unsigned int serial)
      {
        std::vector<twopf_kconfig>::const_iterator t;
        for(t = this->twopf_config_list.begin(); t != this->twopf_config_list.end(); t++)
          {
            if((*t).serial == serial) break;
          }

        if(t == this->twopf_config_list.end()) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TASK_TWOPF_CONFIG_SN_TOO_BIG);

        return(*t);
      }


    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
      {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named two-point function task with specified time-configuration storage policy
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   const range<double>& t, const range<double>& ks,
                   typename integration_task<number>::time_config_storage_policy p);

        //! Construct a named two-point function task with default storage policies
        twopf_task(const std::string& nm, const initial_conditions<number>& i,
                   const range<double>& t, const range<double>& ks)
          : twopf_task(nm, i, t, ks, typename integration_task<number>::default_time_config_storage_policy())
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

      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i,
                                   const range<double>& t, const range<double>& ks,
                                   typename integration_task<number>::time_config_storage_policy p)
      : twopf_list_task<number>(nm, i, t, p)
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
      }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF));

        this->twopf_list_task<number>::serialize(writer);
      }


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {

	      // THREEPF CONFIGURATION STORAGE POLICIES

      public:

		    //! defines a 'threepf-configuration storage policy' data object, passed to a policy specification
		    //! for the purpose of deciding whether a threepf-kconfiguration will be kept
		    class threepf_kconfig_storage_policy_data
			    {
		      public:
				    threepf_kconfig_storage_policy_data(double k, double a, double b, unsigned int s)
					    : k_t(k), alpha(a), beta(b), serial(s)
					    {
								k1 = (k_t/4.0)*(1.0 + alpha + beta);
						    k2 = (k_t/4.0)*(1.0 - alpha + beta);
						    k3 = (k_t/2.0)*(1.0 - beta);
					    }

		      public:
				    double k_t;
				    double alpha;
				    double beta;
				    double k1;
				    double k2;
				    double k3;
				    unsigned int serial;
			    };


		    //! defines a 'threepf-kconfiguration storage policy' object which determines which threepf-kconfigurations
		    //! are retained in the database
		    typedef std::function<bool(threepf_kconfig_storage_policy_data&)> threepf_kconfig_storage_policy;

      protected:

		    //! default threepf kconfig storage policy - store everything
		    class default_threepf_kconfig_storage_policy
			    {
		      public:
				    bool operator() (threepf_kconfig_storage_policy_data&) { return(true); }
			    };


		    // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a threepf-task
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     typename integration_task<number>::time_config_storage_policy p);

        //! deserialization constructor
        threepf_task(const std::string& n, serialization_reader* reader, const initial_conditions<number>& i);

        //! Destroy a three-point function task
        virtual ~threepf_task() = default;


        // INTERFACE

      public:

        //! Get list of k-configurations at which this task will sample the threepf
        const std::vector<threepf_kconfig>& get_threepf_kconfig_list() const { return(this->threepf_config_list); }

        //! Determine whether this task is integrable
        bool is_integrable() const { return(this->integrable); }

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const = 0;


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

        //! Is this threepf task integrable? ie., have we dropped any configurations, and is the spacing linear?
        bool integrable;

      };


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       typename integration_task<number>::time_config_storage_policy p)
      : twopf_list_task<number>(nm, i, t, p),
        serial(0), integrable(true)
      {
      }


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : twopf_list_task<number>(nm, reader, i),
        serial(0)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        //! deserialize integrable status
        reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE, integrable);

        //! deserialize array of k-configurations
        unsigned int configs = reader->start_array(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE);
        threepf_config_list.clear();
        threepf_config_list.reserve(configs);

        for(unsigned int i = 0; i < configs; i++)
          {
            reader->start_array_element();

            threepf_kconfig c;
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN, c.serial);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN, c.index[0]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN, c.index[1]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN, c.index[2]);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG, c.store_background);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1, c.store_twopf_k1);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2, c.store_twopf_k2);
            reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3, c.store_twopf_k3);

            const twopf_kconfig& k1 = this->lookup_twopf_kconfig(c.index[0]);
            const twopf_kconfig& k2 = this->lookup_twopf_kconfig(c.index[1]);
            const twopf_kconfig& k3 = this->lookup_twopf_kconfig(c.index[2]);

            c.k1 = k1.k;
            c.k2 = k2.k;
            c.k3 = k3.k;

            c.k_t   = k1.k + k2.k + k3.k;
            c.beta  = 1.0 - 2.0 * k3.k/(c.k_t);
            c.alpha = 4.0*k2.k/(c.k_t) - 1.0 - c.beta;

            c.k_t_conventional = k1.k_conventional + k2.k_conventional + k3.k_conventional;

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
        // serialize integrable status
        writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_INTEGRABLE, this->integrable);

        // serialize array of k-configurations
        writer.start_array(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE, this->threepf_config_list.size()==0);
        for(std::vector<threepf_kconfig>::const_iterator t = this->threepf_config_list.begin(); t != this->threepf_config_list.end(); t++)
          {
            writer.start_node("arrayelt", false);    // node name is ignored in arrays
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_SN, (*t).serial);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K1_SN, (*t).index[0]);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K2_SN, (*t).index[1]);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_K3_SN, (*t).index[2]);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_BG, (*t).store_background);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF1, (*t).store_twopf_k1);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF2, (*t).store_twopf_k2);
            writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE_TPF3, (*t).store_twopf_k3);
            writer.end_element("arrayelt");
          }
        writer.end_element(__CPP_TRANSPORT_NODE_THREEPF_KCONFIG_STORAGE);

        this->twopf_list_task<number>::serialize(writer);
      }


    template <typename number>
    class threepf_cubic_task: public threepf_task<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks,
        //! with specified policies
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                           const range<double>& t, const range<double>& ks,
                           typename integration_task<number>::time_config_storage_policy tp,
                           typename threepf_task<number>::threepf_kconfig_storage_policy kp);

        //! Construct a named three-point function task based on sampling from a cubix lattice of ks,
        //! with default policies
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                           const range<double>& ks)
	        : threepf_cubic_task(nm, i, t, ks,
	                             typename integration_task<number>::default_time_config_storage_policy(),
	                             typename threepf_task<number>::default_threepf_kconfig_storage_policy())
	        {
	        }

        //! Deserialization constructor
        threepf_cubic_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->spacing*this->spacing*this->spacing); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        double spacing;

      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                                                   const range<double>& t, const range<double>& ks,
                                                   typename integration_task<number>::time_config_storage_policy tp,
                                                   typename threepf_task<number>::threepf_kconfig_storage_policy kp)
	    : threepf_task<number>(nm, i, t, tp)
      {
        bool stored_background = false;

        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            for(unsigned int k = 0; k <= j; k++)
              {
                for(unsigned int l = 0; l <= k; l++)
                  {
                    auto maxij  = (ks[j] > ks[k] ? ks[j] : ks[k]);
                    auto maxijk = (maxij > ks[l] ? maxij : ks[l]);

                    if(ks[j] + ks[k] + ks[l] >= 2.0 * maxijk)   // impose the triangle conditions
                      {
                        threepf_kconfig kconfig;

                        kconfig.k1 = this->comoving_normalize(ks[j]);
                        kconfig.k2 = this->comoving_normalize(ks[k]);
                        kconfig.k3 = this->comoving_normalize(ks[l]);

                        kconfig.k_t   = this->comoving_normalize(ks[j] + ks[k] + ks[l]);
                        kconfig.beta  = 1.0 - 2.0 * ks[l] / (ks[j] + ks[k] + ks[l]);
                        kconfig.alpha = 4.0 * ks[j] / (ks[j] + ks[k] + ks[l]) - 1.0 - kconfig.beta;

                        kconfig.k_t_conventional = ks[j] + ks[k] + ks[l];

                        kconfig.serial = this->serial++;

		                    typename threepf_task<number>::threepf_kconfig_storage_policy_data data(kconfig.k_t_conventional, kconfig.alpha, kconfig.beta, kconfig.serial);
		                    if(kp(data))
			                    {
		                        // check whether any of these k-wavenumbers have been stored before
		                        bool stored;
		                        unsigned int sn;

		                        stored = this->find_comoving_k(ks[j], sn);
		                        if(stored) { kconfig.index[0] = sn; kconfig.store_twopf_k1 = false; }
		                        else { kconfig.index[0] = this->push_twopf_klist(ks[j]); kconfig.store_twopf_k1 = true; }

		                        stored = this->find_comoving_k(ks[k], sn);
		                        if(stored) { kconfig.index[1] = sn; kconfig.store_twopf_k2 = false; }
		                        else { kconfig.index[1] = this->push_twopf_klist(ks[k]); kconfig.store_twopf_k2 = true; }

		                        stored = this->find_comoving_k(ks[l], sn);
		                        if(stored) { kconfig.index[2] = sn; kconfig.store_twopf_k3 = false; }
		                        else { kconfig.index[2] = this->push_twopf_klist(ks[l]); kconfig.store_twopf_k3 = true; }

		                        kconfig.store_background = stored_background ? false : (stored_background=true);

		                        this->threepf_config_list.push_back(kconfig);
			                    }
		                    else this->integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                      }
                  }
              }
          }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);

        // need linear spacing to be integrable
        if(ks.get_spacing() != range<double>::linear) this->integrable = false;
        spacing = (ks.get_max() - ks.get_min())/ks.get_steps();
      }


    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : threepf_task<number>(nm, reader, i)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING, this->spacing);
      }


    template <typename number>
    void threepf_cubic_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC));

        writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_CUBIC_SPACING, this->spacing);

        this->threepf_task<number>::serialize(writer);
      }


    template <typename number>
    class threepf_fls_task: public threepf_task<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta,
        //! with specified storage policies
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                         const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                         typename integration_task<number>::time_config_storage_policy tp,
                         typename threepf_task<number>::threepf_kconfig_storage_policy kp);

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta,
        //! with default storage policies
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                         const range<double>& kts, const range<double>& alphas, const range<double>& betas)
          : threepf_fls_task(nm, i, t, kts, alphas, betas,
                             typename integration_task<number>::default_time_config_storage_policy(),
                             typename threepf_task<number>::default_threepf_kconfig_storage_policy())
          {
          }

        //! Deserialization construcitr
        threepf_fls_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->kt_spacing*this->alpha_spacing*this->beta_spacing); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual task<number>* clone() const override { return new threepf_fls_task<number>(static_cast<const threepf_fls_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! k_t spacing
        double kt_spacing;

        //! alpha spacing
        double alpha_spacing;

        //! beta spacing
        double beta_spacing;

      };


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                               const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                                               typename integration_task<number>::time_config_storage_policy tp,
                                               typename threepf_task<number>::threepf_kconfig_storage_policy kp)
	    : threepf_task<number>(nm, i, t, tp)
      {
        bool stored_background = false;

        for(unsigned int j = 0; j < kts.size(); j++)
          {
            for(unsigned int k = 0; k < alphas.size(); k++)
              {
                for(unsigned int l = 0; l < betas.size(); l++)
                  {
                    if(betas[l] >= 0.0 && betas[l] <= 1.0 && betas[l]-1.0 <= alphas[k] && alphas[k] <= 1.0-betas[l]  // impose triangle conditions
                       && alphas[k] >= 0.0 && betas[l] >= (1.0+alphas[k])/3.0)                                       // impose k1 >= k2 >= k3
                      {
                        threepf_kconfig kconfig;

                        kconfig.k_t    = this->comoving_normalize(kts[j]);
                        kconfig.alpha = alphas[k];
                        kconfig.beta  = betas[l];

                        kconfig.k_t_conventional = kts[j];

                        auto k1 = (kts[j]/4.0)*(1.0 + alphas[k] + betas[l]);
                        auto k2 = (kts[j]/4.0)*(1.0 - alphas[k] + betas[l]);
                        auto k3 = (kts[j]/2.0)*(1.0 - betas[l]);

                        kconfig.k1 = this->comoving_normalize(k1);
                        kconfig.k2 = this->comoving_normalize(k2);
                        kconfig.k3 = this->comoving_normalize(k3);

                        kconfig.serial = this->serial++;

                        typename threepf_task<number>::threepf_kconfig_storage_policy_data data(kconfig.k_t_conventional, kconfig.alpha, kconfig.beta, kconfig.serial);
                        if(k1 > 0.0 && k2 > 0.0 && k3 > 0.0 && kp(data))
	                        {
                            // check whether any of these k-wavenumbers have been stored before
                            bool stored;
                            unsigned int sn;

                            stored = this->find_comoving_k(k1, sn);
                            if(stored) { kconfig.index[0] = sn; kconfig.store_twopf_k1 = false; }
                            else { kconfig.index[0] = this->push_twopf_klist(k1); kconfig.store_twopf_k1 = true; }

                            stored = this->find_comoving_k(k2, sn);
                            if(stored) { kconfig.index[1] = sn; kconfig.store_twopf_k2= false; }
                            else { kconfig.index[1] = this->push_twopf_klist(k2); kconfig.store_twopf_k2 = true; }

                            stored = this->find_comoving_k(k3, sn);
                            if(stored) { kconfig.index[2] = sn; kconfig.store_twopf_k3 = false; }
                            else { kconfig.index[2] = this->push_twopf_klist(k3); kconfig.store_twopf_k3 = true; }

                            kconfig.store_background = stored_background ? false : (stored_background=true);

                            this->threepf_config_list.push_back(kconfig);
	                        }
                        else this->integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                      }
                  }
              }
          }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);

        // need linear spacing to be integrable
        if(kts.get_spacing() != range<double>::linear || alphas.get_spacing() != range<double>::linear || betas.get_spacing() != range<double>::linear) this->integrable = false;
        kt_spacing = (kts.get_max() - kts.get_min())/kts.get_steps();
        alpha_spacing = (alphas.get_max() - alphas.get_min())/alphas.get_steps();
        beta_spacing = (betas.get_max() - betas.get_min())/betas.get_steps();
      }


    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
      : threepf_task<number>(nm, reader, i)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING, kt_spacing);
        reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING, alpha_spacing);
        reader->read_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING, beta_spacing);
      }


    template <typename number>
    void threepf_fls_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_FLS));

        writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_KT_SPACING, this->kt_spacing);
        writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING, this->alpha_spacing);
        writer.write_value(__CPP_TRANSPORT_NODE_THREEPF_FLS_BETA_SPACING, this->beta_spacing);

        this->threepf_task<number>::serialize(writer);
      }


	}   // namespace transport


#endif //__integration_tasks_H_
