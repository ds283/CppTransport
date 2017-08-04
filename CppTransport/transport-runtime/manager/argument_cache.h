//
// Created by David Seery on 09/04/15.
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


#ifndef CPPTRANSPORT_ARGUMENT_CACHE_H
#define CPPTRANSPORT_ARGUMENT_CACHE_H


#include <string>
#include <list>

#include <algorithm>
#include <cmath>

#include "transport-runtime/defaults.h"

#include "boost/serialization/string.hpp"
#include "boost/serialization/list.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string.hpp"


namespace transport
	{

    enum class plot_style
      {
        raw_matplotlib,
        matplotlib_ggplot,
        matplotlib_ticks,
        seaborn
      };

    enum class matplotlib_backend
      {
        unset,
        Agg,
        Cairo,
        MacOSX,
        PDF
      };


    class argument_cache
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! constructor sets default values for all options
        //! Notice that argument_cache is a dependency of the error, warning and message handlers and therefore
        //! cannot itself handle errors; all errors should be pushed back to the caller by status flags or
        //! using exceptions. Then they have to be handled at the call-site rather than within argument_cache.
        argument_cache();

		    //! destructor is default
        ~argument_cache() = default;


        // GENERIC OPTIONS

      public:

        //! Get verbose status
        bool get_verbose() const                                  { return(this->verbose); }

        //! Set verbose status
        void set_verbose(bool g)                                  { this->verbose = g; }

        //! Get model list status
        bool get_model_list() const                               { return(this->list_models); }

        //! Set model list status
        void set_model_list(bool g)                               { this->list_models = g; }

        //! Get colourized output status
        bool get_colour_output() const                            { return(this->colour_output); }

        //! Set colourized output status
        void set_colour_output(bool g)                            { this->colour_output = g; }

        //! Get network mode
        bool get_network_mode() const                             { return(this->network_mode); }

        //! Set network mode
        void set_network_mode(bool m)                             { this->network_mode = m; }
        
        //! Commit failed integrations?
        bool get_commit_failed() const                            { return this->commit_failed; }
        
        //! Set commit-failed mode
        void set_commit_failed(bool c)                            { this->commit_failed = c; }


        // REPOSITORY OPTIONS

      public:

        //! Set create mode
        void set_create_mode(bool g)                              { this->create = g; }

        //! Get create mode status
        bool get_create_model() const                              { return(this->create); }


        // CHECKPOINTING AND RECOVERY

      public:

        //! Set recovery mode
        void set_recovery_mode(bool g)                            { this->recovery = g; }

        //! Get recovery mode status
        bool get_recovery_mode() const                            { return(this->recovery); }

        //! Set checkpoint interval
        bool set_checkpoint_interval(std::string interval);

        //! Get checkpoint interval
        unsigned int get_checkpoint_interval() const              { return(this->checkpoint_interval); }


        // CACHE CAPACITIES

      public:

        //! Set batcher capacity
        void set_batcher_capacity(size_t c)                       { this->batcher_capacity = c; }

        //! Get batcher capacity
        size_t get_batcher_capacity() const                       { return(this->batcher_capacity); }

        //! Set datapipe capacity
        void set_datapipe_capacity(size_t c)                      { this->pipe_capacity = c; }

        //! Get datapipe capacity
        size_t get_datapipe_capacity() const                      { return(this->pipe_capacity); }


        // MPI VISUALIZATION OPTIONS

      public:

        //! Set Gantt chart status
        void set_gantt_chart(bool g)                              { this->gantt_chart = g; }

        //! Get Gantt chart status
        bool get_gantt_chart() const                              { return(this->gantt_chart); }

        //! Set Gantt chart filename
        void set_gantt_filename(const std::string& f)             { this->gantt_filename = f; }

        //! Get Gantt chart filename
        const std::string& get_gantt_filename() const             { return(this->gantt_filename); }

        //! Set journal status
        void set_journal(bool g)                                  { this->journal = g; }

        //! Get journal status
        bool get_journal() const                                  { return(this->journal); }

        //! Set journal filename
        void set_journal_filename(const std::string& f)           { this->journal_filename = f; }

        //! Get journal filename
        const std::string& get_journal_filename() const           { return(this->journal_filename); }


        // PLOTTING OPTIONS

      public:

        //! Set plotting environment; returns true if environment was recognized or false if it was not
        bool set_plot_environment(std::string e);

        //! Set Matplotlib backend; returns true if backend was recognized or galse if it was not
        bool set_matplotlib_backend(std::string e);

        //! Get plotting environment
        plot_style get_plot_environment() const { return this->plot_env; }

        /// Get Matplotlib backend
        matplotlib_backend get_matplotlib_backend() const { return this->mpl_backend; }
        
        
        // TASK PROGRESS REPORTING
        
      public:
        
        //! Set percent interval between progress reports
        bool set_report_percent_interval(unsigned int interval);
        
        //! Set time interval between progress reports; returns true if interval was recognized, or false if not
        bool set_report_time_interval(std::string interval);
    
        //! Set time delay before first report is issued
        bool set_report_time_delay(std::string interval);
        
        //! Get percent interval between progress reports
        unsigned int get_report_percent_interval() const { return this->report_percent_interval; }
        
        //! Get time interval between progress reports; returned in seconds
        unsigned int get_report_time_interval() const { return this->report_time_interval; }
        
        //! Get time interval before first report is issued; returned in seconds
        unsigned int get_report_time_delay() const { return this->report_time_delay; }
        
        //! Set email address used for progress reports
        void set_report_email(const std::vector<std::string>& email);
        
        //! Get email address used for progress reports (may be null if no email address is set)
        const std::list< std::string >& get_report_email() const { return this->report_email; }
        
        //! Set flags for email events
        bool set_email_flags(const std::string& flags);
        
        //! Send email at begin task events?
        bool email_begin() const { return this->mail_begin; }
        
        //! Send email at end task events?
        bool email_end() const { return this->mail_end; }
        
        //! Send email at periodic reporting events?
        bool email_periodic() const { return this->mail_periodic; }
        
        // SEARCH PATHS

      public:

        //! set environment search paths
        template <typename Container>
        void set_search_paths(const Container& path_set);

        //! get search paths
        const std::list< boost::filesystem::path > get_search_paths();
        
        
        // UTILITY FUNCTIONS
  
      private:
    
        //! parse a time interval string into a number of seconds
        //! unit is the default unit (in seconds) if no unit is specified
        //! available units are s, m, h, d for seconds, minutes, hours, days
        bool parse_time_interval(std::string interval, unsigned int& result, unsigned int unit=60);


        // INTERNAL DATA

      private:

        //! produce Gantt chart?
        bool gantt_chart;

        //! filename for Gantt chart, if so
        std::string gantt_filename;

        //! produce journal
        bool journal;

        //! filename for journal, if so
        std::string journal_filename;

        //! verbose output?
        bool verbose;

        //! write out model information?
        bool list_models;

        //! recovery mode?
        bool recovery;

        //! create mode?
        bool create;

        //! colour output?
        bool colour_output;

        //! network mode?
        bool network_mode;
        
        //! commit integrations with failures?
        bool commit_failed;

        //! Storage capacity per batcher
        size_t batcher_capacity;

        //! Data cache capacity per datapipe
        size_t pipe_capacity;

        //! checkpoint interval in seconds. Zero indicates that checkpointing is disabled
        unsigned int checkpoint_interval;

        //! plotting environment
        plot_style plot_env;

        //! Matplotlib backend
        matplotlib_backend mpl_backend;

        //! search paths for assets, eg. jQuery, bootstrap ...
        //! have to use std::string internally since boost::filesystem::path won't serialize
        std::list< std::string > search_paths;
        
        //! percentage interval between updates
        unsigned int report_percent_interval;
        
        //! time interval between updates (value quoted in seconds)
        unsigned int report_time_interval;
        
        //! time interval before first report is issued (value quoted in seconds)
        unsigned int report_time_delay;
        
        //! email address used for updates
        std::list< std::string > report_email;
        
        //! send emails at begin task events?
        bool mail_begin;
        
        //! send emails at end task events?
        bool mail_end;
        
        //! send emails at periodic task events?
        bool mail_periodic;


        // enable boost::serialization support, and hence automated packing for transmission over MPI
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive& ar, unsigned int version)
          {
            ar & gantt_chart;
            ar & gantt_filename;
            ar & journal;
            ar & journal_filename;
            ar & verbose;
            ar & list_models;
            ar & recovery;
            ar & create;
            ar & colour_output;
            ar & network_mode;
            ar & commit_failed;
            ar & batcher_capacity;
            ar & pipe_capacity;
            ar & checkpoint_interval;
            ar & plot_env;
            ar & mpl_backend;
            ar & search_paths;
            ar & report_percent_interval;
            ar & report_time_interval;
            ar & report_time_delay;
            ar & report_email;
            ar & mail_begin;
            ar & mail_end;
            ar & mail_periodic;
          }

	    };


    argument_cache::argument_cache()
	    : gantt_chart(false),
        journal(false),
	      verbose(false),
        list_models(false),
        recovery(false),
        create(false),
        colour_output(true),
        network_mode(false),
        commit_failed(true),
        batcher_capacity(CPPTRANSPORT_DEFAULT_BATCHER_STORAGE),
        pipe_capacity(CPPTRANSPORT_DEFAULT_PIPE_STORAGE),
        checkpoint_interval(CPPTRANSPORT_DEFAULT_CHECKPOINT_INTERVAL),
        plot_env(plot_style::raw_matplotlib),
        mpl_backend(matplotlib_backend::unset),
        report_percent_interval(CPPTRANSPORT_DEFAULT_REPORT_PERCENT_INTERVAL),
        report_time_interval(CPPTRANSPORT_DEFAULT_REPORT_TIME_INTERVAL),
        report_time_delay(CPPTRANSPORT_DEFAULT_REPORT_TIME_DELAY),
        mail_begin(true),
        mail_end(true),
        mail_periodic(true)
	    {
	    }


    bool argument_cache::set_plot_environment(std::string e)
      {
        boost::algorithm::to_lower(e);

        if(e == "raw")          { this->plot_env = plot_style::raw_matplotlib; return true; }
        else if(e == "ggplot")  { this->plot_env = plot_style::matplotlib_ggplot; return true; }
        else if(e == "ticks")   { this->plot_env = plot_style::matplotlib_ticks; return true; }
        else if(e == "seaborn") { this->plot_env = plot_style::seaborn; return true; }

        return false;
      }


    bool argument_cache::set_matplotlib_backend(std::string e)
      {
        boost::algorithm::to_lower(e);

        if(e == "agg")         { this->mpl_backend = matplotlib_backend::Agg; return true; }
        else if(e == "cairo")  { this->mpl_backend = matplotlib_backend::Cairo; return true; }
        else if(e == "macosx") { this->mpl_backend = matplotlib_backend::MacOSX; return true; }
        else if(e == "pdf")    { this->mpl_backend = matplotlib_backend::PDF; return true; }

        return false;
      }


    template <typename Container>
    void argument_cache::set_search_paths(const Container& path_set)
      {
        for(const std::string& path : path_set)
          {
            boost::filesystem::path p = path;

            // if path is not absolute, make relative to current directory
            if(!p.is_absolute())
              {
                p = boost::filesystem::absolute(p);
              }

            this->search_paths.emplace_back(p.string());
          }
      }


    const std::list<boost::filesystem::path> argument_cache::get_search_paths()
      {
        std::list<boost::filesystem::path> list;
        std::copy(this->search_paths.cbegin(), this->search_paths.cend(), std::back_inserter(list));
        return list;
      }
    
    
    bool argument_cache::set_report_percent_interval(unsigned int interval)
      {
        if(interval >= 100) return false;
        
        this->report_percent_interval = interval;
        return true;
      }
    
    
    bool argument_cache::set_report_time_interval(std::string interval)
      {
        // unit defaults to minutes if no interval is given
        return this->parse_time_interval(std::move(interval), this->report_time_interval);
      }
    
    
    bool argument_cache::set_report_time_delay(std::string interval)
      {
        // unit defaults to minutes if no interval is given
        return this->parse_time_interval(std::move(interval), this->report_time_delay);
      }
    
    
    bool argument_cache::parse_time_interval(std::string interval, unsigned int& result, unsigned int unit)
      {
        constexpr unsigned int second = 1;
        constexpr unsigned int minute = 60;
        constexpr unsigned int hour   = 60*60;
        constexpr unsigned int day    = 24*60*60;
        
        if(interval.length() == 0) return false;
        
        if(tolower(interval.back()) == 's')
          {
            unit = second;
            interval.pop_back();
          }
        else if(tolower(interval.back()) == 'm')
          {
            unit = minute;
            interval.pop_back();
          }
        else if(tolower(interval.back()) == 'h')
          {
            unit = hour;
            interval.pop_back();
          }
        else if(tolower(interval.back()) == 'd')
          {
            unit = day;
            interval.pop_back();
          }
    
        int offset = 0;
        double value = 0;

        if((sscanf(interval.c_str(), "%lf%n", &value, &offset) == 1) && offset == interval.length() && value > 0)
          {
            result = static_cast<unsigned int>(std::round(std::abs(value) * unit));
            return true;
          }
        else if((sscanf(interval.c_str(), "%lg%n", &value, &offset) == 1) && offset == interval.length() && value > 0)
          {
            result = static_cast<unsigned int>(std::round(std::abs(value) * unit));
            return true;
          }
        else if((sscanf(interval.c_str(), "%lG%n", &value, &offset) == 1) && offset == interval.length() && value > 0)
          {
            result = static_cast<unsigned int>(std::round(std::abs(value) * unit));
            return true;
          }
        
        return false;
      }
    
    
    void argument_cache::set_report_email(const std::vector<std::string>& email)
      {
        this->report_email.clear();
        std::copy(email.begin(), email.end(), std::back_inserter(this->report_email));
      }
    
    
    bool argument_cache::set_checkpoint_interval(std::string interval)
      {
        // unit defaults to minutes if not specified explicitly, as here
        return this->parse_time_interval(interval, this->checkpoint_interval);
      }
    
    
    bool argument_cache::set_email_flags(const std::string& flags)
      {
        // cache current values, to be reset in the event of failure
        bool begin = this->mail_begin;
        bool end = this->mail_end;
        bool periodic = this->mail_periodic;
        
        this->mail_begin = false;
        this->mail_end = false;
        this->mail_periodic = false;
        
        for(const char& flag : flags)
          {
            switch(tolower(flag))
              {
                case 'b':
                  {
                    this->mail_begin = true;
                    break;
                  }
                
                case 'e':
                  {
                    this->mail_end = true;
                    break;
                  }
                
                case 'p':
                  {
                    this->mail_periodic = true;
                    break;
                  }
    
                default:
                  {
                    this->mail_begin = begin;
                    this->mail_end = end;
                    this->mail_periodic = periodic;
                    return false;
                  }
              }
          }
        
        return true;
      }
    
    
  }   // namespace transport


#endif //CPPTRANSPORT_ARGUMENT_CACHE_H
