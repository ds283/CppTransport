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
// --@@
//


#include "stepper.h"


bool stepper::set_name(const std::string& s, y::lexeme_type& l)
  {
    return SetContextedValue(this->name, s, l, ERROR_STEPPER_REDECLARATION);
  }


bool stepper::set_stepsize(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->stepsize, d, l, ERROR_STEPSIZE_REDECLARATION);
  }


bool stepper::set_relerr(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->relerr, d, l, ERROR_RELERR_REDECLARATION);
  }


bool stepper::set_abserr(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->abserr, d, l, ERROR_ABSERR_REDECLARATION);
  }
