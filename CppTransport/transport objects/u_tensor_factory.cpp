//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include "u_tensor_factory.h"


// *****************************************************************************

// Constructor for parent (virtual) class


u_tensor_factory::u_tensor_factory(script* s)
  : root(s), num_fields(s->get_number_fields()), V(s->get_potential()), M_Planck(s->get_Mp_symbol()),
    field_list(s->get_field_symbols()), deriv_list(s->get_deriv_symbols())
  {
    assert(field_list.size() == num_fields);
    assert(deriv_list.size() == num_fields);
  }


u_tensor_factory* make_u_tensor_factory(script* s)
  {
    // at the moment, nothing to do - only canonical models implemented
    return(new canonical_u_tensor_factory(s));
  }
