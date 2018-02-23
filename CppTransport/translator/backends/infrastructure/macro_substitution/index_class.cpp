//
// Created by David Seery on 06/12/2015.
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


#include <ctype.h>

#include "index_class.h"
#include "msg_en.h"


index_class identify_index(char label)
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


std::string to_string(index_class c)
  {
    switch(c)
      {
        case index_class::parameter: return LABEL_INDEX_CLASS_PARAMETER;
        case index_class::field_only: return LABEL_INDEX_CLASS_FIELD;
        case index_class::full: return LABEL_INDEX_CLASS_FULL;
      }
  }
