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
    num_params(p.get_number_parameters()),
    V(p.get_potential()),
    M_Planck(p.get_Mp_symbol()),
    field_list(p.get_field_symbols()),
    deriv_list(p.get_deriv_symbols()),
    param_list(p.get_parameter_symbols()),
		cache(c),
    fl(2*p.get_number_fields()),
    field_fl(p.get_number_fields())
  {
    assert(field_list.size() == num_fields);
    assert(deriv_list.size() == num_fields);
    assert(param_list.size() == num_params);

		// pause compute timer
		compute_timer.stop();
  }


unsigned int u_tensor_factory::species(unsigned int z)
  {
    return (z >= this->num_fields ? z - this->num_fields : z);
  }


unsigned int u_tensor_factory::momentum(unsigned int z)
  {
    return (z >= this->num_fields ? z : z + this->num_fields);
  }


const GiNaC::symbol& u_tensor_factory::coordinate(unsigned int z)
  {
    return ((z < this->num_fields) ? this->field_list[z] : this->deriv_list[z - this->num_fields]);
  }


bool u_tensor_factory::is_field(unsigned int z)
  {
    return (z < this->num_fields);
  }


bool u_tensor_factory::is_momentum(unsigned int z)
  {
    return (z >= this->num_fields && z < 2 * this->num_fields);
  }


std::unique_ptr<GiNaC::exmap> u_tensor_factory::substitution_map(const std::vector<GiNaC::symbol>& params,
                                                                 const std::vector<GiNaC::symbol>& fields)
  {
    std::unique_ptr<GiNaC::exmap> map = std::make_unique<GiNaC::exmap>();

    if(params.size() != this->num_params) throw u_tensor_exception(ERROR_SUBSTITUTION_FAILURE);
    if(fields.size() != this->num_fields) throw u_tensor_exception(ERROR_SUBSTITUTION_FAILURE);

    for(unsigned int i = 0; i < this->num_params; ++i)
      {
        (*map)[this->param_list[i]] = params[i];
      }

    for(unsigned int i = 0; i < this->num_fields; ++i)
      {
        (*map)[this->field_list[i]] = fields[i];
      }

    return(map);
  }


GiNaC::ex u_tensor_factory::substitute_V(const GiNaC::exmap& map)
  {
    // disable unneeded pattern-matching for speed
    return this->V.subs(map, GiNaC::subs_options::no_pattern);
  }
