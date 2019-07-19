//
// Created by David Seery on 30/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#include "attributes.h"


bool attributes::set_latex(const std::string& ltx, const y::lexeme_type& l)
  {
    return SetContextedValue(this->latex, ltx, l, ERROR_LATEX_REDECLARATION);
  }

// These four functions are used when sampling is enabled for a model, as it enables us to record the information
// about the value & prior of a field (+ it's deriv.) and of a parameter, which is then wrtitten out to the
// CosmoSIS .ini files later.
bool attributes::set_value(const std::string& val, const y::lexeme_type& l)
{
  return SetContextedValue(this->value, val, l, ERROR_VALUE_REDECLARATION);
}

bool attributes::set_derivvalue(const std::string& deriv_val, const y::lexeme_type& l)
{
  return SetContextedValue(this->deriv_value, deriv_val, l, ERROR_DERIVVALUE_REDECLARATION);
}

bool attributes::set_prior(const std::string& pri, const y::lexeme_type& l)
{
  return SetContextedValue(this->prior, pri, l, ERROR_PRIOR_REDECLARATION);
}

bool attributes::set_derivprior(const std::string& deriv_pri, const y::lexeme_type& l)
{
  return SetContextedValue(this->deriv_prior, deriv_pri, l, ERROR_DERIVPRIOR_REDECLARATION);
}
