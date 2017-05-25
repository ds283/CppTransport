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


#include <assert.h>

#include "replacement_rule_package.h"

namespace macro_packages
  {

    replacement_rule_package::replacement_rule_package(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : data_payload(p),
        printer(prn),
        fctry(f),
        cse_worker(cw),
        lambda_mgr(lm),
        fl(p.model.get_number_params(), p.model.get_number_fields()),
        sym_factory(p.get_symbol_factory())
      {
      }

  }
