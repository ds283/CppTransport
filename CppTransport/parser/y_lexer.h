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

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        y_lexer(lexstream<enum keyword_type, enum character_type>* s);

        //! destructor
        ~y_lexer();

        //! get next lexeme
        int yylex(y::y_parser::semantic_type* lval);

		    //! return current lexeme
		    lexeme::lexeme<enum keyword_type, enum character_type>* get_current_lexeme() { return(this->current_lex); }

      private:

        lexstream<enum keyword_type, enum character_type>* stream;

		    lexeme::lexeme<enum keyword_type, enum character_type>* current_lex;

	    };

	}

#endif //__y_lexer_H_
