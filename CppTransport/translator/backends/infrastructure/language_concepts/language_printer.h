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
#include "index_literal.h"
#include "indices.h"
#include "lambdas.h"

#include "disable_warnings.h"
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
    
    //! generate 1D array subscript
    template <typename A>
    std::string
    array_subscript(const std::string& kernel, A a, boost::optional<std::string> flatten) const;
    
    //! generate 2D array subscript
    template <typename A, typename B>
    std::string
    array_subscript(const std::string& kernel, A a, B b, boost::optional<std::string> flatten) const;
    
    //! generate 3D array subscript
    template <typename A, typename B, typename C>
    std::string
    array_subscript(const std::string& kernel, A a, B b, C c, boost::optional<std::string> flatten) const;
    
  protected:
    
    //! format array subscripts
    virtual std::string
    format_array_subscript(const std::string& kernel, const std::initializer_list<std::string> args,
                           const boost::optional<std::string>& flatten) const = 0;


    // INTERFACE -- INITIALIZATION LISTS

  public:

    //! generate initialization list from a set of strings
    virtual std::string initialization_list(const std::vector<std::string>& list, bool quote) const = 0;

    //! generate initialization list from a set of strings - cpptsample style
    virtual std::string cpptsample_fields_list(const std::vector<std::string>& list, bool init) const = 0;


    // INTERFACE -- LAMBDAS

  public:
    
    //! format a lambda invokation string, remembering that the indices it is being invoked with (supplied in
    //! 'indices') may be different from the indices it was defined with
    virtual std::string lambda_invokation(const std::string& name, const generic_lambda& lambda,
                                          const abstract_index_database& indices) const = 0;

    //! format a lambda definition open string
    virtual std::string open_lambda(const abstract_index_database& indices, const std::string& return_type) const = 0;

    //! format a lambda definition close string
    virtual std::string close_lambda() const = 0;

    //! format a return statement
    virtual std::string format_return(const GiNaC::ex& expr) const = 0;
    virtual std::string format_return(const std::string& expr) const = 0;

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


inline std::string to_index_string(const abstract_index& idx)
  {
    return idx.get_loop_variable();
  }


inline std::string to_index_string(const index_literal& idx)
  {
    return to_index_string(idx.get());
  }


template <typename ClassTag>
inline std::string to_index_string(const tensor_index_impl::tensor_index<ClassTag>& idx)
  {
    return std::to_string(idx.get());
  }


inline std::string to_index_string(unsigned int i)
  {
    return std::to_string(i);
  }


inline std::string to_index_string(const std::string& str)
  {
    return str;
  }


template <typename A>
std::string
language_printer::array_subscript(const std::string& kernel, A a, boost::optional<std::string> flatten) const
  {
    std::string arg = to_index_string(a);
    return this->format_array_subscript(kernel, { arg }, flatten);
  }


template <typename A, typename B>
std::string
language_printer::array_subscript(const std::string& kernel, A a, B b, boost::optional<std::string> flatten) const
  {
    std::string arg1 = to_index_string(a);
    std::string arg2 = to_index_string(b);

    return this->format_array_subscript(kernel, { arg1, arg2 }, flatten);
  }


template <typename A, typename B, typename C>
std::string
language_printer::array_subscript(const std::string& kernel, A a, B b, C c, boost::optional<std::string> flatten) const
  {
    std::string arg1 = to_index_string(a);
    std::string arg2 = to_index_string(b);
    std::string arg3 = to_index_string(c);
    
    return this->format_array_subscript(kernel, { arg1, arg2, arg3 }, flatten);
  }


#endif //CPPTRANSPORT_LANGUAGE_PRINTER_H
