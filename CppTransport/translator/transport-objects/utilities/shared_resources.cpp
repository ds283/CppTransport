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

#include "shared_resources.h"


shared_resources::shared_resources(translator_data& p, resource_manager& m, expression_cache& c)
  : mgr(m),
    cache(c),
    payload(p),
    M_Planck(p.model.get_Mp_symbol()),
    sym_factory(p.get_symbol_factory()),
    field_list(p.model.get_field_symbols()),
    deriv_list(p.model.get_deriv_symbols()),
    param_list(p.model.get_param_symbols()),
    num_params(p.model.get_number_params()),
    num_fields(p.model.get_number_fields()),
    num_phase(2*p.model.get_number_fields()),
    fl(p.model.get_number_params(), p.model.get_number_fields())
  {
  }


std::unique_ptr<symbol_list> shared_resources::generate_parameter_symbols(const language_printer& printer) const
  {
    auto list = std::make_unique<symbol_list>();

    const auto& resource = this->mgr.parameters();
    
    const auto max_i = this->get_max_param_index();

    if(resource)
      {
        for(param_index i = param_index(0); i < max_i; ++i)
          {
            std::string variable = printer.array_subscript(*resource, this->fl.flatten(i));
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(param_index i = param_index(0); i < max_i; ++i)
          {
            list->push_back(this->param_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


std::unique_ptr<symbol_list> shared_resources::generate_field_symbols(const language_printer& printer) const
  {
    auto list = std::make_unique<symbol_list>();

    const auto resource = this->mgr.coordinates();
    const auto& flatten = this->mgr.phase_flatten();
    
    const auto max_i = this->get_max_field_index(variance::none);

    if(resource && flatten)
      {
        for(field_index i = field_index(0); i < max_i; ++i)
          {
            std::string variable = printer.array_subscript(resource.get().second, this->fl.flatten(i), *flatten);
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(field_index i = field_index(0); i < max_i; ++i)
          {
            list->push_back(this->field_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


std::unique_ptr<symbol_list> shared_resources::generate_deriv_symbols(const language_printer& printer) const
  {
    auto list = std::make_unique<symbol_list>();

    const auto resource = this->mgr.coordinates();
    const auto& flatten = this->mgr.phase_flatten();
    
    const auto max_i = this->get_max_field_index(variance::none);

    if(resource && flatten)
      {
        for(field_index i = field_index(0); i < max_i; ++i)
          {
            // TODO: explicit offset by this->num_fields is a bit ugly; would be nice to find a better approach
            std::string variable = printer.array_subscript(resource.get().second, this->fl.flatten(i), *flatten, static_cast<unsigned int>(this->num_fields));
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(field_index i = field_index(0); i < max_i; ++i)
          {
            list->push_back(this->deriv_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


bool shared_resources::can_roll_parameters() const
  {
    const auto& resource = this->mgr.parameters();

    return(static_cast<bool>(resource));
  }


bool shared_resources::can_roll_coordinates() const
  {
    const auto resource = this->mgr.coordinates();
    const auto& flatten = this->mgr.phase_flatten();

    return(resource && flatten);
  }


GiNaC::symbol shared_resources::generate_parameter_symbols(const abstract_index& idx, const language_printer& printer) const
  {
    const auto& resource = this->mgr.parameters();

    if(!resource) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(*resource, idx);
    return this->sym_factory.get_symbol(variable);
  }


GiNaC::symbol shared_resources::generate_field_symbols(const abstract_index& idx, const language_printer& printer) const
  {
    const auto resource = this->mgr.coordinates();
    const auto& flatten = this->mgr.phase_flatten();

    if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(resource.get().second, idx, *flatten);
    return this->sym_factory.get_symbol(variable);
  }


GiNaC::symbol shared_resources::generate_deriv_symbols(const abstract_index& idx, const language_printer& printer) const
  {
    const auto resource = this->mgr.coordinates();
    const auto& flatten = this->mgr.phase_flatten();

    if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(resource.get().second, idx, *flatten, static_cast<unsigned int>(this->num_fields));
    return this->sym_factory.get_symbol(variable);
  }


std::string shared_resources::generate_working_type() const
  {
    const auto& working_type = this->mgr.working_type();

    if(!working_type) throw resource_failure("WORKING TYPE");

    return *working_type;
  }


GiNaC::idx shared_resources::generate_index(const field_index& i)
  {
    return GiNaC::idx(static_cast<unsigned int>(i), static_cast<unsigned int>(this->num_fields));
  }


GiNaC::idx shared_resources::generate_index(const phase_index& i)
  {
    return GiNaC::idx(static_cast<unsigned int>(i), 2*static_cast<unsigned int>(this->num_fields));
  }


GiNaC::idx shared_resources::generate_index(const abstract_index& i)
  {
    std::string name = i.get_loop_variable();
    GiNaC::symbol sym = this->sym_factory.get_symbol(name);

    unsigned int size = 0;
    switch(i.get_class())
      {
        case index_class::parameter:
          {
            size = static_cast<unsigned int>(this->num_params);
            break;
          }

        case index_class::field_only:
          {
            size = static_cast<unsigned int>(this->num_fields);
            break;
          }

        case index_class::full:
          {
            size = static_cast<unsigned int>(this->num_phase);
            break;
          }
      }

    return GiNaC::idx(sym, size);
  }
