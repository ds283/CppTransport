//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __task_H_
#define __task_H_


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>


#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/concepts/initial_conditions.h"
#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/concepts/range.h"
#include "transport-runtime-api/derived-products/derived_product.h"
#include "transport-runtime-api/tasks/task_k_configurations.h"

#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE    "twopf-sample-times"
#define __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE    "twopf-sample-wavenumbers"

#define __CPP_TRANSPORT_NODE_THREEPF_TSAMPLE  "threepf-sample-times"
#define __CPP_TRANSPORT_NODE_THREEPF_KSAMPLE  "threepf-sample-wavenumbers"
#define __CPP_TRANSPORT_ATTR_THREEPF_KSAMPLE  "wavenumber-grid"
#define __CPP_TRANSPORT_VAL_THREEPF_CUBIC     "cubic-lattice"
#define __CPP_TRANSPORT_VAL_THREEPF_FLS       "fergusson-liguori-shellard"
#define __CPP_TRANSPORT_NODE_THREEPF_KRANGE   "k-range"
#define __CPP_TRANSPORT_NODE_THREEPF_KTRANGE  "kt-range"
#define __CPP_TRANSPORT_NODE_THREEPF_ARANGE   "alpha-range"
#define __CPP_TRANSPORT_NODE_THREEPF_BRANGE   "beta-range"

#define __CPP_TRANSPORT_NODE_OUTPUT_NAME      "name"
#define __CPP_TRANSPORT_NODE_OUTPUT_ARRAY     "derived-data-tasks"
#define __CPP_TRANSPORT_NODE_OUTPUT_TASK      "task"
#define __CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT     "label"
#define __CPP_TRANSPORT_NODE_OUTPUT_SERIAL    "serial"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS "tags"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG  "tag"


