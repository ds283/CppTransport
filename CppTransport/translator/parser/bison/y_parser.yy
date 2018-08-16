%skeleton "lalr1.cc"
%require "3.0.2"
%locations
%defines
%define api.namespace {y}
%define api.value.type variant
%define parser_class_name {y_parser}
%define parse.trace
%define parse.error verbose

%code requires {
    #include "lexeme.h"
    #include "model_descriptor.h"
    #include "lexical.h"

    #include "disable_warnings.h"
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

    #include "y_common.h"
    #include "y_lexer.h"
    #include "y_driver.h"
    #include "error.h"

    #include "disable_warnings.h"
    #include "ginac/ginac.h"

    static int yylex(y::y_parser::semantic_type* yylval,
                     y::y_parser::location_type* yyloc,
                     y::y_lexer& lexer,
                     y::y_driver& driver)
      {
        return(lexer.yylex(yylval));
      }
}

/*%union {
    lexeme::lexeme<enum keyword_type, enum character_type>* lex;
    attributes*                                             a;
    stepper*                                                s;
		subexpr*                                                e;
    author*                                                 au;
    string_array*                                           sa;
    GiNaC::ex*                                              x;
}*/

%token                                      metadata
%token                                      require_version
%token                                      lagrangian
%token <std::shared_ptr<lexeme_type>>       canonical
%token <std::shared_ptr<lexeme_type>>       nontrivial_metric
%token                                      name
%token                                      author
%token                                      citeguide
%token                                      description
%token                                      license
%token                                      revision
%token                                      references
%token                                      urls
%token                                      email
%token                                      institute
%token                                      field
%token                                      potential
%token                                      metric
%token                                      subexpr
%token                                      value
%token                                      parameter
%token                                      latex
%token                                      templates
%token                                      settings
%token                                      core
%token                                      implementation
%token                                      abserr
%token                                      relerr
%token                                      stepper
%token                                      stepsize
%token <std::shared_ptr<lexeme_type>>       background
%token <std::shared_ptr<lexeme_type>>       perturbations
%token                                      indexorder
%token                                      left
%token                                      right
%token                                      model
%token                                      abs
%token                                      step
%token                                      sqrt
%token                                      sin
%token                                      cos
%token                                      tan
%token                                      sec
%token                                      cosec
%token                                      cot
%token                                      asin
%token                                      acos
%token                                      atan
%token                                      atan2
%token                                      sinh
%token                                      cosh
%token                                      tanh
%token                                      sech
%token                                      cosech
%token                                      coth
%token                                      asinh
%token                                      acosh
%token                                      atanh
%token                                      exp
%token                                      log
%token                                      pow
%token                                      Li2
%token                                      Li
%token                                      G
%token                                      S
%token                                      H
%token                                      zeta
%token                                      zetaderiv
%token                                      tgamma
%token                                      lgamma
%token                                      beta
%token                                      psi
%token                                      factorial
%token                                      binomial
%token                                      open_brace
%token                                      close_brace
%token                                      open_bracket
%token                                      close_bracket
%token                                      open_square
%token                                      close_square
%token                                      comma
%token                                      period
%token                                      colon
%token                                      semicolon
%token <std::shared_ptr<lexeme_type>>       equals
%token                                      plus
%token                                      binary_minus
%token                                      unary_minus
%token                                      star
%token                                      backslash
%token                                      foreslash
%token                                      tilde
%token                                      ampersand
%token                                      circumflex
%token                                      ampersat
%token                                      ellipsis
%token                                      rightarrow
%token <std::shared_ptr<lexeme_type>>       identifier
%token <std::shared_ptr<lexeme_type>>       integer
%token <std::shared_ptr<lexeme_type>>       decimal
%token <std::shared_ptr<lexeme_type>>       string

