// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "y_parser.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "y_parser.tab.hh"

// User implementation prologue.

#line 51 "y_parser.tab.cc" // lalr1.cc:412
// Unqualified %code blocks.
#line 29 "y_parser.yy" // lalr1.cc:413

    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    #include "y_lexer.h"
    #include "y_driver.h"
    #include "error.h"

    #include "ginac/ginac.h"

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_parser::location_type* yyloc,
                     y::y_lexer* lexer,
                     y::y_driver* driver)
      {
        return(lexer->yylex(yylval));
      }

#line 73 "y_parser.tab.cc" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
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


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "y_parser.yy" // lalr1.cc:479
namespace y {
#line 159 "y_parser.tab.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  y_parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  y_parser::y_parser (y_lexer*  lexer_yyarg, y_driver* driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      lexer (lexer_yyarg),
      driver (driver_yyarg)
  {}

  y_parser::~y_parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  y_parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  y_parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  y_parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  y_parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  y_parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  y_parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  y_parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  y_parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  y_parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  y_parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  y_parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  y_parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  y_parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  y_parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  y_parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  y_parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  y_parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  y_parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  y_parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  y_parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  y_parser::symbol_number_type
  y_parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  y_parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  y_parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  y_parser::stack_symbol_type&
  y_parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  y_parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  y_parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  y_parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  y_parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  y_parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
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
#endif // YYDEBUG

  inline y_parser::state_type
  y_parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

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
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, lexer, driver));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 3:
#line 150 "y_parser.yy" // lalr1.cc:859
    { driver->set_potential((yystack_[1].value.x)); }
#line 640 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 4:
#line 151 "y_parser.yy" // lalr1.cc:859
    { driver->set_name((yystack_[1].value.lex)); }
#line 646 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 5:
#line 152 "y_parser.yy" // lalr1.cc:859
    { driver->set_author((yystack_[1].value.lex)); }
#line 652 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 6:
#line 153 "y_parser.yy" // lalr1.cc:859
    { driver->set_tag((yystack_[1].value.lex)); }
#line 658 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 7:
#line 154 "y_parser.yy" // lalr1.cc:859
    { driver->set_core((yystack_[1].value.lex)); }
#line 664 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 155 "y_parser.yy" // lalr1.cc:859
    { driver->set_implementation((yystack_[1].value.lex)); }
#line 670 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 9:
#line 156 "y_parser.yy" // lalr1.cc:859
    { driver->set_model((yystack_[1].value.lex)); }
#line 676 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 10:
#line 157 "y_parser.yy" // lalr1.cc:859
    { driver->set_indexorder_left(); }
#line 682 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 11:
#line 158 "y_parser.yy" // lalr1.cc:859
    { driver->set_indexorder_right(); }
#line 688 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 12:
#line 159 "y_parser.yy" // lalr1.cc:859
    { driver->add_field((yystack_[1].value.lex), (yystack_[2].value.a)); }
#line 694 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 13:
#line 160 "y_parser.yy" // lalr1.cc:859
    { driver->add_parameter((yystack_[1].value.lex), (yystack_[2].value.a)); }
#line 700 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 14:
#line 161 "y_parser.yy" // lalr1.cc:859
    { driver->set_background_stepper((yystack_[1].value.s)); }
#line 706 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 15:
#line 162 "y_parser.yy" // lalr1.cc:859
    { driver->set_perturbations_stepper((yystack_[1].value.s)); }
#line 712 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 17:
#line 166 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.a) = (yystack_[1].value.a); }
#line 718 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 167 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.a) = new attributes; }
#line 724 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 19:
#line 170 "y_parser.yy" // lalr1.cc:859
    { driver->add_latex_attribute((yystack_[3].value.a), (yystack_[1].value.lex)); (yylhs.value.a) = (yystack_[3].value.a); }
#line 730 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 171 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.a) = new attributes; }
#line 736 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 21:
#line 174 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); }
#line 742 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 176 "y_parser.yy" // lalr1.cc:859
    { driver->set_abserr((yystack_[4].value.s), (yystack_[1].value.lex)); (yylhs.value.s) = (yystack_[4].value.s); }
