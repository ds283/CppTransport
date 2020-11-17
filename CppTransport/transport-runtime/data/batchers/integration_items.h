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


#ifndef CPPTRANSPORT_INTEGRATION_ITEMS_H
#define CPPTRANSPORT_INTEGRATION_ITEMS_H


namespace transport
	{

    // data structures for storing individual sample points from each integration

    // NOTE when generating unique identifiers, it's important that they in approximately increasing order
    // This speeds up INSERT performance dramatically and is critical for getting good aggregation speeds
    // see http://stackoverflow.com/questions/788568/sqlite3-disabling-primary-key-index-while-inserting

    // (the data_manager_write routines later sort into order so perhaps we could be more relaxed)

    template <typename number>
    class integration_items
	    {

      public:

        //! Stores a background field configuration associated with single time-point
        class backg_item
	        {
          public:
            backg_item(unsigned ts, unsigned int ss, const std::vector<number>& co, unsigned int ti, unsigned int Nf)
              : time_serial(ts),
                source_serial(ss),
                coords(co),
                time_items(ti),
                Nfields(Nf)
              {
                if(coords.size() != 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_BACKG_SIZE << " (" << coords.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(time_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            unsigned int get_serial() const { return (this->time_serial); }

            //! values
            const std::vector<number> coords;

            //! kconfig serial number for the integration which produced this. Used when unwinding a batch.
            const unsigned int source_serial;

		        //! get_texit() not needed for backg_item but allowed here to prevent template specialization failure
		        //! TODO: would be nice to simplify this
		        double get_texit() const { return(0.0); }
	        };


        //! Stores a real twopf configuration associated with a single time-point and k-configuration
		    //! (there are separate classes for real and imaginary versions so we can adjust template behaviour via type traits)
        class twopf_re_item
	        {
          public:
            twopf_re_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
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
                    msg << CPPTRANSPORT_BATCHER_TWOPF_SIZE << " (" << elements.size() << ")";
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

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

	        };


        //! Stores an imaginary twopf configuration associated with a single time-point and k-configuration
        //! (there are separate classes for real and imaginary versions so we can adjust template behaviour via type traits)
        class twopf_im_item
	        {
          public:
            twopf_im_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
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
                    msg << CPPTRANSPORT_BATCHER_TWOPF_SIZE << " (" << elements.size() << ")";
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

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

	        };


        //! Stores a real twopf 'spectral index' configuration associated with a single time-point and k-configuration
        class twopf_si_re_item
          {
          public:
            twopf_si_re_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
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
                    msg << CPPTRANSPORT_BATCHER_TWOPF_SI_SIZE << " (" << elements.size() << ")";
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

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;

          };


        //! Stores a tensor two-point function configuration, associated with a single time-point and k-configuration
        class tensor_twopf_item
	        {
          public:
            tensor_twopf_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
                if(elements.size() != 2*2)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_TENSOR_TWOPF_SIZE << " (" << elements.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;
	        };


        //! Stores a tensor two-point 'spectral index' function configuration, associated with a single time-point and k-configuration
        class tensor_twopf_si_item
          {
          public:
            tensor_twopf_si_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki)
              {
                if(elements.size() != 2*2)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_TENSOR_TWOPF_SI_SIZE << " (" << elements.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(kconfig_serial*time_items*pages + time_serial*pages + page); }

            //! time serial number for this configuration
            const unsigned int time_serial;

            //! kconfig serial number for this configuration
            const unsigned int kconfig_serial;

            //! number of time serial numbers in job
            const unsigned int time_items;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            const unsigned int source_serial;
          };


        //! Stores a threepf configuration associated with a single time-point and k-configuration,
        //! computed using fields and canonical momenta
        class threepf_momentum_item
	        {
          public:
            threepf_momentum_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields * 2*Nfields * 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_THREEPF_SIZE << " (" << elements.size() << ")";
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

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch
            const unsigned int source_serial;
	        };


        //! Stores a threepf configuration associated with a single time-point and k-configuration,
        //! computed using fields and e-folding derivatives
        class threepf_Nderiv_item
          {
          public:
            threepf_Nderiv_item(unsigned int ts, unsigned int ks, unsigned int ss, const std::vector<number>& e, unsigned int ti, unsigned int ki, unsigned int Nf)
              : time_serial(ts),
                kconfig_serial(ks),
                source_serial(ss),
                elements(e),
                time_items(ti),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(elements.size() != 2*Nfields * 2*Nfields * 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_THREEPF_SIZE << " (" << elements.size() << ")";
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

            //! values
            const std::vector<number> elements;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch
            const unsigned int source_serial;
          };


        //! Stores per-configuration statistics about the performance of the integrator.
        //! Not used by all backends
        class configuration_statistics
	        {
          public:
            configuration_statistics(unsigned int s, boost::timer::nanosecond_type i, boost::timer::nanosecond_type b, unsigned int r, size_t st)
              : serial(s),
                integration(i),
                batching(b),
                refinements(r),
                steps(st)
              {
              }

            //! kconfig serial number for this configuration
            const unsigned int serial;

            //! time spent integrating, in nanoseconds
            const boost::timer::nanosecond_type integration;

            //! time spent batching, in nanoseconds
            const boost::timer::nanosecond_type batching;

            //! number of stepsize refinements needed for this configuration
            const unsigned int refinements;

            //! number of steps taken by the stepper
            const size_t steps;
	        };


		    //! Stores information about the initial conditions for each k-configuration
		    //! (there are separate types for ics_item and ics_kt_item so we can adjust template behaviour via type traits)
		    class ics_item
			    {
		      public:
            ics_item(unsigned int ss, double tx, const std::vector<number>& co, unsigned int ki, unsigned int Nf)
              : source_serial(ss),
                texit(tx),
                coords(co),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(coords.size() != 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_ICS_SIZE << " (" << coords.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(source_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

				    //! kconfig serial number
            const unsigned int source_serial;

            unsigned int get_serial() const { return (this->source_serial); }

				    //! time of horizon exit
            const double texit;
            
            double get_texit() const { return (this->texit); }

		        //! values
		        const std::vector<number> coords;
			    };


        //! Stores information about the initial conditions for each k-configuration
        //! (there are separate types for ics_item and ics_kt_item in order that we can adjust template behaviour via type traits)
        class ics_kt_item
	        {
          public:
            ics_kt_item(unsigned int ss, double tx, const std::vector<number>& co, unsigned int ki, unsigned int Nf)
              : source_serial(ss),
                texit(tx),
                coords(co),
                kconfig_items(ki),
                Nfields(Nf)
              {
                if(coords.size() != 2*Nfields)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_BATCHER_ICS_SIZE << " (" << coords.size() << ")";
                    throw runtime_exception(exception_type::STORAGE_ERROR, msg.str());
                  }
              }

            //! make unique identifier
            unsigned long int get_unique(unsigned int page, unsigned int pages) const { return(source_serial*pages + page); }

            //! cache number of fields in parent model
            const unsigned int Nfields;

            //! number of kconfiguration serial numbers in job
            const unsigned int kconfig_items;

            //! kconfig serial number
            const unsigned int source_serial;

            unsigned int get_serial() const { return (this->source_serial); }

            //! time of horizon exit
            const double texit;

            double get_texit() const { return (this->texit); }

            //! values
            const std::vector<number> coords;
	        };

	    };

	}   // namespace transport


#endif //CPPTRANSPORT_INTEGRATION_ITEMS_H
