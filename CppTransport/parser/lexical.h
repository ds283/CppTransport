//
// Created by David Seery on 17/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __lexical_H_
#define __lexical_H_

#include <string>

enum keyword_type
  {
      k_name, k_author, k_tag, k_field, k_potential, k_subexpr, k_value,
      k_parameter, k_latex, k_core, k_implementation, k_model,
      k_abserr, k_relerr, k_stepper, k_stepsize,
      k_background, k_perturbations,
      k_indexorder, k_left, k_right,
      f_abs, f_step, f_sqrt,
      f_sin, f_cos, f_tan,
      f_asin, f_acos, f_atan, f_atan2,
      f_sinh, f_cosh, f_tanh,
      f_asinh, f_acosh, f_atanh,
      f_exp, f_log, f_pow, f_Li2, f_Li, f_G, f_S, f_H,
      f_zeta, f_zetaderiv, f_tgamma, f_lgamma,
      f_beta, f_psi, f_factorial, f_binomial
  };

#define NUMBER_KEYWORDS (51)

enum character_type
  {
      open_brace, close_brace, open_bracket, close_bracket,
      open_square, close_square, comma, period, colon, semicolon,
      equals, plus, binary_minus, unary_minus, star, backslash, foreslash, tilde,
      ampersand, circumflex, ampersat, ellipsis, rightarrow
  };

#define NUMBER_CHARACTERS (23)


#endif //__lexical_H_
