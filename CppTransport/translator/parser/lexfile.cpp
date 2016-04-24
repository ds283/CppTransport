//
// Created by David Seery on 14/06/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <assert.h>

#include "lexfile.h"
#include "msg_en.h"


// ******************************************************************


lexfile::lexfile(const boost::filesystem::path& fnam, filestack& s)
  : file(fnam),
    stack(s),
    state(lexfile_state::unready),
    char_pos(0)
  {
    stream.open(fnam.string());    // when building with GCC LLVM 4.2, stream.open() doesn't accept std::string

    // build up array of lines
    std::string line;
    while(std::getline(stream, line))
      {
        line_array.push_back(std::make_shared<std::string>(line));
      }

    // reset to the beginning of the stream
    stream.clear();
    stream.seekg(0, std::ios::beg);

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
        case lexfile_state::unready:                       // this will be the most common case; we need to read a new char
          {
            if(this->stream.eof())
              {
                this->state = lexfile_state::eof;
                this->c     = 0;
                break;
              }

            while(this->state == lexfile_state::unready)
              {
                assert(this->stream.is_open());
                assert(!(this->stream.fail()));

                this->stream.get(this->c);
                if(this->stream.eof())                // reached end-of-file
                  {
                    this->state = lexfile_state::eof;
                    this->c     = 0;
                  }
                else if(this->stream.fail())          // there was an error
                  {
                    this->state = lexfile_state::error;
                    this->c     = 0;
                  }
                else                                  // can assume this character is valid
                  {
                    this->state = lexfile_state::ready;
                    if(this->c == '\n')
                      {
                        this->stack.increment_line();
                        this->char_pos = 0;
                      }
                  }
              }

            break;
          }

        case lexfile_state::ready:
          {
            break;                                    // don't need to do anything if we are already 'ready'
          }

        case lexfile_state::eof:
        case lexfile_state::error:
          {
            break;                                    // do nothing if there is already an error or eof condition
          }
      }

    if(this->state == lexfile_state::eof)
      {
        state = lexfile_outcome::eof;
      }
    else if(this->state == lexfile_state::error)
      {
        state = lexfile_outcome::error;
      }
    else
      {
        state = lexfile_outcome::ok;
      }

    return(this->c);
  }

void lexfile::eat()
  {
    assert(this->stream.is_open());

    this->state = lexfile_state::unready;
    this->char_pos++;
  }

enum lexfile_outcome lexfile::current_state() const
  {
    enum lexfile_outcome rval = lexfile_outcome::ok;

    if(this->state == lexfile_state::eof)
      {
        rval = lexfile_outcome::eof;
      }
    else if(this->state == lexfile_state::error)
      {
        rval = lexfile_outcome::error;
      }

    return(rval);
  }


const std::shared_ptr<std::string>& lexfile::get_current_line() const
  {
    unsigned int cline = this->stack.get_line();

    if(cline <= this->line_array.size())
      {
        return this->line_array[cline-1];
      }

    throw std::runtime_error(ERROR_CURRENT_LINE_EMPTY);
  }
