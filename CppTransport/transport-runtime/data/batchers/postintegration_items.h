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

    // NOTE when generating unique identifiers, it's important that they in approximately increasing order
    // This speeds up INSERT performance dramatically and is critical for getting good aggregation speeds
    // see http://stackoverflow.com/questions/788568/sqlite3-disabling-primary-key-index-while-inserting

    template <typename number>
    class postintegration_items
	    {

      public:
        
        //! Stores a zeta twopf configuration
        class zeta_twopf_item
	        {
          public:
            zeta_twopf_item(unsigned int ts, unsigned int ks, unsigned int ss, number v, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                value(v),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique() const { return(time_serial*kconfig_items + kconfig_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

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
            zeta_threepf_item(unsigned int ts, unsigned int ks, unsigned int ss, number v, number rb, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                value(v),
                redbsp(rb),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique() const { return(time_serial*kconfig_items + kconfig_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

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
            fNL_item(unsigned int ts, number bb, number bt, number tt)
              : time_serial(ts),
                BB(bb),
                BT(bt),
                TT(tt)
              {
              }

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
            gauge_xfm1_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page) const { return(page*time_items*kconfig_items + kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number for this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_123_item
          {
          public:
            gauge_xfm2_123_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page) const { return(page*time_items*kconfig_items + kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number for this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_213_item
          {
          public:
            gauge_xfm2_213_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page) const { return(page*time_items*kconfig_items + kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number for this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 312 permutation
        class gauge_xfm2_312_item
          {
          public:
            gauge_xfm2_312_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page) const { return(page*time_items*kconfig_items + kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number for this configuration
            unsigned int kconfig_serial;

            //! number of time serial numbers in job
            unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

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
