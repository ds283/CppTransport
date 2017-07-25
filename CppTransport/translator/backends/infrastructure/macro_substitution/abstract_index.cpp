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


#include "abstract_index.h"


abstract_index::abstract_index(char l, unsigned int f, unsigned int p)
  : label{l},
    classification{identify_index(l)},
    fields{f},
    params{p},
    pre_string{ {"_"} },
    post_string{},
    species_mappings{0}
  {
  }


abstract_index::abstract_index(char l, index_class c, unsigned int f, unsigned int p)
  : label{l},
    classification{c},
    fields{f},
    params{p},
    pre_string{ {"_"} },
    post_string{},
    species_mappings{0}
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


std::string abstract_index::get_loop_variable() const
  {
    std::string r(1, this->label);

    if(this->species_mappings != 0)
      {
        int offset = static_cast<int>(this->fields) * this->species_mappings;

        if(offset > 0)
          {
            r.append("+");
            r.append(std::to_string(offset));
          }
        else
          {
            r.append(std::to_string(offset));
          }
      }

    for(const std::string& s : this->pre_string)
      {
        r.insert(0,s);
      }
    for(const std::string& s : this->post_string)
      {
        r.append(s);
      }

    return r;
  }


void abstract_index::push_post_modifier(std::string s)
  {
    this->post_string.push_back(std::move(s));
  }


void abstract_index::pop_post_modifier()
  {
    if(!this->post_string.empty()) this->post_string.pop_back();
  }


void abstract_index::push_pre_modifier(std::string s)
  {
    this->pre_string.push_front(std::move(s));
  }


void abstract_index::push_pre_modifier()
  {
    if(!this->pre_string.empty()) this->pre_string.pop_front();
  }


void abstract_index::convert_species_to_momentum()
  {
    ++this->species_mappings;
    if(this->species_mappings > 1 || this->species_mappings < -1) throw std::runtime_error(ERROR_SPECIES_MAPPING_OVERFLOW);
  }


void abstract_index::convert_momentum_to_species()
  {
    --this->species_mappings;
    if(this->species_mappings > 1 || this->species_mappings < -1) throw std::runtime_error(ERROR_SPECIES_MAPPING_OVERFLOW);
  }
