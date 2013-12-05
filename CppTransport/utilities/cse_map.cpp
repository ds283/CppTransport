//
// Created by David Seery on 14/11/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "cse_map.h"


cse_map::cse_map(std::vector<GiNaC::ex>* l, replacement_data& d, ginac_printer p)
  : list(l), data(d), printer(p)
  {
//    data.temp_factory.clear();

    if(data.do_cse)
      {
        // parse each component of the container
        for(int i = 0; i < list->size(); i++)
          {
            data.temp_factory.parse((*list)[i]);
          }
      }
  }


std::string cse_map::operator[](unsigned int index)
  {
    std::string rval = "";

    if(index < this->list->size())
      {
        if(data.do_cse)
          {
            rval = this->data.temp_factory.symbol((*this->list)[index]);
          }
        else
          {
            rval = this->printer((*this->list)[index]);
          }
      }

    return(rval);
  }
