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


#include <tuple>

#include "boost/date_time/posix_time/posix_time.hpp"


namespace transport
  {
    
    typedef std::tuple<double, double, double> load_data;
    
    
    class worker_contact_record
      {
        
      public:
        
        //! construct a worker contact record; automatically attaches a timestamp
        worker_contact_record()
          : timestamp(boost::posix_time::second_clock::universal_time())
          {
          }
        
        //! destructor is default
        ~worker_contact_record() = default;
        
        
        // INTERFACE
        
      public:
        
        const boost::posix_time::ptime& operator*() const { return this->timestamp; }
        
        
        // INTERNAL DATA
        
      private:
        
        //! recover timestamp
        const boost::posix_time::ptime timestamp;
        
      };
    
    
    class worker_load_record
      {
        
      public:
        
        //! construct a worker load-average record; automatically attaches a timestamp
        worker_load_record(const double l)
          : load(l),
            timestamp(boost::posix_time::second_clock::universal_time())
          {
          }
        
        
        // INTERFACE
        
      public:
        
        //! recover load data
        double operator*() const { return this->load; }
        
        //! recover timestamp
        const boost::posix_time::ptime& get_timestamp() const { return this->timestamp; }
        
        
        // INTERNAL DATA
        
      private:
        
        //! timestamp
        const boost::posix_time::ptime timestamp;
        
        //! load data
        const double load;
        
      };
    

    class worker_management_data
      {
        
      public:
        
        typedef std::list< worker_contact_record > contact_db;
        
        typedef std::list< worker_load_record > load_db;
      
      public:
        
        //! construct a worker information record
        worker_management_data()
          {
            // tag now as time of first contact
            contact_history.emplace_front();
            
            // generate first load-balance data point at zero
            load_history.emplace_front(0.0);
          }
        
        //! destructor
        ~worker_management_data() = default;
        
        
        // INTERFACE -- CONTACT TIMES
      
      public:
        
        //! update contact time
        void update_contact_time(boost::posix_time::ptime t)
          {
            this->contact_history.emplace_front();
          }
        
        //! get last contact time
        const boost::posix_time::ptime& get_last_contact_time() const
          {
            return *this->contact_history.front();
          }
        
        
        // INTERFACE -- LOAD TRACKING
      
      public:
        
        //! update load average
        void update_load_average(double ld)
          {
            this->load_history.emplace_front(ld);
          }
        
        //! get current load average
        double get_load_average() const
          {
            return *this->load_history.front();
          }
        
        
        // INTERNAL DATA
      
      private:
        
        //! ordered list of last-contact times
        contact_db contact_history;
        
        //! current load average for this worker
        load_db load_history;
        
      };
    
    
    class worker_manager
      {
        
      public:
        
        
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor
        worker_manager(unsigned int nw)
          : number_workers(nw),
            current_data(worker_data.end())
          {
          }
        
        //! destructor is default
        ~worker_manager() = default;
    
    
        // INTERFACE -- PREPARE FOR NEW TASK
  
      public:
    
        //! set up records for a new task
        void new_task(const std::string& name);
        
        
        // INTERFACE -- CONTACT MANAGEMENT
        
      public:
    
        //! update time of last contact with a worker
        void update_contact_time(unsigned int worker, boost::posix_time::ptime time);
        
        
        // INTERFACE -- WORKER LOAD MANAGEMENT
        
      public:
        
        //! update current load average for a given worker
        void update_load_average(unsigned int worker, double load);
        
        //! compute current min, max, avg loads
        load_data compute_load_data();
        
        
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
        
        //! database of worker data, organized by content group name
        typedef std::map< std::string, std::unique_ptr< std::vector<worker_management_data> > > worker_db;
        worker_db worker_data;
        
        //! pointer to currently active set of data records
        worker_db::iterator current_data;
        
      };
    
    
    void worker_manager::new_task(const std::string& task)
      {
        // create new vector of appropriate size
        std::unique_ptr< std::vector<worker_management_data> > data_group = std::make_unique< std::vector<worker_management_data> >(this->number_workers);
    
        std::pair< worker_db::iterator, bool > res = this->worker_data.insert(std::make_pair(task, std::move(data_group)));
        if(!res.second)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WORKER_MANAGER_NEW_TASK_FAIL << " '" << task << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }
        
        this->current_data = res.first;
      }
    
    
    void worker_manager::update_contact_time(unsigned int worker, boost::posix_time::ptime time)
      {
        if(this->current_data == this->worker_data.end()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_NO_ACTIVE_TASK);

        std::vector< worker_management_data >& worker_set = *this->current_data->second;
        if(worker >= worker_set.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_OUT_OF_RANGE);
        
        worker_set[worker].update_contact_time(time);
      }
    
    
    void worker_manager::update_load_average(unsigned int worker, double load)
      {
        if(this->current_data == this->worker_data.end()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_NO_ACTIVE_TASK);
    
        std::vector< worker_management_data >& worker_set = *this->current_data->second;
        if(worker >= worker_set.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_OUT_OF_RANGE);
    
        worker_set[worker].update_load_average(load);
      }
    
    
    const worker_management_data& worker_manager::operator[](unsigned int worker) const
      {
        if(this->current_data == this->worker_data.end()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_NO_ACTIVE_TASK);
    
        std::vector< worker_management_data >& worker_set = *this->current_data->second;
        if(worker >= worker_set.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_OUT_OF_RANGE);
        
        return worker_set[worker];
      }
    
    
    load_data worker_manager::compute_load_data()
      {
        if(this->current_data == this->worker_data.end()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_WORKER_MANAGER_NO_ACTIVE_TASK);
    
        std::vector< worker_management_data >& worker_set = *this->current_data->second;

        double min = 1.0;
        double max = 0.0;
        double total = 0.0;
        
        for(const worker_management_data& data : worker_set)
          {
            double load = data.get_load_average();
            total += load;
            
            if(load < min) min = load;
            if(load > max) max = load;
          }
        
        double avg = worker_set.size() > 0 ? total / worker_set.size() : total;
        return std::make_tuple(min, max, avg);
      }
    
  }


#endif //CPPTRANSPORT_WORKER_MANAGER_H
