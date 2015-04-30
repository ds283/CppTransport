//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
