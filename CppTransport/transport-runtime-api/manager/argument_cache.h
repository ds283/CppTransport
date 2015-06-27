//
// Created by David Seery on 09/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __argument_cache_H_
#define __argument_cache_H_


#include <string>

#include "boost/serialization/string.hpp"
#include "boost/serialization/list.hpp"

#include "transport-runtime-api/defaults.h"


namespace transport
	{
    class argument_cache
	    {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    //! constructor sets default values for all options
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

        //! colour output?
        bool colour_output;

        //! Storage capacity per batcher
        unsigned int batcher_capacity;

        //! Data cache capacity per datapipe
        unsigned int pipe_capacity;

        //! checkpoint interval in seconds. Zero indicates that checkpointing is disabled
        unsigned int checkpoint_interval;

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
            ar & colour_output;
            ar & batcher_capacity;
            ar & pipe_capacity;
            ar & checkpoint_interval;
          }

	    };


    argument_cache::argument_cache()
	    : gantt_chart(false),
        journal(false),
	      verbose(false),
        list_models(false),
        recovery(false),
        colour_output(true),
        batcher_capacity(CPPTRANSPORT_DEFAULT_BATCHER_STORAGE),
        pipe_capacity(CPPTRANSPORT_DEFAULT_PIPE_STORAGE),
        checkpoint_interval(CPPTRANSPORT_DEFAULT_CHECKPOINT_INTERVAL)
	    {
	    }

	}   // namespace transport


#endif //__argument_cache_H_
