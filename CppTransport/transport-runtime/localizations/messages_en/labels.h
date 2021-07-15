//
// Created by David Seery on 05/05/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_LABELS_H
#define CPPTRANSPORT_MESSAGES_EN_LABELS_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_YES                                      = "yes";
    constexpr auto CPPTRANSPORT_NO                                       = "no";
    constexpr auto CPPTRANSPORT_WITH                                     = "with";

    constexpr auto CPPTRANSPORT_EFOLDS                                   = "e-folds";

    constexpr auto CPPTRANSPORT_WEEK                                     = "w";
    constexpr auto CPPTRANSPORT_DAY                                      = "d";
    constexpr auto CPPTRANSPORT_HOUR                                     = "h";
    constexpr auto CPPTRANSPORT_MINUTE                                   = "m";
    constexpr auto CPPTRANSPORT_SECOND                                   = "s";

    constexpr auto CPPTRANSPORT_GIGABYTE                                 = "Gb";
    constexpr auto CPPTRANSPORT_MEGABYTE                                 = "Mb";
    constexpr auto CPPTRANSPORT_KILOBYTE                                 = "kb";
    constexpr auto CPPTRANSPORT_BYTE                                     = "b";

    constexpr auto CPPTRANSPORT_KCONFIG_SERIAL                           = "serial =";
    constexpr auto CPPTRANSPORT_KCONFIG_KEQUALS                          = "comoving k =";
    constexpr auto CPPTRANSPORT_KCONFIG_KTEQUALS                         = "comoving k_t =";
    constexpr auto CPPTRANSPORT_KCONFIG_ALPHAEQUALS                      = "alpha =";
    constexpr auto CPPTRANSPORT_KCONFIG_BETAEQUALS                       = "beta =";
    constexpr auto CPPTRANSPORT_KCONFIG_T_EXIT                           = "t_exit =";
    constexpr auto CPPTRANSPORT_KCONFIG_T_MASSLESS                       = "t_massless =";

    constexpr auto CPPTRANSPORT_TIME_CONFIG_SERIAL                       = "serial =";
    constexpr auto CPPTRANSPORT_TIME_CONFIG_TEQUALS                      = "t =";

    constexpr auto CPPTRANSPORT_ADAPTIVE_ICS                             = "Adaptive initial conditions";
    constexpr auto CPPTRANSPORT_MESH_REFINEMENTS                         = "Maximum number of mesh refinements";
    constexpr auto CPPTRANSPORT_COLLECTING_ICS                           = "Collecting initial conditions";
    constexpr auto CPPTRANSPORT_COLLECTING_SPECTRAL_DATA                 = "Collecting spectral data";
    constexpr auto CPPTRANSPORT_PARENT_TASK                              = "Parent task";

    constexpr auto CPPTRANSPORT_CONTENT_GROUP                            = "Content group";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_LOCKED                     = "locked";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_DATA_ROOT                  = "Content group root directory";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_REPO_ROOT                  = "Repository root directory";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_CREATED                    = "Creation date";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_NOTE                       = "Note";
    constexpr auto CPPTRANSPORT_CONTENT_GROUP_TAGS                       = "tags";

    constexpr auto CPPTRANSPORT_PAYLOAD_INTEGRATION_BACKEND              = "Created by backend";
    constexpr auto CPPTRANSPORT_PAYLOAD_INTEGRATION_DATA                 = "Data container";
    constexpr auto CPPTRANSPORT_PAYLOAD_OUTPUT_CONTENT_PRODUCT           = "Derived product";
    constexpr auto CPPTRANSPORT_PAYLOAD_OUTPUT_CONTENT_PATH              = "filename";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_ZETA_TWO                     = "Cached zeta twopf";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_ZETA_THREE                   = "Cached zeta threepf";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_ZETA_TWO_SPECTRAL            = "Cached zeta twopf spectral index";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_ZETA_REDBSP                  = "Cached zeta reduced bispectrum";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_FNL_LOCAL                    = "Cached fNL_local";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_FNL_EQUI                     = "Cached fNL_equi";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_FNL_ORTHO                    = "Cached fNL_ortho";
    constexpr auto CPPTRANSPORT_PAYLOAD_HAS_FNL_DBI                      = "Cached fNL_DBI";

    constexpr auto CPPTRANSPORT_DATAPRODUCT_NAME                         = "Data product";
    constexpr auto CPPTRANSPORT_DATAPRODUCT_PATH                         = "path to product";
    constexpr auto CPPTRANSPORT_DATAPRODUCT_CREATED                      = "creation date";

    constexpr auto CPPTRANSPORT_LATEX_PRIME_SYMBOL                       = R"(\prime)";
    constexpr auto CPPTRANSPORT_LATEX_PRIME_SYMBOL_SUPERSCRIPT           = R"(^{\prime})";
    constexpr auto CPPTRANSPORT_NONLATEX_PRIME_SYMBOL                    = "'";

    constexpr auto CPPTRANSPORT_LATEX_K_SYMBOL                           = "k";
    constexpr auto CPPTRANSPORT_NONLATEX_K_SYMBOL                        = "k";
    constexpr auto CPPTRANSPORT_LATEX_T_SYMBOL                           = "N";
    constexpr auto CPPTRANSPORT_NONLATEX_T_SYMBOL                        = "N";
    constexpr auto CPPTRANSPORT_LATEX_KT_SYMBOL                          = "k_t";
    constexpr auto CPPTRANSPORT_NONLATEX_KT_SYMBOL                       = "k_t";
    constexpr auto CPPTRANSPORT_LATEX_ALPHA_SYMBOL                       = R"(\alpha)";
    constexpr auto CPPTRANSPORT_NONLATEX_ALPHA_SYMBOL                    = "alpha";
    constexpr auto CPPTRANSPORT_LATEX_BETA_SYMBOL                        = R"(\beta)";
    constexpr auto CPPTRANSPORT_NONLATEX_BETA_SYMBOL                     = "beta";

    constexpr auto CPPTRANSPORT_LATEX_RE_SYMBOL                          = R"(\mathrm{Re}\,)";
    constexpr auto CPPTRANSPORT_NONLATEX_RE_SYMBOL                       = "Re";
    constexpr auto CPPTRANSPORT_LATEX_IM_SYMBOL                          = R"(\mathrm{Im}\,)";
    constexpr auto CPPTRANSPORT_NONLATEX_IM_SYMBOL                       = "Im";

    constexpr auto CPPTRANSPORT_LATEX_ZETA_SYMBOL                        = R"(\zeta)";
    constexpr auto CPPTRANSPORT_NONLATEX_ZETA_SYMBOL                     = "zeta";
    constexpr auto CPPTRANSPORT_LATEX_TENSOR_SYMBOL                      = "h";
    constexpr auto CPPTRANSPORT_NONLATEX_TENSOR_SYMBOL                   = "h";
    constexpr auto CPPTRANSPORT_LATEX_TENSOR_MOMENTUM_SYMBOL             = R"(\dot{h})";
    constexpr auto CPPTRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL          = "dot(h)";
    constexpr auto CPPTRANSPORT_LATEX_R_SYMBOL                           = "r";
    constexpr auto CPPTRANSPORT_NONLATEX_R_SYMBOL                        = "r";

    constexpr auto CPPTRANSPORT_LATEX_DIMENSIONLESS_PZETA_SYMBOL         = R"(\mathcal{P}_{\zeta})";
    constexpr auto CPPTRANSPORT_NONLATEX_DIMENSIONLESS_PZETA_SYMBOL      = "cal(P)_zeta";
    constexpr auto CPPTRANSPORT_LATEX_DIMENSIONLESS_TWOPF_SYMBOL         = R"(\Delta)";
    constexpr auto CPPTRANSPORT_NONLATEX_DIMENSIONLESS_TWOPF_SYMBOL      = "Delta";

    constexpr auto CPPTRANSPORT_LATEX_PZETA_NS_SYMBOL                    = "n_s";
    constexpr auto CPPTRANSPORT_NONLATEX_PZETA_NS_SYMBOL                 = "n_s";
    constexpr auto CPPTRANSPORT_LATEX_TENSOR_NT_SYMBOL                   = "n_t";
    constexpr auto CPPTRANSPORT_NONLATEX_TENSOR_NT_SYMBOL                = "n_t";

    constexpr auto CPPTRANSPORT_LATEX_SPECTRAL_INDEX                     = "(spectral index)";
    constexpr auto CPPTRANSPORT_NONLATEX_SPECTRAL_INDEX                  = "(spectral index)";

    constexpr auto CPPTRANSPORT_LATEX_SHAPE_FACTOR                       = "(k_1 k_2 k_3)^2";
    constexpr auto CPPTRANSPORT_NONLATEX_SHAPE_FACTOR                    = "(k_1 k_2 k_3)^2";

    constexpr auto CPPTRANSPORT_LATEX_REDUCED_BISPECTRUM_SYMBOL          = R"(f_{\mathrm{NL}}(k_1, k_2, k_3))";
    constexpr auto CPPTRANSPORT_NONLATEX_REDUCED_BISPECTRUM_SYMBOL       = "fNL(k1, k2, k3)";

    constexpr auto CPPTRANSPORT_LATEX_FNL_LOCAL_SYMBOL                   = R"(f_{\mathrm{NL}}^{\mathrm{local}})";
    constexpr auto CPPTRANSPORT_NONLATEX_FNL_LOCAL_SYMBOL                = "fNL_local";
    constexpr auto CPPTRANSPORT_FNL_LOCAL                                = "fNL_local";

    constexpr auto CPPTRANSPORT_LATEX_FNL_EQUI_SYMBOL                    = R"(f_{\mathrm{NL}}^{\mathrm{equi}})";
    constexpr auto CPPTRANSPORT_NONLATEX_FNL_EQUI_SYMBOL                 = "fNL_equi";
    constexpr auto CPPTRANSPORT_FNL_EQUI                                 = "fNL_equi";

    constexpr auto CPPTRANSPORT_LATEX_FNL_ORTHO_SYMBOL                   = R"(f_{\mathrm{NL}}^{\mathrm{ortho}})";
    constexpr auto CPPTRANSPORT_NONLATEX_FNL_ORTHO_SYMBOL                = "fNL_ortho";
    constexpr auto CPPTRANSPORT_FNL_ORTHO                                = "fNL_ortho";

    constexpr auto CPPTRANSPORT_LATEX_FNL_DBI_SYMBOL                     = R"(f_{\mathrm{NL}}^{\mathrm{DBI}})";
    constexpr auto CPPTRANSPORT_NONLATEX_FNL_DBI_SYMBOL                  = "fNL_DBI";
    constexpr auto CPPTRANSPORT_FNL_DBI                                  = "fNL_DBI";

    constexpr auto CPPTRANSPORT_LATEX_TIME_SYMBOL                        = R"(\mathrm{integration time})";
    constexpr auto CPPTRANSPORT_NONLATEX_TIME_SYMBOL                     = "integration time";
    constexpr auto CPPTRANSPORT_LATEX_STEPS_SYMBOL                       = R"(\mathrm{integration steps})";
    constexpr auto CPPTRANSPORT_NONLATEX_STEPS_SYMBOL                    = "integration steps";

    constexpr auto CPPTRANSPORT_LATEX_EPSILON_SYMBOL                     = R"(\epsilon)";
    constexpr auto CPPTRANSPORT_NONLATEX_EPSILON_SYMBOL                  = "epsilon";
    constexpr auto CPPTRANSPORT_LATEX_ETA_SYMBOL                         = R"(\eta)";
    constexpr auto CPPTRANSPORT_NONLATEX_ETA_SYMBOL                      = "eta";
    constexpr auto CPPTRANSPORT_LATEX_HUBBLE_SYMBOL                      = "H";
    constexpr auto CPPTRANSPORT_NONLATEX_HUBBLE_SYMBOL                   = "H";
    constexpr auto CPPTRANSPORT_LATEX_A_HUBBLE_SYMBOL                    = "aH";
    constexpr auto CPPTRANSPORT_NONLATEX_A_HUBBLE_SYMBOL                 = "aH";
    constexpr auto CPPTRANSPORT_LATEX_MASS_SPECTRUM_SYMBOL               = "M_i";
    constexpr auto CPPTRANSPORT_NONLATEX_MASS_SPECTRUM_SYMBOL            = "M_i";
    constexpr auto CPPTRANSPORT_LATEX_NORM_MASS_SPECTRUM_SYMBOL          = "M_i/H";
    constexpr auto CPPTRANSPORT_NONLATEX_NORM_MASS_SPECTRUM_SYMBOL       = "M_i/H";
    constexpr auto CPPTRANSPORT_MASS_SPECTRUM_SYMBOL                     = "m";

    constexpr auto CPPTRANSPORT_LATEX_U2_SYMBOL                          = "u";
    constexpr auto CPPTRANSPORT_NONLATEX_U2_SYMBOL                       = "u";
    constexpr auto CPPTRANSPORT_LATEX_U3_SYMBOL                          = "u";
    constexpr auto CPPTRANSPORT_NONLATEX_U3_SYMBOL                       = "u";

    constexpr auto CPPTRANSPORT_LATEX_LARGEST_U2_SYMBOL                  = R"(\mathrm{max} (u_{\bar{\alpha}\beta}))";
    constexpr auto CPPTRANSPORT_NONLATEX_LARGEST_U2_SYMBOL               = "max u_ij";
    constexpr auto CPPTRANSPORT_LATEX_LARGEST_U3_SYMBOL                  = R"(\mathrm{max} (u_{abc}))";
    constexpr auto CPPTRANSPORT_NONLATEX_LARGEST_U3_SYMBOL               = "max u_ijk";

  }   // namespace transport



#endif // CPPTRANSPORT_MESSAGES_EN_LABELS_H
