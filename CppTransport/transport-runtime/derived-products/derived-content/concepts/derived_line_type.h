//
// Created by David Seery on 07/04/2016.
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

#ifndef CPPTRANSPORT_DERIVED_LINE_TYPE_H
#define CPPTRANSPORT_DERIVED_LINE_TYPE_H


namespace transport
  {

    namespace derived_data
      {

        enum class derived_line_type
          {
            background,
            dlogk_twopf_time,
            twopf_time,
            threepf_time,
            tensor_twopf_time,
            tensor_dlogk_twopf_time,
            zeta_twopf_time,
            zeta_ns_time,
            zeta_threepf_time,
            zeta_redbsp_time,
            dlogk_twopf_wavenumber,
            twopf_wavenumber,
            threepf_wavenumber,
            tensor_twopf_wavenumber,
            tensor_dlogk_twopf_wavenumber,
            zeta_twopf_wavenumber,
            zeta_ns_wavenumber,
            zeta_threepf_wavenumber,
            zeta_redbsp_wavenumber,
            fNL_time,
            r_time,
            r_wavenumber,
            background_line,
            u2,
            u3,
            largest_u2,
            largest_u3,
            integration_cost,
          };

      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_LINE_TYPE_H
