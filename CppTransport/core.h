//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __core_H_
#define __core_H_

#include <string>

#include "msg_en.h"

constexpr auto CPPTRANSPORT_NAME            = "CppTransport";
constexpr auto CPPTRANSPORT_VERSION         = "0.13";
constexpr auto CPPTRANSPORT_NUMERIC_VERSION = 0.13;
constexpr auto CPPTRANSPORT_COPYRIGHT       = "(c) University of Sussex 2013-15";

constexpr auto MODEL_SCRIPT_SUFFIX          = ".model";
constexpr auto MODEL_SCRIPT_SUFFIX_LENGTH   = (6);
constexpr auto TEMPLATE_TAG_SUFFIX          = "_";

constexpr auto NEWLINE_CHAR                 = "\n";

constexpr auto INVALID_GUARD_CHARACTERS     = "-/\\|@£$%^&*()`~+=[]{}:;<>,§±";

constexpr auto MPLANCK_SYMBOL               = "__Mp";
constexpr auto MPLANCK_LATEX_SYMBOL         = "M_{\\mathrm{P}}";
constexpr auto MPLANCK_TEXT_NAME            = "M_P";

constexpr auto DEFAULT_RECURSION_DEPTH      = (127);
constexpr auto DEFAULT_MODEL_NAME           = "UNKNOWN_MODEL";
constexpr auto DEFAULT_AUTHOR               = "Anonymous";

constexpr auto TEMPLATE_PATHS_ENV           = "CPPTRANSPORT_TEMPLATE_PATH";

#endif //__core_H_
