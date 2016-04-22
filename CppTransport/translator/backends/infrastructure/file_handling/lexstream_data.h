//
// Created by David Seery on 01/12/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_LEXSTREAM_DATA_H
#define CPPTRANSPORT_LEXSTREAM_DATA_H


#include "error_context.h"
#include "argument_cache.h"
#include "finder.h"


class lexstream_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lexstream_data(const boost::filesystem::path& name, error_context::error_handler e, error_context::warning_handler w, finder& s, argument_cache& c);

    //! destructor is default
    ~lexstream_data() = default;


    // GET ERROR_CONTEXT HANDLERS

  public:

    //! get error handler
    error_context::error_handler& get_error_handler() { return(this->err); }

    //! get warning handler
    error_context::warning_handler& get_warning_handler() { return(this->wrn); }


    // GET UTILITY OBJECTS ASSOCIATED WITH TRANSLATION UNIT

  public:

    //! get finder
    finder& get_finder() { return(this->search_paths); }


    // GET MODEL DATA

  public:

    //! get filename of model descriptor
    const boost::filesystem::path& get_model_input() const { return(this->filename); }


    // INTERNAL DATA

  private:

    //! filename of model description
    boost::filesystem::path filename;


    // HANDLERS FOR ERROR_CONTEXT

    //! error handler
    error_context::error_handler err;

    //! warning handler
    error_context::warning_handler wrn;


    // UTILITY OBJECTS

    //! argument cache
    argument_cache& cache;

    //! search paths
    finder& search_paths;


  };


#endif //CPPTRANSPORT_LEXSTREAM_DATA_H
