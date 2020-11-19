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

    // NOTE when generating unique identifiers, it's important that they are in approximately increasing order
    // This speeds up INSERT performance dramatically and is critical for getting good aggregation speeds
    // see http://stackoverflow.com/questions/788568/sqlite3-disabling-primary-key-index-while-inserting

    // (the data_manager_write routines later sort into order so perhaps we could be more relaxed)

    template <typename number>
    class postintegration_items
	    {

      public:

        //! Stores a zeta twopf configuration
        class zeta_twopf_item
	        {
          public:
            zeta_twopf_item(unsigned int ts, unsigned int ks, unsigned int ss, number v, number si, unsigned int ti, unsigned int ki)
              : time_serial{ts},
                kconfig_serial{ks},
                source_serial{ss},
                value{v},
                spectral_value{si},
                time_items{ti},
                kconfig_items{ki}
              {
              }

            //! make unique identifier
            unsigned long int get_unique() const { return(kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number of this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            //! twopf value
            const number value;

            //! twopf spectral index value (if used; just set to zero if not present)
            const number spectral_value;
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
            unsigned long int get_unique() const { return(kconfig_serial*time_items + time_serial); }

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number of this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            //! threepf
            const number value;

            //! reduced bispectrum
            const number redbsp;
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
            const unsigned int time_serial;

            // bispectrum.bispectrum value
            const number BB;

            // bispectrum.template
            const number BT;

            // template.template
            const number TT;
	        };


        //! Lightweight fNL-like item for performing comparisons using std::set::find only
        class fNL_item_keyonly
          {
          public:
            fNL_item_keyonly(unsigned int ts)
              : time_serial(ts)
              {
              }

            //! time serial number for this configuration
            const unsigned int time_serial;
          };


        //! Stores a linear gauge transformation
        class gauge_xfm1_item
          {
          public:
            gauge_xfm1_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM1_SIZE << " (" << elements.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            // values
            const std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_123_item
          {
          public:
            gauge_xfm2_123_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields * 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM2_SIZE << " (" << elements.size() << ", type=123)";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            // values
            const std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 123 permutation
        class gauge_xfm2_213_item
          {
          public:
            gauge_xfm2_213_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields * 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM2_SIZE << " (" << elements.size() << ", type=213)";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            // values
            const std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation in the 312 permutation
        class gauge_xfm2_312_item
          {
          public:
            gauge_xfm2_312_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields * 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_ZETA_BATCHER_GAUGEXFM2_SIZE << " (" << elements.size() << ", type=312)";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

            // values
            const std::vector<number> elements;
          };


		    struct fNL_item_comparator
			    {
		      public:
			      // enable searching on heterogeneous keys
			      using is_transparent = void;

				    bool operator() (const std::unique_ptr<fNL_item>& A, const std::unique_ptr<fNL_item>& B) const
					    {
						    return(A->time_serial < B->time_serial);
					    }

            bool operator() (const std::unique_ptr<fNL_item>& A, const std::unique_ptr<fNL_item_keyonly>& B) const
              {
                return(A->time_serial < B->time_serial);
              }

            bool operator() (const std::unique_ptr<fNL_item_keyonly>& A, const std::unique_ptr<fNL_item>& B) const
              {
                return(A->time_serial < B->time_serial);
              }
			    };


        typedef std::set< std::unique_ptr<fNL_item>, fNL_item_comparator > fNL_cache;


	    };

	}   // namespace transport


#endif //CPPTRANSPORT_POSTINTEGRATION_ITEMS_H
