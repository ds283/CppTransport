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


#ifndef CPPTRANSPORT_CONFIGURATIONS_H
#define CPPTRANSPORT_CONFIGURATIONS_H


namespace transport
	{


    // Provide specializations for the size methods used in linecache to compute the size of data elements
    namespace linecache
	    {
        
        // template for any container implementing the STL container interface
        template <typename Container>
        unsigned int elementsof_container(const Container& c) { return c.size(); }
        
        // template for any container providing a value_type type
        template <typename Container>
        unsigned int sizeof_container_element() { return sizeof(typename Container::value_type); }

	    }   // namespace linecache -- specializations


	}   // namespace transport


#endif //CPPTRANSPORT_CONFIGURATIONS_H
