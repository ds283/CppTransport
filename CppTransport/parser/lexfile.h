//
// Created by David Seery on 14/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __lexfile_H_
#define __lexfile_H_

#include <iostream>


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
    public:
      lexfile(std::string fnam, filestack* s);
      ~lexfile();

      char                 get(enum lexfile_outcome& state);
      void                 eat();

      enum lexfile_outcome current_state() const;

    private:
      std::string          file;
      std::ifstream        stream;

      filestack*           stack;

      char                 c;
      enum lexfile_state   state;
  };


#endif //__lexfile_H_
