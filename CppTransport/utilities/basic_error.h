//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __basic_error_H_
#define __basic_error_H_


#include <string>


#define WARNING_TOKEN    "warning: "
#define ERROR_TOKEN      ""


void basic_warn (std::string const msg);
void basic_error(std::string const msg);


#endif //__basic_error_H_
