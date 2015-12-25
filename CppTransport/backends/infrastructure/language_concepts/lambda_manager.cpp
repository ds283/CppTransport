//
// Created by David Seery on 25/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "lambda_manager.h"


lambda_manager::lambda_manager(unsigned int s, language_printer& p, translator_data& pd, std::string k)
  : serial_number(s),
    symbol_counter(0),
    temporary_name_kernel(k),
    printer(p),
    data_payload(pd)
  {
  }
