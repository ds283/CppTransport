//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __configurations_H_
#define __configurations_H_


namespace transport
	{

    // data structures for storing k-configurations

    //! Stores a twopf k-configuration
    class twopf_configuration
	    {
      public:
        unsigned serial;
        double k_conventional;
        double k_comoving;
	    };

    //! Stores a threepf k-configuration
    class threepf_configuration
	    {
      public:
        unsigned serial;
        double kt_conventional;
        double kt_comoving;
        double alpha;
        double beta;
        double k1_conventional;
        double k2_conventional;
        double k3_conventional;
        double k1_comoving;
        double k2_comoving;
        double k3_comoving;
        unsigned int k1_serial;
        unsigned int k2_serial;
        unsigned int k3_serial;
	    };


    // Provide specializations for the size methods used in linecache to compute the size of data elements
    namespace linecache
	    {

        template<>
        unsigned int sizeof_container_element< std::vector<twopf_configuration> >() { return(sizeof(twopf_configuration)); }

        template<>
        unsigned int elementsof_container(const std::vector<twopf_configuration>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<threepf_configuration> >() { return(sizeof(threepf_configuration)); }

        template<>
        unsigned int elementsof_container(const std::vector<threepf_configuration>& container) { return(container.size()); }

        template<>
        unsigned int sizeof_container_element< std::vector<double> >() { return(sizeof(double)); }

        template<>
        unsigned int elementsof_container(const std::vector<double>& container) { return(container.size()); }

	    }   // namespace linecache -- specializations


	}   // namespace transport


#endif //__configurations_H_
