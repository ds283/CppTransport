//
// Created by David Seery on 14/06/2013.
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


#ifndef CPPTRANSPORT_LEXFILE_H
#define CPPTRANSPORT_LEXFILE_H


#include <iostream>
#include <memory>
#include <vector>
#include <tuple>

#include "filestack.h"

#include "boost/filesystem/operations.hpp"

//! read in characters from a file - the base step in the lexical analysis stack
class lexfile
	{

    // TYPES

  public:

    // reading from the stream is a finite state machine

    // the states reported via the API are:
    enum class status
      {
        ok,       // everything was ok
        eof,      // we are at the end of the file
        error     // there was an error
      };

    // return type is a character and an outcome
    typedef std::tuple< char, status > value_type;

  protected:

    // the available internal states are:

    enum class internal_state
      {
        unready,  // current value of c does not make sense; need to read a new one
        eof,      // at end of file
        error,    // an error state
        ready     // current value of c is valid; can simply return this
      };


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lexfile(const boost::filesystem::path& fnam, filestack& s);

    //! destructor
    ~lexfile();


    // INTERFACE

  public:

    //! return next character in the input. Can be called as many times as required
    //! without advancing our position in the file
    value_type get();
    value_type operator*() { return this->get(); }

    //! move past current character
    lexfile& operator++();

    //! get current state of the file
    status get_state() const;

    //! get shared_ptr to current line; used to allow lexemes generated from
    //! this line to co-own it
    const std::shared_ptr<std::string>& get_current_line() const;

    //! get current character position within current line
    unsigned int get_current_char_pos() const { return(this->char_pos); }


    // INTERNAL API

  protected:

    //! try to go from a not-ready to a ready state
    void make_ready();


		// INTERNAL DATA

  private:

    //! filename
    boost::filesystem::path file;

    //! stream representing the input
    std::ifstream stream;

    //! filestack representing our current position in processing the input file hierarchy
    filestack& stack;

    //! char read from the file; whether it has current meaning depends on the value
    //! of the internal state variable 'state' below
    char c;

    //! current state of the file
    internal_state state;

    //! current character position in the line
    unsigned int char_pos;

    //! vector of lines; we use these to attach to lexemes, so they have a local context eg. for error messages.
    //! The strings representing each input line must persist for the lifetime of the
    //! translation unit, so that we don't get future dangling references;
    //! we do this using std::shared_ptr<> objects
    std::vector< std::shared_ptr<std::string> > line_array;

	};


#endif //CPPTRANSPORT_LEXFILE_H