namespace transport
  {

		// TASK STRUCTURES -- GENERIC

		//! A 'task' is the basic, abstract task from which all other types of task are derived.

		template <typename number>
		class task: public serializable
			{
		  public:

				// CONSTRUCTOR, DESTRUCTOR

				//! Construct a named task
				task(const std::string& nm)
					: name(nm)
					{
					}

				//! Destroy a task
				virtual ~task() = default;


				// INTERFACE

		    //! Get name
		    const std::string& get_name() const { return(this->name); }


				// CLONE

				//! Clone this task object using a virtual copy idiom.
				//! Given a pointer just to the base class task<>, it isn't possible
				//! to perform a deepy copy. This method avoids that problem.
				virtual task<number>* clone() const = 0;

				// INTERNAL DATA

		  protected:

		    //! Name of this task
		    const std::string name;
			};


    // TASK STRUCTURES -- INTEGRATION TASKS

    template <typename number> class integration_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj);

		//! An 'integration_task' is a specialization of 'task'. It contains the basic information
		//! needed to carry out an integration. The more specialized two- and three-pf integration
		//! tasks are derived from it.

		//! An 'integration_task' contains information on the initial conditions, horizon-crossing
		//! time and sampling times. It is enough to integrate the background.
    template <typename number>
    class integration_task: public task<number>
	    {
      public:

        typedef std::function<double(integration_task<number>*)> kconfig_kstar;

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named integration task with supplied initial conditions
        integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t);

        //! Construct an anonymized integration task with supplied initial conditions.
		    //! Anonymized tasks are used for things like constructing initial conditions,
		    //! integrating the background only, finding H at horizon-crossing etc.
		    //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, const range<double>& t)
          : integration_task(random_string(), i, t)
          {
          }

        //! Destroy an integration task
        virtual ~integration_task() = default;

        // EXTRACT INFORMATION ABOUT THE TASK

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

        //! Get 'range' object representing sample times
        const range<double>& get_times() const { return(this->times); }

        //! Get initial times
        double get_Ninit() const { return(this->times.get_min()); }

        //! Get horizon-crossing time
        double get_Nstar() const { return(this->ics.get_Nstar()); }

        //! Get std::vector of sample times
        const std::vector<double>& get_sample_times() const { return(this->times.get_grid()); }

        //! Get std::vector of initial conditions
        const std::vector<number>& get_initial_conditions() const { return(this->ics.get_vector()); }

        //! Get number of samples
        unsigned int get_N_sample_times() const { return(this->times.size()); }


        // DISABLE SERIALIZATION

		    //! Throw an exception if any attempt is made to serialize an integration_task.
		    //! Only twopf and threepf integration tasks can be serialized.
        virtual void serialize(serialization_writer& writer) const override { throw std::runtime_error(__CPP_TRANSPORT_SERIALIZE_BASE_TASK); }


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new integration_task<number>(static_cast<const integration_task<number>&>(*this)); }


        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const integration_task<number>& obj);

      protected:

        //! Initial conditions for this task (including parameter choices)
        const initial_conditions<number> ics;
        //! Range of times at which to sample for this task
        const range<double>              times;
      };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
      : ics(i), times(t), task<number>(nm)
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
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj)
      {
        out << obj.ics << std::endl;
        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
        return(out);
      }


    //! Base type for a task which can represent a set of two-point functions evaluated at different wavenumbers.
    //! Ultimately, all n-point-function integrations are of this type because they all solve for the two-point function
    //! even if the goal is to compute a higher n-point function.
    //! The key concept associated with a twopf_list_task is a flat vector of wavenumbers
    //! which describe the points at which we sample the twopf.
    template <typename number>
    class twopf_list_task: public integration_task<number>
      {
      public:

        twopf_list_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t)
          : comoving_normalization(0.0), normalization_set(false), integration_task<number>(nm, i, t)
          {
          }

        ~twopf_list_task() = default;

        //! Set comoving wavenumber normalization constant

        //! It is an error to try to set this twice, since it could lead to inconsistent results.
        //! If a second attempt is made to set the normalization, throws a RUNTIME_ERROR exception.
        void set_normalization(typename integration_task<number>::kconfig_kstar kstar)
          {
            if(this->normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_NORM);
            normalization_set = true;
            comoving_normalization = kstar(this);
          }

        //! Add a wavenumber to the list. The wavenumber should be conventionally normalized.

        //! Simultaneously pushes to the conventionally-normalized and comoving-normalized lists, so they
        //! are kept in sync. Requires normalization to have been set using set_normalization(),
        //! otherwise throws a RUNTIME_ERROR exception.
        void push_twopf_klist(double k)
          {
            if(!normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_UNSET);
            this->conventional_k.push_back(k);
            this->comoving_k.push_back(k*this->comoving_normalization);
          }

        //! Convert a conventionally-normalized wavenumber to a comoving wavenumber

        //! Requires normalization to have been set using set_normalization(),
        //! otherwise throws a RUNTIME_ERROR exception.
        double comoving_normalize(double k)
          {
            if(!normalization_set) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TWOPF_TASK_LIST_UNSET);
            return(k*this->comoving_normalization);
          }

        //! Get number of entries in the flat twopf k-list
        unsigned int get_k_list_size() const { return(this->conventional_k.size()); }

        //! Get flattened list of ks at which we sample the two-point function
        const std::vector<double>& get_k_list() const { return(this->conventional_k); }

        //! Get flattened list of comoving ks at which we sample the two-point function
        const std::vector<double>& get_k_list_comoving() const { return(this->comoving_k); }

        //! Get a conventionally normalized k-number identified by its index
        double get_k(unsigned int d) const
          {
            assert(d < this->conventional_k.size());
            if(d < this->conventional_k.size()) return(this->conventional_k[d]);
            else throw std::out_of_range(__CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE);
          }

        //! Get an comoving k-number identified by its index
        double get_k_comoving(unsigned int d) const
          {
            assert(d < this->comoving_k.size());
            if(d < this->comoving_k.size()) return(this->comoving_k[d]);
            else throw std::out_of_range(__CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE);
          }

        // ADD ITEMS TO THE NORMALIZED LIST

        // INTERNAL DATA

      private:

        //! Normalization constant for comoving ks
        double comoving_normalization;

        //! Has the normalization constant been set? This is a precondition for adding any wavenumbers of the list.
        //! It would be cleaner to pass the normalizaiton constant to the constructor,
        //! but to compute the normalization itself requires a properly-formed task object.
        //! Therefore we cannot do so until the base constructor has completed.
        //! This approach is a workaround.
        bool normalization_set;

        //! Flattened list of conventionally-normalized ks
        std::vector<double> conventional_k;

        //! Flattened list of comoving-normalized ks
        std::vector<double> comoving_k;
      };


    // two-point function task
    // we need to specify the wavenumbers k at which we want to sample it
    template <typename number>
    class twopf_task: public twopf_list_task<number>
      {
      public:
        //! Construct a named two-point function task
        twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized two-point function task
        twopf_task(const initial_conditions<number>& i, const range<double>& t,
                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
          : twopf_task(random_string(), i, t, ks, kstar)
          {
          }

        //! Destroy a two-point function task
        ~twopf_task() = default;

        //! Get list of k-configurations at which this task will sample the twopf
        const std::vector<twopf_kconfig>& get_sample() const { return(this->config_list); }

        //! Get the number of k-configurations at which this task will sample the twopf
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }


        // SERIALIZATION (implements a 'serializable' interface)

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new twopf_task<number>(static_cast<const twopf_task<number>&>(*this)); }


		    // INTERNAL DATA

      protected:

        //! List of k-configurations associated with this task
        std::vector<twopf_kconfig> config_list;

        //! Original range of wavenumber ks used to produce this task. Retained for serialization to the repository
        const range<double>        original_ks;
      };


    // build a twopf task
    template <typename number>
    twopf_task<number>::twopf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                   const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
      : original_ks(ks), twopf_list_task<number>(nm, i, t)
      {
        this->set_normalization(kstar);

        bool stored_background = false;
        // the mapping from the provided list ks to the work list is just one-to-one
        for(unsigned int j = 0; j < ks.size(); j++)
          {
            twopf_kconfig kconfig;

            kconfig.index  = j;
            kconfig.serial = j;
            kconfig.k      = this->comoving_normalize(ks[j]);

            kconfig.store_background = stored_background ? false : (stored_background = true);

            config_list.push_back(kconfig);

            this->push_twopf_klist(ks[j]);
          }

        if(!stored_background) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_BACKGROUND_STORE);
      }


    // serialize a twopf task to the repository
    template <typename number>
    void twopf_task<number>::serialize(serialization_writer& writer) const
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE, false);
        this->times.serialize(writer);
        this->end_element(writer, __CPP_TRANSPORT_NODE_TWOPF_TSAMPLE);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE, false);
        this->original_ks.serialize(writer);
        this->end_element(writer, __CPP_TRANSPORT_NODE_TWOPF_KSAMPLE);
      }


    namespace
	    {

        namespace twopf_task_helper
	        {

            template <typename number>
            transport::twopf_task<number> deserialize(serialization_reader* reader, const std::string& name,
                                                      const transport::initial_conditions<number>& ics, typename transport::integration_task<number>::kconfig_kstar kstar)
	            {
								reader->start_node(__CPP_TRANSPORT_NODE_TWOPF_TSAMPLE);
		            reader->push_bookmark();
                transport::range<double> times = range::deserialize<double>(reader);
		            reader->pop_bookmark();
		            reader->end_element(__CPP_TRANSPORT_NODE_TWOPF_TSAMPLE);

		            reader->start_node(__CPP_TRANSPORT_NODE_TWOPF_KSAMPLE);
		            reader->push_bookmark();
                transport::range<double> ks = range::deserialize<double>(reader);
		            reader->pop_bookmark();
		            reader->end_element(__CPP_TRANSPORT_NODE_TWOPF_KSAMPLE);

		            return transport::twopf_task<number>(name, ics, times, ks, kstar);
	            }

	        }

	    }   // unnamed namespace


    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
      {
      public:

        typedef enum { cubic_lattice, fergusson_liguori_shellard } wavenumber_grid_type;

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling from a cubic lattice of ks
        threepf_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
          : threepf_task(random_string(), i, t, ks, kstar)
          {
          }

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta
        threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename integration_task<number>::kconfig_kstar kstar);

        //! Construct an anonymized three-point function task based on sampling in FLS parameters
        threepf_task(const initial_conditions<number>& i, const range<double>& t,
                     const range<double>& kts, const range<double>& alphas, const range<double>& betas,
                     typename integration_task<number>::kconfig_kstar kstar)
          : threepf_task(random_string(), i, t, kts, alphas, betas, kstar)
          {
          }

        //! Destroy a three-point function task
        ~threepf_task() = default;

        //! Get list of k-configurations at which this task will sample the threepf
        const std::vector<threepf_kconfig>& get_sample() const { return(this->config_list); }

        //! Get the number of k-configurations at which this task will sample the threepf
        unsigned int get_number_kconfigs() const { return(this->config_list.size()); }


        // SERIALIZATION (implements a 'serialiazble' interface)

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new threepf_task<number>(static_cast<const threepf_task<number>&>(*this)); }


		    // INTERNAL DATA

      protected:
        //! List of k-configurations associated with this task
        std::vector<threepf_kconfig> config_list;

        //! Wavenumber lattice used to construct this task, retained for serialization
        const wavenumber_grid_type original_lattice;
        //! Original cubic lattice, if used, retained for serialization
        const range<double> original_ks;
        //! Original kt-grid, if used, retained for serialization
        const range<double> original_kts;
        //! Original alpha-grid, if used, retained for serialization
        const range<double> original_alphas;
        //! Original beta-grid, if used, retained for serialization
        const range<double> original_betas;
      };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t,
                                       const range<double>& ks, typename integration_task<number>::kconfig_kstar kstar)
      : original_lattice(cubic_lattice), original_ks(ks),
        original_kts(), original_alphas(), original_betas(),
        twopf_list_task<number>(nm, i, t)
      {
        this->set_normalization(kstar);

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

                        config_list.push_back(kconfig);
                      }
                  }
              }

            if(!stored_twopf) throw std::logic_error(__CPP_TRANSPORT_TWOPF_STORE);

            this->push_twopf_klist(ks[j]);
          }

        if(!stored_background) throw std::logic_error(__CPP_TRANSPORT_BACKGROUND_STORE);
      }


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, const range<double>& kts, const range<double>& alphas, const range<double>& betas, typename integration_task<number>::kconfig_kstar kstar)
      : original_lattice(fergusson_liguori_shellard), original_ks(),
        original_kts(kts), original_alphas(alphas), original_betas(betas),
        twopf_list_task<number>(nm, i, t)
      {
        assert(false);
      }


    // serialize a threepf task to the repository
    template <typename number>
    void threepf_task<number>::serialize(serialization_writer& writer) const
      {
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
              throw std::runtime_error(__CPP_TRANSPORT_TASK_THREEPF_TYPE);
          }
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

								throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_BADLY_FORMED_TASK);
	            }

	        }

      }


    // TASK STRUCTURES -- OUTPUT TASK

    // An output task is simply a list of derived products to be generated.
		// We can optionally enforce a set of metadata tags for the whole set of
		// content-providers which compose the product.

		template <typename number> class output_task_element;

		template <typename number>
		std::ostream& operator<<(std::ostream& out, const output_task_element<number>& obj);

    template <typename number>
    class output_task_element
	    {
      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct an output task element, given a derived product and a list of tags to match.
		    //! (Recall that the derived product knows which task it belongs to, so that information is
		    //! also implicitly available.)
        output_task_element(const derived_data::derived_product<number>& dp, const std::list<std::string>& tgs, unsigned int sn)
	        : product(dp.clone()), tags(tgs), serial(sn)
	        {
		        assert(product != nullptr);
	        }

		    //! Override the default copy constructor to perform a deep copy of the stored derived_product<>
		    output_task_element(const output_task_element<number>& obj)
		      : product(obj.product->clone()), tags(obj.tags), serial(obj.serial)
			    {
			    }

        //! Destroy an output task element
        ~output_task_element()
	        {
		        assert(this->product != nullptr);

            delete this->product;
	        }


        // INTERFACE - EXTRACT DETAILS

        //! Get name of derived product associated with this task element
        const std::string& get_product_name() const { return(this->product->get_name()); }

		    //! Get derived product associated with this task element
		    derived_data::derived_product<number>* get_product() const { return(this->product); }

        //! Get tags associated with this task element
        const std::list<std::string>& get_tags() const { return(this->tags); }

		    //! Get serial number of this element
		    unsigned int get_serial() const { return(this->serial); }


		    //! Write to a standard output stream
		    friend std::ostream& operator<< <>(std::ostream& out, const output_task_element<number>& obj);

        // INTERNAL DATA

      protected:

        //! Pointer to derived data product (part of the task description, specifying which eg. plot to produce) which which this output is associated
        derived_data::derived_product<number>* product;

        //! Optional list of tags to enforce for each content provider in the product
        std::list<std::string> tags;

		    //! Internal serial number
		    unsigned int serial;
	    };


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task_element<number>& obj)
	    {
        out << "  " << __CPP_TRANSPORT_OUTPUT_ELEMENT_OUTPUT << " " << obj.get_product_name() << ",";
        out << " " << __CPP_TRANSPORT_OUTPUT_ELEMENT_TAGS  << ": ";

        unsigned int count = 0;
        const std::list<std::string>& tags = obj.get_tags();
        for (std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); u++)
	        {
            if(count > 0) out << ", ";
            out << *u;
	        }

        return(out);
	    }


    template <typename number> class output_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task<number>& obj);

    //! An 'output_task' is a specialization of 'task' which generates a set of derived products.

    template <typename number>
    class output_task: public task<number>
	    {
      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct a named output task using a supplied list of elements

        output_task(const std::string& nm, typename std::vector< output_task_element<number> >& eles)
	        : serial(0), elements(eles), task<number>(nm)
	        {
		        serial = elements.size();
	        }

        //! Construct a named output task using a supplied single derived_product<> object.
        //! No tags provided.
        //! (Would be nice to delegate to the constructor which accepts tags, but delegation
        //! has to happen in the initializer list -- as far as I know)
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod)
	        : serial(0), task<number>(nm)
	        {
            elements.clear();

            // set up empty list of tags
            std::list<std::string> tags;

            elements.push_back(output_task_element<number>(prod, tags, serial++));
	        }

        //! Construct a named output task using a supplied single derived_product<> object.
        //! Tags provided.
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod, const std::list<std::string>& tags)
	        : serial(0), task<number>(nm)
	        {
            elements.clear();

            elements.push_back(output_task_element<number>(prod, tags, serial++));
	        }

        //! Destroy an output task
        ~output_task() = default;

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const output_task<number>& obj);


        // INTERFACE

        //! Obtain number of task elements
        unsigned int size() const { return(this->elements.size()); }

		    //! Obtain task elements
		    const typename std::vector< output_task_element<number> >& get_elements() const
			    {
				    return(this->elements);
			    }

        //! Obtain a specific element
        const output_task_element<number>& get(unsigned int i) const
	        {
            if(i >= this->elements.size())
	            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_OUTPUT_TASK_RANGE);

            return(this->elements[i]);
	        }

		    //! Add an element, no tags provided
		    void add_element(const derived_data::derived_product<number>& prod)
			    {
				    // set up empty list of tags
		        std::list<std::string> tags;

				    elements.push_back(output_task_element<number>(prod, tags, serial++));
			    }

		    //! Add an element, tags provided
		    void add_element(const derived_data::derived_product<number>& prod, const std::list<std::string>& tags)
			    {
				    elements.push_back(output_task_element<number>(prod, tags, serial++));
			    }

        // SERIALIZATION (implements a 'serializable' interface)

        //! Serialize this task to the repository
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

        //! Virtual copy
        virtual task<number>* clone() const override { return new output_task<number>(static_cast<const output_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! List of output elements which make up this task
        typename std::vector< output_task_element<number> > elements;

		    //! Serial number of added tasks
		    unsigned int serial;
	    };


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task<number>& obj)
	    {
        out << __CPP_TRANSPORT_OUTPUT_ELEMENTS << std::endl;
        for(typename std::vector< output_task_element<number> >::const_iterator t = obj.elements.begin(); t != obj.elements.end(); t++)
	        {
		        out << *t << std::endl;
            out << std::endl;
	        }

        return(out);
	    }


    // serialize an output task to the repository
    template <typename number>
    void output_task<number>::serialize(serialization_writer& writer) const
	    {
        this->begin_array(writer, __CPP_TRANSPORT_NODE_OUTPUT_ARRAY, this->elements.size() == 0);    // node name is actually ignored for an array

        for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); t++)
	        {
            this->begin_node(writer, "arrayelt", false);    // node name is ignored for arrays

            this->write_value_node(writer, __CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT, (*t).get_product_name());
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_OUTPUT_SERIAL, (*t).get_serial());

            const std::list<std::string>& tags = (*t).get_tags();

            this->begin_array(writer, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS, tags.size() == 0);      // node name is ignored for an array ...
            for(std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); u++)
	            {
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, *u);
	            }
            this->end_element(writer, __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

            this->end_element(writer, "arrayelt");
	        }

        this->end_element(writer, __CPP_TRANSPORT_NODE_OUTPUT_ARRAY);
	    }


    namespace output_task_helper
	    {


        template <typename number>
        struct derived_product_finder
	        {
            typedef typename std::function< transport::derived_data::derived_product<number>*(const std::string&) > type;
	        };


        template <typename number>
        transport::output_task<number> deserialize(serialization_reader* reader, const std::string& name,
                                                   typename derived_product_finder<number>::type pfinder)
	        {
            unsigned int num_elements = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY);

            typename std::vector< output_task_element<number> > elements;

            for(unsigned int i = 0; i < num_elements; i++)
	            {
                reader->start_array_element();

                std::string product_name;
                reader->read_value(__CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT, product_name);

		            unsigned int serial;
		            reader->read_value(__CPP_TRANSPORT_NODE_OUTPUT_SERIAL, serial);

                std::list<std::string> tags;
                unsigned int num_tags = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);
                for(unsigned int j = 0; j < num_tags; j++)
	                {
                    reader->start_array_element();
                    std::string tag;
                    reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, tag);
                    tags.push_back(tag);
                    reader->end_array_element();
	                }
                reader->end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

                reader->end_array_element();

                derived_data::derived_product<number>* dp = pfinder(dynamic_cast< integration_task<number>* >(product_name);

		            // construct a output_task_element<> object wrapping these elements, and push it to the list
		            elements.push_back(output_task_element<number>(*dp, tags, serial));

		            // the repository-supplied objects can now be deleted; output_task_element performs a deep copy,
		            // so there is no risk of dangling pointers or references
		            delete dp;
	            }

            reader->end_element(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY);

            return transport::output_task<number>(name, elements);
	        }

	    }   // namespace output_task_helper


	}   // namespace transport


#endif //__task_H_
