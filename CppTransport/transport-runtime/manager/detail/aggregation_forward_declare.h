//
// Created by David Seery on 22/01/2016.
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

#if !defined(CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H) && !defined(CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION)
#define CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H


namespace transport
  {

    namespace master_controller_impl
      {

        template <typename number> class integration_aggregator;
        template <typename number> class postintegration_aggregator;
        template <typename number> class derived_content_aggregator;

        class aggregation_record;
        template <typename number> class integration_aggregation_record;
        template <typename number> class postintegration_aggregation_record;
        template <typename number> class derived_content_aggregation_record;

      }   // namespace master_controller_impl

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H