#line 748 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 23:
#line 177 "y_parser.yy" // lalr1.cc:859
    { driver->set_relerr((yystack_[4].value.s), (yystack_[1].value.lex)); (yylhs.value.s) = (yystack_[4].value.s); }
#line 754 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 24:
#line 178 "y_parser.yy" // lalr1.cc:859
    { driver->set_stepper((yystack_[4].value.s), (yystack_[1].value.lex)); (yylhs.value.s) = (yystack_[4].value.s); }
#line 760 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 179 "y_parser.yy" // lalr1.cc:859
    { driver->set_stepsize((yystack_[4].value.s), (yystack_[1].value.lex)); (yylhs.value.s) = (yystack_[4].value.s); }
#line 766 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 26:
#line 180 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = new stepper; }
#line 772 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 27:
#line 183 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = (yystack_[0].value.x); }
#line 778 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 28:
#line 184 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->add((yystack_[2].value.x), (yystack_[0].value.x)); }
#line 784 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 29:
#line 185 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->sub((yystack_[2].value.x), (yystack_[0].value.x)); }
#line 790 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 30:
#line 188 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = (yystack_[0].value.x); }
#line 796 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 31:
#line 189 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->mul((yystack_[2].value.x), (yystack_[0].value.x)); }
#line 802 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 32:
#line 190 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->div((yystack_[2].value.x), (yystack_[0].value.x)); }
#line 808 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 33:
#line 193 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = (yystack_[0].value.x); }
#line 814 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 34:
#line 194 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->pow((yystack_[2].value.x), (yystack_[0].value.x)); }
#line 820 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 35:
#line 197 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->get_integer((yystack_[0].value.lex)); }
#line 826 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 36:
#line 198 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->get_decimal((yystack_[0].value.lex)); }
#line 832 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 37:
#line 199 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->get_identifier((yystack_[0].value.lex)); }
#line 838 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 38:
#line 200 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = (yystack_[0].value.x); }
#line 844 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 39:
#line 201 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = (yystack_[1].value.x); }
#line 850 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 40:
#line 202 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->unary_minus((yystack_[0].value.x)); }
#line 856 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 41:
#line 205 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->abs((yystack_[1].value.x)); }
#line 862 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 42:
#line 206 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->step((yystack_[1].value.x)); }
#line 868 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 43:
#line 207 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->sqrt((yystack_[1].value.x)); }
#line 874 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 44:
#line 208 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->sin((yystack_[1].value.x)); }
#line 880 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 45:
#line 209 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->cos((yystack_[1].value.x)); }
#line 886 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 46:
#line 210 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->tan((yystack_[1].value.x)); }
#line 892 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 47:
#line 211 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->asin((yystack_[1].value.x)); }
#line 898 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 48:
#line 212 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->acos((yystack_[1].value.x)); }
#line 904 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 49:
#line 213 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->atan((yystack_[1].value.x)); }
#line 910 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 50:
#line 214 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->atan2((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 916 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 51:
#line 215 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->sinh((yystack_[1].value.x)); }
#line 922 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 52:
#line 216 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->cosh((yystack_[1].value.x)); }
#line 928 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 53:
#line 217 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->tanh((yystack_[1].value.x)); }
#line 934 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 54:
#line 218 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->asinh((yystack_[1].value.x)); }
#line 940 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 55:
#line 219 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->acosh((yystack_[1].value.x)); }
#line 946 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 56:
#line 220 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->atanh((yystack_[1].value.x)); }
#line 952 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 57:
#line 221 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->exp((yystack_[1].value.x)); }
#line 958 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 58:
#line 222 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->log((yystack_[1].value.x)); }
#line 964 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 59:
#line 223 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->pow((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 970 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 60:
#line 224 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->Li2((yystack_[1].value.x)); }
#line 976 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 61:
#line 225 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->Li((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 982 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 62:
#line 226 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->G((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 988 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 63:
#line 227 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->G((yystack_[5].value.x), (yystack_[3].value.x), (yystack_[1].value.x)); }
#line 994 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 64:
#line 228 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->S((yystack_[5].value.x), (yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1000 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 65:
#line 229 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->H((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1006 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 66:
#line 230 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->zeta((yystack_[1].value.x)); }
#line 1012 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 67:
#line 231 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->zeta((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1018 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 68:
#line 232 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->zetaderiv((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1024 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 69:
#line 233 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->tgamma((yystack_[1].value.x)); }
#line 1030 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 70:
#line 234 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->lgamma((yystack_[1].value.x)); }
#line 1036 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 71:
#line 235 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->beta((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1042 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 72:
#line 236 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->psi((yystack_[1].value.x)); }
#line 1048 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 73:
#line 237 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->psi((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1054 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 74:
#line 238 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->factorial((yystack_[1].value.x)); }
#line 1060 "y_parser.tab.cc" // lalr1.cc:859
    break;

  case 75:
