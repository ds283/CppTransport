//
// Created by David Seery on 07/06/2017.
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

#ifndef CPPTRANSPORT_RULES_COMMON_H
#define CPPTRANSPORT_RULES_COMMON_H


// use vector to allow index subscripting
typedef std::vector< contexted_value<std::string> > macro_argument_list;


namespace macro_packages
  {

    class rule_apply_fail: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        rule_apply_fail(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~rule_apply_fail() = default;

      };


    class argument_mismatch: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        argument_mismatch(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~argument_mismatch() = default;

      };


    class index_mismatch: public std::runtime_error
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        index_mismatch(const std::string x)
          : std::runtime_error(std::move(x))
          {
          }

        //! destructor is default
        virtual ~index_mismatch() = default;

      };
  
  };


#endif //CPPTRANSPORT_RULES_COMMON_H
