//
// Created by David Seery on 06/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include "index_abstract.h"


index_abstract::index_abstract(char l, unsigned int f, unsigned int p)
  : label(l),
    classification(identify_index(l)),
    fields(f),
    params(p)
  {
  }


index_abstract::index_abstract(char l, enum index_class c, unsigned int f, unsigned int p)
  : label(l),
    classification(c),
    fields(f),
    params(p)
  {
  }


unsigned int index_abstract::numeric_range() const
  {
    switch(this->classification)
      {
        case index_class::field_only:
          {
            return(this->fields);
          }

        case index_class::full:
          {
            return(2*this->fields);
          }

        case index_class::parameter:
          {
            return(this->params);
          }
      }
  }
