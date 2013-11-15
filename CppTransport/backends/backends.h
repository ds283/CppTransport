//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __backends_H_
#define __backends_H_


#include "input.h"


typedef bool (*backend_function)(const struct input& data, finder& path, bool do_cse);

#define NUMBER_BACKENDS (1)


#include "cpp_backend.h"

#endif //__backends_H_