%type  <std::shared_ptr<attributes>>        attribute_block
%type  <std::shared_ptr<attributes>>        attributes
%type  <std::shared_ptr<author>>            author_block
%type  <std::shared_ptr<author>>            author_attributes
%type  <std::shared_ptr<string_array>>      string_array
%type  <std::shared_ptr<string_array>>      string_group
%type  <std::shared_ptr<stepper>>           stepper_block
%type  <std::shared_ptr<stepper>>           stepper_attributes
%type  <std::shared_ptr<subexpr>>           subexpr_block
%type  <std::shared_ptr<subexpr>>           subexpr_def
%type  <std::shared_ptr<field_metric>>      metric_def
%type  <std::shared_ptr<field_metric>>      metric_literal
%type  <std::shared_ptr<field_metric_base>> metric_element_list
%type  <std::shared_ptr<GiNaC::ex>>         expression
%type  <std::shared_ptr<GiNaC::ex>>         leaf
%type  <std::shared_ptr<GiNaC::ex>>         built_in_function

%left plus binary_minus
%left star foreslash
%right circumflex

%precedence unary_minus

%%

program: script
        ;

script: script metadata metadata_block semicolon
        | script field identifier attribute_block semicolon                             { driver.model.add_field(*$3, $4); }
        | script parameter identifier attribute_block semicolon                         { driver.model.add_parameter(*$3, $4); }
        | script subexpr identifier subexpr_block semicolon                             { driver.model.add_subexpr(*$3, $4); }
        | script potential equals expression semicolon                                  { driver.model.set_potential(*$3, $4); }
        | script metric equals metric_def semicolon                                     { driver.model.set_metric(*$3, $4); }
        | script model string model_block semicolon                                     { driver.templates.set_model(*$3); }
        | script background stepper_block semicolon                                     { driver.templates.set_background_stepper(*$2, $3); }
        | script perturbations stepper_block semicolon                                  { driver.templates.set_perturbations_stepper(*$2, $3); }
        | script author string author_block semicolon                                   { driver.meta.add_author(*$3, $4); }
        | script templates template_block semicolon
        | script settings settings_block semicolon
        |
        ;

metric_def: metric_literal                                                              { $$ = $1; }
        | metric_def plus metric_def                                                    { $$ = std::make_shared<field_metric_binop>(field_metric_binop::op::add, $1, $3); }
        | metric_def binary_minus metric_def                                            { $$ = std::make_shared<field_metric_binop>(field_metric_binop::op::sub, $1, $3); }
        | expression star metric_def                                                    { $3->multiply(*$1); $$ = $3; }
        | metric_def star expression                                                    { $1->multiply(*$3); $$ = $1; }
        | metric_def foreslash expression                                               { $1->multiply(GiNaC::ex(1) / *$3); $$ = $1; }
        | unary_minus metric_def                                                        { $2->multiply(GiNaC::ex(-1)); $$ = $2; }
        | open_bracket metric_def close_bracket                                         { $$ = $2; }
        ;

metric_literal: open_square metric_element_list close_square                            { $$ = $2; }
        ;

metric_element_list: metric_element_list identifier comma identifier equals expression semicolon
                                                                                        { driver.model.add_metric_component(*$1, *$2, *$4, *$6, *$5); $$ = $1; }
        |                                                                               { $$ = driver.model.make_field_metric_base(); }
        ;

metadata_block: open_brace metadata_def close_brace

metadata_def: metadata_def require_version equals integer semicolon                     { driver.misc.set_required_version(*$4); }
        |
        ;

template_block: open_brace template_def close_brace

template_def: template_def core equals string semicolon                                 { driver.templates.set_core(*$4); }
        | template_def implementation equals string semicolon                           { driver.templates.set_implementation(*$4); }
        |
        ;

settings_block: open_brace settings_def close_brace

settings_def: settings_def indexorder equals left semicolon                             { driver.misc.set_indexorder_left(); }
        | settings_def indexorder equals right semicolon                                { driver.misc.set_indexorder_right(); }
        |
        ;

attribute_block: open_brace attributes close_brace                                      { $$ = $2; }
        |                                                                               { $$ = std::make_shared<attributes>(); }
        ;

attributes: attributes latex equals string semicolon                                    { driver.misc.set_attribute_latex(*$1, *$4); $$ = $1; }
        |                                                                               { $$ = std::make_shared<attributes>(); }
        ;

string_array: open_square string_group close_square                                     { $$ = $2; }
        | open_square close_square                                                      { $$ = std::make_shared<string_array>(); }

string_group: string_group comma string                                                 { driver.misc.add_string(*$1, *$3); $$ = $1; }
        | string                                                                        { $$ = std::make_shared<string_array>(); driver.misc.add_string(*$$, *$1); }
        ;

