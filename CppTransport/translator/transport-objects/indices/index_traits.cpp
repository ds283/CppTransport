//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include <sstream>


#include "index_traits.h"


bool index_traits::is_species(const phase_index& index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return(raw < this->num_fields);
  }


bool index_traits::is_momentum(const phase_index& index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return(raw >= this->num_fields && raw < 2 * this->num_fields);
  }


phase_index index_traits::to_momentum(const field_index& index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    return phase_index(raw + this->num_fields);
  }


phase_index index_traits::to_momentum(const phase_index& index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    if(raw < this->num_fields) raw += this->num_fields;
    return phase_index(raw);
  }


field_index index_traits::to_species(const phase_index& index)
  {
    unsigned raw = static_cast<unsigned int>(index);
    if(raw >= this->num_fields) raw -= this->num_fields;
    return field_index(raw);
  }


abstract_index index_traits::species_to_species(const abstract_index& index)
  {
    abstract_index converted(index.get_label(), index_class::field_only, index.get_number_fields(), index.get_number_parameters());
    return(converted);
  }


abstract_index index_traits::momentum_to_species(const abstract_index& index)
  {
    abstract_index converted(index.get_label(), index_class::field_only, index.get_number_fields(), index.get_number_parameters());

    std::ostringstream tag;
    tag << "-" << index.get_number_fields();
    converted.set_post_string(tag.str());

    return(converted);
  }
