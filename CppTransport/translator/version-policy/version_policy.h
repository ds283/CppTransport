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

#ifndef CPPTRANSPORT_VERSION_POLICY_H
#define CPPTRANSPORT_VERSION_POLICY_H


#include <map>

#include "policies.h"


class version_policy
  {
    
    // TYPES
    
  private:
    
    //! policy table is a map from a policy identifier number to a minimum CppTransport
    //! version number at which the switch in policy was made
    typedef std::map< unsigned int, unsigned int > policy_table;
    
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    version_policy();
    
    //! destructor
    ~version_policy() = default;
    
    
    // VERSION NUMBER
    
  public:
    
    //! advise which version number of CppTransport we should behave like
    void advise_minimum_version(unsigned int version);
    
    
    // POLICY ADJUDICATION
    
  public:
    
    //! lookup a specific policy number
    bool adjudicate_policy(unsigned int number) const;
    
    
    // INTERNAL DATA
    
  private:
  
    //! version of CppTransport that we should use when adjudicating policies
    unsigned int version;
  
    //! policy table
    policy_table policies;
  
  };


#endif //CPPTRANSPORT_VERSION_POLICY_H
