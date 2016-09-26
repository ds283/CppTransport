//
// Created by David Seery on 25/09/2016.
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


#ifndef CPPTRANSPORT_REPORT_MANAGER_H
#define CPPTRANSPORT_REPORT_MANAGER_H


#include <map>

#include "transport-runtime/manager/worker_scheduler.h"
#include "transport-runtime/manager/worker_manager.h"

#include "transport-runtime/instruments/busyidle_timer_set.h"
#include "transport-runtime/repository/writers/aggregation_profiler.h"

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/message_handlers.h"

#include "boost/date_time/posix_time/posix_time.hpp"


namespace transport
  {
    
    
    namespace report_manager_impl
      {
        
        class report_alert
          {
            
            // CONSTRUCTOR, DESTRUCTOR
            
          public:
            
            //! constructor
            report_alert(std::string m);
            
            //! destructor is default
            ~report_alert()= default;
            
            
            // INTERFACE
            
          public:
            
            //! increment use count
            void operator++();
            
            //! get use count
            unsigned int get_count() const { return this->count; }
            
            //! get alert message
            const std::string& get_message() const { return this->msg; }
            
            //! get creation datestamp
            const boost::posix_time::ptime& get_creation_time() const { return this->created; }
            
            //! get last-update datestamp
            const boost::posix_time::ptime& get_update_time() const { return this->last_update; }
            
            
            // INTERNAL DATA
            
          private:
            
            //! stored alert message
            std::string msg;
            
            //! use count
            unsigned int count;
            
            //! datestamp - creation
            boost::posix_time::ptime created;
            
            //! datestamp - last update
            boost::posix_time::ptime last_update;
            
          };
    
    
        report_alert::report_alert(std::string m)
          : msg(m),
            count(1),
            created(boost::posix_time::second_clock::universal_time()),
            last_update(created)
          {
          }
    
    
        void report_alert::operator++()
          {
            ++this->count;
            this->last_update = boost::posix_time::second_clock::universal_time();
          }
    
      }
    
    
    using namespace report_manager_impl;
    
    
    class report_manager
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        report_manager(worker_scheduler& s, worker_manager& m, busyidle_timer_set& t, std::list< aggregation_profiler >& p,
                       local_environment& le, argument_cache& ac)
          : scheduler(s),
            manager(m),
            timers(t),
            agg_profiler(p),
            local_env(le),
            arg_cache(ac),
            last_report_time(boost::posix_time::second_clock::local_time()),
            task_start_time(last_report_time),
            last_report_percent(0),
            err(le, ac),
            warn(le, ac),
            msg(le, ac)
          {
          }
        
        
        //! destructor
        ~report_manager() = default;
        
        
        // INTERFACE -- ISSUE REPORTS
      
      public:
        
        //! determine whether a report is due, and issue one if so
        template <typename WriterObject>
        void check_report(WriterObject& writer);
        
        //! advise starting a new task
        void new_task(std::string name, unsigned int n, unsigned int n_max);
        
        //! advise ending task
        void end_task();
        
        //! issue announcement other than starting a new task
        void announce(const std::string& msg);
        
        //! issue summary report after *all* tasks have completed;
        //! load_data is provided by client
        //! and summarizes the load average on the workers
        //! during the task
        void summary_report(unsigned int tasks_complete, const load_data& data);
        
      private:
        
        //! apply policy to determine whether a report is due
        bool check_report_policy();
        
        
        // INTERFACE -- AGGREGATE ALERTS
        
      public:
        
        //! add an alert
        void add_alert(const std::string& msg);
        
      private:
        
        //! flush list of alerts
        void flush_alerts();
        
        
        // INTERNAL API -- GENERATE REPORTS
        
      private:
        
        //! write statistics report
        void statistics_report(std::ostream& stream, reporting::key_value::print_options options);
        
        //! write resources estimate report
        void resources_report(std::ostream& stream, reporting::key_value::print_options options);
        
        //! write worker status report
        template <typename WriterObject>
        void workers_report(WriterObject& writer, std::ostream& stream, reporting::key_value::print_options options);
        
        //! write alerts report
        void alerts_report(std::ostream& stream);
        
        //! generate short reports
        void issue_short_report();
        
        //! generate long/detailed reports
        template <typename WriterObject>
        void issue_detailed_reports(WriterObject& writer);
        
        
        // INTERNAL DATA
      
      private:
        
        // TRACKING
        
        //! timestamp of last report
        boost::posix_time::ptime last_report_time;
        
        //! timestamp of beginning of current task
        boost::posix_time::ptime task_start_time;
        
        //! time of last report in % completed
        unsigned int last_report_percent;
        
        //! current task
        std::string task_name;
        
        
        // AGENTS
        
        //! reference to worker scheduling object
        worker_scheduler& scheduler;
        
        //! reference to worker managing object
        worker_manager& manager;
        
        //! reference to busy/idle timers for master node
        busyidle_timer_set& timers;
        
        //! reference to aggregation profiler collection from master node
        std::list< aggregation_profiler >& agg_profiler;
        
        //! reference to local environment
        local_environment& local_env;
        
        //! reference to argument cache object
        argument_cache& arg_cache;
        
        
        // ALERT DATABASE
        
        typedef std::map< std::string, std::unique_ptr<report_alert> > alert_db;
        typedef std::list< alert_db::iterator > alert_queue;
        
        //! database of alert messages
        alert_db alert_data;
        
        //! list
        alert_queue alerts;

        
        // ERROR AND WARNING HANDLERS
        
        //! error handler
        error_handler err;
        
        //! warning handler
        warning_handler warn;
        
        //! message handler
        message_handler msg;
        
      };
    
    
    void report_manager::new_task(std::string name, unsigned int n, unsigned int n_max)
      {
        this->task_name = std::move(name);
        
        // reset timestamp of last report
        this->last_report_time = boost::posix_time::second_clock::local_time();
        this->task_start_time  = this->last_report_time;
    
        std::ostringstream msg;
        msg << CPPTRANSPORT_PROCESSING_TASK_A << " '" << this->task_name << "' (" << n << " "
            << CPPTRANSPORT_PROCESSING_TASK_OF << " " << n_max << ")";
    
        // emit terminal notification of next task to process, if we are processing multiple tasks
        if(n_max > 1) this->announce(msg.str());
      }
    
    
    void report_manager::end_task()
      {
      }
    
    
    template <typename WriterObject>
    void report_manager::check_report(WriterObject& writer)
      {
        // apply policy
        // will update report timestamp and % completed if a report is required
        if(!check_report_policy()) return;
        
        this->issue_short_report();
        this->issue_detailed_reports(writer);

        // flush any accumulated alerts
        this->flush_alerts();
      }
    
    
    void report_manager::statistics_report(std::ostream& stream, reporting::key_value::print_options options)
      {
        std::ostringstream percent_complete;
        percent_complete << std::setprecision(3);
        percent_complete << 100.0 * this->scheduler.query_completion();
        
        reporting::key_value store(this->local_env, this->arg_cache);
    
        store.insert_back(CPPTRANSPORT_REPORT_WORK_ITEMS_PROCESSED, boost::lexical_cast<std::string>(this->scheduler.get_items_processsed()));
        store.insert_back(CPPTRANSPORT_REPORT_WORK_ITEMS_INFLIGHT, boost::lexical_cast<std::string>(this->scheduler.get_items_inflight()));
        store.insert_back(CPPTRANSPORT_REPORT_REMAIN, boost::lexical_cast<std::string>(this->scheduler.get_items_remaining()));
        store.insert_back(CPPTRANSPORT_REPORT_COMPLETE, percent_complete.str());
        store.insert_back(CPPTRANSPORT_REPORT_MEAN_TIME_PER_ITEM, format_time(this->scheduler.get_mean_time_per_item()));
        store.insert_back(CPPTRANSPORT_REPORT_TARGET_DURATION, format_time(this->scheduler.get_target_assignment()));
    
        store.set_tiling(true);
        store.write(stream, options);
      }
    
    
    void report_manager::resources_report(std::ostream& stream, reporting::key_value::print_options options)
      {
        boost::posix_time::ptime estimated_completion = this->scheduler.get_estimated_completion();
        boost::posix_time::time_duration time_remaining = estimated_completion - this->last_report_time;
        std::ostringstream estimate_msg;
        estimate_msg << boost::posix_time::to_simple_string(estimated_completion)
                     << " (" << format_time(time_remaining.total_nanoseconds()) << " " << CPPTRANSPORT_REPORT_FROM_NOW << ")";
        
        reporting::key_value store(this->local_env, this->arg_cache);
    
        store.insert_back(CPPTRANSPORT_REPORT_COMPLETION_ESTIMATE, estimate_msg.str());
        store.insert_back(CPPTRANSPORT_REPORT_CPU_TIME_ESTIMATE, format_time(this->scheduler.get_estimated_CPU_time()));
        
        store.set_tiling(true);
        store.write(stream, options);
      }
    
    
    void report_manager::issue_short_report()
      {
        // emit notification to terminal
        if(!this->arg_cache.get_verbose()) return;

        std::ostringstream update_msg;
        update_msg << CPPTRANSPORT_TASK_MANAGER_LABEL << " " << boost::posix_time::to_simple_string(this->last_report_time);
    
        bool colour = this->local_env.has_colour_terminal_support() && this->arg_cache.get_colour_output();
    
        if(colour) std::cout << ColourCode(ANSI_colour::magenta);
        std::cout << update_msg.str();
        if(colour) std::cout << ColourCode(ANSI_colour::normal);
        std::cout << '\n';
    
        this->statistics_report(std::cout, reporting::key_value::print_options::none);
        this->resources_report(std::cout, reporting::key_value::print_options::none);
      }
    
    
    template <typename WriterObject>
    void report_manager::issue_detailed_reports(WriterObject& writer)
      {
        // emit notification to report
        base_writer::reporter& report = writer.get_report();
    
        std::ostringstream update_msg;
        update_msg << CPPTRANSPORT_REPORT_HEADING_A << " " << this->task_name << " "
                   << CPPTRANSPORT_REPORT_HEADING_B << " "
                   << boost::posix_time::to_simple_string(this->last_report_time)
                   << '\n';
    
        this->statistics_report(update_msg, reporting::key_value::print_options::fixed_width);
        this->resources_report(update_msg, reporting::key_value::print_options::fixed_width);
        this->alerts_report(update_msg);
        this->workers_report(writer, update_msg, reporting::key_value::print_options::fixed_width);
    
        BOOST_LOG(report) << update_msg.str();
      }
    
    
    template <typename WriterObject>
    void report_manager::workers_report(WriterObject& writer, std::ostream& stream, reporting::key_value::print_options options)
      {
        double load_task = this->timers.get_load_average(writer.get_name());
        double load_global = this->timers.get_load_average(CPPTRANSPORT_DEFAULT_TIMER);
        
        reporting::key_value master(this->local_env, this->arg_cache);
        
        master.insert_back(CPPTRANSPORT_REPORT_MASTER_LOAD_TASK, format_number(load_task, 2));
        master.insert_back(CPPTRANSPORT_REPORT_MASTER_LOAD_GLOBAL, format_number(load_global, 2));
        master.set_tiling(true);
        master.write(stream, options);
        
        for(unsigned int i = 0; i < this->scheduler.size(); ++i)
          {
            const worker_scheduling_data& scheduling_data = this->scheduler[i];
            const worker_management_data& management_data = this->manager[i];

            reporting::key_value store(this->local_env, this->arg_cache);
            store.insert_back(CPPTRANSPORT_WORKER_DATA_NUMBER, boost::lexical_cast<std::string>(scheduling_data.get_number()));
            store.insert_back(CPPTRANSPORT_WORKER_DATA_PROCESSED_ITEMS, boost::lexical_cast<std::string>(scheduling_data.get_number_items()));
            store.insert_back(CPPTRANSPORT_WORKER_DATA_ASSIGNED, scheduling_data.is_assigned() ? CPPTRANSPORT_WORKER_DATA_YES : CPPTRANSPORT_WORKER_DATA_NO);
            store.insert_back(CPPTRANSPORT_WORKER_DATA_INTEGRATION_TIME, format_time(scheduling_data.get_total_time()));
            store.insert_back(CPPTRANSPORT_WORKER_DATA_MEAN_TIME_PER_ITEM, format_time(scheduling_data.get_mean_time_per_work_item()));
            store.insert_back(CPPTRANSPORT_WORKER_DATA_LOAD_AVERAGE, format_number(management_data.get_load_average(), 2));
            store.insert_back(CPPTRANSPORT_WORKER_DATA_LAST_CONTACT, boost::posix_time::to_simple_string(management_data.get_last_contact_time()));

            store.set_tiling(true);
            store.write(stream, options);
          }
      }
    
    
    bool report_manager::check_report_policy()
      {
        // get time now
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        
        // compute seconds since last report, and seconds since start of task
        boost::posix_time::time_duration time_since_last_report = now - this->last_report_time;
        boost::posix_time::time_duration time_since_start_task  = now - this->task_start_time;
    
        // if less time has elapsed than the specified delay, don't issue a report
        unsigned int policy_delay = this->arg_cache.get_report_time_delay();
        
        if(policy_delay > 0 && time_since_start_task.total_seconds() < policy_delay) return false;
    
        bool due = false;
        
        // a report is due if we're over the time interval between reports
        unsigned int policy_interval = this->arg_cache.get_report_time_interval();
        if(policy_interval > 0 && time_since_last_report.total_seconds() >= policy_interval) due = true;
        
        // otherwise, a report is due if we're over the percentage interva between reports
        unsigned int policy_percent = this->arg_cache.get_report_percent_interval();
        unsigned int completed = static_cast<unsigned int>(std::round(100.0*this->scheduler.query_completion()));
        unsigned int percent_since_last_report = completed - this->last_report_percent;
        if(policy_percent > 0 && percent_since_last_report >= policy_percent) due = true;
        
        if(!due) return false;
        
        this->last_report_time = now;
        this->last_report_percent = completed;
        
        return true;
      }
    
    
    void report_manager::announce(const std::string& msg)
      {
        std::ostringstream tagged_msg;
        tagged_msg << CPPTRANSPORT_TASK_MANAGER_LABEL << " " << msg;

        this->msg(tagged_msg.str(), message_handler::highlight::heading);
      }
    
    
    void report_manager::summary_report(unsigned int tasks_complete, const load_data& data)
      {
        std::ostringstream msg;
        msg << CPPTRANSPORT_PROCESSED_TASKS_A << " " << tasks_complete << " ";
        if(tasks_complete > 1)
          {
            msg << CPPTRANSPORT_PROCESSED_TASKS_B_PLURAL;
          }
        else
          {
            msg << CPPTRANSPORT_PROCESSED_TASKS_B_SINGULAR;
          }
    
        // compute total elapsed time and add to message
        boost::timer::nanosecond_type total_time = this->timers.get_total_time(CPPTRANSPORT_DEFAULT_TIMER);
    
        msg << " " << CPPTRANSPORT_PROCESSED_TASKS_WALLCLOCK << " " << format_time(total_time);
    
        // get master global load average and add to message
        msg << " | " << CPPTRANSPORT_PROCESSED_TASKS_MASTER_LOAD
            << " " << format_number(this->timers.get_load_average(CPPTRANSPORT_DEFAULT_TIMER), 2);
    
        // get worker global load averages and add to message
        msg << " | " << CPPTRANSPORT_PROCESSED_TASKS_WORKER_LOAD
            << " " << format_number(std::get<0>(data), 2)
            << "," << format_number(std::get<1>(data), 2)
            << "," << format_number(std::get<2>(data), 2);
    
        this->announce(msg.str());
      }
    
    
    void report_manager::add_alert(const std::string& msg)
      {
        alert_db::iterator t = this->alert_data.find(msg);
        
        if(t != this->alert_data.end())
          {
            // increament use count and return
            ++(*t->second);
            return;
          }
        
        std::unique_ptr<report_alert> alert = std::make_unique<report_alert>(msg);
        std::pair< alert_db::iterator, bool > res = this->alert_data.insert(std::make_pair(msg, std::move(alert)));
        
        // if insert was successful, add pointer to queue
        if(res.second)
          {
            this->alerts.push_back(res.first);
          }
      }
    
    
    void report_manager::flush_alerts()
      {
        this->alert_data.clear();
        this->alerts.clear();
      }
    
    
    void report_manager::alerts_report(std::ostream& stream)
      {
        if(this->alerts.empty()) return;
        
        stream << CPPTRANSPORT_REPORT_ALERTS << '\n';
        
        for(alert_db::iterator& t : this->alerts)
          {
            const alert_db::value_type& rec = *t;
            const report_alert& report = *rec.second;
            
            stream << boost::posix_time::to_simple_string(report.get_creation_time()) << " ";
            stream << report.get_message();
            
            if(report.get_count() > 1)
              {
                stream << " (" << CPPTRANSPORT_REPORT_ALERT_MULTIPLE_A << " " << report.get_count() << " "
                       << CPPTRANSPORT_REPORT_ALERT_MULTIPLE_B << " "
                       << boost::posix_time::to_simple_string(report.get_update_time()) << ")";
              }
            
            stream << '\n';
          }
      }
    
  }   // namespace transport


#endif //CPPTRANSPORT_REPORT_MANAGER_H
