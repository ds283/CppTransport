//
// Created by David Seery on 26/03/15.
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


#ifndef __configurations_H_
#define __configurations_H_


namespace transport
	{


    // Provide specializations for the size methods used in linecache to compute the size of data elements
    namespace linecache
	    {

        template<>
        unsigned int sizeof_container_element< std::vector<twopf_kconfig> >() { return(sizeof(twopf_kconfig)); }

        template<>
        unsigned int elementsof_container(const std::vector<twopf_kconfig>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<threepf_kconfig> >() { return(sizeof(threepf_kconfig)); }

        template<>
        unsigned int elementsof_container(const std::vector<threepf_kconfig>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<time_config> >() { return(sizeof(time_config)); }

        template<>
        unsigned int elementsof_container(const std::vector<time_config>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<kconfiguration_statistics> >() { return(sizeof(kconfiguration_statistics)); }

        template<>
        unsigned int elementsof_container(const std::vector<kconfiguration_statistics>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<double> >() { return(sizeof(double)); }

        template<>
        unsigned int elementsof_container(const std::vector<double>& container) { return(container.size()); }

	    }   // namespace linecache -- specializations


	}   // namespace transport


#endif //__configurations_H_
