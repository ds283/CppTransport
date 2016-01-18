//
// Created by David Seery on 26/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_TO_PRINTABLE_H
#define CPPTRANSPORT_TO_PRINTABLE_H

#include <iostream>
#include <string>


std::string to_printable(const std::string& input, bool quote=true, bool allow_newlines=false);


#endif //CPPTRANSPORT_TO_PRINTABLE_H
