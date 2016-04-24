//
// Created by David Seery on 14/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#include "cse_map.h"
#include "msg_en.h"


cse_map::cse_map(std::unique_ptr< std::vector<GiNaC::ex> > l, cse& c)
  : list(std::move(l)),
    cse_worker(c)
  {
    // parse the whole vector of expressions;
    // if CSE is disabled, will have no effect
    for(const GiNaC::ex& expr: *list)
      {
        cse_worker.parse(expr);
      }
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    // check whether subscripting is within bounds;
    // if so, return appropriate symbol
    // if CSE is disabled this will be just the raw expression
    if(index < this->list->size()) return this->cse_worker.get_symbol_with_use_count((*this->list)[index]);

    throw std::out_of_range(ERROR_OUT_OF_BOUNDS_CSE_MAP);
  }
