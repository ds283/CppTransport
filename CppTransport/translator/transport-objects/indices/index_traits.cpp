//
// Created by David Seery on 19/12/2015.
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


#include <sstream>


#include "index_traits.h"


bool index_traits::is_species(const phase_index& index)
  {
    auto raw = static_cast<unsigned int>(index);
    return raw < this->num_fields;
  }


bool index_traits::is_momentum(const phase_index& index)
  {
    auto raw = static_cast<unsigned int>(index);
    return raw >= this->num_fields && raw < 2 * this->num_fields;
  }


phase_index index_traits::to_momentum(const field_index& index)
  {
    auto raw = static_cast<unsigned int>(index);
    return {raw + this->num_fields, index.get_variance()};
  }


phase_index index_traits::to_momentum(const phase_index& index)
  {
    auto raw = static_cast<unsigned int>(index);
    if(raw < this->num_fields) raw += this->num_fields;
    return {raw, index.get_variance()};
  }


field_index index_traits::to_species(const phase_index& index)
  {
    auto raw = static_cast<unsigned int>(index);
    if(raw >= this->num_fields) raw -= this->num_fields;
    return {raw, index.get_variance()};
  }


std::pair<std::unique_ptr<abstract_index>, std::unique_ptr<index_literal>>
index_traits::species_to_species(const index_literal& index)
  {
    // TODO: consider whether this is the best strategy. Currently we just manufacture new instance of abstract_index and index_literal

    const abstract_index& original_abst = index;

    auto new_abst = std::make_unique<abstract_index>(original_abst.get_label(), index_class::field_only,
                                                     original_abst.get_number_fields(), original_abst.get_number_parameters());
    auto new_index = std::make_unique<index_literal>(index);
    new_index->reassign(*new_abst);
    
    return std::make_pair(std::move(new_abst), std::move(new_index));
  }


std::pair<std::unique_ptr<abstract_index>, std::unique_ptr<index_literal>> index_traits::momentum_to_species(
  const index_literal& index)
  {
    // TODO: consider whether this is the best strategy. Currently we just manufacture new instance of abstract_index and index_literal
    
    
    const abstract_index& original_abst = index;
    
    auto new_abst = std::make_unique<abstract_index>(original_abst.get_label(), index_class::field_only,
                                                     original_abst.get_number_fields(), original_abst.get_number_parameters());
    auto new_index = std::make_unique<index_literal>(index);
    new_index->reassign(*new_abst);

    // add post-modified to loop label that subtracts an appropriate value to turn this momentum value
    // into a species value
    std::ostringstream post_modifier;
    post_modifier << "-" << original_abst.get_number_fields();
    new_abst->push_post_modifier(post_modifier.str());
    
    return std::make_pair(std::move(new_abst), std::move(new_index));
  }
