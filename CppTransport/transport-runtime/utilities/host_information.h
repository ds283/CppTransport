//
// Created by David Seery on 11/03/15.
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


#ifndef CPPTRANSPORT_HOST_INFORMATION_H
#define CPPTRANSPORT_HOST_INFORMATION_H


#include <iterator>

#include "mpi.h"

#include "boost/optional.hpp"

#include <sys/utsname.h>
#include <cpuid.h>


namespace transport
	{
    
    namespace host_information_impl
      {

        struct cpuid_vendor_string
          {
            unsigned int ebx;
            unsigned int edx;
            unsigned int ecx;
        
            // TODO: not portable? Assumes a little-endian architecture
            // this is guaranteed to be a 12-char string
            std::string to_string() const { return std::string{reinterpret_cast<const char*>(this), 12}; }
          };
        
        
        struct cpuid_brand_string_block
          {
            unsigned int eax;
            unsigned int ebx;
            unsigned int ecx;
            unsigned int edx;
            
            // TODO: not portable? Assumes a little-endian architecture
            // read no more than 16 characters to form a string;
            // this may include some nulls
            std::string to_string() const { return std::string{reinterpret_cast<const char*>(this), 16}; }
          };
        
      }

		class cpu_vendor_id
			{
        
        // CONSTRUCTOR, DESTRUCTOR

		  public:

				//! constructor reads and stores vendor string
				cpu_vendor_id();

        //! destructor is default
				~cpu_vendor_id() = default;


				// GET VENDOR ID STRING
      
      public:

				//! get vendor id
				const std::string& get_vendor_id() const { return(this->vendor_id); }


				// INTERNAL DATA

		  protected:

				//! store vendor data registers
        host_information_impl::cpuid_vendor_string vendor_data;

				//! cache reconstructed vendor id string
				std::string vendor_id;

			};
    
    
    class cpu_brand_string
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor reads brandstring and stores it
        cpu_brand_string();
        
        //! destructor is default
        ~cpu_brand_string() = default;
        
        
        // GET BRAND STRING
        
      public:
        
        //! get brand string
        const boost::optional< std::string >& get_brand_string() const { return this->brand_string; }
        
        // INTERNAL DATA
      
      protected:

        // Store brand string data registers
        
        //! workspace for 1st block
        host_information_impl::cpuid_brand_string_block block1;
    
        //! workspace for 2nd block
        host_information_impl::cpuid_brand_string_block block2;
    
        //! workspace for 3rd block
        host_information_impl::cpuid_brand_string_block block3;
        
        
        //! cache reconstructed brandstring
        boost::optional< std::string > brand_string;
        
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
        
        //! get CPU brand string, if supported
        const boost::optional< std::string >& get_cpu_brand_string() const { return this->brand_string.get_brand_string(); }

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
        
        //! CPU brand stirng
        cpu_brand_string brand_string;

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
				constexpr unsigned int level = 0;
				unsigned int eax = 0;

				__get_cpuid(level, &eax, &vendor_data.ebx, &vendor_data.ecx, &vendor_data.edx);

				vendor_id = vendor_data.to_string();
			}
    
    
    cpu_brand_string::cpu_brand_string()
      {
        // check whether brand string is supported
        constexpr unsigned int getCode = 0x80000000;
        
        unsigned int eax = 0;
        unsigned int ebx = 0;
        unsigned int ecx = 0;
        unsigned int edx = 0;
        
        __get_cpuid(getCode, &eax, &ebx, &ecx, &edx);
        
        constexpr unsigned int bk1 = 0x80000002;
        constexpr unsigned int bk2 = 0x80000003;
        constexpr unsigned int bk3 = 0x80000004;
        
        // exit if bk3 opcode not available
        if(eax < bk3) return;
        
        __get_cpuid(bk1, &block1.eax, &block1.ebx, &block1.ecx, &block1.edx);
        __get_cpuid(bk2, &block2.eax, &block2.ebx, &block2.ecx, &block2.edx);
        __get_cpuid(bk3, &block3.eax, &block3.ebx, &block3.ecx, &block3.edx);
        
        std::string temp = block1.to_string() + block2.to_string() + block3.to_string();
        
        std::string temp2;
        auto ins = std::back_inserter(temp2);
        for(auto c : temp)
          {
            if(c == '\0') break;
            
            ins = c;
            ++ins;
          }
        
        brand_string = temp2;
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
