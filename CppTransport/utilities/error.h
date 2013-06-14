//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __error_H_
#define __error_H_

#include <string>
#include <deque>

#include "core.h"

#define ERROR_PATH_LEVEL (1)
#define WARN_PATH_LEVEL  (1)

#define WARNING_TOKEN    "warning: "
#define ERROR_TOKEN      ""

void warn (std::string msg);
void error(std::string msg);

void warn (std::string msg, unsigned int line, std::deque<struct inclusion>& path);
void error(std::string msg, unsigned int line, std::deque<struct inclusion>& path);

void warn (std::string msg, unsigned int line, std::deque<struct inclusion>& path, unsigned int level);
void error(std::string msg, unsigned int line, std::deque<struct inclusion>& path, unsigned int level);

#endif //__error_H_
