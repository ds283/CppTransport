//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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

        //! index of this k-configuration in the serial list
        unsigned int index;

        //! comoving k-value
        double       k;

        //! flag which indicates to the integrator whether to store the background
        bool         store_background;

        //! serial number - guaranteed to be unique.
        //! used to identify this k-configuration in the database
        unsigned int serial;
        unsigned int get_serial() const { return(this->serial); }

        //! Output to a standard stream
        friend std::ostream& operator<<(std::ostream& out, twopf_kconfig& obj);
	    };


    std::ostream& operator<<(std::ostream& out, const twopf_kconfig& obj)
	    {
        std::ostringstream str;
        str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k;
        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KEQUALS << " " << str.str() << std::endl;

        return(out);
	    }


    class threepf_kconfig
	    {
      public:

        //! index of k1, k2, k3 into serial list of k-modes
        //! used to look up appropriate values of the power spectrum when constructing reduced 3pfs
        std::array<unsigned int, 3> index;

        //! comoving (k1,k2,k3) coordinates for this k-configuration
        double                      k1;
        double                      k2;
        double                      k3;

        //! Fergusson-Shellard-Liguori coordinates for this k-configuration
        double                      k_t;
        double                      alpha;
        double                      beta;
        double                      k_t_conventional; // conventionally normalized k_t

        //! flags which indicate to the integrator whether to
        //! store the background and twopf results from this integration
        bool                        store_background;
        bool                        store_twopf;

        //! serial number of this k-configuration
        unsigned int                serial;
        unsigned int                get_serial() const { return(this->serial); }

        //! Output to a standard stream
        friend std::ostream& operator<<(std::ostream& out, threepf_kconfig& obj);
	    };


    std::ostream& operator<<(std::ostream& out, const threepf_kconfig& obj)
	    {
        std::ostringstream kt_str;
        std::ostringstream alpha_str;
        std::ostringstream beta_str;

        kt_str    << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.k_t;
        alpha_str << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.alpha;
        beta_str  << std::setprecision(__CPP_TRANSPORT_DEFAULT_K_PRECISION) << obj.beta;

        out << __CPP_TRANSPORT_KCONFIG_SERIAL << " " << obj.serial << ", " << __CPP_TRANSPORT_KCONFIG_KTEQUALS << " " << kt_str.str()
	        << ", " << __CPP_TRANSPORT_KCONFIG_ALPHAEQUALS << " " << alpha_str.str()
	        << ", " << __CPP_TRANSPORT_KCONFIG_BETAEQUALS << " " << beta_str.str()
	        << std::endl;

        return(out);
	    }

	}


#endif //__task_configurations_H_
