//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_CONTENT_FINDER_DECL_H
#define CPPTRANSPORT_CONTENT_FINDER_DECL_H


#include <string>
#include <functional>
#include <list>

// forward-declare repository records if necessary
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

#include "transport-runtime/repository/repository_forward_declare.h"


namespace transport
  {

    template <typename number>
    class integration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        explicit integration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~integration_content_finder() = default;


        // INTERFACE

      public:

        //! find content group
        std::unique_ptr< content_group_record<integration_payload> > operator()(const std::string& name, const tag_list& tags);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class postintegration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        explicit postintegration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~postintegration_content_finder() = default;


        // INTERFACE

      public:

        //! find content group
        std::unique_ptr< content_group_record<postintegration_payload> > operator()(const std::string& name, const tag_list& tags);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_FINDER_DECL_H
