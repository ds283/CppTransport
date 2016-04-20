//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_HOST_INFORMATION_H
#define CPPTRANSPORT_HOST_INFORMATION_H

#include "mpi.h"
#include <sys/utsname.h>
#include <cpuid.h>


namespace transport
	{

		class cpu_vendor_id
			{

		  protected:

				struct vendor_string
					{
						unsigned int ebx;
						unsigned int edx;
				    unsigned int ecx;

						std::string to_string() const { return std::string(reinterpret_cast<const char*>(this), 12); }
					};

		  public:

				//! Construct
				cpu_vendor_id();

				~cpu_vendor_id() = default;


				// GET VENDOR ID STRING

				//! get vendor id
				const std::string& get_vendor_id() const { return(this->vendor_id); }


				// INTERNAL DATA

		  protected:

				//! store vendor data registers
				vendor_string vendor_data;

				//! store reconstructed vendor id string
				std::string vendor_id;

			};


    class host_information
	    {

      public:

        //! Construct a host_information object
        host_information();

        ~host_information() = default;


        // RETURN HOST INFORMATION

        //! get hostname reported by MPI environment
        const std::string& get_host_name() const { return(this->host_name); }

        //! get CPU vendor id
        const std::string& get_cpu_vendor_id() const { return(this->vendor_id.get_vendor_id()); }

        //! get operating system name
        const std::string& get_os_name() const { return(this->os_name); }

        //! get operating system version
        const std::string& get_os_version() const { return(this->os_version); }

        //! get operating system release
        const std::string& get_os_release() const { return(this->os_release); }

        //! get machine identifier
        const std::string& get_architecture() const { return(this->machine); }

        // INTERNAL DATA

      protected:

        //! host name
        std::string host_name;

        //! vendor id string
        cpu_vendor_id vendor_id;

        //! operating system name
        std::string os_name;

        //! operating system version
        std::string os_version;

        //! operating system release
        std::string os_release;

        //! machine identifier
        std::string machine;

	    };


		cpu_vendor_id::cpu_vendor_id()
			{
				unsigned int level = 0;
				unsigned int eax = 0;

				__get_cpuid(level, &eax, &vendor_data.ebx, &vendor_data.ecx, &vendor_data.edx);

				vendor_id = vendor_data.to_string();
			}


		host_information::host_information()
			{
		    // get MPI to report the local host name
		    char p_name[MPI_MAX_PROCESSOR_NAME];
		    int p_len;
		    MPI_Get_processor_name(p_name, &p_len);
		    host_name = std::string(p_name, p_len);

				// interrogate uname
				struct utsname data;
				uname(&data);

				os_name = std::string(data.sysname);
				os_version = std::string(data.version);
				os_release = std::string(data.release);
				machine = std::string(data.machine);
			}

	}   // namespace transport


#endif //CPPTRANSPORT_HOST_INFORMATION_H
