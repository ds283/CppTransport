//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __core_H_
#define __core_H_

#include <string>

#include "msg_en.h"

#define CPPTRANSPORT_NAME            "CppTransport"
#define CPPTRANSPORT_VERSION         "0.02"
#define CPPTRANSPORT_NUMERIC_VERSION (0.02)
#define CPPTRANSPORT_COPYRIGHT       "(c) University of Sussex 2013"

#define MODEL_SCRIPT_SUFFIX          ".model"
#define MODEL_SCRIPT_SUFFIX_LENGTH   (6)

// data structure for tracking the source of any lexeme
struct inclusion
  {
    std::string  name;
    unsigned int line;
  };


#endif //__core_H_
