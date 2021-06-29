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

#ifndef CPPTRANSPORT_CONTENT_FINDER_IMPL_H
#define CPPTRANSPORT_CONTENT_FINDER_IMPL_H

#include "transport-runtime/repository/records/content_finder_decl.h"


namespace transport
  {


    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> >
    integration_content_finder<number>::operator()(const std::string& name, const tag_list& tags)
      {
        return this->repo.find_integration_task_output(name, tags);
      }


    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> >
    postintegration_content_finder<number>::operator()(const std::string& name, const tag_list& tags)
      {
        return this->repo.find_postintegration_task_output(name, tags);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_FINDER_IMPL_H
