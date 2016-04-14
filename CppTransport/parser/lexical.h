//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_LEXICAL_H
#define CPPTRANSPORT_LEXICAL_H

#include <string>


enum class keyword_type
  {
    name, author, citeguide,
    description, license, revision,
    references, urls, email, institute,
    field, potential, subexpr, value,
    parameter, latex, core, impl, model,
    abserr, relerr, stepper, stepsize,
    background, perturbations,
    indexorder, left, right,
    abs, step, sqrt,
    sin, cos, tan,
    asin, acos, atan, atan2,
    sinh, cosh, tanh,
    asinh, acosh, atanh,
    exp, log, pow, Li2, Li, G, S, H,
    zeta, zetaderiv, tgamma, lgamma,
    beta, psi, factorial, binomial
  };


enum class character_type
  {
      open_brace, close_brace, open_bracket, close_bracket,
      open_square, close_square, comma, period, colon, semicolon,
      equals, plus, binary_minus, unary_minus, star, backslash, foreslash, tilde,
      ampersand, circumflex, ampersat, ellipsis, rightarrow
  };


namespace std
  {

    // allow hashing of these enums
    template<> struct hash<keyword_type>
      {

        size_t operator()(const keyword_type& t) const
          {
            return static_cast< std::underlying_type<keyword_type>::type >(t);
          }

      };


    template<> struct hash<character_type>
      {

        size_t operator()(const character_type& t) const
          {
            return static_cast< std::underlying_type<character_type>::type >(t);
          }

      };

  }


#endif //CPPTRANSPORT_LEXICAL_H
