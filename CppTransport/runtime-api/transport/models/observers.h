//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __observers_H_
#define __observers_H_


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport/messages_en.h"

#include <boost/timer/timer.hpp>


namespace transport
  {

    class timing_observer
      {
      public:
        timing_observer(double t_int=1.0, bool s=false, unsigned int p=3)
          : t_interval(t_int), silent(s), ready(false), t_last(0), precision(p)
          {
            timer.stop();
          }

        void start(double t)
          {
            if(!ready || t > this->t_last + this->t_interval)
              {
                this->timer.stop();
                this->t_last = t;
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t;
                std::cout << msg.str();
                if(ready)
                  {
                    std::cout << " " << __CPP_TRANSPORT_OBSERVER_ELAPSED << " =" << this->timer.format();
                  }
                else
                  {
                    std::cout << std::endl;
                  }
                ready = false;
              }
          }

        void stop()
          {
            if(!ready)
              {
                this->timer.start();
                ready = true;
              }
          }

      protected:
        bool                    silent;      // whether to generate output during observations
        bool                    ready;
        double                  t_last;      // last value at which we emitted output
        double                  t_interval;  // t-interval at which to emit output
        unsigned int            precision;   // precision used in output

        boost::timer::cpu_timer timer;
      };

  } // namespace transport


#endif //__observers_H_
