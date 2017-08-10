//
// Created by David Seery on 01/04/2016.
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

#ifndef CPPTRANSPORT_METADATA_H
#define CPPTRANSPORT_METADATA_H


#include <string>
#include <map>

#include "boost/timer/timer.hpp"


namespace transport
  {

    class worker_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        worker_record(unsigned int wg, unsigned int wk, std::string be, std::string bs,
                      const std::string ps, double b_atol, double b_rtol, double p_atol, double p_rtol,
                      std::string hn, std::string on, std::string ov, std::string orl,
                      std::string ar, std::string cb, std::string cv)
          : workgroup(wg),
            worker(wk),
            backend(std::move(be)),
            backg_stepper(std::move(bs)),
            pert_stepper(std::move(ps)),
            backg_tol(b_atol, b_rtol),
            pert_tol(p_atol, p_rtol),
            hostname(std::move(hn)),
            os_name(std::move(on)),
            os_version(std::move(ov)),
            os_release(std::move(orl)),
            architecture(std::move(ar)),
            cpu_brand(std::move(cb)),
            cpu_vendor(std::move(cv))
          {
          }

        //! destructor is default
        ~worker_record() = default;


        // INTERFACE

      public:

        //! return workgroup number
        unsigned int get_workgroup() const { return(this->workgroup); }

        //! return worker number
        unsigned int get_worker() const { return worker; }

        //! return backend identifier
        const std::string& get_backend() const { return backend; }

        //! return background stepper identifier
        const std::string& get_backg_stepper() const { return backg_stepper; }

        //! return perturbations stepper identifier
        const std::string& get_pert_stepper() const { return pert_stepper; }

        //! return tolerances for background
        const std::pair<double, double>& get_backg_tol() const { return backg_tol; }

        //! return tolerances for perturbations
        const std::pair<double, double>& get_pert_tol() const { return pert_tol; }

        //! return hostname
        const std::string& get_hostname() const { return hostname; }

        //! return OS name
        const std::string& get_os_name() const { return os_name; }

        //! return OS version
        const std::string& get_os_version() const { return os_version; }

        //! return OS release
        const std::string& get_os_release() const { return os_release; }

        //! return architecture
        const std::string& get_architecture() const { return architecture; }

        //! return CPU brand string
        const std::string& get_cpu_brand_string() const { return cpu_brand; }
        
        //! return CPU vendor
        const std::string& get_cpu_vendor() const { return cpu_vendor; }


        // INTERNAL DATA

      private:

        //! workgroup number
        const unsigned int workgroup;

        //! worker number
        const unsigned int worker;

        //! backend name
        const std::string backend;

        //! background stepper
        const std::string backg_stepper;

        //! perturbations stepper
        const std::string pert_stepper;

        //! background tolerances
        std::pair< double, double > backg_tol;

        //! perturbations tolerances
        std::pair< double, double > pert_tol;

        //! hostname for this worker
        std::string hostname;

        //! OS name for this worker
        std::string os_name;

        //! OS version for this worker
        std::string os_version;

        //! OS release for this worker
        std::string os_release;

        //! CPU architecture
        std::string architecture;

        //! CPU brand
        std::string cpu_brand;
        
        //! CPU vendor
        std::string cpu_vendor;

      };


    //! typesef a map from (workgroup, worker) pairs to a worker information record;
    //! used as a database of worker information
    typedef std::map< std::pair<unsigned int, unsigned int>, std::unique_ptr< worker_record > > worker_information_db;


    class timing_record
      {

        // CONSTRUCTOR DESTRUCTOR

      public:

        //! constructor
        timing_record(unsigned int sn, boost::timer::nanosecond_type it, boost::timer::nanosecond_type bt,
                      unsigned int st, unsigned int rf, unsigned int wg, unsigned int wk)
          : serial(sn),
            integration_time(it),
            batch_time(bt),
            steps(st),
            refinements(rf),
            workgroup(wg),
            worker(wk)
          {
          }

        //! destructor is default
        ~timing_record() = default;


        // INTERFACE

      public:

        //! get serial number
        unsigned int get_serial() const { return serial; }

        //! get integration time
        boost::timer::nanosecond_type get_integration_time() const { return integration_time; }

        //! get batching time
        boost::timer::nanosecond_type get_batch_time() const { return batch_time; }

        //! get number of steps
        unsigned int get_steps() const { return steps; }

        //! get number of refinements
        unsigned int get_refinements() const { return refinements; }

        //! get workgroup identifier
        unsigned int get_workgroup() const { return workgroup; }

        //! get worker identifier
        unsigned int get_worker() const { return worker; }


        // INTERNAL DATA

      private:

        //! serial number of the k-configuration associated with this record (may be a twopf or a threepf serial)
        unsigned int serial;

        //! integration time for this configuration
        boost::timer::nanosecond_type integration_time;

        //! batch time for this configuration
        boost::timer::nanosecond_type batch_time;

        //! number of steps required for this configuration
        unsigned int steps;

        //! number of refinements required for this configuration
        unsigned int refinements;

        //! workgroup identifier of the worker which processed this configuration
        unsigned int workgroup;

        //! worker identifier of the worker which processed this configuration
        unsigned int worker;

    };


    //! typedef a map from serial number to timing record; acts as a database of timing information
    typedef std::map< unsigned int, std::unique_ptr< timing_record > > timing_db;


  }   // namespace transport


#endif //CPPTRANSPORT_METADATA_H
