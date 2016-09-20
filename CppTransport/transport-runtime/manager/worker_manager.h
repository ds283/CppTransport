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

#ifndef CPPTRANSPORT_WORKER_MANAGER_H
#define CPPTRANSPORT_WORKER_MANAGER_H


namespace transport
  {

    class worker_management_data
      {
      
      public:
        
        //! construct a worker information record
        worker_management_data()
          : last_contact(boost::posix_time::second_clock::universal_time()),
            busy_time(0),
            idle_time(0)
          {
          }
        
        //! destructor
        ~worker_management_data() = default;
        
        
        // INTERFACE -- CONTACT TIMES
      
      public:
        
        //! update last contact time
        void update_contact_time(boost::posix_time::ptime t) { this->last_contact = t; }
        
        //! get last contact time
        boost::posix_time::ptime get_last_contact_time() const { return this->last_contact; }
        
        
        // INTERFACE -- LOAD TRACKING
      
      public:
        
        //! update busy and idle times
        void update_busy_idle_time(boost::timer::nanosecond_type b, boost::timer::nanosecond_type i) { this->busy_time = b; this->idle_time = i; }
        
        //! get busy time
        boost::timer::nanosecond_type get_busy_time() const { return this->busy_time; }
        
        //! get idle time
        boost::timer::nanosecond_type get_idle_time() const { return this->idle_time; }
        
        //! compute load average
        double get_load_average() const { return static_cast<double>(this->busy_time) / static_cast<double>(this->busy_time + this->idle_time); }
        
        
        // INTERNAL DATA
      
      private:
        
        //! time of last contact with this worker
        boost::posix_time::ptime last_contact;
        
        
        // LOAD TRACKING
        
        //! busy time reported by this worker
        boost::timer::nanosecond_type busy_time;
        
        //! idle time reported by this worker
        boost::timer::nanosecond_type idle_time;
        
      };
    
    
    class worker_manager
      {
        
      public:
        
        
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor
        worker_manager(unsigned int nw)
          : number_workers(nw)
          {
            this->populate_new_records();
          }
        
        //! destructor is default
        ~worker_manager() = default;
    
    
        // INTERFACE -- PREPARE FOR NEW TASK
  
      public:
    
        //! set up records for a new task
        void new_task();
        
      private:
        
        //! create a new set of records and populate them
        void populate_new_records();
        
        
        // INTERFACE -- CONTACT MANAGEMENT
        
      public:
    
        //! update time of last contact with a worker
        void update_contact_time(unsigned int worker, boost::posix_time::ptime time);
        
        
        // INTERFACE -- INTERROGATE DATA
        
      public:
        
        //! query individual worker data, returned in read-only format
        const worker_management_data& operator[](unsigned int worker) const;
   
        //! query for size
        size_t size() { return this->number_workers; }
    
        // INTERNAL DATA
        
      private:
        
        //! total number of workers in the pool
        const unsigned int number_workers;
        
        //! list of worker records in forwards order, so most
        //! recent task is at the back
        std::vector< std::vector< worker_management_data > > worker_data;
        
      };
    
    
    void worker_manager::new_task()
      {
        this->populate_new_records();
      }
    
    
    void worker_manager::populate_new_records()
      {
        // create new vector of appropriate size at front of worker_data
        this->worker_data.emplace_back(this->number_workers);
      }
    
    
    void worker_manager::update_contact_time(unsigned int worker, boost::posix_time::ptime time)
      {
        std::vector< worker_management_data >& worker_set = this->worker_data.back();
        if(worker >= worker_set.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_OUT_OF_RANGE);
        
        worker_set[worker].update_contact_time(time);
      }
    
    
    const worker_management_data& worker_manager::operator[](unsigned int worker) const
      {
        const std::vector< worker_management_data >& worker_set = this->worker_data.back();
        if(worker >= worker_set.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_OUT_OF_RANGE);
        
        return worker_set[worker];
      }
    
  }


#endif //CPPTRANSPORT_WORKER_MANAGER_H
