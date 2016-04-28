%skeleton "lalr1.cc"
%require "3.0.2"
%locations
%defines
%define api.namespace{y}
%define parser_class_name{y_parser}
%define parse.trace
%define parse.error verbose

%code requires {
    #include "lexeme.h"
    #include "script.h"
    #include "lexical.h"

    #include "ginac/ginac.h"

    namespace y {
        class y_driver;
        class y_lexer;
    }
}

%lex-param   { y_lexer&  lexer }
%parse-param { y_lexer&  lexer }

%lex-param   { y_driver& driver }
%parse-param { y_driver& driver }

%code {
    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    #include "y_lexer.h"
    #include "y_driver.h"
    #include "error.h"

    #include "ginac/ginac.h"

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_parser::location_type* yyloc,
                     y::y_lexer& lexer,
                     y::y_driver& driver)
      {
        return(lexer.yylex(yylval));
      }
}

%union {
    lexeme::lexeme<enum keyword_type, enum character_type>* lex;
    attributes*                                             a;
    stepper*                                                s;
		subexpr*                                                e;
    author*                                                 au;
    string_array*                                           sa;
    GiNaC::ex*                                              x;
}

%token          name
%token          author
%token          citeguide
%token          description
%token          license
%token          revision
%token          references
%token          urls
%token          email
%token          institute
%token          field
%token          potential
%token          subexpr
%token          value
%token          parameter
%token          latex
%token          templates
%token          settings
%token          core
%token          implementation
%token          abserr
%token          relerr
%token          stepper
%token          stepsize
%token <lex>    background
%token <lex>    perturbations
%token          indexorder
%token          left
%token          right
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
%token          pow
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
%token <lex>    equals
%token          plus
%token          binary_minus
%token          unary_minus
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

%type  <a>      attribute_block
%type  <a>      attributes
%type  <au>     author_block
%type  <au>     author_attributes
%type  <sa>     string_array
%type  <sa>     string_group
%type  <s>      stepper_block
%type  <s>      stepper_attributes
%type  <e>      subexpr_block
%type  <e>      subexpr_def
%type  <x>      expression
%type  <x>      term
%type  <x>      factor
%type  <x>      leaf
%type  <x>      built_in_function

%%

program: script
        ;

script: script potential equals expression semicolon                                    { driver.set_potential($4, $3); }
        | script model string model_block semicolon                                     { driver.set_model($3); }
        | script author string author_block semicolon                                   { driver.add_author($3, $4); }
        | script templates template_block semicolon
        | script settings settings_block semicolon
        | script field identifier attribute_block semicolon                             { driver.add_field($3, $4); }
        | script parameter identifier attribute_block semicolon                         { driver.add_parameter($3, $4); }
        | script background stepper_block semicolon                                     { driver.set_background_stepper($3, $2); }
        | script perturbations stepper_block semicolon                                  { driver.set_perturbations_stepper($3, $2); }
				| script subexpr identifier subexpr_block semicolon                             { driver.add_subexpr($3, $4); }
        |
        ;

template_block: open_brace template_def close_brace

template_def: template_def core equals string semicolon                                 { driver.set_core($4); }
        | template_def implementation equals string semicolon                           { driver.set_implementation($4); }
        |
        ;

settings_block: open_brace settings_def close_brace

settings_def: settings_def indexorder equals left semicolon                             { driver.set_indexorder_left(); }
        | settings_def indexorder equals right semicolon                                { driver.set_indexorder_right(); }
        |
        ;

attribute_block: open_brace attributes close_brace                                      { $$ = $2; }
        |                                                                               { $$ = new attributes; }
        ;

attributes: attributes latex equals string semicolon                                    { driver.add_latex_attribute($1, $4); $$ = $1; }
        |                                                                               { $$ = new attributes; }
        ;

string_array: open_square string_group close_square                                     { $$ = $2; }
        | open_square close_square                                                      { $$ = new string_array; }

string_group: string_group comma string                                                 { driver.add_string($1, $3); $$ = $1; }
        | string                                                                        { $$ = new string_array; driver.add_string($$, $1); }
        ;

model_block: open_brace model_attributes close_brace

model_attributes: model_attributes name equals string semicolon                         { driver.set_name($4); }
        | model_attributes citeguide equals string semicolon                            { driver.set_citeguide($4); }
        | model_attributes description equals string semicolon                          { driver.set_description($4); }
        | model_attributes license equals string semicolon                              { driver.set_license($4); }
        | model_attributes revision equals integer semicolon                            { driver.set_revision($4); }
        | model_attributes references equals string_array semicolon                     { driver.set_references($4); }
        | model_attributes urls equals string_array semicolon                           { driver.set_urls($4); }
        |
        ;

author_block: open_brace author_attributes close_brace                                  { $$ = $2; }
        |                                                                               { $$ = new author; }
        ;

author_attributes: author_attributes email equals string semicolon                      { driver.add_email($1, $4); $$ = $1; }
        | author_attributes institute equals string semicolon                           { driver.add_institute($1, $4); $$ = $1; }
        |                                                                               { $$ = new author; }
        ;

stepper_block: open_brace stepper_attributes close_brace                                { $$ = $2; }

