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

#ifndef CPPTRANSPORT_BUSYIDLE_TIMER_SET_H
#define CPPTRANSPORT_BUSYIDLE_TIMER_SET_H


#include <map>

#include "transport-runtime/exceptions.h"
#include "transport-runtime/localizations/messages_en.h"


namespace transport
  {
    
    namespace busyidle_timer_set_impl
      {
        
        class busyidle_timer
          {
            
            // CONSTRUCTOR, DESTUCTOR
            
          public:
            
            //! constructor is empty, since timer starts in busy mode
            busyidle_timer()
              : stopped(false)
              {
              }
            
            //! destructor is default
            ~busyidle_timer() = default;
            
            
            // INTERFACE -- SWITCH STATE
            
          public:
            
            //! switch to busy state
            void busy()
              {
                if(stopped) return;
                busy_time.resume();
              }
            
            //! switch to idle state
            void idle()
              {
                if(stopped) return;
                busy_time.stop();
              }
            
            //! stop the timer
            //! currently, once stopped the timer can't be resumed
            void stop()
              {
                total_time.stop();
                busy_time.stop();
                stopped = true;
              }
            
            
            // INTERFACE -- READ ELAPSED TIMES
            
          public:
            
            //! get total time
            boost::timer::nanosecond_type get_total_time() const { return this->total_time.elapsed().wall; }
            
            //! get busy time
            boost::timer::nanosecond_type get_busy_time() const { return this->busy_time.elapsed().wall; }
            
            //! get idle time
            boost::timer::nanosecond_type get_idle_time() const { return(this->get_total_time() - this->get_busy_time()); }
            
            //! get load average
            double get_load_average() const { return static_cast<double>(this->get_busy_time()) / static_cast<double>(this->get_total_time()); }
            
            
            // INTERNAL DATA
            
          private:
            
            //! total elapsed time
            boost::timer::cpu_timer total_time;
            
            //! total busy time
            boost::timer::cpu_timer busy_time;
            
            //! is this timer stopped?
            bool stopped;
            
          };
        
      }
    
    
    using namespace busyidle_timer_set_impl;

    
    class busyidle_timer_set
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor sets default state to busy
        busyidle_timer_set()
          : busy_mode(true)
          {
          }
        
        //! destructor is default
        ~busyidle_timer_set() = default;
        
        
        // INTERFACE -- TIMER MANAGEMENT
        
      public:
        
        //! add a new named timer
        void add_new_timer(const std::string& name);
        
        //! stop a named timer
        void stop_timer(const std::string& name);
        
        
        // INTERFACE -- SWITCH BUSY/IDLE STATE
        
      public:
        
        //! switch to busy state
        void busy();
        
        //! switch to idle state
        void idle();
        
        //! read busy state
        bool is_busy() const { return this->busy_mode; }
        
        
        // INTERFACE -- READ ELAPSED TIME
        
      public:
        
        //! get total elapsed time for a named timer
        boost::timer::nanosecond_type get_total_time(const std::string& name);
        
        //! get elapsed busy time for a named timer
        boost::timer::nanosecond_type get_busy_time(const std::string& name);
        
        //! get elapsed idle time for a named timer
        boost::timer::nanosecond_type get_idle_time(const std::string& name);
        
        //! get load average for a named timer
        double get_load_average(const std::string& name);
        
        
        // INTERNAL DATA
        
      private:
        
        // STATE
        
        //! are we currently in the busy state?
        bool busy_mode;
        
        
        // TIMERS
        
        typedef std::map< std::string, std::shared_ptr<busyidle_timer> > timer_db;
        typedef std::map< std::string, timer_db::iterator > active_timer_db;
        
        //! timer database -- this includes all timers, stopped and running
        timer_db timers;
        
        //! active timer set -- this only includes running timers
        active_timer_db active_timers;
        
      };
    
    
    
    class busyidle_instrument
      {
        
        // CONSTRUCTOR, DESTRUCTOR
  
      public:
        
        //! constructor captures busy/idle set and records its state
        busyidle_instrument(busyidle_timer_set& cp)
          : captured_set(cp),
            was_busy(cp.is_busy())
          {
            if(!was_busy) captured_set.busy();
          }
        
        //! destructor restores state
        ~busyidle_instrument()
          {
            // reset timers to busy if they were busy on entry, otherwise reset to idle
            if(this->was_busy)
              {
                this->captured_set.busy();
              }
            else
              {
                this->captured_set.idle();
              }
          }
        
        
        // INTERFACE
        
      public:
        
        //! switch to busy mode
        void busy()
          {
            this->captured_set.busy();
          }
        
        //! switch to idle mode
        void idle()
          {
            this->captured_set.idle();
          }
        
        
        // INTERNAL DATA
        
      private:
        
        //! captured busy/idle set
        busyidle_timer_set& captured_set;
        
        //! was the busy/idle set in busy mode at capture?
        const bool was_busy;
        
      };
    
    
    void busyidle_timer_set::add_new_timer(const std::string& name)
      {
        std::unique_ptr<busyidle_timer> timer = std::make_unique<busyidle_timer>();

        // set timer to idle if that's the state we're in
        if(!this->busy_mode) timer->idle();
        
        std::pair< timer_db::iterator, bool > res1 = this->timers.insert(std::make_pair(name, std::move(timer)));
        if(!res1.second)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_INSERT_EXISTING_TIMER << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
        
        std::pair< active_timer_db::iterator, bool > res2 = this->active_timers.insert(std::make_pair(name, res1.first));
        if(!res2.second)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_ACTIVE_INSERT_FAILED << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
      }
    
    
    void busyidle_timer_set::stop_timer(const std::string& name)
      {
        active_timer_db::iterator t = this->active_timers.find(name);
        if(t == this->active_timers.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_TIMER_NOT_FOUND << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
        
        timer_db::iterator u = t->second;
        
        u->second->stop();
        this->active_timers.erase(t);
      }
    
    
    void busyidle_timer_set::busy()
      {
        if(this->busy_mode) return;    // nothing to do
        
        this->busy_mode = true;
        for(active_timer_db::value_type timer_pair : this->active_timers)
          {
            timer_db::iterator u = timer_pair.second;
            u->second->busy();
          }
      }
    
    
    void busyidle_timer_set::idle()
      {
        if(!this->busy_mode) return;   // nothing to do
        
        this->busy_mode = false;
        for(active_timer_db::value_type timer_pair : this->active_timers)
          {
            timer_db::iterator u = timer_pair.second;
            u->second->idle();
          }
      }
    
    
    boost::timer::nanosecond_type busyidle_timer_set::get_total_time(const std::string& name)
      {
        timer_db::iterator t = this->timers.find(name);
        if(t == this->timers.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_TIMER_NOT_FOUND << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
        
        return t->second->get_total_time();
      }
    
    
    boost::timer::nanosecond_type busyidle_timer_set::get_busy_time(const std::string& name)
      {
        timer_db::iterator t = this->timers.find(name);
        if(t == this->timers.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_TIMER_NOT_FOUND << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
    
        return t->second->get_busy_time();
      }
    
    
    boost::timer::nanosecond_type busyidle_timer_set::get_idle_time(const std::string& name)
      {
        timer_db::iterator t = this->timers.find(name);
        if(t == this->timers.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_TIMER_NOT_FOUND << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
    
        return t->second->get_idle_time();
      }
    
    
    double busyidle_timer_set::get_load_average(const std::string& name)
      {
        timer_db::iterator t = this->timers.find(name);
        if(t == this->timers.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_BUSYIDLE_TIMER_NOT_FOUND << " '" << name << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
    
        return t->second->get_load_average();
      }
    
  }



#endif //CPPTRANSPORT_BUSYIDLE_TIMER_SET_H
