//
// Created by David Seery on 09/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_ARGUMENT_CACHE_H
#define CPPTRANSPORT_ARGUMENT_CACHE_H


#include <string>
#include <list>

#include <algorithm>

#include "transport-runtime/defaults.h"

#include "boost/serialization/string.hpp"
#include "boost/serialization/list.hpp"
#include "boost/filesystem/operations.hpp"


namespace transport
	{

    enum class plot_style
      {
        raw_matplotlib,
        matplotlib_ggplot,
        matplotlib_ticks,
        seaborn
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
        void set_checkpoint_interval(unsigned int i)              { this->checkpoint_interval = i; }

        //! Get checkpoint interval
        unsigned int get_checkpoint_interval() const              { return(this->checkpoint_interval); }


        // CACHE CAPACITIES

      public:

        //! Set batcher capacity
        void set_batcher_capacity(unsigned int c)                 { this->batcher_capacity = c; }

        //! Get batcher capacity
        unsigned int get_batcher_capacity() const                 { return(this->batcher_capacity); }

        //! Set datapipe capacity
        void set_datapipe_capacity(unsigned int c)                { this->pipe_capacity = c; }

        //! Get datapipe capacity
        unsigned int get_datapipe_capacity() const                { return(this->pipe_capacity); }


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
        bool set_plot_environment(const std::string& e);

        //! Get plotting environment
        plot_style get_plot_environment() const                   { return(this->plot_env); }


        // SEARCH PATHS

      public:

        //! set environment search paths
        template <typename Container>
        void set_search_paths(const Container& path_set);

        //! get search paths
        const std::list< boost::filesystem::path > get_search_paths();


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

        //! Storage capacity per batcher
        unsigned int batcher_capacity;

        //! Data cache capacity per datapipe
        unsigned int pipe_capacity;

        //! checkpoint interval in seconds. Zero indicates that checkpointing is disabled
        unsigned int checkpoint_interval;

        //! plotting environemtn
        plot_style plot_env;

        //! search paths for assets, eg. jQuery, bootstrap ...
        //! have to use std::string internally since boost::filesystem::path won't serialize
        std::list< std::string > search_paths;


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
            ar & batcher_capacity;
            ar & pipe_capacity;
            ar & checkpoint_interval;
            ar & plot_env;
            ar & search_paths;
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
        batcher_capacity(CPPTRANSPORT_DEFAULT_BATCHER_STORAGE),
        pipe_capacity(CPPTRANSPORT_DEFAULT_PIPE_STORAGE),
        checkpoint_interval(CPPTRANSPORT_DEFAULT_CHECKPOINT_INTERVAL),
        plot_env(plot_style::raw_matplotlib)
	    {
	    }


    bool argument_cache::set_plot_environment(const std::string& e)
      {
        if(e == "raw")          { this->plot_env = plot_style::raw_matplotlib; return true; }
        else if(e == "ggplot")  { this->plot_env = plot_style::matplotlib_ggplot; return true; }
        else if(e == "ticks")   { this->plot_env = plot_style::matplotlib_ticks; return true; }
        else if(e == "seaborn") { this->plot_env = plot_style::seaborn; return true; }

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


  }   // namespace transport


#endif //CPPTRANSPORT_ARGUMENT_CACHE_H
