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


#ifndef CPPTRANSPORT_POSTINTEGRATION_ITEMS_H
#define CPPTRANSPORT_POSTINTEGRATION_ITEMS_H


namespace transport
	{

    // data structures for storing individual sample points from each integration

    template <typename number>
    class postintegration_items
	    {

      public:


        //! Stores a zeta twopf configuration
        class zeta_twopf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            //! twopf value
            number value;

            //! reduced bispectrum not used, but included here to prevent template specialization failure
            number redbsp;
	        };


        //! Stores a zeta threepf configuration
		    //! (we distinguish between threepf configurations and reduced bispectrum configurations, whose classes
		    //! carry the same data, so we can adjust template behaviour using type traits)
        class zeta_threepf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            //! threepf
            number value;

            //! reduced bispectrum
            number redbsp;
	        };


        //! Used to tag a zeta bispectrum contribution via data traits, but has no meaning beyond this
        class zeta_redbsp_item
	        {
          public:
	        };


        //! Stores an fNL configuration
        class fNL_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            // bispectrum.bispectrum
            number BB;

            // bispectrum.template
            number BT;

            // template.template
            number TT;
	        };


        //! Stores a linear gauge transformation
        class gauge_xfm1_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_123_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_213_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 312 permutation
        class gauge_xfm2_312_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
          };


		    struct fNL_item_comparator
			    {
		      public:
				    bool operator() (const fNL_item& A, const fNL_item& B)
					    {
						    return(A.time_serial < B.time_serial);
					    }
			    };

	    };

	}   // namespace transport


#endif //CPPTRANSPORT_POSTINTEGRATION_ITEMS_H
