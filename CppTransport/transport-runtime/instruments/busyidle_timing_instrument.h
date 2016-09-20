//
// Created by David Seery on 20/09/2016.
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

#ifndef CPPTRANSPORT_DUAL_TIMING_INSTRUMENT_H
#define CPPTRANSPORT_DUAL_TIMING_INSTRUMENT_H


#include "transport-runtime/instruments/timing_instrument.h"

namespace transport
  {
    
    class dual_timing_instrument
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor captures 'busy' and 'idle' timers
        //! and sets them into busy mode
        dual_timing_instrument(boost::timer::cpu_timer& b, boost::timer::cpu_timer& i)
          : busy_timer(b),
            idle_timer(i)
          {
            busy_timer.resume();
            idle_timer.stop();
          }
        
        //! destructor is default; destruction of timing_instrument objects
        //! will reset timers to whatever state they had on entry
        ~dual_timing_instrument() = default;
        
        
        // INTERFACE
        
      public:
        
        //! switch to busy
        void busy()
          {
            this->idle_timer.stop();
            this->busy_timer.resume();
          }
        
        //! switch to idle
        void idle()
          {
            this->busy_timer.stop();
            this->idle_timer.resume();
          }
        
        
        // INTERNAL DATA
        
      private:
        
        // TIMERS
        
        //! busy timer
        timing_instrument busy_timer;
        
        //! idle timer
        timing_instrument idle_timer;
        
      };

  }



#endif //CPPTRANSPORT_DUAL_TIMING_INSTRUMENT_H
