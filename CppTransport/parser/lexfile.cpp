//
// Created by David Seery on 14/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <iostream>
#include <fstream>
#include <assert.h>

#include "lexfile.h"


// ******************************************************************


lexfile::lexfile(std::string fnam)
  : file(fnam), line(0), c(0), state(lexfile_unready)
  {
    stream.open(fnam.c_str());    // when building with GCC LLVM 4.2, stream.open() doesn't accept std::string
    assert(stream.is_open());
  }

lexfile::~lexfile()
  {
    if(this->stream.is_open())
      {
        this->stream.close();
      }
  }


// ******************************************************************


char lexfile::get(enum lexfile_outcome& state)
  {
    switch(this->state)
      {
        case lexfile_unready:                       // this will be the most common case; we need to read a new char
          if(this->stream.eof())
            {
              this->state = lexfile_eof;
              this->c     = 0;
              break;
            }
          while(this->state == lexfile_unready)
            {
              assert(this->stream.is_open());
              assert(!(this->stream.fail()));

              this->stream.get(this->c);
              if(this->stream.eof())                // reached end-of-file
                {
                  this->state = lexfile_eof;
                  this->c     = 0;
                }
              else if(this->stream.fail())          // there was an error
                {
                  this->state = lexfile_error;
                  this->c     = 0;
                }
              else                                  // can assume this character is valid
                {
                  this->state = lexfile_ready;
                  if(this->c == '\n') this->line++;
                }
            }
          break;

        case lexfile_ready:
          break;                                    // don't need to do anything if we are already 'ready'

        case lexfile_eof:
        case lexfile_error:
          break;                                    // do nothing if there is already and error or eof condition

        default:
          assert(false);                            // shouldn't ready here
      }

    if(this->state == lexfile_eof)
      {
        state = lex_eof;
      }
    else if(this->state == lexfile_error)
      {
        state = lex_error;
      }
    else
      {
        state = lex_ok;
      }

    return(this->c);
  }

void lexfile::eat()
  {
    assert(this->stream.is_open());

    this->state = lexfile_unready;
  }

enum lexfile_outcome lexfile::current_state()
  {
    enum lexfile_outcome rval = lex_ok;

    if(this->state == lexfile_eof)
      {
        rval = lex_eof;
      }
    else if(this->state == lexfile_error)
      {
        rval = lex_error;
      }

    return(rval);
  }

unsigned int lexfile::current_line()
  {
    return(this->line);
  }
