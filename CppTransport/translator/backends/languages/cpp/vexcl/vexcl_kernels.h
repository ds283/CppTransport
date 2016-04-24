//
// Created by David Seery on 08/12/2013.
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


#ifndef CPPTRANSPORT_MACROS_VEXCL_KERNELS_H
#define CPPTRANSPORT_MACROS_VEXCL_KERNELS_H


#include "replacement_rule_package.h"


namespace vexcl
  {

    constexpr unsigned int IMPORT_KERNEL_FILE_ARGUMENT = 0;
    constexpr unsigned int IMPORT_KERNEL_NAME_ARGUMENT = 1;
    constexpr unsigned int IMPORT_KERNEL_TRAILING_ARGUMENT = 2;
    constexpr unsigned int IMPORT_KERNEL_TOTAL_ARGUMENTS = 3;


    class import_kernel : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        import_kernel(std::string n, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_simple(std::move(n), IMPORT_KERNEL_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~import_kernel() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class vexcl_kernels : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_kernels(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~vexcl_kernels() = default;

      };

  } // namespace vexcl


#endif //CPPTRANSPORT_MACROS_VEXCL_KERNELS_H
