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

static int parse(const std::string filename, finder* search,
                 std::list<struct lexeme> lexeme_list, std::list<std::string> inclusions);

// ******************************************************************

// convert the contents of 'filename' to a string of lexemes, descending into
// included files as necessary
lexstream::lexstream(const std::string filename, finder* search)
  : spaths(search)
  {
    assert(search != NULL);

    std::list<std::string> inclusions;

    if(parse(filename, search, lexeme_list, inclusions) != 0)
      {
        std:: ostringstream msg;
        msg << ERROR_OPEN_FILE << " '" << filename << "'";
        error(msg.str());
      }
  }

// destroy lexeme stream
lexstream::~lexstream(void)
  {
  }

// ******************************************************************

static int parse(const std::string filename, finder* search,
                 std::list<struct lexeme> lexeme_list, std::list<std::string> inclusions)
  {
    assert(search != NULL);

    unsigned int line = 0;
    std::ifstream input;

    input.open(filename);
  }
