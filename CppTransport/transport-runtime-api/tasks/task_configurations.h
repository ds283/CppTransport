//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __task_configurations_H_
#define __task_configurations_H_


#include <iostream>

#include "transport-runtime-api/messages.h"


#define __CPP_TRANSPORT_DEFAULT_K_PRECISION (2)
#define __CPP_TRANSPORT_DEFAULT_T_PRECISION (2)


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
				unsigned int get_serial() const { return(this->serial); }

				//! Output to a standard stream
				friend std::ostream& operator<<(std::ostream& out, time_config& obj);
			};


		std::ostream& operator<<(std::ostream& out, const time_config& obj)
			{
		    std::ostringstream str;
				str << std::setprecision(__CPP_TRANSPORT_DEFAULT_T_PRECISION) << obj.t;
				out << __CPP_TRANSPORT_TIME_CONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_TIME_CONFIG_TEQUALS << " " << str.str() << std::endl;

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

        //! comoving k-value
        double       k_comoving;
        double       k_conventional;

        //! flag which indicates to the integrator whether to store the background
        bool         store_background;

        //! Output to a standard stream
        friend std::ostream& operator<<(std::ostream& out, twopf_kconfig& obj);
	    };


    std::ostream& operator<<(std::ostream& out, const twopf_kconfig& obj)
	    {
        std::ostringstream str;
        str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k_comoving;
        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KEQUALS << " " << str.str() << std::endl;

        return(out);
	    }


    class threepf_kconfig
	    {
      public:

        //! serial number of this k-configuration
        unsigned int                serial;
        unsigned int                get_serial() const { return(this->serial); }

        //! serial numbers of k1, k2, k3 into list of twopf-kconfigurations
        //! eg. used to look up appropriate values of the power spectrum when constructing reduced 3pfs
        std::array<unsigned int, 3> index;

        //! (k1,k2,k3) coordinates for this k-configuration
        double                      k1_comoving;
        double                      k2_comoving;
        double                      k3_comoving;
        double                      k1_conventional;
        double                      k2_conventional;
        double                      k3_conventional;

        //! Fergusson-Shellard-Liguori coordinates for this k-configuration
        double                      k_t_comoving;     // comoving normalize k_t
        double                      k_t_conventional; // conventionally normalized k_t
        double                      alpha;
        double                      beta;

        //! flags which indicate to the integrator whether to
        //! store the background and twopf results from this integration
        bool                        store_background;
        bool                        store_twopf_k1;
        bool                        store_twopf_k2;
        bool                        store_twopf_k3;

        //! Output to a standard stream
        friend std::ostream& operator<<(std::ostream& out, threepf_kconfig& obj);
	    };


    std::ostream& operator<<(std::ostream& out, const threepf_kconfig& obj)
	    {
        std::ostringstream kt_str;
        std::ostringstream alpha_str;
        std::ostringstream beta_str;

        kt_str    << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k_t_comoving;
        alpha_str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.alpha;
        beta_str  << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.beta;

        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KTEQUALS << " " << kt_str.str()
	        << ", " << __CPP_TRANSPORT_KCONFIG_ALPHAEQUALS << " " << alpha_str.str()
	        << ", " << __CPP_TRANSPORT_KCONFIG_BETAEQUALS << " " << beta_str.str()
	        << std::endl;

        return(out);
	    }


    // OUTPUT DATA

    // forward-declare derived_product
    namespace derived_data
      {
        template <typename number> class derived_product;
      }

    template <typename number> class output_task_element;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task_element<number>& obj);

    template <typename number>
    class output_task_element
      {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct an output task element: requires a derived product and a list of tags to match.
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
        out << std::endl;

        return(out);
      }

	}


#endif //__task_configurations_H_
