//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __error_H_
#define __error_H_

#include <string>
#include <deque>
#include <memory>

#include "core.h"
#include "filestack.h"


#define ERROR_PATH_LEVEL (5)
#define WARN_PATH_LEVEL  (5)


void warn (std::string const msg);
void error(std::string const msg);

void warn (std::string const msg, std::shared_ptr<filestack> path);
void error(std::string const msg, std::shared_ptr<filestack> path);

void warn (std::string const msg, std::shared_ptr<filestack> path, unsigned int level);
void error(std::string const msg, std::shared_ptr<filestack> path, unsigned int level);


#endif //__error_H_
