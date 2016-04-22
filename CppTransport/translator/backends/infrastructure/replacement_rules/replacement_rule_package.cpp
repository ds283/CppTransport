//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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
        fl(p.get_number_parameters(), p.get_number_fields()),
        sym_factory(p.get_symbol_factory())
      {
      }

  }
