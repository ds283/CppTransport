//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CORE_H
#define CPPTRANSPORT_CORE_H

#include <string>

#include "msg_en.h"

constexpr auto CPPTRANSPORT_NAME                     = "CppTransport";
constexpr auto CPPTRANSPORT_VERSION                  = "0.13";
constexpr auto CPPTRANSPORT_NUMERIC_VERSION          = 0.13;
constexpr auto CPPTRANSPORT_COPYRIGHT                = "(c) University of Sussex 2013-15";

constexpr auto MODEL_SCRIPT_SUFFIX                   = ".model";
constexpr auto MODEL_SCRIPT_SUFFIX_LENGTH            = (6);
constexpr auto TEMPLATE_TAG_SUFFIX                   = "_";

constexpr auto NEWLINE_CHAR                          = "\n";

constexpr auto INVALID_GUARD_CHARACTERS              = "-/\\|@£$%^&*()`~+=[]{}:;<>,§±";

constexpr auto MPLANCK_SYMBOL                        = "__Mp";
constexpr auto MPLANCK_LATEX_SYMBOL                  = "M_{\\mathrm{P}}";
constexpr auto MPLANCK_TEXT_NAME                     = "M_P";

constexpr auto DEFAULT_RECURSION_DEPTH               = (127);
constexpr auto DEFAULT_MODEL_NAME                    = "UNKNOWN_MODEL";
constexpr auto DEFAULT_AUTHOR                        = "Anonymous";

constexpr auto TEMPLATE_PATHS_ENV                    = "CPPTRANSPORT_TEMPLATE_PATH";

constexpr auto DEFAULT_UNROLL_MAX                    = 1000;
// output strings

constexpr auto BACKEND_MACRO_PREFIX                  = "$$__";
constexpr auto BACKEND_LINE_SPLIT_EQUAL              = "$$=";
constexpr auto BACKEND_LINE_SPLIT_SUM_EQUAL          = "$$+=";

constexpr auto OUTPUT_CPPTRANSPORT_TAG               = "__CPPTRANSPORT";
constexpr auto OUTPUT_TIME_FORMAT                    = "%X on %d %m %Y";
constexpr auto OUTPUT_DEFAULT_POOL_TEMPLATE          = "auto $1 = $2;";

constexpr auto OUTPUT_DEFAULT_CPP_CSE_TEMPORARY_NAME = "__t";

constexpr auto OUTPUT_DEFAULT_U2_NAME                = "__u2";
constexpr auto OUTPUT_DEFAULT_U3_NAME                = "__u3";
constexpr auto OUTPUT_DEFAULT_M_NAME                 = "__M";
constexpr auto OUTPUT_DEFAULT_K_NAME                 = "__k";
constexpr auto OUTPUT_DEFAULT_K1_NAME                = "__k1";
constexpr auto OUTPUT_DEFAULT_K2_NAME                = "__k2";
constexpr auto OUTPUT_DEFAULT_K3_NAME                = "__k3";
constexpr auto OUTPUT_DEFAULT_A_NAME                 = "__a";

constexpr auto OUTPUT_DEFAULT_HSQ_NAME               = "__Hsq";
constexpr auto OUTPUT_DEFAULT_EPS_NAME               = "__eps";

constexpr auto OUTPUT_DEFAULT_ONEINDEX_NAME          = "__1index";
constexpr auto OUTPUT_DEFAULT_TWOINDEX_NAME          = "__2index";
constexpr auto OUTPUT_DEFAULT_THREEINDEX_NAME        = "__3index";

constexpr auto OUTPUT_DEFAULT_STEPPER_STATE_NAME     = "double";

constexpr auto OUTPUT_TEMPORARY_POOL_START           = "BEGIN TEMPORARY POOL";
constexpr auto OUTPUT_TEMPORARY_POOL_END             = "END TEMPORARY POOL";
constexpr auto OUTPUT_TEMPORARY_POOL_SEQUENCE        = "sequence";
constexpr auto OUTPUT_KERNEL_LOCATION                = "VEXCL KERNEL INSERTION";

constexpr auto OUTPUT_VEXCL_KERNEL_PRE               = ", \"";
constexpr auto OUTPUT_VEXCL_KERNEL_POST              = "\"";
constexpr auto OUTPUT_VEXCL_UNKNOWN_KERNEL           = ", \"<UNKNOWN_KERNEL_NAME>\"";

constexpr auto OUTPUT_VEXCL_KERNEL_LINE_PRE          = "\"";
constexpr auto OUTPUT_VEXCL_KERNEL_LINE_POST         = "\\n\"";

constexpr auto OUTPUT_OPENCL_DEFAULT_LABEL           = "idx";

#endif //CPPTRANSPORT_CORE_H
