//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_LEXICAL_H
#define CPPTRANSPORT_LEXICAL_H

#include <string>


enum class keyword_type
  {
    name, author, tag, field, potential, subexpr, value,
    parameter, latex, core, impl, model,
    abserr, relerr, k_stepper, stepsize,
    background, perturbations,
    indexorder, left, right,
    f_abs, f_step, f_sqrt,
    f_sin, f_cos, f_tan,
    f_asin, f_acos, f_atan, f_atan2,
    f_sinh, f_cosh, f_tanh,
    f_asinh, f_acosh, f_atanh,
    f_exp, f_log, f_pow, f_Li2, f_Li, f_G, f_S, f_H,
    f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
    f_beta, f_psi, f_factorial, f_binomial
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
