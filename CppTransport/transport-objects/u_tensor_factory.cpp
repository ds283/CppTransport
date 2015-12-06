//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "u_tensor_factory.h"
#include "translation_unit.h"


// *****************************************************************************

// Constructor for parent (virtual) class


u_tensor_factory::u_tensor_factory(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& c)
  : data_payload(p),
    num_fields(p.get_number_fields()),
    V(p.get_potential()), M_Planck(p.get_Mp_symbol()),
    field_list(p.get_field_symbols()),
    deriv_list(p.get_deriv_symbols()),
		cache(c)
  {
    assert(field_list.size() == num_fields);
    assert(deriv_list.size() == num_fields);

		// pause compute timer
		compute_timer.stop();
  }


std::unique_ptr<u_tensor_factory> make_u_tensor_factory(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  {
    // at the moment, nothing to do - only canonical models implemented
    return std::make_unique<canonical_u_tensor_factory>(p, cache);
  }
