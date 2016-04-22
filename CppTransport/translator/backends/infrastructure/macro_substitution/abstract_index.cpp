//
// Created by David Seery on 06/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#include "abstract_index.h"


abstract_index::abstract_index(char l, unsigned int f, unsigned int p)
  : label(l),
    classification(identify_index(l)),
    fields(f),
    params(p),
    pre_string("__"),
    post_string("")
  {
  }


abstract_index::abstract_index(char l, enum index_class c, unsigned int f, unsigned int p)
  : label(l),
    classification(c),
    fields(f),
    params(p),
    pre_string("__"),
    post_string("")
  {
  }


unsigned int abstract_index::numeric_range() const
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
