//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_GENERAL_TPLOT_PRODUCT_H
#define __CPP_TRANSPORT_MESSAGES_EN_GENERAL_TPLOT_PRODUCT_H


#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH      "Internal error: unexpected number of fields in index selector object"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_A    "received"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_B    "expected"

#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LINE_TYPE_UNKNOWN   "Internal error: Cannot deserialize unknown 'general_time_plot' line type"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TWOPF_DATA_UNKNOWN  "Internal error: Unknown twopf type in 'general_time_plot'"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_THREEPF_DOT_UNKNOWN "Internal error: Unknown threepf type in 'general_time_plot'"

#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TITLE_A       "General time-line plot"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TITLE_B       "containing the following plot groups:"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_INDICES       "Plotting indices"

#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_BACKGROUND    "background line group"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TWOPF         "twopf line group"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_THREEPF       "threepf line group"

#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_KCONFIG_SN_LABEL    "k-configurations: including serial numbers"

#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_DATAPIPE       "Internal error: Null datapipe when attempting to build background plot"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL          "Internal error: Null model in 'general_time_plot'"
#define __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER         "Internal error: Null serialization reader in 'general_time_plot'"


#endif // __CPP_TRANSPORT_MESSAGES_EN_GENERAL_TPLOT_PRODUCT_H
