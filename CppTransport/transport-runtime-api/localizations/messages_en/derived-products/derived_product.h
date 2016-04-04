//
// Created by David Seery on 14/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H
#define CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H


#define CPPTRANSPORT_PRODUCT_DATALINE_AXIS_MISMATCH  "Internal error: Axis and sample size mismatch in 'time_data_line'"

#define CPPTRANSPORT_PRODUCT_UNKNOWN_TYPE            "Internal error: Unknown derived-product type"

#define CPPTRANSPORT_INDEX_UNKNOWN_RANGE_TYPE        "Internal error: Unknown 'index_selector' range type"
#define CPPTRANSPORT_INDEX_PRESET_MISMATCH           "Internal error: Mismatched number of presets provided to 'index_selector'; expected"
#define CPPTRANSPORT_INDEX_PRESET_MISMATCH_A         "received"

#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH          "Internal error: unexpected number of fields in index selector object"
#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A        "received"
#define CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B        "expected"

#define CPPTRANSPORT_PRODUCT_INDEX_BOUNDS            "Out-of-bounds index value"

#define CPPTRANSPORT_DERIVED_PRODUCT_FAILED          "Error: I/O error; failed to generate derived product"

#define CPPTRANSPORT_DERIVED_PRODUCT_FILENAME        "Filename"

#define CPPTRANSPORT_DERIVED_PRODUCT_LINE_2D_PLOT    "2d plot"
#define CPPTRANSPORT_DERIVED_PRODUCT_LINE_TABLE      "table"

#endif // CPPTRANSPORT_MESSAGES_EN_DERIVED_PRODUCT_H