stepper_attributes: stepper_attributes abserr equals decimal semicolon                  { driver.set_abserr($1, $4); $$ = $1; }
        | stepper_attributes relerr equals decimal semicolon                            { driver.set_relerr($1, $4); $$ = $1; }
        | stepper_attributes stepper equals string semicolon                            { driver.set_stepper($1, $4); $$ = $1; }
        | stepper_attributes stepsize equals decimal semicolon                          { driver.set_stepsize($1, $4); $$ = $1; }
        |                                                                               { $$ = new stepper; }
        ;

subexpr_block: open_brace subexpr_def close_brace                                       { $$ = $2; }
        |                                                                               { $$ = new subexpr; }
				;

subexpr_def: subexpr_def latex equals string semicolon                                  { driver.add_latex_attribute($1, $4); $$ = $1; }
        | subexpr_def value equals expression semicolon                                 { driver.add_value_attribute($1, $4, $3); $$ = $1; }
        |                                                                               { $$ = new subexpr; }
        ;

expression: term                                                                        { $$ = $1; }
        | expression plus term                                                          { $$ = driver.add($1, $3); }
        | expression binary_minus term                                                  { $$ = driver.sub($1, $3); }
        ;

term: factor                                                                            { $$ = $1; }
        | term star factor                                                              { $$ = driver.mul($1, $3); }
        | term backslash factor                                                         { $$ = driver.div($1, $3); }
        ;

factor: leaf                                                                            { $$ = $1; }
        | leaf circumflex leaf                                                          { $$ = driver.pow($1, $3); }
        ;

leaf: integer                                                                           { $$ = driver.get_integer($1); }
        | decimal                                                                       { $$ = driver.get_decimal($1); }
        | identifier                                                                    { $$ = driver.get_identifier($1); }
        | built_in_function                                                             { $$ = $1; }
        | open_bracket expression close_bracket                                         { $$ = $2; }
        | unary_minus expression                                                        { $$ = driver.unary_minus($2); }
        ;

built_in_function: abs open_bracket expression close_bracket                            { $$ = driver.abs($3); }
        | step open_bracket expression close_bracket                                    { $$ = driver.step($3); }
        | sqrt open_bracket expression close_bracket                                    { $$ = driver.sqrt($3); }
        | sin open_bracket expression close_bracket                                     { $$ = driver.sin($3); }
        | cos open_bracket expression close_bracket                                     { $$ = driver.cos($3); }
        | tan open_bracket expression close_bracket                                     { $$ = driver.tan($3); }
        | asin open_bracket expression close_bracket                                    { $$ = driver.asin($3); }
        | acos open_bracket expression close_bracket                                    { $$ = driver.acos($3); }
        | atan open_bracket expression close_bracket                                    { $$ = driver.atan($3); }
        | atan2 open_bracket expression comma expression close_bracket                  { $$ = driver.atan2($3, $5); }
        | sinh open_bracket expression close_bracket                                    { $$ = driver.sinh($3); }
        | cosh open_bracket expression close_bracket                                    { $$ = driver.cosh($3); }
        | tanh open_bracket expression close_bracket                                    { $$ = driver.tanh($3); }
        | asinh open_bracket expression close_bracket                                   { $$ = driver.asinh($3); }
        | acosh open_bracket expression close_bracket                                   { $$ = driver.acosh($3); }
        | atanh open_bracket expression close_bracket                                   { $$ = driver.atanh($3); }
        | exp open_bracket expression close_bracket                                     { $$ = driver.exp($3); }
        | log open_bracket expression close_bracket                                     { $$ = driver.log($3); }
				| pow open_bracket expression comma expression close_bracket                    { $$ = driver.pow($3, $5); }
        | Li2 open_bracket expression close_bracket                                     { $$ = driver.Li2($3); }
        | Li open_bracket expression comma expression close_bracket                     { $$ = driver.Li($3, $5); }
        | G open_bracket expression comma expression close_bracket                      { $$ = driver.G($3, $5); }
        | G open_bracket expression comma expression comma expression close_bracket     { $$ = driver.G($3, $5, $7); }
        | S open_bracket expression comma expression comma expression close_bracket     { $$ = driver.S($3, $5, $7); }
        | H open_bracket expression comma expression close_bracket                      { $$ = driver.H($3, $5); }
        | zeta open_bracket expression close_bracket                                    { $$ = driver.zeta($3); }
        | zeta open_bracket expression comma expression close_bracket                   { $$ = driver.zeta($3, $5); }
        | zetaderiv open_bracket expression comma expression close_bracket              { $$ = driver.zetaderiv($3, $5); }
        | tgamma open_bracket expression close_bracket                                  { $$ = driver.tgamma($3); }
        | lgamma open_bracket expression close_bracket                                  { $$ = driver.lgamma($3); }
        | beta open_bracket expression comma expression close_bracket                   { $$ = driver.beta($3, $5); }
        | psi open_bracket expression close_bracket                                     { $$ = driver.psi($3); }
        | psi open_bracket expression comma expression close_bracket                    { $$ = driver.psi($3, $5); }
        | factorial open_bracket expression close_bracket                               { $$ = driver.factorial($3); }
        | binomial open_bracket expression comma expression close_bracket               { $$ = driver.binomial($3, $5); }

%%

void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    std::ostringstream msg;
    lexeme::lexeme<enum keyword_type, enum character_type>* current_lexeme = this->lexer.get_current_lexeme();

		if(current_lexeme != nullptr)
			{
        current_lexeme->error(err_message);
			}
		else
			{
        // bypass context-based error reporting
		    driver.error(err_message);
			}
  }