#line 239 "y_parser.yy" // lalr1.cc:859
    { (yylhs.value.x) = driver->binomial((yystack_[3].value.x), (yystack_[1].value.x)); }
#line 1066 "y_parser.tab.cc" // lalr1.cc:859
    break;


#line 1070 "y_parser.tab.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
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
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
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

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
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
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  y_parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  y_parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char y_parser::yypact_ninf_ = -67;

  const signed char y_parser::yytable_ninf_ = -1;

  const short int
  y_parser::yypact_[] =
  {
     -67,     8,    -1,   -67,   -66,   -57,    11,    48,    34,    48,
      26,    51,    82,    82,    -8,    65,    93,    95,   109,   -67,
      91,   118,   134,   150,   166,   182,   196,   204,   206,   215,
     217,   226,   228,   236,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,    34,    34,   -67,   -67,   -67,    55,   -50,
     -67,   111,   -67,   123,   125,   139,   -67,   141,   153,   155,
     157,   169,   -67,   -67,   -67,    79,   171,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,   -44,
     105,   -67,    34,    34,    34,    34,    34,   173,   -67,   -67,
      80,   -67,   -67,   -67,   -67,   -67,   106,   -67,   -67,    32,
      57,    60,    62,    67,    72,    83,    94,    96,   188,    98,
     100,   110,   112,   116,   126,   128,   130,   199,   132,   201,
     203,   210,   212,    39,   214,   142,   144,   221,    43,   146,
     223,   -67,   -50,   -50,   -67,   -67,   -67,   -67,   258,   259,
     260,   261,   -67,   263,   -67,   -67,   -67,   -67,   -67,   -67,
     -67,   -67,   -67,    34,   -67,   -67,   -67,   -67,   -67,   -67,
     -67,   -67,    34,   -67,    34,    34,    34,    34,   -67,    34,
      34,   -67,   -67,    34,   -67,    34,   -67,    34,   235,   237,
     238,   262,   -67,   148,   158,   160,    50,   225,   162,   164,
     174,   176,   178,   180,   264,   265,   266,   267,   -67,   -67,
     -67,   -67,    34,    34,   -67,   -67,   -67,   -67,   -67,   -67,
     -67,   -67,   -67,   -67,   190,   192,   -67,   -67
  };

  const unsigned char
  y_parser::yydefact_[] =
  {
      16,     0,     2,     1,     0,     0,     0,    18,     0,    18,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,    35,    36,     0,    27,
      30,    33,    38,     0,     0,     0,    26,     0,     0,     0,
       0,     0,     4,     5,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     3,     0,     0,     0,     0,     0,     0,     7,     8,
       0,    14,    15,    10,    11,     9,     0,    17,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    39,    28,    29,    31,    32,    34,    13,     0,     0,
       0,     0,    21,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     0,    51,    52,    53,    54,    55,    56,
      57,    58,     0,    60,     0,     0,     0,     0,    66,     0,
       0,    69,    70,     0,    72,     0,    74,     0,     0,     0,
       0,     0,    19,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    59,
      61,    62,     0,     0,    65,    67,    68,    71,    73,    75,
      22,    23,    24,    25,     0,     0,    63,    64
  };

  const short int
  y_parser::yypgoto_[] =
  {
     -67,   -67,   -67,   306,   -67,   304,   -67,   -53,   138,   179,
     205,   -67
  };

  const signed char
  y_parser::yydefgoto_[] =
  {
      -1,     1,     2,    20,    75,    67,   120,    58,    59,    60,
      61,    62
  };

  const unsigned char
  y_parser::yytable_[] =
  {
     109,   110,     4,     5,     6,     7,     8,     9,     3,    10,
      11,    69,    70,   161,    16,    12,    13,    14,   114,   115,
      15,   112,   113,    17,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,   126,   174,
      53,    18,   168,   169,   170,   171,   198,   112,   113,   199,
     204,    54,    19,   205,   112,   113,    64,   231,   112,   113,
     232,    55,    56,    57,   175,   112,   113,   176,   111,   177,
     112,   113,   112,   113,   178,   112,   113,   112,   113,   179,
     213,    65,   112,   113,   127,   172,    66,   112,   113,   214,
     180,   215,   216,   217,   218,    71,   219,   220,   112,   113,
     221,   181,   222,   182,   223,   184,    72,   185,    73,   112,
     113,   112,   113,   112,   113,   112,   113,   186,    76,   187,
     112,   113,    74,   188,    77,   112,   113,   112,   113,   244,
     245,   112,   113,   189,   116,   190,   173,   191,   118,   193,
      78,   112,   113,   112,   113,   112,   113,   112,   113,   201,
     117,   202,   119,   206,   121,   228,    79,   112,   113,   112,
     113,   112,   113,   112,   113,   229,   122,   230,   123,   234,
     124,   235,    80,   112,   113,   112,   113,   112,   113,   112,
     113,   236,   125,   237,   128,   238,   167,   239,    81,   112,
     113,   112,   113,   112,   113,   112,   113,   246,   183,   247,
     162,   163,    82,   112,   113,   112,   113,   112,   113,   192,
      83,   194,    84,   195,   112,   113,   112,   113,   112,   113,
     196,    85,   197,    86,   200,   112,   113,   112,   113,   112,
     113,   203,    87,   207,    88,   233,   112,   113,   112,   113,
     112,   113,    89,   164,   165,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   224,    63,   225,    68,   226,     0,
       0,   166,   208,   209,   210,   211,   212,   240,   241,   242,
     243,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   227
  };

  const short int
  y_parser::yycheck_[] =
  {
      53,    54,     3,     4,     5,     6,     7,     8,     0,    10,
      11,    19,    20,    57,    80,    16,    17,    18,    68,    69,
      21,    65,    66,    80,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,     9,    57,
      56,    80,    12,    13,    14,    15,    57,    65,    66,    60,
      57,    67,    54,    60,    65,    66,    80,    57,    65,    66,
      60,    77,    78,    79,    57,    65,    66,    57,    63,    57,
      65,    66,    65,    66,    57,    65,    66,    65,    66,    57,
     183,    80,    65,    66,    55,    55,    54,    65,    66,   192,
      57,   194,   195,   196,   197,    80,   199,   200,    65,    66,
     203,    57,   205,    57,   207,    57,    63,    57,    63,    65,
      66,    65,    66,    65,    66,    65,    66,    57,    77,    57,
      65,    66,    63,    57,    56,    65,    66,    65,    66,   232,
     233,    65,    66,    57,    73,    57,    80,    57,    63,    57,
      56,    65,    66,    65,    66,    65,    66,    65,    66,    57,
      77,    57,    63,    57,    63,    57,    56,    65,    66,    65,
      66,    65,    66,    65,    66,    57,    63,    57,    63,    57,
      63,    57,    56,    65,    66,    65,    66,    65,    66,    65,
      66,    57,    63,    57,    63,    57,    63,    57,    56,    65,
      66,    65,    66,    65,    66,    65,    66,    57,    60,    57,
     112,   113,    56,    65,    66,    65,    66,    65,    66,    60,
      56,    60,    56,    60,    65,    66,    65,    66,    65,    66,
      60,    56,    60,    56,    60,    65,    66,    65,    66,    65,
      66,    60,    56,    60,    56,    60,    65,    66,    65,    66,
      65,    66,    56,   114,   115,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    79,     9,    79,    13,    80,    -1,
      -1,   116,    64,    64,    64,    64,    63,    63,    63,    63,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    79
  };

  const unsigned char
  y_parser::yystos_[] =
  {
       0,    82,    83,     0,     3,     4,     5,     6,     7,     8,
      10,    11,    16,    17,    18,    21,    80,    80,    80,    54,
      84,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    56,    67,    77,    78,    79,    88,    89,
      90,    91,    92,    84,    80,    80,    54,    86,    86,    19,
      20,    80,    63,    63,    63,    85,    77,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    88,
      88,    63,    65,    66,    68,    69,    73,    77,    63,    63,
      87,    63,    63,    63,    63,    63,     9,    55,    63,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    57,    89,    89,    90,    90,    91,    63,    12,    13,
      14,    15,    55,    80,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    60,    57,    57,    57,    57,    57,    57,
      57,    57,    60,    57,    60,    60,    60,    60,    57,    60,
      60,    57,    57,    60,    57,    60,    57,    60,    64,    64,
      64,    64,    63,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    79,    79,    80,    79,    57,    57,
      57,    57,    60,    60,    57,    57,    57,    57,    57,    57,
      63,    63,    63,    63,    88,    88,    57,    57
  };

  const unsigned char
  y_parser::yyr1_[] =
  {
       0,    81,    82,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    84,    84,    85,
      85,    86,    87,    87,    87,    87,    87,    88,    88,    88,
      89,    89,    89,    90,    90,    91,    91,    91,    91,    91,
      91,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92
  };

  const unsigned char
  y_parser::yyr2_[] =
  {
       0,     2,     1,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     5,     5,     4,     4,     0,     3,     0,     4,
       0,     3,     5,     5,     5,     5,     0,     1,     3,     3,
       1,     3,     3,     1,     3,     1,     1,     1,     1,     3,
       2,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       6,     4,     4,     4,     4,     4,     4,     4,     4,     6,
       4,     6,     6,     8,     8,     6,     4,     6,     6,     4,
       4,     6,     4,     6,     4,     6
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const y_parser::yytname_[] =
  {
  "$end", "error", "$undefined", "name", "author", "tag", "field",
  "potential", "parameter", "latex", "core", "implementation", "abserr",
  "relerr", "stepper", "stepsize", "background", "perturbations",
  "indexorder", "left", "right", "model", "abs", "step", "sqrt", "sin",
  "cos", "tan", "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh",
  "asinh", "acosh", "atanh", "exp", "log", "pow", "Li2", "Li", "G", "S",
  "H", "zeta", "zetaderiv", "tgamma", "lgamma", "beta", "psi", "factorial",
  "binomial", "open_brace", "close_brace", "open_bracket", "close_bracket",
  "open_square", "close_square", "comma", "period", "colon", "semicolon",
  "equals", "plus", "binary_minus", "unary_minus", "star", "backslash",
  "foreslash", "tilde", "ampersand", "circumflex", "ampersat", "ellipsis",
  "rightarrow", "identifier", "integer", "decimal", "string", "$accept",
  "program", "script", "attribute_block", "attributes", "stepper_block",
  "stepper_attributes", "expression", "term", "factor", "leaf",
  "built_in_function", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  y_parser::yyrline_[] =
  {
       0,   147,   147,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   166,   167,   170,
     171,   174,   176,   177,   178,   179,   180,   183,   184,   185,
     188,   189,   190,   193,   194,   197,   198,   199,   200,   201,
     202,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239
  };

  // Print the state stack on the debug stream.
  void
  y_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  y_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80
    };
    const unsigned int user_token_number_max_ = 335;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 5 "y_parser.yy" // lalr1.cc:1167
} // y
#line 1663 "y_parser.tab.cc" // lalr1.cc:1167
#line 241 "y_parser.yy" // lalr1.cc:1168


void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    std::ostringstream msg;
    lexeme::lexeme<enum keyword_type, enum character_type>* current_lexeme = this->lexer->get_current_lexeme();

		if(current_lexeme != nullptr)
			{
				const filestack* path = current_lexeme->get_path();
				msg << ERROR_MESSAGE_AT_LINE << " " << path->write() << std::endl << ERROR_MESSAGE_WRAP_PAD << err_message;
			}
		else
			{
		    msg << err_message;
			}

    this->driver->error(msg.str());
  }
