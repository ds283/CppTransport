//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_LANGUAGE_PRINTER_H
#define CPPTRANSPORT_LANGUAGE_PRINTER_H


#include <functional>
#include <string>

#include "ginac/ginac.h"

#include "boost/optional.hpp"


//! defines an abstract 'language_printer' interface; this is a policy
//! class which handles the details of formatting for output in a particular
//! language
class language_printer
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    language_printer()
      : debug(false)
      {
      }

    //! destructor is default
    virtual ~language_printer() = default;


    // INTERFACE -- PRINT TO STRING

  public:

    //! print a GiNaC expression to a string
    virtual std::string ginac(const GiNaC::ex& expr) const = 0;

    //! print a comment to a string
    virtual std::string comment(const std::string tag) const = 0;

    void set_debug_output(bool g) { this->debug = g; }


    // INTERFACE -- FORMATTING

  public:

    //! return delimiter for start-of-block, if used in this language
    virtual boost::optional< std::string > get_start_block_delimiter() const = 0;

    //! return delimiter for end-of-block, if used in this language
    virtual boost::optional< std::string > get_end_block_delimiter() const = 0;

    //! return indent-level for start-of-block delimitere
    virtual unsigned get_block_delimiter_indent() const = 0;

    //! return indent-level for block
    virtual unsigned get_block_indent() const = 0;


    // INTERFACE -- CODE PLANTING

  public:

    //! plant a 'for' loop appropriate for this backend; should be supplied by a concrete class
    virtual std::string plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const = 0;


    // INTERNAL DATA

  protected:

    //! generate debug output?
    bool debug;

  };


#endif //CPPTRANSPORT_LANGUAGE_PRINTER_H
