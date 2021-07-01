//
// Created by David Seery on 01/07/2021.
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

#ifndef CPPTRANSPORT_REPOSITORY_RECORD_DATABASES_H
#define CPPTRANSPORT_REPOSITORY_RECORD_DATABASES_H


#include <unordered_map>


namespace transport
  {

    //! database type for packages
    template <typename number>
    struct package_db
      {
        using type = std::unordered_map< std::string, std::unique_ptr< package_record<number> > >;
        using value_type = typename type::value_type ;
      };

    //! database type for tasks
    template <typename number>
    struct task_db
      {
        using type = std::unordered_map< std::string, std::unique_ptr< task_record<number> > >;
        using value_type = typename type::value_type;
      };

    //! database type for derived products
    template <typename number>
    struct derived_product_db
      {
        using type = std::unordered_map< std::string, std::unique_ptr< derived_product_record<number> > >;
        using value_type = typename type::value_type;
      };


    template <typename Payload>
    class content_group_record_set
      {
      public:
        using type = std::map< std::string, std::unique_ptr< content_group_record<Payload> > >;
        using value_type = typename type::value_type;
      };


    //! database type for integration content groups
    using integration_content_db = typename content_group_record_set<integration_payload>::type;

    //! database type for postintegration content groups
    using postintegration_content_db = typename content_group_record_set<postintegration_payload>::type;

    //! database type for output content groups
    using output_content_db = typename content_group_record_set<output_payload>::type;

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_RECORD_DATABASES_H
