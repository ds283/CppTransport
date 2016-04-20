//
// Created by David Seery on 07/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
            twopf_time,
            threepf_time,
            tensor_twopf_time,
            zeta_twopf_time,
            zeta_threepf_time,
            zeta_redbsp_time,
            twopf_wavenumber,
            threepf_wavenumber,
            tensor_twopf_wavenumber,
            zeta_twopf_wavenumber,
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
