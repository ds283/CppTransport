//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __ginac_printer_H_
#define __ginac_printer_H_


#include <functional>
#include "ginac/ginac.h"


// define the concept of a ginac_printer - something which takes an GiNaC expression
// and formats to a string appropriate for a particular language

typedef std::function<std::string(const GiNaC::ex&)> ginac_printer;


#endif // __ginac_printer_H_
