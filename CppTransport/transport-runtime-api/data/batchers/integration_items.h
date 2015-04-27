//
// Created by David Seery on 27/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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

            //! values
            std::vector<number> coords;

            //! kconfig serial number for the integration which produced this. Used when unwinding a batch.
            unsigned int        source_serial;

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


        //! Stores a threepf configuration associated with a single time-point and k-configuration
        class threepf_item
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
		    class ics_item
			    {
		      public:

				    //! kconfig serial number
				    unsigned int        source_serial;

		        //! values
		        std::vector<number> coords;
			    };

	    };

	}   // namespace transport


#endif //__integration_items_H_
