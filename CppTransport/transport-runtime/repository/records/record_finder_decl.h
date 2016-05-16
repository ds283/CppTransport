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


#ifndef CPPTRANSPORT_RECORD_FINDER_DECL_H
#define CPPTRANSPORT_RECORD_FINDER_DECL_H


#include <string>
#include <functional>

// forward-declare repository records if necessary
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

#include "transport-runtime/repository/repository_forward_declare.h"

namespace transport
	{

    template <typename number>
    class package_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        package_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~package_finder() = default;


        // INTERFACE

      public:

        //! find package
        std::unique_ptr< package_record<number> > operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class task_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        task_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~task_finder() = default;


        // INTERFACE

      public:

        //! find package
        std::unique_ptr< task_record<number> > operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class derived_product_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        derived_product_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~derived_product_finder() = default;


        // INTERFACE

      public:

        //! find package
        std::unique_ptr< derived_product_record<number> > operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };

	}   // namespace transport


#endif //CPPTRANSPORT_RECORD_FINDER_DECL_H
