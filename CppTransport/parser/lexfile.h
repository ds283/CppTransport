//
// Created by David Seery on 14/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __lexfile_H_
#define __lexfile_H_


#include <iostream>
#include <memory>
#include <vector>

#include "filestack.h"


// reading from the stream is a finite state machine
// the available internal states are:

enum lexfile_state
	{
    lexfile_unready,  // current value of c does not make sense; need to read a new one
    lexfile_eof,      // at end of file
    lexfile_error,    // an error state
    lexfile_ready     // current value of c is valid; can simply return this
	};

// the states reported via the API are:

enum lexfile_outcome
	{
    lex_ok,       // everything was ok
    lex_eof,      // we are at the end of the file
    lex_error     // there was an error
	};


// read in characters from a file - the base step in the lexical analysis stack

class lexfile
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lexfile(const std::string& fnam, filestack& s);

    //! destructor
    ~lexfile();


    // INTERFACE

  public:

    //! return next character in the input. Can be called as many times as required
    //! without advancing our position in the file
    char get(enum lexfile_outcome& state);

    //! move past current character
    void eat();

    //! get current state of the file
    enum lexfile_outcome current_state() const;

    //! get shared_ptr to current line; used to allow lexemes generated from
    //! this line to co-own it
    const std::shared_ptr<std::string>& get_current_line() const;

    //! get current character position within current line
    unsigned int get_current_char_pos() const { return(this->char_pos); }

		// INTERNAL DATA

  private:

    //! filename
    std::string file;

    //! stream representing the input
    std::ifstream stream;

    //! filestack representing our current position in processing the input file hierarchy
    filestack& stack;

    //! char read from the file
    char c;

    //! current state of the file
    enum lexfile_state state;

    //! current character position in the line
    unsigned int char_pos;

    //! vector of lines; we use these to attach to lexemes, so they have a local context eg. for error messages.
    //! The strings representing each input line must persist for the lifetime of the
    //! translation unit, so that we don't get future dangling references;
    //! we do this using std::shared_ptr<> objects
    std::vector< std::shared_ptr<std::string> > line_array;

	};


#endif //__lexfile_H_
