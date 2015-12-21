//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "index_traits.h"


bool index_traits::is_species(phase_index index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return(raw < this->num_fields);
  }


bool index_traits::is_momentum(phase_index index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return(raw >= this->num_fields && raw < 2 * this->num_fields);
  }


phase_index index_traits::to_momentum(field_index index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return phase_index(raw + this->num_fields);
  }


phase_index index_traits::to_momentum(phase_index index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    if(raw < this->num_fields) raw += this->num_fields;
    return phase_index(raw);
  }


field_index index_traits::to_species(phase_index index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    if(raw >= this->num_fields) raw -= this->num_fields;
    return field_index(raw);
  }
