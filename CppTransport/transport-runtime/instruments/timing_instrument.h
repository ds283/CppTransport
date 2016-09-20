//
// Created by David Seery on 18/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
        was_stopped(cp.is_stopped()),
        stopped(false)
      {
        // restart timer if it was previously stopped, otherwise just let it keep going
        if(was_stopped) captured_timer.resume();
      }

    //! destructor
    ~timing_instrument()
      {
        // stop timer if it was stopped on capture, provided stop() has not already been called
        if(!stopped && was_stopped) captured_timer.stop();
      }


    // INTERFACE

  public:

    //! stop the timer (if it was previously stopped)
    void stop()
      {
        // stop timer if it was stopped on capture, and flag that stop() has been called
        // on this instrument
        if(!this->stopped && was_stopped)
          {
            captured_timer.stop();
            this->stopped = true;
          }
      }


    // INTERNAL DATA

  private:

    //! reference to timer we are managing
    boost::timer::cpu_timer& captured_timer;

    //! record whether timer was stopped on construction
    bool was_stopped;
    
    //! record whether stop() has been called on this instrument
    bool stopped;

  };


#endif //CPPTRANSPORT_TIMING_INSTRUMENT_H
