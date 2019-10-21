//
// Created by David Seery on 17/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_LEXICAL_H
#define CPPTRANSPORT_LEXICAL_H

#include <string>


enum class keyword_type
  {
    metadata, require_version, lagrangian,
    canonical, nontrivial_metric,
    name, author, citeguide,
    description, license, revision,
    references, urls, email, institute,
    field, potential, metric,
    subexpr, value,
    parameter, latex, templates, settings,
    core, impl, model,
    abserr, relerr, stepper, stepsize,
    background, perturbations,
    indexorder, left, right,
    abs, step, sqrt,
    sin, cos, tan, sec, cosec, cot,
    asin, acos, atan, atan2,
    sinh, cosh, tanh, sech, cosech, coth,
    asinh, acosh, atanh,
    exp, log, pow, Li2, Li, G, S, H,
    zeta, zetaderiv, tgamma, lgamma,
    beta, psi, factorial, binomial,
    cosmology, cosmo_template,
    H0, h0, Omega_B, Omega_Bh2,
    Omega_CDM, Omega_CDMh2, tau, Tcmb
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