model_block: open_brace model_attributes close_brace

model_attributes: model_attributes name equals string semicolon                         { driver.meta.set_name(*$4); }
        | model_attributes lagrangian equals lagrangian_specifier semicolon
        | model_attributes citeguide equals string semicolon                            { driver.meta.set_citeguide(*$4); }
        | model_attributes description equals string semicolon                          { driver.meta.set_description(*$4); }
        | model_attributes license equals string semicolon                              { driver.meta.set_license(*$4); }
        | model_attributes revision equals integer semicolon                            { driver.meta.set_revision(*$4); }
        | model_attributes references equals string_array semicolon                     { driver.meta.set_references(*$3, $4); }
        | model_attributes urls equals string_array semicolon                           { driver.meta.set_urls(*$3, $4); }
        |
        ;

lagrangian_specifier: canonical                                                         { driver.model.set_lagrangian_type(model_type::canonical, *$1); }
        | nontrivial_metric                                                             { driver.model.set_lagrangian_type(model_type::nontrivial_metric, *$1); }
        ;

author_block: open_brace author_attributes close_brace                                  { $$ = $2; }
        |                                                                               { $$ = std::make_shared<author>(); }
        ;

author_attributes: author_attributes email equals string semicolon                      { driver.meta.set_author_email(*$1, *$4); $$ = $1; }
        | author_attributes institute equals string semicolon                           { driver.meta.set_author_institute(*$1, *$4); $$ = $1; }
        |                                                                               { $$ = std::make_shared<author>(); }
        ;

stepper_block: open_brace stepper_attributes close_brace                                { $$ = $2; }

stepper_attributes: stepper_attributes abserr equals decimal semicolon                  { driver.templates.set_abserr(*$1, *$4); $$ = $1; }
        | stepper_attributes relerr equals decimal semicolon                            { driver.templates.set_relerr(*$1, *$4); $$ = $1; }
        | stepper_attributes stepper equals string semicolon                            { driver.templates.set_stepper(*$1, *$4); $$ = $1; }
        | stepper_attributes stepsize equals decimal semicolon                          { driver.templates.set_stepsize(*$1, *$4); $$ = $1; }
        |                                                                               { $$ = std::make_shared<stepper>(); }
        ;

subexpr_block: open_brace subexpr_def close_brace                                       { $$ = $2; }
        |                                                                               { $$ = std::make_shared<subexpr>(); }
				;

subexpr_def: subexpr_def latex equals string semicolon                                  { driver.model.set_subexpr_latex(*$1, *$4); $$ = $1; }
        | subexpr_def value equals expression semicolon                                 { driver.model.set_subexpr_value(*$1, *$4, *$3); $$ = $1; }
        |                                                                               { $$ = std::make_shared<subexpr>(); }
        ;

expression: leaf                                                                        { $$ = $1; }
        | expression plus expression                                                    { $$ = driver.expr.add(*$1, *$3); }
        | expression binary_minus expression                                            { $$ = driver.expr.sub(*$1, *$3); }
        | expression star expression                                                    { $$ = driver.expr.mul(*$1, *$3); }
        | expression foreslash expression                                               { $$ = driver.expr.div(*$1, *$3); }
        | expression circumflex expression                                              { $$ = driver.expr.pow(*$1, *$3); }
        | open_bracket expression close_bracket                                         { $$ = $2; }
        | unary_minus expression                                                        { $$ = driver.expr.unary_minus(*$2); }
        ;

leaf: integer                                                                           { $$ = driver.expr.get_integer(*$1); }
        | decimal                                                                       { $$ = driver.expr.get_decimal(*$1); }
        | identifier                                                                    { $$ = driver.expr.get_identifier(*$1); }
        | built_in_function                                                             { $$ = $1; }
        ;

