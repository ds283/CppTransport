//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <sstream>

#include "shared_resources.h"


shared_resources::shared_resources(translator_data& p, resource_manager& m, expression_cache& c)
  : mgr(m),
    cache(c),
    payload(p),
    M_Planck(p.get_Mp_symbol()),
    sym_factory(p.get_symbol_factory()),
    field_list(p.get_field_symbols()),
    deriv_list(p.get_deriv_symbols()),
    param_list(p.get_parameter_symbols()),
    num_params(p.get_number_parameters()),
    num_fields(p.get_number_fields()),
    num_phase(2*p.get_number_fields()),
    fl(p.get_number_parameters(), p.get_number_fields())
  {
  }


std::unique_ptr<symbol_list> shared_resources::generate_parameters(const language_printer& printer) const
  {
    std::unique_ptr<symbol_list> list = std::make_unique< std::vector<GiNaC::symbol> >();

    const boost::optional< contexted_value<std::string> >& resource = this->mgr.parameters();

    if(resource)
      {
        for(param_index i = param_index(0); i < this->num_params; ++i)
          {
            std::string variable = printer.array_subscript(*resource, this->fl.flatten(i));
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(param_index i = param_index(0); i < this->num_params; ++i)
          {
            list->push_back(this->param_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


std::unique_ptr<symbol_list> shared_resources::generate_fields(const language_printer& printer) const
  {
    std::unique_ptr<symbol_list> list = std::make_unique< std::vector<GiNaC::symbol> >();

    const boost::optional< contexted_value<std::string> >& resource = this->mgr.coordinates();
    const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

    if(resource && flatten)
      {
        for(field_index i = field_index(0); i < this->num_fields; ++i)
          {
            std::string variable = printer.array_subscript(*resource, this->fl.flatten(i), *flatten);
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(field_index i = field_index(0); i < this->num_fields; ++i)
          {
            list->push_back(this->field_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


std::unique_ptr<symbol_list> shared_resources::generate_derivs(const language_printer& printer) const
  {
    std::unique_ptr<symbol_list> list = std::make_unique< std::vector<GiNaC::symbol> >();

    const boost::optional< contexted_value<std::string> >& resource = this->mgr.coordinates();
    const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

    if(resource && flatten)
      {
        for(field_index i = field_index(0); i < this->num_fields; ++i)
          {
            // TODO: explicit offset by this->num_fields is a bit ugly; would be nice to find a better approach
            std::string variable = printer.array_subscript(*resource, this->fl.flatten(i), *flatten, static_cast<unsigned int>(this->num_fields));
            GiNaC::symbol sym = this->sym_factory.get_symbol(variable);
            list->push_back(sym);
          }
      }
    else
      {
        for(field_index i = field_index(0); i < this->num_fields; ++i)
          {
            list->push_back(this->deriv_list[this->fl.flatten(i)]);
          }
      }

    return(list);
  }


bool shared_resources::roll_parameters() const
  {
    const boost::optional< contexted_value<std::string> >& resource = this->mgr.parameters();

    return(static_cast<bool>(resource));
  }


bool shared_resources::roll_coordinates() const
  {
    const boost::optional< contexted_value<std::string> >& resource = this->mgr.coordinates();
    const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

    return(resource && flatten);
  }


GiNaC::symbol shared_resources::generate_parameters(const abstract_index& idx, const language_printer& printer) const
  {
    const boost::optional< contexted_value<std::string> >& resource = this->mgr.parameters();

    if(!resource) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(*resource, idx);
    return this->sym_factory.get_symbol(variable);
  }


GiNaC::symbol shared_resources::generate_fields(const abstract_index& idx, const language_printer& printer) const
  {
    const boost::optional< contexted_value<std::string> >& resource = this->mgr.coordinates();
    const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

    if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(*resource, idx, *flatten);
    return this->sym_factory.get_symbol(variable);
  }


GiNaC::symbol shared_resources::generate_derivs(const abstract_index& idx, const language_printer& printer) const
  {
    const boost::optional< contexted_value<std::string> >& resource = this->mgr.coordinates();
    const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

    if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

    std::string variable = printer.array_subscript(*resource, idx, *flatten, static_cast<unsigned int>(this->num_fields));
    return this->sym_factory.get_symbol(variable);
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
