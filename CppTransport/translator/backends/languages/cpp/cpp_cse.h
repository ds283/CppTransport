//
// Created by David Seery on 04/12/2013.
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
// --@@
//



#ifndef CPPTRANSPORT_CPP_CSE_H
#define CPPTRANSPORT_CPP_CSE_H

#include "cse.h"
#include "msg_en.h"

namespace cpp
  {

    class cpp_cse: public cse
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        //! s  = initial serial number for temporaries, typically 0 for a new translation unit
        //! p  = printer appropriate for language
        //! pd = payload from translation_unit
        //! k  = kernel name for temporary identifiers
        cpp_cse(unsigned int s, language_printer& p, translator_data& pd, std::string k=OUTPUT_DEFAULT_CSE_TEMPORARY_NAME)
          : cse(s, p, pd, k)
          {
          }

        //! destructor is default
        virtual ~cpp_cse() = default;


		    // INTERNAL API

      protected:

        //! print a GiNaC expression; if use_count is set then any temporaries which are
        //! used will be marked for deposition
        virtual std::string print         (const GiNaC::ex& expr, bool use_count) override;

        //! print the operands to a GiNaC expression; if use_count is set then any temporaries
        //! which are used will be marked for deposition
        virtual std::string print_operands(const GiNaC::ex& expr, std::string op, bool use_count) override;

        //! special implementation of print_operands() to print a power;
        //! uses strength reduction to compute integer powers by multiplication for small enough exponents
        std::string print_power           (const GiNaC::ex& expr, bool use_count);

      };

  } // namespace cpp


#endif //CPPTRANSPORT_CPP_CSE_H
