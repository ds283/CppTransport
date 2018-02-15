//
// Created by David Seery on 25/05/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_LINE_COLLECTION_PRODUCT_H
#define CPPTRANSPORT_MESSAGES_EN_LINE_COLLECTION_PRODUCT_H


#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_A   "General line collection"
#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_B   "containing the following line groups:"
#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES   "Plotting indices"

#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LINE      "Line"

#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGX      "logarithmic x-axis"
#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGY      "logarithmic y-axis"
#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_ABSY      "absolute values on y-axis"
#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LATEX     "use LaTeX-format labels"

#define CPPTRANSPORT_PRODUCT_LINE_COLLECTION_AXIS_MISMATCH   "line_collection: attempt to add new line group with axis type which doesn't agree with existing axes"


#define CPPTRANSPORT_TWOPF_TYPE_REAL                         "real"
#define CPPTRANSPORT_TWOPF_TYPE_IMAGINARY                    "imaginary"
#define CPPTRANSPORT_DOT_TYPE_DERIVATIVES                    "time derivatives"
#define CPPTRANSPORT_DOT_TYPE_MOMENTA                        "canonical momenta"
#define CPPTRANSPORT_KLABEL_TYPE_CONVENTIONAL                "conventional"
#define CPPTRANSPORT_KLABEL_TYPE_COMOVING                    "comoving"
#define CPPTRANSPORT_BACKGROUND_QUANTITY_EPSILON             "epsilon"
#define CPPTRANSPORT_BACKGROUND_QUANTITY_ETA                 "eta"
#define CPPTRANSPORT_BACKGROUND_QUANTITY_HUBBLE              "H"
#define CPPTRANSPORT_BACKGROUND_QUANTITY_A_HUBBLE            "aH"
#define CPPTRANSPORT_LINE_TYPE_SCATTERED                     "scattered"
#define CPPTRANSPORT_LINE_TYPE_CONTINUOUS                    "continuous"

#define CPPTRANSPORT_LEGEND_POS_TOP_LEFT                     "top left"
#define CPPTRANSPORT_LEGEND_POS_TOP_RIGHT                    "top right"
#define CPPTRANSPORT_LEGEND_POS_BOTTOM_LEFT                  "bottom left"
#define CPPTRANSPORT_LEGEND_POS_BOTTOM_RIGHT                 "bottom right"
#define CPPTRANSPORT_LEGEND_POS_CENTRE_LEFT                  "centre left"
#define CPPTRANSPORT_LEGEND_POS_CENTRE_RIGHT                 "centre right"
#define CPPTRANSPORT_LEGEND_POS_UPPER_CENTRE                 "upper centre"
#define CPPTRANSPORT_LEGEND_POS_LOWER_CENTRE                 "lower centre"
#define CPPTRANSPORT_LEGEND_POS_CENTRE                       "centre"
#define CPPTRANSPORT_LEGEND_POS_RIGHT                        "right"

#define CPPTRANSPORT_AXIS_CLASS_TIME                         "time"
#define CPPTRANSPORT_AXIS_CLASS_WAVENUMBER                   "wavenumber"
#define CPPTRANSPORT_AXIS_CLASS_3PFCONFIG                    "3pf configuration"

#define CPPTRANSPORT_AXIS_VALUE_EFOLDS                       "e-folds"
#define CPPTRANSPORT_AXIS_VALUE_WAVENUMBER                   "wavenumber"
#define CPPTRANSPORT_AXIS_VALUE_EFOLDS_EXIT                  "horizon-exit time"
#define CPPTRANSPORT_AXIS_VALUE_ALPHA                        "alpha"
#define CPPTRANSPORT_AXIS_VALUE_BETA                         "beta"
#define CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K1                 "squeezing k1/kt"
#define CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K2                 "squeezing k2/kt"
#define CPPTRANSPORT_AXIS_VALUE_SQUEEZING_K3                 "squeezing k3/kt"
#define CPPTRANSPORT_AXIS_VALUE_UNSET                        "unset"

#define CPPTRANSPORT_VALUE_TYPE_FIELD                        "field"
#define CPPTRANSPORT_VALUE_TYPE_MOMENTUM                     "momentum"
#define CPPTRANSPORT_VALUE_TYPE_CORRELATION                  "correlation function"
#define CPPTRANSPORT_VALUE_TYPE_FNL                          "fNL-like"
#define CPPTRANSPORT_VALUE_TYPE_R                            "tensor-scalar ratio"
#define CPPTRANSPORT_VALUE_TYPE_SPECTRAL_INDEX               "spectral index"
#define CPPTRANSPORT_VALUE_TYPE_DIMENSIONLESS                "dimensionless quantity"
#define CPPTRANSPORT_VALUE_TYPE_TIME                         "time"
#define CPPTRANSPORT_VALUE_TYPE_STEPS                        "number of steps"

#define CPPTRANSPORT_ANALYSIS_TYPE_2PF                       "2pf"
#define CPPTRANSPORT_ANALYSIS_TYPE_3PF                       "3pf"

#endif // CPPTRANSPORT_MESSAGES_EN_LINE_COLLECTION_PRODUCT_H
