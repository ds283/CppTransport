//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TIMING_INSTRUMENT_H
#define CPPTRANSPORT_TIMING_INSTRUMENT_H


#include "boost/timer/timer.hpp"


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
        if(was_stopped) captured_timer.stop();
      }


    // INTERFACE

  public:

    //! stop the timer (if it was previously stopped)
    void stop() { if(was_stopped) captured_timer.stop(); }


    // INTERNAL DATA

  private:

    //! reference to timer we are managing
    boost::timer::cpu_timer& captured_timer;

    //! record whether timer was stopped on construction
    bool was_stopped;

  };


#endif //CPPTRANSPORT_TIMING_INSTRUMENT_H
