//
// Created by Alessandro Maraio on 20/10/2019.
//
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_COSMOLOGY_MACROS_H
#define CPPTRANSPORT_COSMOLOGY_MACROS_H

#include "msg_en.h"
#include "replacement_rule_package.h"

namespace macro_packages {

  // Argument lists for cosmology macros
  constexpr unsigned int COSMOLOGY_TEMPLATE_TOTALARGS = 0;
  constexpr unsigned int COSMOLOGY_HUBBLECONST_TOTALARGS = 0;
  constexpr unsigned int COSMOLOGY_OMEGAB_TOTALARGS = 0;
  constexpr unsigned int COSMOLOGY_OMEGACDM_TOTALARGS = 0;
  constexpr unsigned int COSMOLOGY_TAU_TOTALARGS = 0;
  constexpr unsigned int COSMOLOGY_TCMB_TOTALARGS = 0;

  class replace_template : public replacement_rule_simple
  {
    public:
    // Constructor
    replace_template(std::string n, translator_data& p, language_printer& prn)
      : replacement_rule_simple(std::move(n), COSMOLOGY_TEMPLATE_TOTALARGS),
        data_payload(p),
        printer(prn)
    {
    }

    // Destructor
    ~replace_template() override = default;

    protected:
    // Function that gets called at macro evaluation
    std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
    // data payload
    translator_data& data_payload;

    // language printer
    language_printer& printer;
  };


  class replace_hubble : public replacement_rule_simple
  {
    public:
      // Constructor
      replace_hubble(std::string n, translator_data& p, language_printer& prn)
        : replacement_rule_simple(std::move(n), COSMOLOGY_HUBBLECONST_TOTALARGS),
          data_payload(p),
          printer(prn)
      {
      }

      // Destructor
      ~replace_hubble() override = default;

    protected:
      // Function that gets called at macro evaluation
      std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
      // data payload
      translator_data& data_payload;

      // language printer
      language_printer& printer;
  };


  class replace_omegab : public replacement_rule_simple
  {
    public:
    // Constructor
    replace_omegab(std::string n, translator_data& p, language_printer& prn)
      : replacement_rule_simple(std::move(n), COSMOLOGY_OMEGAB_TOTALARGS),
        data_payload(p),
        printer(prn)
    {
    }

    // Destructor
    ~replace_omegab() override = default;

    protected:
    // Function that gets called at macro evaluation
    std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
    // data payload
    translator_data& data_payload;

    // language printer
    language_printer& printer;
  };


  class replace_omegacdm : public replacement_rule_simple
  {
    public:
    // Constructor
    replace_omegacdm(std::string n, translator_data& p, language_printer& prn)
      : replacement_rule_simple(std::move(n), COSMOLOGY_OMEGACDM_TOTALARGS),
        data_payload(p),
        printer(prn)
    {
    }

    // Destructor
    ~replace_omegacdm() override = default;

    protected:
    // Function that gets called at macro evaluation
    std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
    // data payload
    translator_data& data_payload;

    // language printer
    language_printer& printer;
  };


  class replace_tau : public replacement_rule_simple
  {
    public:
    // Constructor
    replace_tau(std::string n, translator_data& p, language_printer& prn)
      : replacement_rule_simple(std::move(n), COSMOLOGY_TAU_TOTALARGS),
        data_payload(p),
        printer(prn)
    {
    }

    // Destructor
    ~replace_tau() override = default;

    protected:
    // Function that gets called at macro evaluation
    std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
    // data payload
    translator_data& data_payload;

    // language printer
    language_printer& printer;
  };

  class replace_tcmb : public replacement_rule_simple
  {
    public:
    // Constructor
    replace_tcmb(std::string n, translator_data& p, language_printer& prn)
      : replacement_rule_simple(std::move(n), COSMOLOGY_TCMB_TOTALARGS),
        data_payload(p),
        printer(prn)
    {
    }

    // Destructor
    ~replace_tcmb() override = default;

    protected:
    // Function that gets called at macro evaluation
    std::string evaluate(const macro_argument_list& args) override;

    // Internal data
    private:
    // data payload
    translator_data& data_payload;

    // language printer
    language_printer& printer;
  };



  class cosmology_macros: public replacement_rule_package {

    // Constructor and destructor
    public:
      cosmology_macros(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

      ~cosmology_macros() override = default;

  };

} // namespace macro_packages

#endif //CPPTRANSPORT_COSMOLOGY_MACROS_H
