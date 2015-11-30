//
// Created by David Seery on 29/11/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_LOCAL_ENVIRONMENT_H
#define CPPTRANSPORT_LOCAL_ENVIRONMENT_H


#include <list>

#include "boost/filesystem/operations.hpp"


class local_environment
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    local_environment();

    //! destructor is default
    ~local_environment() = default;


    // TERMINAL PROPERTIES

    //! does the terminal support colourized output?
    bool has_colour_terminal_support() const { return(this->colour_terminal); }


    // ENVIRONMENT PATHS

    //! get template search paths specified via environment variables
    const std::list<std::string>& search_paths() const { return(this->template_search); }


    // INTERNAL API

  private:

    //! detect terminal colour support
    void set_terminal_colour_support();

    //! detect template search paths
    void set_template_search_paths();


    // INTERNAL DATA

  private:

    // TERMINAL PROPERTIES

    //! does terminal support colour output?
    bool colour_terminal;


    // ENVIRONMENT PATHS
    std::list<std::string> template_search;

  };


#endif //CPPTRANSPORT_LOCAL_ENVIRONMENT_H
