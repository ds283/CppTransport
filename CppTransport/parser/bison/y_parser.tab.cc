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
#line 27 "y_parser.yy"

    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    #include "y_lexer.h"
    #include "y_driver.h"
    #include "error.h"

    #include "ginac/ginac.h"

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_lexer* lexer,
                     y::y_driver* driver)
      {
        return(lexer->yylex(yylval));
      }


/* Line 290 of lalr1.cc  */
#line 70 "y_parser.tab.cc"


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
#line 166 "y_parser.tab.cc"

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
          case 3:
/* Line 664 of lalr1.cc  */
#line 135 "y_parser.yy"
    { driver->get_script()->add_declaration((yysemantic_stack_[(2) - (2)].d)); }
    break;

  case 4:
/* Line 664 of lalr1.cc  */
#line 136 "y_parser.yy"
    { driver->get_script()->set_potential((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 5:
/* Line 664 of lalr1.cc  */
#line 137 "y_parser.yy"
    { driver->set_name((yysemantic_stack_[(4) - (3)].lex)); }
    break;

  case 6:
/* Line 664 of lalr1.cc  */
#line 138 "y_parser.yy"
    { driver->set_author((yysemantic_stack_[(4) - (3)].lex)); }
    break;

  case 7:
/* Line 664 of lalr1.cc  */
#line 139 "y_parser.yy"
    { driver->set_tag((yysemantic_stack_[(4) - (3)].lex)); }
    break;

  case 8:
/* Line 664 of lalr1.cc  */
#line 140 "y_parser.yy"
    { driver->set_class((yysemantic_stack_[(4) - (3)].lex)); }
    break;

  case 9:
/* Line 664 of lalr1.cc  */
#line 141 "y_parser.yy"
    { driver->set_model((yysemantic_stack_[(4) - (3)].lex)); }
    break;

  case 11:
/* Line 664 of lalr1.cc  */
#line 145 "y_parser.yy"
    { (yyval.d) = (yysemantic_stack_[(1) - (1)].d); }
    break;

  case 12:
/* Line 664 of lalr1.cc  */
#line 146 "y_parser.yy"
    { (yyval.d) = (yysemantic_stack_[(1) - (1)].d); }
    break;

  case 13:
/* Line 664 of lalr1.cc  */
#line 149 "y_parser.yy"
    { (yyval.d) = driver->make_field((yysemantic_stack_[(4) - (3)].lex), (yysemantic_stack_[(4) - (2)].a)); }
    break;

  case 14:
/* Line 664 of lalr1.cc  */
#line 152 "y_parser.yy"
    { (yyval.d) = driver->make_parameter((yysemantic_stack_[(4) - (3)].lex), (yysemantic_stack_[(4) - (2)].a)); }
    break;

  case 15:
/* Line 664 of lalr1.cc  */
#line 155 "y_parser.yy"
    { (yyval.a) = (yysemantic_stack_[(3) - (2)].a); }
    break;

  case 16:
/* Line 664 of lalr1.cc  */
#line 156 "y_parser.yy"
    { (yyval.a) = new attributes; }
    break;

  case 17:
/* Line 664 of lalr1.cc  */
#line 159 "y_parser.yy"
    { driver->add_latex_attribute((yysemantic_stack_[(4) - (1)].a), (yysemantic_stack_[(4) - (3)].lex)); (yyval.a) = (yysemantic_stack_[(4) - (1)].a); }
    break;

  case 18:
/* Line 664 of lalr1.cc  */
#line 160 "y_parser.yy"
    { (yyval.a) = new attributes; }
    break;

  case 19:
/* Line 664 of lalr1.cc  */
#line 163 "y_parser.yy"
    { (yyval.x) = (yysemantic_stack_[(1) - (1)].x); }
    break;

  case 20:
/* Line 664 of lalr1.cc  */
#line 164 "y_parser.yy"
    { (yyval.x) = driver->add((yysemantic_stack_[(3) - (1)].x), (yysemantic_stack_[(3) - (3)].x)); }
    break;

  case 21:
/* Line 664 of lalr1.cc  */
#line 165 "y_parser.yy"
    { (yyval.x) = driver->sub((yysemantic_stack_[(3) - (1)].x), (yysemantic_stack_[(3) - (3)].x)); }
    break;

  case 22:
/* Line 664 of lalr1.cc  */
#line 168 "y_parser.yy"
    { (yyval.x) = (yysemantic_stack_[(1) - (1)].x); }
    break;

  case 23:
/* Line 664 of lalr1.cc  */
#line 169 "y_parser.yy"
    { (yyval.x) = driver->mul((yysemantic_stack_[(3) - (1)].x), (yysemantic_stack_[(3) - (3)].x)); }
    break;

  case 24:
/* Line 664 of lalr1.cc  */
#line 170 "y_parser.yy"
    { (yyval.x) = driver->div((yysemantic_stack_[(3) - (1)].x), (yysemantic_stack_[(3) - (3)].x)); }
    break;

  case 25:
/* Line 664 of lalr1.cc  */
#line 173 "y_parser.yy"
    { (yyval.x) = (yysemantic_stack_[(1) - (1)].x); }
    break;

  case 26:
/* Line 664 of lalr1.cc  */
#line 174 "y_parser.yy"
    { (yyval.x) = driver->pow((yysemantic_stack_[(3) - (1)].x), (yysemantic_stack_[(3) - (3)].x)); }
    break;

  case 27:
/* Line 664 of lalr1.cc  */
#line 177 "y_parser.yy"
    { (yyval.x) = driver->get_integer((yysemantic_stack_[(1) - (1)].lex)); }
    break;

  case 28:
/* Line 664 of lalr1.cc  */
#line 178 "y_parser.yy"
    { (yyval.x) = driver->get_decimal((yysemantic_stack_[(1) - (1)].lex)); }
    break;

  case 29:
/* Line 664 of lalr1.cc  */
#line 179 "y_parser.yy"
    { (yyval.x) = driver->get_identifier((yysemantic_stack_[(1) - (1)].lex)); }
    break;

  case 30:
/* Line 664 of lalr1.cc  */
#line 180 "y_parser.yy"
    { (yyval.x) = (yysemantic_stack_[(1) - (1)].x); }
    break;

  case 31:
/* Line 664 of lalr1.cc  */
#line 181 "y_parser.yy"
    { (yyval.x) = (yysemantic_stack_[(3) - (2)].x); }
    break;

  case 32:
/* Line 664 of lalr1.cc  */
#line 184 "y_parser.yy"
    { (yyval.x) = driver->abs((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 33:
/* Line 664 of lalr1.cc  */
#line 185 "y_parser.yy"
    { (yyval.x) = driver->step((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 34:
/* Line 664 of lalr1.cc  */
#line 186 "y_parser.yy"
    { (yyval.x) = driver->sqrt((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 35:
/* Line 664 of lalr1.cc  */
#line 187 "y_parser.yy"
    { (yyval.x) = driver->sin((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 36:
/* Line 664 of lalr1.cc  */
#line 188 "y_parser.yy"
    { (yyval.x) = driver->cos((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 37:
/* Line 664 of lalr1.cc  */
#line 189 "y_parser.yy"
    { (yyval.x) = driver->tan((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 38:
/* Line 664 of lalr1.cc  */
#line 190 "y_parser.yy"
    { (yyval.x) = driver->asin((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 39:
/* Line 664 of lalr1.cc  */
#line 191 "y_parser.yy"
    { (yyval.x) = driver->acos((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 40:
/* Line 664 of lalr1.cc  */
#line 192 "y_parser.yy"
    { (yyval.x) = driver->atan((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 41:
/* Line 664 of lalr1.cc  */
#line 193 "y_parser.yy"
    { (yyval.x) = driver->atan2((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 42:
/* Line 664 of lalr1.cc  */
#line 194 "y_parser.yy"
    { (yyval.x) = driver->sinh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 43:
/* Line 664 of lalr1.cc  */
#line 195 "y_parser.yy"
    { (yyval.x) = driver->cosh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 44:
/* Line 664 of lalr1.cc  */
#line 196 "y_parser.yy"
    { (yyval.x) = driver->tanh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 45:
/* Line 664 of lalr1.cc  */
#line 197 "y_parser.yy"
    { (yyval.x) = driver->asinh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 46:
/* Line 664 of lalr1.cc  */
#line 198 "y_parser.yy"
    { (yyval.x) = driver->acosh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 47:
/* Line 664 of lalr1.cc  */
#line 199 "y_parser.yy"
    { (yyval.x) = driver->atanh((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 48:
/* Line 664 of lalr1.cc  */
#line 200 "y_parser.yy"
    { (yyval.x) = driver->exp((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 49:
/* Line 664 of lalr1.cc  */
#line 201 "y_parser.yy"
    { (yyval.x) = driver->log((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 50:
/* Line 664 of lalr1.cc  */
#line 202 "y_parser.yy"
    { (yyval.x) = driver->Li2((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 51:
/* Line 664 of lalr1.cc  */
#line 203 "y_parser.yy"
    { (yyval.x) = driver->Li((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 52:
/* Line 664 of lalr1.cc  */
#line 204 "y_parser.yy"
    { (yyval.x) = driver->G((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 53:
/* Line 664 of lalr1.cc  */
#line 205 "y_parser.yy"
    { (yyval.x) = driver->G((yysemantic_stack_[(8) - (3)].x), (yysemantic_stack_[(8) - (5)].x), (yysemantic_stack_[(8) - (7)].x)); }
    break;

  case 54:
/* Line 664 of lalr1.cc  */
#line 206 "y_parser.yy"
    { (yyval.x) = driver->S((yysemantic_stack_[(8) - (3)].x), (yysemantic_stack_[(8) - (5)].x), (yysemantic_stack_[(8) - (7)].x)); }
    break;

  case 55:
/* Line 664 of lalr1.cc  */
#line 207 "y_parser.yy"
    { (yyval.x) = driver->H((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 56:
/* Line 664 of lalr1.cc  */
#line 208 "y_parser.yy"
    { (yyval.x) = driver->zeta((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 57:
/* Line 664 of lalr1.cc  */
#line 209 "y_parser.yy"
    { (yyval.x) = driver->zeta((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 58:
/* Line 664 of lalr1.cc  */
#line 210 "y_parser.yy"
    { (yyval.x) = driver->zetaderiv((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 59:
/* Line 664 of lalr1.cc  */
#line 211 "y_parser.yy"
    { (yyval.x) = driver->tgamma((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 60:
/* Line 664 of lalr1.cc  */
#line 212 "y_parser.yy"
    { (yyval.x) = driver->lgamma((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 61:
/* Line 664 of lalr1.cc  */
#line 213 "y_parser.yy"
    { (yyval.x) = driver->beta((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 62:
/* Line 664 of lalr1.cc  */
#line 214 "y_parser.yy"
    { (yyval.x) = driver->psi((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 63:
/* Line 664 of lalr1.cc  */
#line 215 "y_parser.yy"
    { (yyval.x) = driver->psi((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;

  case 64:
/* Line 664 of lalr1.cc  */
#line 216 "y_parser.yy"
    { (yyval.x) = driver->factorial((yysemantic_stack_[(4) - (3)].x)); }
    break;

  case 65:
/* Line 664 of lalr1.cc  */
#line 217 "y_parser.yy"
    { (yyval.x) = driver->binomial((yysemantic_stack_[(6) - (3)].x), (yysemantic_stack_[(6) - (5)].x)); }
    break;


/* Line 664 of lalr1.cc  */
#line 803 "y_parser.tab.cc"
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
  const signed char y_parser::yypact_ninf_ = -52;
  const short int
  y_parser::yypact_[] =
  {
       -52,    16,    -2,   -52,    15,    18,    28,    68,    36,    68,
      56,    60,   -52,   -52,   -52,    93,   107,   122,   -52,    86,
     116,   125,   134,   145,   154,   163,   174,   204,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,    36,   -52,   -52,   -52,   -41,   162,   -52,   128,   -52,
     139,   232,   233,   -52,   -52,   -52,    70,   234,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    38,
     -52,    36,    36,    36,    36,    36,   235,   -52,   -52,   236,
     -52,   -52,    44,    50,    53,    59,    62,    64,    80,    83,
      85,   173,    89,    98,   100,   103,   109,   112,   114,   118,
     123,   176,   179,   182,   185,   -39,   188,   127,   129,   191,
      34,   132,   194,   -52,   162,   162,   -52,   -52,   -52,   -52,
     237,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,
      36,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,
      36,    36,    36,    36,   -52,    36,    36,   -52,   -52,    36,
     -52,    36,   -52,    36,   -52,   138,   141,    40,   197,   143,
     147,   152,   156,   158,   161,   -52,   -52,   -52,    36,    36,
     -52,   -52,   -52,   -52,   -52,   -52,   167,   170,   -52,   -52
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  y_parser::yydefact_[] =
  {
        10,     0,     2,     1,     0,     0,     0,    16,     0,    16,
       0,     0,     3,    11,    12,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    27,    28,     0,    19,    22,    25,    30,
       0,     0,     0,     5,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     0,     0,     0,     0,     0,     8,     9,     0,
      15,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    20,    21,    23,    24,    26,    14,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    42,    43,    44,    45,    46,    47,    48,    49,    50,
       0,     0,     0,     0,    56,     0,     0,    59,    60,     0,
      62,     0,    64,     0,    17,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    41,    51,    52,     0,     0,
      55,    57,    58,    61,    63,    65,     0,     0,    53,    54
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  y_parser::yypgoto_[] =
  {
       -52,   -52,   -52,   -52,   -52,   -52,   268,   -52,   -51,    39,
     149,   175,   -52
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  y_parser::yydefgoto_[] =
  {
        -1,     1,     2,    12,    13,    14,    19,    66,    55,    56,
      57,    58,    59
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char y_parser::yytable_ninf_ = -1;
  const unsigned char
  y_parser::yytable_[] =
  {
        99,     4,     5,     6,     7,     8,     9,   174,    10,    11,
     175,   100,   101,   102,   101,   102,     3,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,   109,
     180,    51,    15,   181,   143,    16,   197,   101,   102,   198,
     151,   101,   102,   101,   102,    17,   152,   101,   102,   153,
      52,    53,    54,   101,   102,   154,   101,   102,   155,   185,
     156,    18,   101,   102,   110,   101,   102,   101,   102,   186,
     187,   188,   189,    61,   190,   191,   157,    62,   192,   158,
     193,   159,   194,   101,   102,   161,   101,   102,   101,   102,
     144,   145,   101,   102,   162,    63,   163,   206,   207,   164,
      67,   101,   102,   101,   102,   165,   101,   102,   166,    64,
     167,    68,   101,   102,   168,   101,   102,   101,   102,   169,
      69,   101,   102,   177,    65,   178,   101,   102,   182,    70,
     101,   102,   101,   102,   195,   101,   102,   196,   105,   200,
      71,   101,   102,   201,   101,   102,   101,   102,   202,    72,
     101,   102,   203,   106,   204,   101,   102,   205,    73,   101,
     102,   101,   102,   208,   101,   102,   209,   103,   104,    74,
     101,   102,   160,   101,   102,   170,   101,   102,   171,   101,
     102,   172,   101,   102,   173,   101,   102,   176,   101,   102,
     179,   101,   102,   183,   101,   102,   199,   101,   102,    75,
     101,   102,   146,   147,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    60,     0,     0,
     148,     0,     0,     0,   107,   108,   111,   149,     0,   184,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   150
  };

  /* YYCHECK.  */
  const short int
  y_parser::yycheck_[] =
  {
        51,     3,     4,     5,     6,     7,     8,    46,    10,    11,
      49,    52,    53,    54,    53,    54,     0,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     9,
      46,    45,    67,    49,    46,    67,    46,    53,    54,    49,
      46,    53,    54,    53,    54,    67,    46,    53,    54,    46,
      64,    65,    66,    53,    54,    46,    53,    54,    46,   160,
      46,    43,    53,    54,    44,    53,    54,    53,    54,   170,
     171,   172,   173,    67,   175,   176,    46,    67,   179,    46,
     181,    46,   183,    53,    54,    46,    53,    54,    53,    54,
     101,   102,    53,    54,    46,    52,    46,   198,   199,    46,
      64,    53,    54,    53,    54,    46,    53,    54,    46,    52,
      46,    45,    53,    54,    46,    53,    54,    53,    54,    46,
      45,    53,    54,    46,    52,    46,    53,    54,    46,    45,
      53,    54,    53,    54,    46,    53,    54,    46,    60,    46,
      45,    53,    54,    46,    53,    54,    53,    54,    46,    45,
      53,    54,    46,    64,    46,    53,    54,    46,    45,    53,
      54,    53,    54,    46,    53,    54,    46,    55,    56,    45,
      53,    54,    49,    53,    54,    49,    53,    54,    49,    53,
      54,    49,    53,    54,    49,    53,    54,    49,    53,    54,
      49,    53,    54,    49,    53,    54,    49,    53,    54,    45,
      53,    54,   103,   104,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,     9,    -1,    -1,
     105,    -1,    -1,    -1,    52,    52,    52,    52,    -1,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  y_parser::yystos_[] =
  {
         0,    69,    70,     0,     3,     4,     5,     6,     7,     8,
      10,    11,    71,    72,    73,    67,    67,    67,    43,    74,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    45,    64,    65,    66,    76,    77,    78,    79,    80,
      74,    67,    67,    52,    52,    52,    75,    64,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    76,
      52,    53,    54,    55,    56,    60,    64,    52,    52,     9,
      44,    52,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    46,    77,    77,    78,    78,    79,    52,
      67,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      49,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      49,    49,    49,    49,    46,    49,    49,    46,    46,    49,
      46,    49,    46,    49,    52,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    46,    46,    46,    49,    49,
      46,    46,    46,    46,    46,    46,    76,    76,    46,    46
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
     315,   316,   317,   318,   319,   320,   321,   322
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  y_parser::yyr1_[] =
  {
         0,    68,    69,    70,    70,    70,    70,    70,    70,    70,
      70,    71,    71,    72,    73,    74,    74,    75,    75,    76,
      76,    76,    77,    77,    77,    78,    78,    79,    79,    79,
      79,    79,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  y_parser::yyr2_[] =
  {
         0,     2,     1,     2,     4,     4,     4,     4,     4,     4,
       0,     1,     1,     4,     4,     3,     0,     4,     0,     1,
       3,     3,     1,     3,     3,     1,     3,     1,     1,     1,
       1,     3,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     6,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     6,     6,     8,     8,     6,     4,     6,     6,     4,
       4,     6,     4,     6,     4,     6
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const y_parser::yytname_[] =
  {
    "$end", "error", "$undefined", "name", "author", "tag", "field",
  "potential", "parameter", "latex", "k_class", "model", "abs", "step",
  "sqrt", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "sinh",
  "cosh", "tanh", "asinh", "acosh", "atanh", "exp", "log", "Li2", "Li",
  "G", "S", "H", "zeta", "zetaderiv", "tgamma", "lgamma", "beta", "psi",
  "factorial", "binomial", "open_brace", "close_brace", "open_bracket",
  "close_bracket", "open_square", "close_square", "comma", "period",
  "colon", "semicolon", "plus", "minus", "star", "backslash", "foreslash",
  "tilde", "ampersand", "circumflex", "ampersat", "ellipsis", "rightarrow",
  "identifier", "integer", "decimal", "string", "$accept", "program",
  "script", "declaration", "field_decl", "parameter_decl",
  "attribute_block", "attributes", "expression", "term", "factor", "leaf",
  "built_in_function", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const y_parser::rhs_number_type
  y_parser::yyrhs_[] =
  {
        69,     0,    -1,    70,    -1,    70,    71,    -1,    70,     7,
      76,    52,    -1,    70,     3,    67,    52,    -1,    70,     4,
      67,    52,    -1,    70,     5,    67,    52,    -1,    70,    10,
      67,    52,    -1,    70,    11,    67,    52,    -1,    -1,    72,
      -1,    73,    -1,     6,    74,    64,    52,    -1,     8,    74,
      64,    52,    -1,    43,    75,    44,    -1,    -1,    75,     9,
      67,    52,    -1,    -1,    77,    -1,    76,    53,    77,    -1,
      76,    54,    77,    -1,    78,    -1,    77,    55,    78,    -1,
      77,    56,    78,    -1,    79,    -1,    79,    60,    79,    -1,
      65,    -1,    66,    -1,    64,    -1,    80,    -1,    45,    76,
      46,    -1,    12,    45,    76,    46,    -1,    13,    45,    76,
      46,    -1,    14,    45,    76,    46,    -1,    15,    45,    76,
      46,    -1,    16,    45,    76,    46,    -1,    17,    45,    76,
      46,    -1,    18,    45,    76,    46,    -1,    19,    45,    76,
      46,    -1,    20,    45,    76,    46,    -1,    21,    45,    76,
      49,    76,    46,    -1,    22,    45,    76,    46,    -1,    23,
      45,    76,    46,    -1,    24,    45,    76,    46,    -1,    25,
      45,    76,    46,    -1,    26,    45,    76,    46,    -1,    27,
      45,    76,    46,    -1,    28,    45,    76,    46,    -1,    29,
      45,    76,    46,    -1,    30,    45,    76,    46,    -1,    31,
      45,    76,    49,    76,    46,    -1,    32,    45,    76,    49,
      76,    46,    -1,    32,    45,    76,    49,    76,    49,    76,
      46,    -1,    33,    45,    76,    49,    76,    49,    76,    46,
      -1,    34,    45,    76,    49,    76,    46,    -1,    35,    45,
      76,    46,    -1,    35,    45,    76,    49,    76,    46,    -1,
      36,    45,    76,    49,    76,    46,    -1,    37,    45,    76,
      46,    -1,    38,    45,    76,    46,    -1,    39,    45,    76,
      49,    76,    46,    -1,    40,    45,    76,    46,    -1,    40,
      45,    76,    49,    76,    46,    -1,    41,    45,    76,    46,
      -1,    42,    45,    76,    49,    76,    46,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  y_parser::yyprhs_[] =
  {
         0,     0,     3,     5,     8,    13,    18,    23,    28,    33,
      38,    39,    41,    43,    48,    53,    57,    58,    63,    64,
      66,    70,    74,    76,    80,    84,    86,    90,    92,    94,
      96,    98,   102,   107,   112,   117,   122,   127,   132,   137,
     142,   147,   154,   159,   164,   169,   174,   179,   184,   189,
     194,   199,   206,   213,   222,   231,   238,   243,   250,   257,
     262,   267,   274,   279,   286,   291
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  y_parser::yyrline_[] =
  {
         0,   132,   132,   135,   136,   137,   138,   139,   140,   141,
     142,   145,   146,   149,   152,   155,   156,   159,   160,   163,
     164,   165,   168,   169,   170,   173,   174,   177,   178,   179,
     180,   181,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217
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
      65,    66,    67
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int y_parser::yyeof_ = 0;
  const int y_parser::yylast_ = 303;
  const int y_parser::yynnts_ = 13;
  const int y_parser::yyempty_ = -2;
  const int y_parser::yyfinal_ = 3;
  const int y_parser::yyterror_ = 1;
  const int y_parser::yyerrcode_ = 256;
  const int y_parser::yyntokens_ = 68;

  const unsigned int y_parser::yyuser_token_number_max_ = 322;
  const y_parser::token_number_type y_parser::yyundef_token_ = 2;

/* Line 1135 of lalr1.cc  */
#line 5 "y_parser.yy"
} // y
/* Line 1135 of lalr1.cc  */
#line 1412 "y_parser.tab.cc"
/* Line 1136 of lalr1.cc  */
#line 219 "y_parser.yy"


void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    this->driver->error(err_message);
  }
