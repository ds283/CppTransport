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

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_lexer* lexer,
                     y::y_driver* driver)
      {
        return(lexer->yylex(yylval));
      }
}

%union {
    lexeme<enum keyword_type, enum symbol_type>* lex;
    parse_tree*                                  tree;
}

%token          name
%token          author
%token          tag
%token          field
%token          potential
%token          parameter
%token          latex
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

%%

script: script declaration
        |
        ;

declaration: name_declaration
        | author_declaration
        | tag_declaration
        | field_declaration
        | parameter_declaration
        | potential_declaration
        ;

name_declaration: name string semicolon
        ;

author_declaration: author string semicolon
        ;

tag_declaration: tag string semicolon
        ;

field_declaration: field info_block identifier semicolon
        ;

parameter_declaration: parameter info_block identifier semicolon
        ;

potential_declaration: potential expression
        ;

info_block: open_brace info_declarations close_brace
        |
        ;

info_declarations: info_declarations info_declaration
        |
        ;

info_declaration: latex string semicolon
        ;

expression: term
        | expression plus term
        | expression minus term
        ;
        
term: factor
        | term star factor
        | term backslash factor
        ;

factor: leaf
        | leaf circumflex leaf
        ;

leaf: integer
        | decimal
        | identifier
        | built_in_function
        | open_bracket expression close_bracket
        ;

built_in_function: abs open_bracket expression close_bracket
        | step open_bracket expression close_bracket
        | sqrt open_bracket expression close_bracket
        | sin open_bracket expression close_bracket
        | cos open_bracket expression close_bracket
        | tan open_bracket expression close_bracket
        | asin open_bracket expression close_bracket
        | acos open_bracket expression close_bracket
        | atan open_bracket expression close_bracket
        | atan2 open_bracket expression comma expression close_bracket
        | sinh open_bracket expression close_bracket
        | cosh open_bracket expression close_bracket
        | tanh open_bracket expression close_bracket
        | asinh open_bracket expression close_bracket
        | acosh open_bracket expression close_bracket
        | atanh open_bracket expression close_bracket
        | exp open_bracket expression close_bracket
        | log open_bracket expression close_bracket
        | Li2 open_bracket expression close_bracket
        | Li open_bracket expression comma expression close_bracket
        | G open_bracket expression comma expression close_bracket
        | G open_bracket expression comma expression comma expression close_bracket
        | S open_bracket expression comma expression comma expression close_bracket
        | H open_bracket expression comma expression close_bracket
        | zeta open_bracket expression close_bracket
        | zeta open_bracket expression comma expression close_bracket
        | zetaderiv open_bracket expression comma expression close_bracket
        | tgamma open_bracket expression close_bracket
        | lgamma open_bracket expression close_bracket
        | beta open_bracket expression comma expression close_bracket
        | psi open_bracket expression close_bracket
        | psi open_bracket expression comma expression close_bracket
        | factorial open_bracket expression close_bracket
        | binomial open_bracket expression close_bracket

%%

void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    ::error(err_message); // :: forces link to default namespace, so finds error.h implementation
  }
