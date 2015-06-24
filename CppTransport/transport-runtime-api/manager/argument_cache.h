//
// Created by David Seery on 09/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __argument_cache_H_
#define __argument_cache_H_


#include <string>


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
        bool get_verbose() const                       { return(this->verbose); }

        //! Set verbose status
        void set_verbose(bool g)                       { this->verbose = g; }

        //! Get model list status
        bool get_model_list() const                    { return(this->list_models); }

        //! Set model list status
        void set_model_list(bool g)                    { this->list_models = g; }

        //! Get colourized output status
        bool get_colour_output() const                 { return(this->colour_output); }

        //! Set colourized output status
        void set_colour_output(bool g)                 { this->colour_output = g; }


        // CHECKPOINTING AND RECOVERY

        public:

        //! Set recovery mode
        void set_recovery_mode(bool g)                 { this->recovery = g; }

        //! Get recovery mode status
        bool get_recovery_mode() const                 { return(this->recovery); }



        // MPI VISUALIZATION OPTIONS

      public:

        //! Set Gantt chart status
        void set_gantt_chart(bool g)                   { this->gantt_chart = g; }

        //! Get Gantt chart status
        bool get_gantt_chart() const                   { return(this->gantt_chart); }

        //! Set Gantt chart filename
        void set_gantt_filename(const std::string& f)  { this->gantt_filename = f; }

        //! Get Gantt chart filename
        const std::string& get_gantt_filename() const  { return(this->gantt_filename); }


        // INTERNAL DATA

      private:

        //! produce Gantt chart?
        bool gantt_chart;

        //! filename for Gantt chart, if so
        std::string gantt_filename;

        //! verbose output?
        bool verbose;

        //! write out model information?
        bool list_models;

        //! recovery mode?
        bool recovery;

        //! colour output?
        bool colour_output;

	    };


    argument_cache::argument_cache()
	    : gantt_chart(false),
	      verbose(false),
        list_models(false),
        recovery(false),
        colour_output(true)
	    {
	    }

	}   // namespace transport


#endif //__argument_cache_H_
