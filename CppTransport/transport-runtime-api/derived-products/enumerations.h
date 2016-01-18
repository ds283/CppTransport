//
// Created by David Seery on 20/08/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H
#define CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H


namespace transport
  {

    namespace derived_data
      {


        //! which type of twopf are we using?
        enum class twopf_type { real, imaginary };

        //! do dots mean time derivatives or moments?
        enum class dot_type { derivatives, momenta };

        //! are wavenumbers expressed in conventional or comoving units?
        enum class klabel_type { conventional, comoving };

        //! background objects
        enum class background_quantity { epsilon, Hubble, aHubble };

        //! select scattered or continuous data line
        enum class data_line_type
          {
            scattered_data, continuous_data
          };

        //! select position of legend on plots
        enum class legend_pos
          {
            top_left, top_right, bottom_left, bottom_right,
            right, centre_right, centre_left,
            upper_centre, lower_centre, centre
          };

        //! derived lines are associated with one a three classes of axis
        enum class axis_class
          {
            time_axis, wavenumber_axis, threepf_kconfig_axis
          };

        //! possible axis choices for a given derived line
        enum class axis_value
          {
            efolds_axis, k_axis, efolds_exit_axis, alpha_axis, beta_axis,
            squeezing_fraction_k1_axis,
            squeezing_fraction_k2_axis,
            squeezing_fraction_k3_axis,
            unset_axis
          };

        //! possible value choices for a given derived line
        enum class value_type
          {
            field_value, momentum_value, correlation_function_value, fNL_value,
            r_value, spectral_index_value, dimensionless_value,
            time_value, steps_value
          };

        //! analysis type for integration-cost analysis
        enum class analysis_type { twopf_analysis, threepf_analysis };


      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H
