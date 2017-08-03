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

#include "transport-runtime/reporting/key_value.h"
#include "transport-runtime/reporting/email.h"

#include "boost/date_time/posix_time/posix_time.hpp"


namespace transport
  {
    
    
    template <typename number>
    class derived_content_writer;


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
        report_manager(worker_scheduler& s, worker_manager& m, busyidle_timer_set& t,
                       local_environment& le, argument_cache& ac)
          : scheduler(s),
            manager(m),
            timers(t),
            local_env(le),
            arg_cache(ac),
            task_start_time(boost::posix_time::second_clock::local_time()),
            issued_final_report(false),
            err(le, ac),
            warn(le, ac),
            msg(le, ac)
          {
          }
        
        
        //! destructor
        ~report_manager() = default;
        
        
        // INTERFACE -- ISSUE REPORTS
      
      public:
        
        //! determine whether a periodic report is due, and issue one if so
        template <typename WriterObject>
        void periodic_report(WriterObject& writer);
        
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
        
        //! issue standalone database report, covering time since last periodic report
        template <typename WriterObject>
        void database_report(WriterObject& writer);
        
      private:
        
        //! apply policy to determine whether a report is due;
        //! updates time of last report and percent-complete at last report
        //! if it decides a report is to be issued, so these numbers are updated
        //! *before* the corresponding report is issued
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
        bool statistics_report(std::ostream& stream, reporting::key_value::print_options options, bool title=false);
        
        //! write resources estimate report
        bool resources_report(std::ostream& stream, reporting::key_value::print_options options, bool title=false);
        
        //! write worker status report
        template <typename WriterObject>
        bool workers_report(WriterObject& writer, std::ostream& stream, reporting::key_value::print_options options, bool title=false);
        
        //! write alerts report
        bool alerts_report(std::ostream& stream, bool title=false);
        
        //! write database performance report -- derived_content_writer only (needs special treatment; no report is issued)
        template <typename number>
        bool
        database_report(derived_content_writer<number>& writer, std::ostream& stream, boost::posix_time::ptime ref_time,
                        reporting::key_value::print_options options, bool title = false);
        
        //! write database performance report -- generic writer
        template <typename WriterObject>
        bool database_report(WriterObject& writer, std::ostream& stream, boost::posix_time::ptime ref_time,
                             reporting::key_value::print_options options, bool title = false);
        
        //! generate short reports
        void issue_short_report();
        
        //! generate long/detailed reports
        template <typename WriterObject>
        void issue_detailed_reports(WriterObject& writer);
        
        
        // INTERNAL DATA
      
      private:
        
        // TRACKING
        
        //! list of timestamps at which reports were issued
        std::list< boost::posix_time::ptime > report_times;
    
        //! time of percentage-complete points at which reports were issued
        std::list< unsigned int > report_percentages;
        
        //! timestamp of beginning of current task
        boost::posix_time::ptime task_start_time;
        
        //! current task
        std::string task_name;
        
        //! have we issued a final report for this task?
        bool issued_final_report;
        
        
        // AGENTS
        
        //! reference to worker scheduling object
        worker_scheduler& scheduler;
        
        //! reference to worker managing object
        worker_manager& manager;
        
        //! reference to busy/idle timers for master node
        busyidle_timer_set& timers;
        
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
        this->report_times.clear();
        this->report_percentages.clear();
        this->task_start_time  = boost::posix_time::second_clock::local_time();
    
        std::ostringstream report;
        report << CPPTRANSPORT_PROCESSING_TASK_A << " '" << this->task_name << "' (" << n << " "
            << CPPTRANSPORT_PROCESSING_TASK_OF << " " << n_max << ")";
    
        // emit terminal notification of next task to process, if we are processing multiple tasks
        if(n_max > 1) this->announce(report.str());
    
        // issue email version if required
        if(this->arg_cache.email_begin())
          {
            reporting::email msg(this->local_env, this->arg_cache);
            
            std::ostringstream subject;
            subject << CPPTRANSPORT_BEGIN_TASK_SUBJECT << " '" << this->task_name << "'";
    
            msg.set_to(this->arg_cache.get_report_email()).set_subject(subject.str()).set_body(report.str());
            msg.send();
          }
      }
    
    
    void report_manager::end_task()
      {
        // issue email version if required
        if(this->arg_cache.email_end())
          {
            reporting::email msg(this->local_env, this->arg_cache);
    
            std::ostringstream subject;
            subject << CPPTRANSPORT_END_TASK_SUBJECT << " '" << this->task_name << "'";
            
            std::ostringstream report;
            report << CPPTRANSPORT_END_TASK_BODY << " '" << this->task_name << "'";
    
            msg.set_to(this->arg_cache.get_report_email()).set_subject(subject.str()).set_body(report.str());
            msg.send();
          }
      }
    
    
    template <typename WriterObject>
    void report_manager::periodic_report(WriterObject& writer)
      {
        // apply policy
        // will update report timestamp and % completed if a report is required
        if(!check_report_policy()) return;
        
        this->issue_short_report();
        this->issue_detailed_reports(writer);

        // flush any accumulated alerts
        this->flush_alerts();
      }
    
    
    void report_manager::issue_short_report()
      {
        // write to terminal
        if(!this->arg_cache.get_verbose()) return;

        std::ostringstream update_msg;
        update_msg << CPPTRANSPORT_TASK_MANAGER_LABEL << " " << (!this->report_times.empty() ? boost::posix_time::to_simple_string(this->report_times.front()) : "");
    
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
        // write to reporting sink
        base_writer::reporter& report = writer.get_report();
    
        std::ostringstream report_msg;
        report_msg << CPPTRANSPORT_REPORT_HEADING_A << " " << this->task_name << " "
                   << CPPTRANSPORT_REPORT_HEADING_B << " "
                   << (!this->report_times.empty() ? boost::posix_time::to_simple_string(this->report_times.front()) : "")
                   << '\n' << '\n';
    
        if(this->statistics_report(report_msg, reporting::key_value::print_options::fixed_width, true))       report_msg << '\n';
        if(this->resources_report(report_msg, reporting::key_value::print_options::fixed_width, true))        report_msg << '\n';
        if(this->alerts_report(report_msg, true))                                                             report_msg << '\n';
        if(this->workers_report(writer, report_msg, reporting::key_value::print_options::fixed_width, true))  report_msg << '\n';
    
        // determine reference time for database report, which should be time of the previous report
        // (if one exists), or if this is the first report then the reference time should
        // be the start of the task
        boost::posix_time::ptime ref_time = boost::posix_time::not_a_date_time;
        if(this->report_times.size() > 1) ref_time = *(++this->report_times.begin());
        else                              ref_time = this->task_start_time;

        this->database_report(writer, report_msg, ref_time, reporting::key_value::print_options::fixed_width, true);
    
        // write report to log file
        BOOST_LOG(report) << report_msg.str();
        
        // issue email version if required
        if(this->arg_cache.email_periodic())
          {
            reporting::email msg(this->local_env, this->arg_cache);
    
            std::ostringstream subject;
            subject << CPPTRANSPORT_PERIODIC_REPORT_SUBJECT << " '" << this->task_name << "'";
    
            msg.set_to(this->arg_cache.get_report_email()).set_subject(subject.str()).set_body(report_msg.str());
            msg.send(writer);
          }
      }
    
    
    template <typename WriterObject>
    void report_manager::database_report(WriterObject& writer)
      {
        base_writer::reporter& report = writer.get_report();
        
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    
        std::ostringstream report_msg;
        
        // determine reference time for report, which should be the last time a report was issued.
        // Remember that this->report_times() won't have been updated in this case
        boost::posix_time::ptime ref_time = boost::posix_time::not_a_date_time;
        if(this->report_times.size() >= 1) ref_time = this->report_times.front();
        else                               ref_time = this->task_start_time;
        
        if(this->database_report(writer, report_msg, ref_time, reporting::key_value::print_options::fixed_width, true))
          {
            BOOST_LOG(report) << CPPTRANSPORT_REPORT_HEADING_C << " " << this->task_name << " "
                              << CPPTRANSPORT_REPORT_HEADING_B << " "
                              << boost::posix_time::to_simple_string(now)
                              << '\n' << '\n'
                              << report_msg.str();
          }
      }
    
    
    bool report_manager::statistics_report(std::ostream& stream, reporting::key_value::print_options options, bool title)
      {
        if(title) stream << CPPTRANSPORT_REPORT_STATISTICS << '\n' << '\n';

        std::ostringstream percent_complete;
        percent_complete << std::setprecision(3);
        percent_complete << 100.0 * this->scheduler.query_completion() << "%";
        
        reporting::key_value store(this->local_env, this->arg_cache);
        
        store.insert_back(CPPTRANSPORT_REPORT_WORK_ITEMS_PROCESSED, boost::lexical_cast<std::string>(this->scheduler.get_items_processsed()));
        store.insert_back(CPPTRANSPORT_REPORT_WORK_ITEMS_INFLIGHT, boost::lexical_cast<std::string>(this->scheduler.get_items_inflight()));
        store.insert_back(CPPTRANSPORT_REPORT_REMAIN, boost::lexical_cast<std::string>(this->scheduler.get_items_remaining()));
        store.insert_back(CPPTRANSPORT_REPORT_COMPLETE, percent_complete.str());
        store.insert_back(CPPTRANSPORT_REPORT_MEAN_TIME_PER_ITEM, format_time(this->scheduler.get_mean_time_per_item()));
        store.insert_back(CPPTRANSPORT_REPORT_TARGET_DURATION, format_time(this->scheduler.get_target_assignment()));
        
        store.set_tiling(true);
        store.write(stream, options);
        
        return true;
      }
    
    
    bool report_manager::resources_report(std::ostream& stream, reporting::key_value::print_options options, bool title)
      {
        if(title) stream << CPPTRANSPORT_REPORT_RESOURCES << '\n' << '\n';

        boost::posix_time::ptime estimated_completion = this->scheduler.get_estimated_completion();
        boost::posix_time::time_duration time_remaining = estimated_completion - (!this->report_times.empty() ? this->report_times.front() : this->task_start_time);
        std::ostringstream estimate_msg;
        estimate_msg << boost::posix_time::to_simple_string(estimated_completion)
                     << " (" << format_time(time_remaining.total_nanoseconds()) << " " << CPPTRANSPORT_REPORT_FROM_NOW << ")";
        
        reporting::key_value store(this->local_env, this->arg_cache);
        
        store.insert_back(CPPTRANSPORT_REPORT_COMPLETION_ESTIMATE, estimate_msg.str());
        store.insert_back(CPPTRANSPORT_REPORT_CPU_TIME_ESTIMATE, format_time(this->scheduler.get_estimated_CPU_time()));
        
        if(!this->issued_final_report && this->scheduler.get_items_remaining() == 0)
          {
            this->issued_final_report = true;
            store.insert_back(CPPTRANSPORT_REPORT_WORK_COMPLETE, !this->report_times.empty() ? boost::posix_time::to_simple_string(this->report_times.front()) : "");
          }
        
        store.set_tiling(true);
        store.write(stream, options);
        
        return true;
      }
    
    
    template <typename WriterObject>
    bool report_manager::workers_report(WriterObject& writer, std::ostream& stream, reporting::key_value::print_options options, bool title)
      {
        if(title) stream << CPPTRANSPORT_REPORT_WORKERS << '\n' << '\n';

        double load_task = this->timers.get_load_average(writer.get_name());
        double load_global = this->timers.get_load_average(CPPTRANSPORT_DEFAULT_TIMER);
        
        reporting::key_value master(this->local_env, this->arg_cache);
        
        master.insert_back(CPPTRANSPORT_REPORT_MASTER_LOAD_TASK, format_number(load_task, 2));
        master.insert_back(CPPTRANSPORT_REPORT_MASTER_LOAD_GLOBAL, format_number(load_global, 2));
        master.set_tiling(true);
        master.write(stream, options);
        
        for(unsigned int i = 0; i < this->scheduler.size(); ++i)
          {
            stream << '\n';
            
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
        
        return true;
      }
    
    
    bool report_manager::alerts_report(std::ostream& stream, bool title)
      {
        if(this->alerts.empty()) return false;
        
        if(title) stream << CPPTRANSPORT_REPORT_ALERTS << '\n' << '\n';
        
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
        
        return true;
      }

    
    template <typename number>
    bool report_manager::database_report(derived_content_writer<number>& writer, std::ostream& stream,
                                         boost::posix_time::ptime ref_time,
                                         reporting::key_value::print_options options, bool title)
      {
        // derived_content_writer has no database events to profile
        return false;
      }
    
    
    template <typename WriterObject>
    bool
    report_manager::database_report(WriterObject& writer, std::ostream& stream, boost::posix_time::ptime ref_time,
                                    reporting::key_value::print_options options, bool title)
      {
        aggregation_profiler& profiler = writer.get_aggregation_profiler();
        
        // exit of no aggregations have been profiled
        if(profiler.empty()) return false;
        
        std::list< aggregation_profiler::const_iterator > items;
        for(aggregation_profiler::const_iterator t = profiler.begin(); t != profiler.end(); ++t)
          {
            const std::unique_ptr<aggregation_profile_record>& rec = *t;
            if(rec->get_creation_time() >= ref_time) items.push_back(t);
          }
        
        // any aggregation events to report?
        if(items.empty()) return false;
        
        if(title) stream << CPPTRANSPORT_REPORT_DATABASE << '\n' << '\n';
        
        unsigned int count = 0;
        for(aggregation_profiler::const_iterator& t : items)
          {
            const std::unique_ptr<aggregation_profile_record>& rec = *t;
            
            reporting::key_value store(this->local_env, this->arg_cache);
            
            store.insert_back(CPPTRANSPORT_REPORT_DATABASE_EVENT_TIME, boost::posix_time::to_simple_string(rec->get_creation_time()));
            store.insert_back(CPPTRANSPORT_REPORT_DATABASE_ROWS, format_number(rec->get_rows(), 5));
            
            auto total_time = rec->get_total_time();
            if(total_time)
              {
                constexpr unsigned int second = 1000*1000*1000;
                store.insert_back(CPPTRANSPORT_REPORT_DATABASE_TIME, format_time(*total_time));
                
                double rows = static_cast<double>(rec->get_rows());
                double seconds = static_cast<double>(*total_time) / static_cast<double>(second);
                store.insert_back(CPPTRANSPORT_REPORT_DATABASE_ROWS_PER_SEC, format_number(rows/seconds, 5));
              }
            
            auto container_size = rec->get_container_size();
            if(container_size) store.insert_back(CPPTRANSPORT_REPORT_DATABASE_CTR_SIZE, format_memory(*container_size));
            
            auto temporary_size = rec->get_temporary_size();
            if(temporary_size) store.insert_back(CPPTRANSPORT_REPORT_DATABASE_TEMP_SIZE, format_memory(*temporary_size));
            
            if(count > 0) stream << '\n';

            stream << rec->get_container_path().string() << '\n';
            store.set_tiling(true);
            store.write(stream, options);
            
            ++count;
          }
        
        return true;
      }
    
    
    bool report_manager::check_report_policy()
      {
        // if no work items left and we have not issued a final report, then issue one
        bool final_report_override = false;
        if(!this->issued_final_report && this->scheduler.get_items_remaining() == 0) final_report_override = true;
        
        // get time now
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        
        // compute seconds since last report, and seconds since start of task
        boost::posix_time::time_duration time_since_last_report = now - (!this->report_times.empty() ? this->report_times.front() : this->task_start_time);
        boost::posix_time::time_duration time_since_start_task  = now - this->task_start_time;
    
        // if less time has elapsed than the specified delay, don't issue a report
        unsigned int policy_delay = this->arg_cache.get_report_time_delay();
    
        if(!final_report_override && policy_delay > 0 && time_since_start_task.total_seconds() < policy_delay)
          return false;
    
        bool due = final_report_override;
        
        // a report is due if we're over the time interval between reports
        unsigned int policy_interval = this->arg_cache.get_report_time_interval();
        if(policy_interval > 0 && time_since_last_report.total_seconds() >= policy_interval) due = true;
        
        // otherwise, a report is due if we're over the percentage interval between reports
        unsigned int policy_percent = this->arg_cache.get_report_percent_interval();
        unsigned int completed = static_cast<unsigned int>(std::round(100.0*this->scheduler.query_completion()));
        unsigned int percent_since_last_report = completed - (!this->report_percentages.empty() ? this->report_percentages.front() : 0);
        if(policy_percent > 0 && percent_since_last_report >= policy_percent) due = true;
        
        if(!due) return false;
        
        this->report_times.push_front(now);
        this->report_percentages.push_front(completed);
        
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
            // increment use count and return
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
    
  }   // namespace transport


#endif //CPPTRANSPORT_REPORT_MANAGER_H
