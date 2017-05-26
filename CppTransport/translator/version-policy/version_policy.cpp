//
// Created by David Seery on 26/05/2017.
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

#include <iostream>

#include "version_policy.h"


version_policy::version_policy()
  : version(POLICY_DEFAULT_VERSION),
    policies( { { POLICY_HEX_UUID, POLICY_HEX_UUID_VERSION } } )
  {
  }


void version_policy::advise_minimum_version(unsigned int version)
  {
    if(version >= POLICY_DEFAULT_VERSION) this->version = version;
  }


bool version_policy::adjudicate_policy(unsigned int number) const
  {
    policy_table::const_iterator t = this->policies.find(number);
    
    // return false if no policy with the specified number has been registered
    if(t == this->policies.cend()) return false;
    
    return this->version >= t->second;
  }
