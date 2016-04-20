//
// Created by David Seery on 06/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#include "index_class.h"

#include <ctype.h>


enum index_class identify_index(char label)
  {
    if(islower(label))        // lower-case indices label fields
      {
        return index_class::field_only;
      }
    else if(isupper(label))   // upper-case indices label fields+momenta, ie. phase space
      {
        return index_class::full;
      }
    else if(isdigit(label))  // numeric indices label parameters
      {
        return index_class::parameter;
      }

    // assume an unclassified index ranges over all of phase-space
    return index_class::full;
  }
