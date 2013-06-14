//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <assert.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "lexstream.h"
#include "error.h"

// ******************************************************************

static bool parse(const std::string filename, finder* search,
                  std::deque<struct lexeme> lexeme_list, std::deque<struct inclusion>& inclusions);

// ******************************************************************

// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
lexstream::lexstream(const std::string filename, finder* search)
  {
    assert(search != NULL);

    std::deque<struct inclusion> inclusions;

    if(parse(filename, search, lexeme_list, inclusions) == false)
      {
        std::ostringstream msg;
        msg << ERROR_OPEN_FILE << " '" << filename << "'";
        error(msg.str());
      }
  }

// destroy lexeme stream
lexstream::~lexstream(void)
  {
  }

// ******************************************************************

static bool parse(const std::string filename, finder* search,
                  std::deque<struct lexeme> lexeme_list, std::deque<struct inclusion>& inclusions)
  {
    assert(search != NULL);

    unsigned int  line = 0;
    std::string   path = "";
    std::ifstream input;

    bool found = search->fqpn(filename, path);

    if(found)
      {
        std::cout << "Opened file '" << path << "'\n";

        // remember this inclusion
        struct inclusion inc;
        inc.line = 0;                // line doesn't count for the topmost file on the stack
        inc.name = path;

        inclusions.push_front(inc);  // note this takes a copy of inc

        // LEXICALIZE!

        inclusions.pop_front();      // pop this file from the stack
      }

    return(found);
  }
