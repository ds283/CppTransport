/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */

/* Line 283 of lalr1.cc  */
#line 38 "y_parser.tab.cc"


#include "y_parser.tab.hh"

/* User implementation prologue.  */

/* Line 289 of lalr1.cc  */
#line 46 "y_parser.tab.cc"
/* Unqualified %code blocks.  */
/* Line 290 of lalr1.cc  */
#line 25 "y_parser.yy"

    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    #include "y_lexer.h"
    #include "y_driver.h"
    #include "error.h"

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_lexer* lexer,
                     y::y_driver* driver)
      {
        return(lexer->yylex(yylval));
      }


/* Line 290 of lalr1.cc  */
#line 68 "y_parser.tab.cc"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

/* Line 357 of lalr1.cc  */
#line 5 "y_parser.yy"
namespace y {
/* Line 357 of lalr1.cc  */
#line 164 "y_parser.tab.cc"

  /// Build a parser object.
  y_parser::y_parser (y_lexer*  lexer_yyarg, y_driver* driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      lexer (lexer_yyarg),
      driver (driver_yyarg)
  {
  }

  y_parser::~y_parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  y_parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    YYUSE (yytype);
  }


  void
  y_parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  y_parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    YYUSE (yytype);
  }

  void
  y_parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  y_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  y_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  y_parser::debug_level_type
  y_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  y_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  y_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  y_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  y_parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_.clear ();
    yysemantic_stack_.clear ();
    yylocation_stack_.clear ();
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, lexer, driver);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
        
