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
        enum class twopf_type
          {
            real, imaginary
          };

        inline std::string twopf_type_to_string(twopf_type type)
          {
            switch(type)
              {
                case twopf_type::real:
                  return std::string(CPPTRANSPORT_TWOPF_TYPE_REAL);

                case twopf_type::imaginary:
                  return std::string(CPPTRANSPORT_TWOPF_TYPE_IMAGINARY);
              }
          }

        //! do dots mean time derivatives or moments?
        enum class dot_type
          {
            derivatives, momenta
          };

        inline std::string dot_type_to_string(dot_type type)
          {
            switch(type)
              {
                case dot_type::derivatives:
                  return std::string(CPPTRANSPORT_DOT_TYPE_DERIVATIVES);

                case dot_type::momenta:
                  return std::string(CPPTRANSPORT_DOT_TYPE_MOMENTA);
              }
          }

        //! are wavenumbers expressed in conventional or comoving units?
        enum class klabel_type
          {
            conventional, comoving
          };

        inline std::string klabel_type_to_string(klabel_type type)
          {
            switch(type)
              {
                case klabel_type::conventional:
                  return std::string(CPPTRANSPORT_KLABEL_TYPE_CONVENTIONAL);

                case klabel_type::comoving:
                  return std::string(CPPTRANSPORT_KLABEL_TYPE_COMOVING);
              }
          }

        //! background objects
        enum class background_quantity
          {
            epsilon, Hubble, aHubble
          };

        inline std::string background_quantity_to_string(background_quantity object)
          {
            switch(object)
              {
                case background_quantity::epsilon:
                  return std::string(CPPTRANSPORT_BACKGROUND_QUANTITY_EPSILON);

                case background_quantity::Hubble:
                  return std::string(CPPTRANSPORT_BACKGROUND_QUANTITY_HUBBLE);

                case background_quantity::aHubble:
                  return std::string(CPPTRANSPORT_BACKGROUND_QUANTITY_A_HUBBLE);
              }
          }

        //! select scattered or continuous data line
        enum class data_line_type
          {
            scattered_data, continuous_data
          };

        inline std::string data_line_type_to_string(data_line_type type)
          {
            switch(type)
              {
                case data_line_type::scattered_data:
                  return std::string(CPPTRANSPORT_LINE_TYPE_SCATTERED);

                case data_line_type::continuous_data:
                  return std::string(CPPTRANSPORT_LINE_TYPE_CONTINUOUS);
              }
          }

        //! select position of legend on plots
        enum class legend_pos
          {
            top_left, top_right, bottom_left, bottom_right,
            right, centre_right, centre_left,
            upper_centre, lower_centre, centre
          };

        inline std::string legend_pos_to_string(legend_pos pos)
          {
            switch(pos)
              {
                case legend_pos::top_left:
                  return std::string(CPPTRANSPORT_LEGEND_POS_TOP_LEFT);

                case legend_pos::top_right:
                  return std::string(CPPTRANSPORT_LEGEND_POS_TOP_RIGHT);

                case legend_pos::bottom_left:
                  return std::string(CPPTRANSPORT_LEGEND_POS_BOTTOM_LEFT);

                case legend_pos::bottom_right:
                  return std::string(CPPTRANSPORT_LEGEND_POS_BOTTOM_RIGHT);

                case legend_pos::centre_left:
                  return std::string(CPPTRANSPORT_LEGEND_POS_CENTRE_LEFT);

                case legend_pos::centre_right:
                  return std::string(CPPTRANSPORT_LEGEND_POS_CENTRE_RIGHT);

                case legend_pos::upper_centre:
                  return std::string(CPPTRANSPORT_LEGEND_POS_UPPER_CENTRE);

                case legend_pos::lower_centre:
                  return std::string(CPPTRANSPORT_LEGEND_POS_LOWER_CENTRE);

                case legend_pos::centre:
                  return std::string(CPPTRANSPORT_LEGEND_POS_CENTRE);

                case legend_pos::right:
                  return std::string(CPPTRANSPORT_LEGEND_POS_RIGHT);
              }
          }

        //! derived lines are associated with one of three classes of axis
        enum class axis_class
          {
            time_axis, wavenumber_axis, threepf_kconfig_axis
          };

        inline std::string axis_class_to_string(axis_class cls)
          {
            switch(cls)
              {
                case axis_class::time_axis:
                  return std::string(CPPTRANSPORT_AXIS_CLASS_TIME);

                case axis_class::wavenumber_axis:
                  return std::string(CPPTRANSPORT_AXIS_CLASS_WAVENUMBER);

                case axis_class::threepf_kconfig_axis:
                  return std::string(CPPTRANSPORT_AXIS_CLASS_3PFCONFIG);
              }
          }

        //! possible axis choices for a given derived line
        enum class axis_value
          {
            efolds_axis, k_axis, efolds_exit_axis, alpha_axis, beta_axis,
            squeezing_fraction_k1_axis,
            squeezing_fraction_k2_axis,
            squeezing_fraction_k3_axis,
            unset_axis
          };

        inline std::string axis_value_to_string(axis_value value)
          {
            switch(value)
              {
                case axis_value::efolds_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_EFOLDS);

                case axis_value::k_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_WAVENUMBER);

                case axis_value::efolds_exit_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_EFOLDS_EXIT);

                case axis_value::alpha_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_ALPHA);

                case axis_value::beta_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_BETA);

                case axis_value::squeezing_fraction_k1_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K1);

                case axis_value::squeezing_fraction_k2_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K2);

                case axis_value::squeezing_fraction_k3_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K3);

                case axis_value::unset_axis:
                  return std::string(CPPTRANSPORT_AXIS_VALUE_UNSET);
              }
          }

        //! possible value choices for a given derived line
        enum class value_type
          {
            field_value, momentum_value, correlation_function_value, fNL_value,
            r_value, spectral_index_value, dimensionless_value,
            time_value, steps_value
          };

        inline std::string value_type_to_string(value_type type)
          {
            switch(type)
              {
                case value_type::field_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_FIELD);

                case value_type::momentum_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_MOMENTUM);

                case value_type::correlation_function_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_CORRELATION);

                case value_type::fNL_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_FNL);

                case value_type::r_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_R);

                case value_type::spectral_index_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_SPECTRAL_INDEX);

                case value_type::dimensionless_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_DIMENSIONLESS);

                case value_type::time_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_TIME);

                case value_type::steps_value:
                  return std::string(CPPTRANSPORT_VALUE_TYPE_STEPS);
              }
          }

        //! analysis type for integration-cost analysis
        enum class analysis_type
          {
            twopf_analysis, threepf_analysis
          };

        inline std::string analysis_type_to_string(analysis_type type)
          {
            switch(type)
              {
                case analysis_type::twopf_analysis:
                  return std::string(CPPTRANSPORT_ANALYSIS_TYPE_2PF);

                case analysis_type::threepf_analysis:
                  return std::string(CPPTRANSPORT_ANALYSIS_TYPE_3PF);
              }
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H
