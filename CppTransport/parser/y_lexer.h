//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __y_lexer_H_
#define __y_lexer_H_

#undef  YY_DECL
#define YY_DECL int y::y_lexer::yylex()

#include <iostream>

#include "lexical.h"
#include "lexeme.h"
#include "lexstream.h"

#include "y_parser.tab.hh"

namespace y
  {

    class y_lexer
      {
        public:
          y_lexer(lexstream<enum keyword_type, enum character_type>* s);
          ~y_lexer();

          int yylex(y::y_parser::semantic_type* lval);

        private:
			    lexstream<enum keyword_type, enum character_type>* stream;
      };

  }

#endif //__y_lexer_H_
