//
// Created by David Seery on 01/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_METADATA_H
#define CPPTRANSPORT_METADATA_H


#include <string>
#include <map>


namespace transport
  {

    class worker_information
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        worker_information(unsigned int wg, unsigned int wk, std::string be, std::string bs,
                           const std::string ps, double b_atol, double b_rtol, double p_atol, double p_rtol,
                           std::string hn, std::string on, std::string ov, std::string orl,
                           std::string ar, std::string cv)
          : workgroup(wg),
            worker(wk),
            backend(be),
            backg_stepper(bs),
            pert_stepper(ps),
            backg_tol(b_atol, b_rtol),
            pert_tol(p_atol, p_rtol),
            hostname(hn),
            os_name(on),
            os_version(ov),
            os_release(orl),
            architecture(ar),
            cpu_vendor(cv)
          {
          }

        //! destructor is default
        ~worker_information() = default;


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

        //! CPU vendor
        std::string cpu_vendor;

      };


    //! typesef a map from (workgroup, worker) pairs to a worker information record;
    //! used as a database of worker information
    typedef std::map< std::pair<unsigned int, unsigned int>, std::unique_ptr< worker_information > > worker_information_db;

  }   // namespace transport


#endif //CPPTRANSPORT_METADATA_H
