//
// Created by David Seery on 27/03/15.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef __integration_items_H_
#define __integration_items_H_


namespace transport
	{

    // data structures for storing individual sample points from each integration

    template <typename number>
    class integration_items
	    {

      public:


        //! Stores a background field configuration associated with single time-point
        class backg_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;
            unsigned int        get_serial() const { return(this->time_serial); }

            //! values
            std::vector<number> coords;

            //! kconfig serial number for the integration which produced this. Used when unwinding a batch.
            unsigned int        source_serial;

		        //! get_texit() not needed for backg_item but allowed here to prevent template specialization failure
		        //! TODO: would be nice to simplify this
		        double get_texit() const { return(0.0); }

	        };


        //! Stores a real twopf configuration associated with a single time-point and k-configuration
		    //! (there are separate classes for real and imaginary versions so we can adjust template behaviour via type traits)
        class twopf_re_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            // values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

	        };


        //! Stores an imaginary twopf configuration associated with a single time-point and k-configuration
        //! (there are separate classes for real and imaginary versions so we can adjust template behaviour via type traits)
        class twopf_im_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            // values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

	        };


        //! Stores a tensor two-point function configuration, associated with a single time-point and k-configuration
        class tensor_twopf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            // values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;
	        };


        //! Stores a threepf configuration associated with a single time-point and k-configuration,
        //! computed using fields and canonical momenta
        class threepf_momentum_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch
            unsigned int        source_serial;
	        };


        //! Stores a threepf configuration associated with a single time-point and k-configuration,
        //! computed using fields and e-folding derivatives
        class threepf_Nderiv_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! values
            std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch
            unsigned int        source_serial;
          };


        //! Stores per-configuration statistics about the performance of the integrator.
        //! Not used by all backends
        class configuration_statistics
	        {
          public:

            //! kconfig serial number for this configuration
            unsigned int                  serial;

            //! time spent integrating, in nanoseconds
            boost::timer::nanosecond_type integration;

            //! time spent batching, in nanoseconds
            boost::timer::nanosecond_type batching;

            //! number of stepsize refinements needed for this configuration
            unsigned int                  refinements;

            //! number of steps taken by the stepper
            size_t                        steps;
	        };


		    //! Stores information about the initial conditions for each k-configuration
		    //! (there are separate types for ics_item and ics_kt_item so we can adjust template behaviour via type traits)
		    class ics_item
			    {
		      public:

				    //! kconfig serial number
				    unsigned int        source_serial;
				    unsigned int        get_serial() const { return(this->source_serial); }

				    //! time of horizon exit
				    double              texit;
				    double              get_texit() const { return(this->texit); }

		        //! values
		        std::vector<number> coords;
			    };


        //! Stores information about the initial conditions for each k-configuration
        //! (there are separate types for ics_item and ics_kt_item so we can adjust template behaviour via type traits)
        class ics_kt_item
	        {
          public:

            //! kconfig serial number
            unsigned int        source_serial;
            unsigned int        get_serial() const { return(this->source_serial); }

            //! time of horizon exit
            double              texit;
            double              get_texit() const { return(this->texit); }

            //! values
            std::vector<number> coords;
	        };

	    };

	}   // namespace transport


#endif //__integration_items_H_
