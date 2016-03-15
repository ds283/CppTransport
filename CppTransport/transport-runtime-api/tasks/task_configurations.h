//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef __task_configurations_H_
#define __task_configurations_H_


#include <iostream>

#include "transport-runtime-api/messages.h"

// forward-declare derived products if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

#include "boost/log/utility/formatting_ostream.hpp"


#define CPPTRANSPORT_DEFAULT_K_PRECISION (6)
#define CPPTRANSPORT_DEFAULT_T_PRECISION (5)


namespace transport
	{

		// TIME CONFIGURATION DATA

		class time_config
			{
		  public:

				//! time value
				double t;

				//! serial number
				unsigned int serial;

				unsigned int  get_serial() const { return(this->serial); }

				double&       get_value()        { return(this->t); }
				const double& get_value()  const { return(this->t); }

        template <typename Stream> void write(Stream& out) const;
      };


    template <typename Stream>
    void time_config::write(Stream& out) const
      {
        std::ostringstream str;
        str << std::setprecision(CPPTRANSPORT_DEFAULT_T_PRECISION) << this->t;
        out << CPPTRANSPORT_TIME_CONFIG_SERIAL << " " << this->serial << ", " << CPPTRANSPORT_TIME_CONFIG_TEQUALS << " " << str.str() << '\n';
      }


    template <typename Char, typename Traits>
		std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const time_config& obj)
			{
        obj.write(out);
				return(out);
			}


    template <typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const time_config& obj)
      {
        obj.write(out);
        return(out);
      }


    // K-CONFIGURATION DATA

    class twopf_kconfig
	    {
      public:

        //! serial number - guaranteed to be unique.
        //! used to identify this k-configuration in the database
        unsigned int serial;
        unsigned int get_serial() const { return(this->serial); }

        //! comoving-normalized k-value (normalized so that k = aH at horizon exit)
        double k_comoving;

		    //! conventionally normalized k-value (normalized so that k=1 at time N*)
		    double k_conventional;

		    //! time of horizon exit
		    double t_exit;

        //! time where massless approximation is marginal, ie k/a = M^2 where M^2 is largest eigenvalue of mass matrix
        double t_massless;

        template <typename Stream> void write(Stream& out) const;
	    };


    template <typename Stream>
    void twopf_kconfig::write(Stream& out) const
      {
        std::ostringstream str;
        str << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->k_comoving;

        std::ostringstream exit_str;
        exit_str << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->t_exit;

        std::ostringstream massless_str;
        massless_str << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->t_massless;

        out << CPPTRANSPORT_KCONFIG_SERIAL << " " << this->serial << ", "
          << CPPTRANSPORT_KCONFIG_KEQUALS << " " << str.str() << ", "
          << CPPTRANSPORT_KCONFIG_T_EXIT << " " << exit_str.str() << ", "
          << CPPTRANSPORT_KCONFIG_T_MASSLESS << " " << massless_str.str();
        out << '\n';
      }


    template <typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const twopf_kconfig& obj)
	    {
        obj.write(out);
        return(out);
	    }


    template <typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const twopf_kconfig& obj)
      {
        obj.write(out);
        return(out);
      }


    class threepf_kconfig
	    {
      public:

        //! serial number of this k-configuration
        unsigned int serial;
        unsigned int get_serial() const { return(this->serial); }

        //! serial numbers of k1, k2, k3 into list of twopf-kconfigurations
        //! eg. used to look up appropriate values of the power spectrum when constructing reduced 3pfs
		    unsigned int k1_serial;
		    unsigned int k2_serial;
		    unsigned int k3_serial;

        //! (k1,k2,k3) coordinates for this k-configuration
        double k1_comoving;
        double k2_comoving;
        double k3_comoving;
        double k1_conventional;
        double k2_conventional;
        double k3_conventional;

        //! Fergusson-Shellard-Liguori coordinates for this k-configuration
        double kt_comoving;
        double kt_conventional;
        double alpha;
        double beta;

		    //! horizon-exit time for k_t/3.0
		    double t_exit;

        //! time where massless approximation is marginal, ie k/a = M^2 where M^2 is largest eigenvalue of mass matrix
        double t_massless;

        template <typename Stream> void write(Stream& out) const;
	    };


    template <typename Stream>
    void threepf_kconfig::write(Stream& out) const
      {
        std::ostringstream kt_str;
        std::ostringstream alpha_str;
        std::ostringstream beta_str;

        kt_str    << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->kt_comoving;
        alpha_str << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->alpha;
        beta_str  << std::setprecision(CPPTRANSPORT_DEFAULT_K_PRECISION) << this->beta;

        out << CPPTRANSPORT_KCONFIG_SERIAL << " " << this->serial << ", " << CPPTRANSPORT_KCONFIG_KTEQUALS << " " << kt_str.str()
        << ", " << CPPTRANSPORT_KCONFIG_ALPHAEQUALS << " " << alpha_str.str()
        << ", " << CPPTRANSPORT_KCONFIG_BETAEQUALS << " " << beta_str.str()
        << '\n';
      }


    template <typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const threepf_kconfig& obj)
	    {
        obj.write(out);
        return(out);
	    }


    template <typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const threepf_kconfig& obj)
      {
        obj.write(out);
        return(out);
      }


		class kconfiguration_statistics
			{
		  public:

		    //! serial number of this configuration
		    unsigned int serial;

		    //! time spent integrating, in nanoseconds
		    boost::timer::nanosecond_type integration;

		    //! time spent batching, in nanoseconds
		    boost::timer::nanosecond_type batching;

		    //! number of stepsize refinements needed for this configuration
		    unsigned int refinements;

		    //! number of steps taken by the stepper
		    size_t steps;

        //! workgroup which produced this configuration
        unsigned int workgroup;

        //! worker which produced this configuration
        unsigned int worker;

        //! backend which produced this configuration
        std::string backend;

        //! background stepper used
        std::string background_stepper;

        //! perturbation stepper used
        std::string perturbation_stepper;

        //! hostname of worker
        std::string hostname;
			};


    // OUTPUT DATA


    template <typename number>
    class output_task_element
      {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct an output task element: requires a derived product and a list of tags to match.
        output_task_element(const derived_data::derived_product<number>& dp, const std::list<std::string>& tgs, unsigned int sn)
          : product(dp.clone()),
            tags(tgs),
            serial(sn)
          {
            assert(product != nullptr);
          }

        //! Override the default copy constructor to perform a deep copy of the stored derived_product<>
        output_task_element(const output_task_element<number>& obj)
          : product(obj.product->clone()),
            tags(obj.tags),
            serial(obj.serial)
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


        // WRITE TO STREAM

      public:

        //! write self to stream
        template <typename Stream>
        void write(Stream& out) const;


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
    template <typename Stream>
    void output_task_element<number>::write(Stream& out) const
      {
        out << "  " << CPPTRANSPORT_OUTPUT_ELEMENT_OUTPUT << " " << this->get_product_name() << ",";
        out << " " << CPPTRANSPORT_OUTPUT_ELEMENT_TAGS  << ": ";

        unsigned int count = 0;
        const std::list<std::string>& tags = this->get_tags();
        for (std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); ++u)
          {
            if(count > 0) out << ", ";
            out << *u;
          }
        out << '\n';
      }


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const output_task_element<number>& obj)
      {
        obj.write(out);
        return(out);
      }


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const output_task_element<number>& obj)
      {
        obj.write(out);
        return(out);
      }

	}


#endif //__task_configurations_H_
