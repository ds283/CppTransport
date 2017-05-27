//
// Created by David Seery on 01/12/2015.
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

#ifndef CPPTRANSPORT_LEXSTREAM_DATA_H
#define CPPTRANSPORT_LEXSTREAM_DATA_H


#include "error_context.h"
#include "argument_cache.h"
#include "finder.h"

#include "boost/optional.hpp"


class lexstream_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lexstream_data(const boost::filesystem::path& name, error_context::error_handler e, error_context::warning_handler w,
                   finder& s, argument_cache& c, boost::optional<unsigned int> ml = boost::none);

    //! destructor is default
    ~lexstream_data() = default;


    // GET ERROR_CONTEXT HANDLERS

  public:

    //! get error handler
    error_context::error_handler& get_error_handler() { return this->err; }

    //! get warning handler
    error_context::warning_handler& get_warning_handler() { return this->wrn; }


    // GET UTILITY OBJECTS ASSOCIATED WITH TRANSLATION UNIT

  public:

    //! get finder
    finder& get_finder() { return(this->search_paths); }


    // GET MODEL DATA

  public:

    //! get input filename
    const boost::filesystem::path& get_input_file() const { return this->filename; }

    //! get maximum number of lines to read from input file
    const boost::optional<unsigned int>& get_max_lines() const { return this->max_lines; }

    // INTERNAL DATA

  private:

    //! filename of model description
    boost::filesystem::path filename;
    
    //! maximum number of lines to read, if supplied
    boost::optional<unsigned int> max_lines;


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
