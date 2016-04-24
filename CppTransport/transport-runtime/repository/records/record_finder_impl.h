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

#ifndef CPPTRANSPORT_RECORD_FINDER_IMPL_H
#define CPPTRANSPORT_RECORD_FINDER_IMPL_H


#include "transport-runtime/repository/records/record_finder_decl.h"


namespace transport
  {


    template <typename number>
    std::unique_ptr< package_record<number> > package_finder<number>::operator()(const std::string& name)
      {
        // query_*() methods without a transaction manager always return readonly records
        return this->repo.query_package(name);
      }


    template <typename number>
    std::unique_ptr< task_record<number> > task_finder<number>::operator()(const std::string& name)
      {
        // query_*() methods without a transaction manager always return readonly records
        return this->repo.query_task(name);
      }


    template <typename number>
    std::unique_ptr< derived_product_record<number> > derived_product_finder<number>::operator()(const std::string& name)
      {
        // query_*() methods without a transaction manager always return readonly records
        return this->repo.query_derived_product(name);
      }

  }


#endif //CPPTRANSPORT_RECORD_FINDER_IMPL_H
