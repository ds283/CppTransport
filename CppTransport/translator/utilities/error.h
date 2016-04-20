//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ERROR_H
#define CPPTRANSPORT_ERROR_H

#include <string>
#include <deque>
#include <memory>

#include "translator/core.h"
#include "error_context.h"
#include "argument_cache.h"
#include "local_environment.h"


constexpr unsigned int ERROR_PATH_LEVEL = 5;
constexpr unsigned int WARN_PATH_LEVEL  = 5;


// PLAIN ERROR MESSAGES, NO CONTEXT INFORMATION

void warn (const std::string& msg, const argument_cache& cache, const local_environment& env);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env);


// CONTEXT BASED ERROR MESSAGES, WITH INPUT LINE AND CHARACTER POSITION IF AVAILABLE


void warn (const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context);

void warn (const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context, unsigned int level);
void error(const std::string& msg, const argument_cache& cache, const local_environment& env, const error_context& context, unsigned int level);


#endif //CPPTRANSPORT_ERROR_H
