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

#define CPPTRANSPORT_NAME            "CppTransport"
#define CPPTRANSPORT_VERSION         "0.11"
#define CPPTRANSPORT_NUMERIC_VERSION (0.11)
#define CPPTRANSPORT_COPYRIGHT       "(c) University of Sussex 2013-15"

#define MODEL_SCRIPT_SUFFIX          ".model"
#define MODEL_SCRIPT_SUFFIX_LENGTH   (6)
#define TEMPLATE_TAG_SUFFIX          "_"

#define NEWLINE_CHAR                 "\n"

#define INVALID_GUARD_CHARACTERS     "-/\\|@£$%^&*()`~+=[]{}:;<>,§±"

#define MPLANCK_SYMBOL               "__Mp"
#define MPLANCK_LATEX_SYMBOL         "M_{\\mathrm{P}}"
#define MPLANCK_TEXT_NAME            "M_P"

#define DEFAULT_RECURSION_DEPTH      (127)

#endif //__core_H_
