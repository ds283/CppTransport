//
// Created by David Seery on 09/12/2013.
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


#ifndef CPPTRANSPORT_LANGUAGE_PRINTER_H
#define CPPTRANSPORT_LANGUAGE_PRINTER_H


#include <functional>
#include <string>

#include "abstract_index.h"
#include "lambdas.h"

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


    // INTERFACE -- CONTROL STRUCTURES

  public:

    //! generate a 'for' loop appropriate for this backend; should be supplied by a concrete class
    virtual std::string for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const = 0;


    // INTERFACE -- ARRAY SUBSCRIPTING

  public:

    //! generate 1D array subscript without flattening
    virtual std::string array_subscript(const std::string& kernel, unsigned int a, unsigned int offset=0) const = 0;

    //! generate 1D array subscript
    virtual std::string array_subscript(const std::string& kernel, unsigned int a, const std::string& flatten, unsigned int offset=0) const = 0;

    //! generate 2D array subscript
    virtual std::string array_subscript(const std::string& kernel, unsigned int a, unsigned int b, const std::string& flatten) const = 0;

    //! generate 3D array subscript
    virtual std::string array_subscript(const std::string& kernel, unsigned int a, unsigned int b, unsigned int c, const std::string& flatten) const = 0;

    //! generate 1D array subscript without flattening
    virtual std::string array_subscript(const std::string& kernel, const abstract_index& a, unsigned int offset=0) const = 0;

    //! generate 1D array subscript
    virtual std::string array_subscript(const std::string& kernel, const abstract_index& a, const std::string& flatten, unsigned int offset=0) const = 0;

    //! generate 2D array subscript
    virtual std::string array_subscript(const std::string& kernel, const abstract_index& a, const abstract_index& b, const std::string& flatten) const = 0;

    //! generate 3D array subscript
    virtual std::string array_subscript(const std::string& kernel, const abstract_index& a, const abstract_index& b, const abstract_index& c, const std::string& flatten) const = 0;


    // INTERFACE -- INITIALIZATION LISTS

  public:

    //! generate initialization list from a set of strings
    virtual std::string initialization_list(const std::vector<std::string>& list, bool quote) const = 0;


    // INTERFACE -- LAMBDAS

  public:

    //! format a lambda invokation string
    virtual std::string lambda_invokation(const std::string& name, const generic_lambda& lambda) const = 0;

    //! format a lambda definition open string
    virtual std::string open_lambda(const abstract_index_list& indices, const std::string& return_type) const = 0;

    //! format a lambda definition close string
    virtual std::string close_lambda() const = 0;

    //! format a return statement
    virtual std::string format_return(const GiNaC::ex& expr) const = 0;

    //! format an if statement
    virtual std::string format_if(const std::list<GiNaC::ex>& conditions) const = 0;

    //! format an else-if statement
    virtual std::string format_elseif(const std::list<GiNaC::ex>& conditions) const = 0;

    //! format an else statement
    virtual std::string format_else(const GiNaC::ex& expr) const = 0;

    // INTERNAL DATA

  protected:

    //! generate debug output?
    bool debug;

  };


#endif //CPPTRANSPORT_LANGUAGE_PRINTER_H
