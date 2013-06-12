//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __lexstream_H_
#define __lexstream_H_

#include <iostream>
#include <string>
#include <list>

#include "finder.h"

enum lexeme_type
  {
    keyword, identifier, integer, decimal, string, open_brace, close_brace, open_bracket, close_bracket,
    open_square, close_square, comma, period, semicolon };
  };

enum keyword_type
  {
    name, tag, fields, potential
  };

struct lexeme
  {
    enum lexeme_type type;
    union
      {
        enum keyword_type keyword;
        std::string       identifier;
        int               integer;
        double            decimal;
        std::string       string;
      } value;

    std::list<std::string> path;
    unsigned int line;
  };

class lexstream
  {
    public:
      lexstream(const std::string filename, finder search);
      ~lexstream();

      lexeme read();

      void dump(std::ostream);

    private:
      std::list<struct lexeme> lexeme_list;

      finder* spaths;
  };


#endif //__lexstream_H_
