//
// Created by David Seery on 14/12/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#include "cse_map.h"


cse_map::cse_map(std::unique_ptr< std::vector<GiNaC::ex> > l, cse& c)
  : list(std::move(l)),
    cse_worker(c)
  {
    // if doing CSE, parse the whole vector of expressions
    if(cse_worker.do_cse())
      {
        for(const GiNaC::ex& expr: *list)
          {
            cse_worker.parse(expr);
          }
      }
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    if(index < this->list->size())
      {
        if(this->cse_worker.do_cse())
          {
            rval = this->cse_worker.get_symbol_with_use_count((*this->list)[index]);
          }
        else
          {
            rval = (this->cse_worker.get_ginac_printer()).ginac((*this->list)[index]);
          }
      }

    return(rval);
  }