/* Line 664 of lalr1.cc  */
#line 429 "y_parser.tab.cc"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  y_parser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char y_parser::yypact_ninf_ = -64;
  const short int
  y_parser::yypact_[] =
  {
       -64,     1,   -64,   -63,   -51,    15,    42,    36,    42,   -64,
     -64,   -64,   -64,   -64,   -64,   -64,    43,    59,    69,   -64,
      61,    85,    98,   109,   118,   127,   138,   147,   167,   179,
     185,   187,   193,   195,   201,   203,   208,   213,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,    36,   -64,   -64,   -64,   -39,   199,   -64,    92,
     -64,    74,   -64,   -64,   -64,    68,   115,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,   -41,    36,
      36,    36,    36,    36,   129,   143,   -64,   -64,   -64,    38,
      44,    60,    62,    78,    81,    83,    87,    93,    56,    96,
     102,   105,   107,   111,   116,   120,   122,   125,   172,   180,
     182,   188,    34,   190,   131,   134,   196,    40,   136,   140,
     -64,   199,   199,   -64,   -64,   -64,   -64,   144,   -64,   -64,
     -64,   -64,   -64,   -64,   -64,   -64,   -64,    36,   -64,   -64,
     -64,   -64,   -64,   -64,   -64,   -64,   -64,    36,    36,    36,
      36,   -64,    36,    36,   -64,   -64,    36,   -64,    36,   -64,
     -64,   -64,   145,   149,    50,   198,   151,   154,   160,   163,
     165,   -64,   -64,   -64,    36,    36,   -64,   -64,   -64,   -64,
     -64,   169,   174,   -64,   -64
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  y_parser::yydefact_[] =
  {
         3,     0,     1,     0,     0,     0,    17,     0,    17,     2,
       4,     5,     6,     7,     8,     9,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    29,    30,    15,    21,    24,    27,
      32,     0,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    16,    18,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    22,    23,    25,    26,    28,    14,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    44,    45,
      46,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,    58,     0,     0,    61,    62,     0,    64,     0,    66,
      67,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    53,    54,     0,     0,    57,    59,    60,    63,
      65,     0,     0,    55,    56
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  y_parser::yypgoto_[] =
  {
       -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   191,
     -64,   -64,   -52,   155,   156,   170,   -64
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  y_parser::yydefgoto_[] =
  {
        -1,     1,     9,    10,    11,    12,    13,    14,    15,    20,
      65,   107,    56,    57,    58,    59,    60
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char y_parser::yytable_ninf_ = -1;
  const unsigned char
  y_parser::yytable_[] =
  {
        98,     2,    16,   140,     3,     4,     5,     6,     7,     8,
      99,   100,    99,   100,    17,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,   105,   171,    52,
      18,   172,   148,    19,   177,    99,   100,   178,   149,    99,
     100,    99,   100,    62,   193,    99,   100,   194,    53,    54,
      55,    99,   100,   157,   150,   182,   151,    99,   100,    63,
     106,    99,   100,    99,   100,   183,   184,   185,   186,    64,
     187,   188,   152,    66,   189,   153,   190,   154,    67,    99,
     100,   155,    99,   100,    99,   100,   104,   156,    99,   100,
     158,    68,   201,   202,    99,   100,   159,    99,   100,   160,
     103,   161,    69,    99,   100,   162,    99,   100,    99,   100,
     163,    70,    99,   100,   164,   108,   165,    99,   100,   166,
      71,    99,   100,    99,   100,   174,    99,   100,   175,   146,
     179,    72,    99,   100,   180,    99,   100,    99,   100,   191,
      73,    99,   100,   192,   181,   196,    99,   100,   197,    61,
      99,   100,    99,   100,   198,    99,   100,   199,   147,   200,
      74,    99,   100,   203,    99,   100,    99,   100,   204,   167,
      99,   100,    75,    99,   100,    99,   100,   168,    76,   169,
      77,    99,   100,    99,   100,   170,    78,   173,    79,    99,
     100,    99,   100,   176,    80,   195,    81,    99,   100,    99,
     100,    82,   101,   102,   141,   142,    83,   143,   144,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,   145
  };

  /* YYCHECK.  */
  const unsigned char
  y_parser::yycheck_[] =
  {
        52,     0,    65,    44,     3,     4,     5,     6,     7,     8,
      51,    52,    51,    52,    65,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     9,    44,    43,
      65,    47,    44,    41,    44,    51,    52,    47,    44,    51,
      52,    51,    52,    50,    44,    51,    52,    47,    62,    63,
      64,    51,    52,    47,    44,   157,    44,    51,    52,    50,
      42,    51,    52,    51,    52,   167,   168,   169,   170,    50,
     172,   173,    44,    62,   176,    44,   178,    44,    43,    51,
      52,    44,    51,    52,    51,    52,    62,    44,    51,    52,
      44,    43,   194,   195,    51,    52,    44,    51,    52,    44,
      58,    44,    43,    51,    52,    44,    51,    52,    51,    52,
      44,    43,    51,    52,    44,    50,    44,    51,    52,    44,
      43,    51,    52,    51,    52,    44,    51,    52,    44,    50,
      44,    43,    51,    52,    44,    51,    52,    51,    52,    44,
      43,    51,    52,    44,    50,    44,    51,    52,    44,     8,
      51,    52,    51,    52,    44,    51,    52,    44,    65,    44,
      43,    51,    52,    44,    51,    52,    51,    52,    44,    47,
      51,    52,    43,    51,    52,    51,    52,    47,    43,    47,
      43,    51,    52,    51,    52,    47,    43,    47,    43,    51,
      52,    51,    52,    47,    43,    47,    43,    51,    52,    51,
      52,    43,    53,    54,    99,   100,    43,   101,   102,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
      43,    43,    43,   103
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  y_parser::yystos_[] =
  {
         0,    67,     0,     3,     4,     5,     6,     7,     8,    68,
      69,    70,    71,    72,    73,    74,    65,    65,    65,    41,
      75,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    43,    62,    63,    64,    78,    79,    80,    81,
      82,    75,    50,    50,    50,    76,    62,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    78,    51,
      52,    53,    54,    58,    62,     9,    42,    77,    50,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      44,    79,    79,    80,    80,    81,    50,    65,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    47,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    47,    47,    47,
      47,    44,    47,    47,    44,    44,    47,    44,    47,    44,
      44,    50,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    44,    44,    44,    47,    47,    44,    44,    44,    44,
      44,    78,    78,    44,    44
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  y_parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  y_parser::yyr1_[] =
  {
         0,    66,    67,    67,    68,    68,    68,    68,    68,    68,
      69,    70,    71,    72,    73,    74,    75,    75,    76,    76,
      77,    78,    78,    78,    79,    79,    79,    80,    80,    81,
      81,    81,    81,    81,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  y_parser::yyr2_[] =
  {
         0,     2,     2,     0,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     4,     4,     2,     3,     0,     2,     0,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     1,
       1,     1,     1,     3,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     6,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     6,     6,     8,     8,     6,     4,     6,
       6,     4,     4,     6,     4,     6,     4,     4
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const y_parser::yytname_[] =
  {
    "$end", "error", "$undefined", "name", "author", "tag", "field",
  "potential", "parameter", "latex", "abs", "step", "sqrt", "sin", "cos",
  "tan", "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh", "asinh",
  "acosh", "atanh", "exp", "log", "Li2", "Li", "G", "S", "H", "zeta",
  "zetaderiv", "tgamma", "lgamma", "beta", "psi", "factorial", "binomial",
  "open_brace", "close_brace", "open_bracket", "close_bracket",
  "open_square", "close_square", "comma", "period", "colon", "semicolon",
  "plus", "minus", "star", "backslash", "foreslash", "tilde", "ampersand",
  "circumflex", "ampersat", "ellipsis", "rightarrow", "identifier",
  "integer", "decimal", "string", "$accept", "script", "declaration",
  "name_declaration", "author_declaration", "tag_declaration",
  "field_declaration", "parameter_declaration", "potential_declaration",
  "info_block", "info_declarations", "info_declaration", "expression",
  "term", "factor", "leaf", "built_in_function", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const y_parser::rhs_number_type
  y_parser::yyrhs_[] =
  {
        67,     0,    -1,    67,    68,    -1,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,     3,
      65,    50,    -1,     4,    65,    50,    -1,     5,    65,    50,
      -1,     6,    75,    62,    50,    -1,     8,    75,    62,    50,
      -1,     7,    78,    -1,    41,    76,    42,    -1,    -1,    76,
      77,    -1,    -1,     9,    65,    50,    -1,    79,    -1,    78,
      51,    79,    -1,    78,    52,    79,    -1,    80,    -1,    79,
      53,    80,    -1,    79,    54,    80,    -1,    81,    -1,    81,
      58,    81,    -1,    63,    -1,    64,    -1,    62,    -1,    82,
      -1,    43,    78,    44,    -1,    10,    43,    78,    44,    -1,
      11,    43,    78,    44,    -1,    12,    43,    78,    44,    -1,
      13,    43,    78,    44,    -1,    14,    43,    78,    44,    -1,
      15,    43,    78,    44,    -1,    16,    43,    78,    44,    -1,
      17,    43,    78,    44,    -1,    18,    43,    78,    44,    -1,
      19,    43,    78,    47,    78,    44,    -1,    20,    43,    78,
      44,    -1,    21,    43,    78,    44,    -1,    22,    43,    78,
      44,    -1,    23,    43,    78,    44,    -1,    24,    43,    78,
      44,    -1,    25,    43,    78,    44,    -1,    26,    43,    78,
      44,    -1,    27,    43,    78,    44,    -1,    28,    43,    78,
      44,    -1,    29,    43,    78,    47,    78,    44,    -1,    30,
      43,    78,    47,    78,    44,    -1,    30,    43,    78,    47,
      78,    47,    78,    44,    -1,    31,    43,    78,    47,    78,
      47,    78,    44,    -1,    32,    43,    78,    47,    78,    44,
      -1,    33,    43,    78,    44,    -1,    33,    43,    78,    47,
      78,    44,    -1,    34,    43,    78,    47,    78,    44,    -1,
      35,    43,    78,    44,    -1,    36,    43,    78,    44,    -1,
      37,    43,    78,    47,    78,    44,    -1,    38,    43,    78,
      44,    -1,    38,    43,    78,    47,    78,    44,    -1,    39,
      43,    78,    44,    -1,    40,    43,    78,    44,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  y_parser::yyprhs_[] =
  {
         0,     0,     3,     6,     7,     9,    11,    13,    15,    17,
      19,    23,    27,    31,    36,    41,    44,    48,    49,    52,
      53,    57,    59,    63,    67,    69,    73,    77,    79,    83,
      85,    87,    89,    91,    95,   100,   105,   110,   115,   120,
     125,   130,   135,   140,   147,   152,   157,   162,   167,   172,
     177,   182,   187,   192,   199,   206,   215,   224,   231,   236,
     243,   250,   255,   260,   267,   272,   279,   284
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  y_parser::yyrline_[] =
  {
         0,   113,   113,   114,   117,   118,   119,   120,   121,   122,
     125,   128,   131,   134,   137,   140,   143,   144,   147,   148,
     151,   154,   155,   156,   159,   160,   161,   164,   165,   168,
     169,   170,   171,   172,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208
  };

  // Print the state stack on the debug stream.
  void
  y_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  y_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  y_parser::token_number_type
  y_parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int y_parser::yyeof_ = 0;
  const int y_parser::yylast_ = 273;
  const int y_parser::yynnts_ = 17;
  const int y_parser::yyempty_ = -2;
  const int y_parser::yyfinal_ = 2;
  const int y_parser::yyterror_ = 1;
  const int y_parser::yyerrcode_ = 256;
  const int y_parser::yyntokens_ = 66;

  const unsigned int y_parser::yyuser_token_number_max_ = 320;
  const y_parser::token_number_type y_parser::yyundef_token_ = 2;

/* Line 1135 of lalr1.cc  */
#line 5 "y_parser.yy"
} // y
/* Line 1135 of lalr1.cc  */
#line 1032 "y_parser.tab.cc"
/* Line 1136 of lalr1.cc  */
#line 210 "y_parser.yy"


void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    ::error(err_message); // :: forces link to default namespace, so finds error.h implementation
  }
