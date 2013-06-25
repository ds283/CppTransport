//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __y_driver_H_
#define __y_driver_H_

#include <iostream>
#include <string>

#include "lexeme.h"
#include "lexical.h"
#include "quantity.h"
#include "parse_tree.h"


namespace y
  {

    class y_driver
      {
        public:
          y_driver();
          ~y_driver();

          void         error              (std::string msg);

          script*      get_script         ();

          declaration* make_field         (lexeme::lexeme<keyword_type, character_type>* lex, attributes* a);
          declaration* make_parameter     (lexeme::lexeme<keyword_type, character_type>* lex, attributes* a);

          void         set_name           (lexeme::lexeme<keyword_type, character_type> *lex);
          void         set_author         (lexeme::lexeme<keyword_type, character_type> *lex);
          void         set_tag            (lexeme::lexeme<keyword_type, character_type> *lex);
          void         set_class          (lexeme::lexeme<keyword_type, character_type> *lex);
          void         set_model          (lexeme::lexeme<keyword_type, character_type> *lex);

          void         add_latex_attribute(attributes* a, lexeme::lexeme<keyword_type, character_type> *lex);

          GiNaC::ex*   get_integer        (lexeme::lexeme<keyword_type, character_type> *lex);
          GiNaC::ex*   get_decimal        (lexeme::lexeme<keyword_type, character_type> *lex);
          GiNaC::ex*   get_identifier     (lexeme::lexeme<keyword_type, character_type> *lex);

          GiNaC::ex*   add                (GiNaC::ex* l, GiNaC::ex* r);
          GiNaC::ex*   sub                (GiNaC::ex* l, GiNaC::ex* r);
          GiNaC::ex*   mul                (GiNaC::ex* l, GiNaC::ex* r);
          GiNaC::ex*   div                (GiNaC::ex* l, GiNaC::ex* r);
          GiNaC::ex*   pow                (GiNaC::ex* l, GiNaC::ex* r);
          GiNaC::ex*   abs                (GiNaC::ex* arg);
          GiNaC::ex*   step               (GiNaC::ex* arg);
          GiNaC::ex*   sqrt               (GiNaC::ex* arg);
          GiNaC::ex*   sin                (GiNaC::ex* arg);
          GiNaC::ex*   cos                (GiNaC::ex* arg);
          GiNaC::ex*   tan                (GiNaC::ex* arg);
          GiNaC::ex*   asin               (GiNaC::ex* arg);
          GiNaC::ex*   acos               (GiNaC::ex* arg);
          GiNaC::ex*   atan               (GiNaC::ex* arg);
          GiNaC::ex*   atan2              (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   sinh               (GiNaC::ex* arg);
          GiNaC::ex*   cosh               (GiNaC::ex* arg);
          GiNaC::ex*   tanh               (GiNaC::ex* arg);
          GiNaC::ex*   asinh              (GiNaC::ex* arg);
          GiNaC::ex*   acosh              (GiNaC::ex* arg);
          GiNaC::ex*   atanh              (GiNaC::ex* arg);
          GiNaC::ex*   exp                (GiNaC::ex* arg);
          GiNaC::ex*   log                (GiNaC::ex* arg);
          GiNaC::ex*   Li2                (GiNaC::ex* arg);
          GiNaC::ex*   Li                 (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   G                  (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   G                  (GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3);
          GiNaC::ex*   S                  (GiNaC::ex* a1, GiNaC::ex* a2, GiNaC::ex* a3);
          GiNaC::ex*   H                  (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   zeta               (GiNaC::ex* arg);
          GiNaC::ex*   zeta               (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   zetaderiv          (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   tgamma             (GiNaC::ex* arg);
          GiNaC::ex*   lgamma             (GiNaC::ex* arg);
          GiNaC::ex*   beta               (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   psi                (GiNaC::ex* arg);
          GiNaC::ex*   psi                (GiNaC::ex* a1, GiNaC::ex* a2);
          GiNaC::ex*   factorial          (GiNaC::ex* arg);
          GiNaC::ex*   binomial           (GiNaC::ex* a1, GiNaC::ex* a2);

        private:
          script* root;
      };

  }


#endif //__y_driver_H_
