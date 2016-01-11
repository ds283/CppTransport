//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H
#define CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H


#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_CONTENT_TYPE_UNKNOWN               "Internal error: Cannot deserialize unknown 'derived_line' content type"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN                 "Internal error: Unknown twopf type in 'derived_line'"


#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_LABEL                              "Derived content:"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME                          "Derived from task"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME                         "model"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_CLASS                       "axis data represent"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_TIME_LABEL                   "time"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_WAVENUMBER_LABEL             "k-value"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_THREEPF_CONFIGURATION_LABEL  "threepf configuration"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_SUPPORTED                   "supported x-axis values:"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS                     "current x-axis value:"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS                "e-folds"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K                     "k-value"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS_EXIT           "efolds from horizone exit"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_ALPHA                 "alpha"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_BETA                  "beta"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K1          "k1/kt"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K2          "k2/kt"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_SQUEEZING_K3          "k3/kt"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING                        "correlation functions involve"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE                     "field derivatives"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA                        "momenta"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING                     "wavenumber normalization is"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING                    "comoving"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL                "conventional"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_CLASS               "derived_line error: unknown x-axis class"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE               "derived_line error: unknown x-axis value"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN                   "derived_line error: unknown dot meaning"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN                "derived_line error: unknown klabel meaning"

#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_NOT_DERIVABLE_TASK                 "derived_line error: cannot derive data from a non-derivable task"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_AXIS_TYPES                      "derived_line error: no supported axis types"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_NOT_SUPPORTED               "derived_line error: unsupported x-axis type"
#define CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNSUPPORTED_X_AXIS_TYPE            "derived_line error: attempt to set x-axis to unsupported type"


#endif // CPPTRANSPORT_MESSAGES_EN_DERIVED_LINE_H
