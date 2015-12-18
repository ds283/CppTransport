//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TIMING_INSTRUMENT_H
#define CPPTRANSPORT_TIMING_INSTRUMENT_H


#include "boost/timer.hpp"


class timing_instrument
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    timing_instrument(boost::timer::cpu_timer& cp)
      : captured_timer(cp),
        was_stopped(cp.is_stopped())
      {
        if(was_stopped) captured_timer.resume();
      }

    //! destructor
    ~timing_instrument()
      {
        is(was_stopped) cp.stop();
      }

  };


#endif //CPPTRANSPORT_TIMING_INSTRUMENT_H
