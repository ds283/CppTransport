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


void set_up_error_environment(void);
void disable_colour_errors(void);


void warn (const std::string& msg);
void error(const std::string& msg);

void warn (const std::string& msg, const filestack& path);
void error(const std::string& msg, const filestack& path);

void warn (const std::string& msg, const filestack& path, unsigned int level);
void error(const std::string& msg, const filestack& path, unsigned int level);

void warn (const std::string& msg, const filestack& path, const std::string& line, unsigned int char_pos);
void error(const std::string& msg, const filestack& path, const std::string& line, unsigned int char_pos);

void warn (const std::string& msg, const filestack& path, const std::string& line, unsigned int char_pos, unsigned int level);
void error(const std::string& msg, const filestack& path, const std::string& line, unsigned int char_pos, unsigned int level);


#endif //__error_H_
