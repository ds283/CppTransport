//
// Created by David Seery on 18/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_FORMATTER_H
#define CPPTRANSPORT_FORMATTER_H


#include <boost/timer/timer.hpp>


std::string format_time(boost::timer::nanosecond_type time, unsigned int precision=3);



#endif //CPPTRANSPORT_FORMATTER_H
