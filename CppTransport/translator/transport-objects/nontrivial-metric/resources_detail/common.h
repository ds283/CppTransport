//
// Created by David Seery on 04/07/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_COMMON_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_COMMON_H


namespace nontrivial_metric
  {

    namespace resource_impl
      {

        const auto I_INDEX_NAME = "i";
        const auto J_INDEX_NAME = "j";
        const auto K_INDEX_NAME = "k";

      }   // namespace resource_impl


    using resource_impl::I_INDEX_NAME;
    using resource_impl::J_INDEX_NAME;
    using resource_impl::K_INDEX_NAME;
  
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_COMMON_H
