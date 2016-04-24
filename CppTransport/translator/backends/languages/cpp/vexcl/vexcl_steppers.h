//
// Created by David Seery on 06/12/2013.
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



#ifndef CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H
#define CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H


#include "replacement_rule_package.h"
#include "stepper.h"


namespace vexcl
  {

    constexpr unsigned int BACKG_STEPPER_STATE_ARGUMENT = 0;
    constexpr unsigned int BACKG_STEPPER_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int PERT_STEPPER_STATE_ARGUMENT = 0;
    constexpr unsigned int PERT_STEPPER_TOTAL_ARGUMENTS = 1;

    constexpr unsigned int BACKG_NAME_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PERT_NAME_TOTAL_ARGUMENTS = 0;


    class replace_backg_stepper : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_backg_stepper(std::string n, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_simple(std::move(n), BACKG_STEPPER_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_backg_stepper() = default;


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


    class replace_pert_stepper : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_pert_stepper(std::string n, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_simple(std::move(n), PERT_STEPPER_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~replace_pert_stepper() = default;


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


    class stepper_name : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        stepper_name(std::string n, translator_data& p, language_printer& prn)
          : ::macro_packages::replacement_rule_simple(std::move(n), BACKG_NAME_TOTAL_ARGUMENTS),
            data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~stepper_name() = default;


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


    class vexcl_steppers: public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_steppers(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~vexcl_steppers() = default;

      };

  } // namespace cpp


#endif //CPPTRANSPORT_MACROS_VEXCL_STEPPERS_H
