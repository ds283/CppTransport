//
// Created by David Seery on 14/11/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "cse_map.h"


cse_map::cse_map(std::vector<GiNaC::ex>* l, replacement_data& d)
  : list(l), data(d)
  {
    data.temp_factory.clear();

    if(data.do_cse)
      {
        // parse each component of the container
        for(int i = 0; i < list->size(); i++)
          {
            data.temp_factory.parse((*list)[i]);
          }

        // deposit any temporaries generated
        // by this process in the current temporary pool
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location
        data.buffer.insert(data.pool, data.temp_factory.temporaries(data.pool_template));
      }
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    if(index < this->list->size())
      {
        if(data.do_cse)
          {
            std::ostringstream out;
            out << this->data.temp_factory.symbol((*this->list)[index]);
            rval = out.str();
            this->data.ms->apply(rval);
          }
        else
          {
            std::ostringstream out;
            out << GiNaC::csrc << (*this->list)[index];
            rval = out.str();
          }
      }

    return(rval);
  }
