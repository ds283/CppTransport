%skeleton "lalr1.cc"
%require "2.5"
%debug
%defines
%define namespace "y"
%define parser_class_name "y_parser"

%code requires {
    #include "lexeme.h"
    #include "parse_tree.h"
    #include "lexical.h"

    #include "ginac/ginac.h"

    namespace y {
        class y_driver;
        class y_lexer;
    }
}

%lex-param   { y_lexer*  lexer }
%parse-param { y_lexer*  lexer }

%lex-param   { y_driver* driver }
%parse-param { y_driver* driver }

%code {
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
}

%union {
    lexeme::lexeme<enum keyword_type, enum character_type>* lex;
    declaration*                                            d;
    attributes*                                             a;
    GiNaC::ex*                                              x;
}

%token          name
%token          author
%token          tag
%token          field
%token          potential
%token          parameter
%token          latex
%token          k_class
%token          model
%token          abs
%token          step
%token          sqrt
%token          sin
%token          cos
%token          tan
%token          asin
%token          acos
%token          atan
%token          atan2
%token          sinh
%token          cosh
%token          tanh
%token          asinh
%token          acosh
%token          atanh
%token          exp
%token          log
%token          Li2
%token          Li
%token          G
%token          S
%token          H
%token          zeta
%token          zetaderiv
%token          tgamma
%token          lgamma
%token          beta
%token          psi
%token          factorial
%token          binomial
%token          open_brace
%token          close_brace
%token          open_bracket
%token          close_bracket
%token          open_square
%token          close_square
%token          comma
%token          period
%token          colon
%token          semicolon
%token          plus
%token          minus
%token          star
%token          backslash
%token          foreslash
%token          tilde
%token          ampersand
%token          circumflex
%token          ampersat
%token          ellipsis
%token          rightarrow
%token <lex>    identifier
%token <lex>    integer
%token <lex>    decimal
%token <lex>    string

%type  <d>      declaration
%type  <d>      field_decl
%type  <d>      parameter_decl
%type  <a>      attribute_block
%type  <a>      attributes
%type  <x>      expression
%type  <x>      term
%type  <x>      factor
%type  <x>      leaf
%type  <x>      built_in_function

%%

program: script
        ;

script: script declaration                                          { driver->get_script()->add_declaration($2); }
        | script potential expression semicolon                     { driver->get_script()->set_potential($3); }
        | script name string semicolon                              { driver->set_name($3); }
        | script author string semicolon                            { driver->set_author($3); }
        | script tag string semicolon                               { driver->set_tag($3); }
        | script k_class string semicolon                           { driver->set_class($3); }
        | script model string semicolon                             { driver->set_model($3); }
        |
        ;

declaration: field_decl                                             { $$ = $1; }
        | parameter_decl                                            { $$ = $1; }
        ;

field_decl: field attribute_block identifier semicolon              { $$ = driver->make_field($3, $2); }
        ;

parameter_decl: parameter attribute_block identifier semicolon      { $$ = driver->make_parameter($3, $2); }
        ;

attribute_block: open_brace attributes close_brace                  { $$ = $2; }
        |                                                           { $$ = new attributes; }
        ;

attributes: attributes latex string semicolon                       { driver->add_latex_attribute($1, $3); $$ = $1; }
        |                                                           { $$ = new attributes; }
        ;

expression: term                                                    { $$ = $1; }
        | expression plus term                                      { $$ = driver->add($1, $3); }
        | expression minus term                                     { $$ = driver->sub($1, $3); }
        ;
        
term: factor                                                        { $$ = $1; }
        | term star factor                                          { $$ = driver->mul($1, $3); }
        | term backslash factor                                     { $$ = driver->div($1, $3); }
        ;

factor: leaf                                                        { $$ = $1; }
        | leaf circumflex leaf                                      { $$ = driver->pow($1, $3); }
        ;

leaf: integer                                                       { $$ = driver->get_integer($1); }
        | decimal                                                   { $$ = driver->get_decimal($1); }
        | identifier                                                { $$ = driver->get_identifier($1); }
        | built_in_function                                         { $$ = $1; }
        | open_bracket expression close_bracket                     { $$ = $2; }
        ;

built_in_function: abs open_bracket expression close_bracket                            { $$ = driver->abs($3); }
        | step open_bracket expression close_bracket                                    { $$ = driver->step($3); }
        | sqrt open_bracket expression close_bracket                                    { $$ = driver->sqrt($3); }
        | sin open_bracket expression close_bracket                                     { $$ = driver->sin($3); }
        | cos open_bracket expression close_bracket                                     { $$ = driver->cos($3); }
        | tan open_bracket expression close_bracket                                     { $$ = driver->tan($3); }
        | asin open_bracket expression close_bracket                                    { $$ = driver->asin($3); }
        | acos open_bracket expression close_bracket                                    { $$ = driver->acos($3); }
        | atan open_bracket expression close_bracket                                    { $$ = driver->atan($3); }
        | atan2 open_bracket expression comma expression close_bracket                  { $$ = driver->atan2($3, $5); }
        | sinh open_bracket expression close_bracket                                    { $$ = driver->sinh($3); }
        | cosh open_bracket expression close_bracket                                    { $$ = driver->cosh($3); }
        | tanh open_bracket expression close_bracket                                    { $$ = driver->tanh($3); }
        | asinh open_bracket expression close_bracket                                   { $$ = driver->asinh($3); }
        | acosh open_bracket expression close_bracket                                   { $$ = driver->acosh($3); }
        | atanh open_bracket expression close_bracket                                   { $$ = driver->atanh($3); }
        | exp open_bracket expression close_bracket                                     { $$ = driver->exp($3); }
        | log open_bracket expression close_bracket                                     { $$ = driver->log($3); }
        | Li2 open_bracket expression close_bracket                                     { $$ = driver->Li2($3); }
        | Li open_bracket expression comma expression close_bracket                     { $$ = driver->Li($3, $5); }
        | G open_bracket expression comma expression close_bracket                      { $$ = driver->G($3, $5); }
        | G open_bracket expression comma expression comma expression close_bracket     { $$ = driver->G($3, $5, $7); }
        | S open_bracket expression comma expression comma expression close_bracket     { $$ = driver->S($3, $5, $7); }
        | H open_bracket expression comma expression close_bracket                      { $$ = driver->H($3, $5); }
        | zeta open_bracket expression close_bracket                                    { $$ = driver->zeta($3); }
        | zeta open_bracket expression comma expression close_bracket                   { $$ = driver->zeta($3, $5); }
        | zetaderiv open_bracket expression comma expression close_bracket              { $$ = driver->zetaderiv($3, $5); }
        | tgamma open_bracket expression close_bracket                                  { $$ = driver->tgamma($3); }
        | lgamma open_bracket expression close_bracket                                  { $$ = driver->lgamma($3); }
        | beta open_bracket expression comma expression close_bracket                   { $$ = driver->beta($3, $5); }
        | psi open_bracket expression close_bracket                                     { $$ = driver->psi($3); }
        | psi open_bracket expression comma expression close_bracket                    { $$ = driver->psi($3, $5); }
        | factorial open_bracket expression close_bracket                               { $$ = driver->factorial($3); }
        | binomial open_bracket expression comma expression close_bracket               { $$ = driver->binomial($3, $5); }

%%

void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    this->driver->error(err_message);
  }