built_in_function: abs open_bracket expression close_bracket                            { $$ = driver.expr.abs(*$3); }
        | step open_bracket expression close_bracket                                    { $$ = driver.expr.step(*$3); }
        | sqrt open_bracket expression close_bracket                                    { $$ = driver.expr.sqrt(*$3); }
        | sin open_bracket expression close_bracket                                     { $$ = driver.expr.sin(*$3); }
        | cos open_bracket expression close_bracket                                     { $$ = driver.expr.cos(*$3); }
        | tan open_bracket expression close_bracket                                     { $$ = driver.expr.tan(*$3); }
        | sec open_bracket expression close_bracket                                     { $$ = driver.expr.sec(*$3); }
        | cosec open_bracket expression close_bracket                                   { $$ = driver.expr.cosec(*$3); }
        | cot open_bracket expression close_bracket                                     { $$ = driver.expr.cot(*$3); }
        | asin open_bracket expression close_bracket                                    { $$ = driver.expr.asin(*$3); }
        | acos open_bracket expression close_bracket                                    { $$ = driver.expr.acos(*$3); }
        | atan open_bracket expression close_bracket                                    { $$ = driver.expr.atan(*$3); }
        | atan2 open_bracket expression comma expression close_bracket                  { $$ = driver.expr.atan2(*$3, *$5); }
        | sinh open_bracket expression close_bracket                                    { $$ = driver.expr.sinh(*$3); }
        | cosh open_bracket expression close_bracket                                    { $$ = driver.expr.cosh(*$3); }
        | tanh open_bracket expression close_bracket                                    { $$ = driver.expr.tanh(*$3); }
        | sech open_bracket expression close_bracket                                    { $$ = driver.expr.sech(*$3); }
        | cosech open_bracket expression close_bracket                                  { $$ = driver.expr.cosech(*$3); }
        | coth open_bracket expression close_bracket                                    { $$ = driver.expr.coth(*$3); }
        | asinh open_bracket expression close_bracket                                   { $$ = driver.expr.asinh(*$3); }
        | acosh open_bracket expression close_bracket                                   { $$ = driver.expr.acosh(*$3); }
        | atanh open_bracket expression close_bracket                                   { $$ = driver.expr.atanh(*$3); }
        | exp open_bracket expression close_bracket                                     { $$ = driver.expr.exp(*$3); }
        | log open_bracket expression close_bracket                                     { $$ = driver.expr.log(*$3); }
				| pow open_bracket expression comma expression close_bracket                    { $$ = driver.expr.pow(*$3, *$5); }
        | Li2 open_bracket expression close_bracket                                     { $$ = driver.expr.Li2(*$3); }
        | Li open_bracket expression comma expression close_bracket                     { $$ = driver.expr.Li(*$3, *$5); }
        | G open_bracket expression comma expression close_bracket                      { $$ = driver.expr.G(*$3, *$5); }
        | G open_bracket expression comma expression comma expression close_bracket     { $$ = driver.expr.G(*$3, *$5, *$7); }
        | S open_bracket expression comma expression comma expression close_bracket     { $$ = driver.expr.S(*$3, *$5, *$7); }
        | H open_bracket expression comma expression close_bracket                      { $$ = driver.expr.H(*$3, *$5); }
        | zeta open_bracket expression close_bracket                                    { $$ = driver.expr.zeta(*$3); }
        | zeta open_bracket expression comma expression close_bracket                   { $$ = driver.expr.zeta(*$3, *$5); }
        | zetaderiv open_bracket expression comma expression close_bracket              { $$ = driver.expr.zetaderiv(*$3, *$5); }
        | tgamma open_bracket expression close_bracket                                  { $$ = driver.expr.tgamma(*$3); }
        | lgamma open_bracket expression close_bracket                                  { $$ = driver.expr.lgamma(*$3); }
        | beta open_bracket expression comma expression close_bracket                   { $$ = driver.expr.beta(*$3, *$5); }
        | psi open_bracket expression close_bracket                                     { $$ = driver.expr.psi(*$3); }
        | psi open_bracket expression comma expression close_bracket                    { $$ = driver.expr.psi(*$3, *$5); }
        | factorial open_bracket expression close_bracket                               { $$ = driver.expr.factorial(*$3); }
        | binomial open_bracket expression comma expression close_bracket               { $$ = driver.expr.binomial(*$3, *$5); }

%%

void y::y_parser::error(const y::y_parser::location_type &l,
                        const std::string& err_message)
  {
    std::ostringstream msg;
    boost::optional< lexeme_type& > current_lexeme = this->lexer.get_current_lexeme();

		if(current_lexeme)
			{
        current_lexeme->error(err_message);
			}
		else
			{
        // bypass context-based error reporting
		    driver.error(err_message);
			}
  }
