//
// Created by David Seery on 20/08/2015.
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

#ifndef CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H
#define CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H


#include <stdexcept>

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
                  {
                    return std::string{CPPTRANSPORT_TWOPF_TYPE_REAL};
                  }

                case twopf_type::imaginary:
                  {
                    return std::string{CPPTRANSPORT_TWOPF_TYPE_IMAGINARY};
                  }

                default:
                  throw std::invalid_argument("Invalid twopf_type");
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
                  {
                    return std::string{CPPTRANSPORT_DOT_TYPE_DERIVATIVES};
                  }

                case dot_type::momenta:
                  {
                    return std::string{CPPTRANSPORT_DOT_TYPE_MOMENTA};
                  }

                default:
                  throw std::invalid_argument("Invalid dot_type");
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
                  {
                    return std::string{CPPTRANSPORT_KLABEL_TYPE_CONVENTIONAL};
                  }

                case klabel_type::comoving:
                  {
                    return std::string{CPPTRANSPORT_KLABEL_TYPE_COMOVING};
                  }

                default:
                  throw std::invalid_argument("Invalid klabel_type");
              }
          }


        //! background objects
        enum class background_quantity
          {
            epsilon, eta, Hubble, aHubble, mass_spectrum, norm_mass_spectrum
          };


        inline std::string background_quantity_to_string(background_quantity object)
          {
            switch(object)
              {
                case background_quantity::epsilon:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_EPSILON};
                  }

                case background_quantity::eta:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_ETA};
                  }

                case background_quantity::Hubble:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_HUBBLE};
                  }

                case background_quantity::aHubble:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_A_HUBBLE};
                  }

                case background_quantity::mass_spectrum:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_MASS_SPECTRUM};
                  }

                case background_quantity::norm_mass_spectrum:
                  {
                    return std::string{CPPTRANSPORT_BACKGROUND_QUANTITY_NORM_MASS_SPECTRUM};
                  }

                default:
                  throw std::invalid_argument("Invalid background_quantity");
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
                  {
                    return std::string{CPPTRANSPORT_LINE_TYPE_SCATTERED};
                  }

                case data_line_type::continuous_data:
                  {
                    return std::string{CPPTRANSPORT_LINE_TYPE_CONTINUOUS};
                  }

                default:
                  throw std::invalid_argument("Invalid data_line_type");
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
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_TOP_LEFT};
                  }

                case legend_pos::top_right:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_TOP_RIGHT};
                  }

                case legend_pos::bottom_left:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_BOTTOM_LEFT};
                  }

                case legend_pos::bottom_right:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_BOTTOM_RIGHT};
                  }

                case legend_pos::centre_left:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_CENTRE_LEFT};
                  }

                case legend_pos::centre_right:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_CENTRE_RIGHT};
                  }

                case legend_pos::upper_centre:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_UPPER_CENTRE};
                  }

                case legend_pos::lower_centre:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_LOWER_CENTRE};
                  }

                case legend_pos::centre:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_CENTRE};
                  }

                case legend_pos::right:
                  {
                    return std::string{CPPTRANSPORT_LEGEND_POS_RIGHT};
                  }

                default:
                  throw std::invalid_argument("Invalid legend_position");
              }
          }


        //! derived lines are associated with one of three classes of axis
        enum class axis_class
          {
            time, wavenumber, threepf_kconfig
          };


        inline std::string axis_class_to_string(axis_class cls)
          {
            switch(cls)
              {
                case axis_class::time:
                  {
                    return std::string{CPPTRANSPORT_AXIS_CLASS_TIME};
                  }

                case axis_class::wavenumber:
                  {
                    return std::string{CPPTRANSPORT_AXIS_CLASS_WAVENUMBER};
                  }

                case axis_class::threepf_kconfig:
                  {
                    return std::string{CPPTRANSPORT_AXIS_CLASS_3PFCONFIG};
                  }

                default:
                  throw std::invalid_argument("Invalid axis_class");
              }
          }


        //! possible axis choices for a given derived line
        enum class axis_value
          {
            efolds,       // time in e-folds, suitable for a time axis
            k,            // wavenumber
            efolds_exit,  // efolds between k* and horizon exit for this mode
            alpha,
            beta,
            squeeze_k1,
            squeeze_k2,
            squeeze_k3,
            unset
          };


        inline std::string axis_value_to_string(axis_value value)
          {
            switch(value)
              {
                case axis_value::efolds:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_EFOLDS};
                  }

                case axis_value::k:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_WAVENUMBER};
                  }

                case axis_value::efolds_exit:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_EFOLDS_EXIT};
                  }

                case axis_value::alpha:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_ALPHA};
                  }

                case axis_value::beta:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_BETA};
                  }

                case axis_value::squeeze_k1:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K1};
                  }

                case axis_value::squeeze_k2:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K2};
                  }

                case axis_value::squeeze_k3:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K3};
                  }

                case axis_value::unset:
                  {
                    return std::string{CPPTRANSPORT_AXIS_VALUE_UNSET};
                  }

                default:
                  throw std::invalid_argument("Invalid axis_value");
              }
          }


        //! possible value choices for a given derived line
        enum class value_type
          {
            field, momentum, correlation_function, fNL,
            r, spectral_index, dimensionless, mass,
            time, steps
          };


        inline std::string value_type_to_string(value_type type)
          {
            switch(type)
              {
                case value_type::field:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_FIELD};
                  }

                case value_type::momentum:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_MOMENTUM};
                  }

                case value_type::correlation_function:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_CORRELATION};
                  }

                case value_type::fNL:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_FNL};
                  }

                case value_type::r:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_R};
                  }

                case value_type::spectral_index:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_SPECTRAL_INDEX};
                  }

                case value_type::dimensionless:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_DIMENSIONLESS};
                  }

                case value_type::mass:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_MASS};
                  }

                case value_type::time:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_TIME};
                  }

                case value_type::steps:
                  {
                    return std::string{CPPTRANSPORT_VALUE_TYPE_STEPS};
                  }

                default:
                  throw std::invalid_argument("Invalid value_type");
              }
          }


        //! analysis type for integration-cost analysis
        enum class analysis_type
          {
            twopf, threepf
          };


        inline std::string analysis_type_to_string(analysis_type type)
          {
            switch(type)
              {
                case analysis_type::twopf:
                  {
                    return std::string{CPPTRANSPORT_ANALYSIS_TYPE_2PF};
                  }

                case analysis_type::threepf:
                  {
                    return std::string{CPPTRANSPORT_ANALYSIS_TYPE_3PF};
                  }

                default:
                  throw std::invalid_argument("Invalid analysis_type");
              }
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCTS_ENUMERATIONS_H
