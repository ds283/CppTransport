//
// Created by David Seery on 21/06/2021.
// --@@
// Copyright (c) 2021 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_COMMON_H
#define CPPTRANSPORT_COMMON_H


#include <set>
#include <list>
#include <unordered_set>


namespace transport
  {

    //! data type for holding a list of missing serial numbers, used during integrity testing
    using serial_number_list = std::set<unsigned int>;

    //! tag list
    using tag_list = std::list<std::string>;


    //! content group list
    using content_group_name_set = std::unordered_set<std::string>;

    //! group of record names
    using record_name_set = std::unordered_set<std::string>;

    //! ordered group of record names
    using ordered_record_name_set = std::list<std::string>;


  }   // namespace transport


#endif //CPPTRANSPORT_COMMON_H
