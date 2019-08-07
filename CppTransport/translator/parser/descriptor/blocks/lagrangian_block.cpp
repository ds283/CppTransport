//
// Created by David Seery on 24/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include "lagrangian_block.h"
#include "generics_descriptor.h"


const auto DERIV_PREFIX = "__d";
const auto DERIV_SUFFIX = "_Dot";
const auto INIT_SUFFIX 	= "_Init";


// CONSTRUCTOR


y::lexeme_type::keyword_map keywords;
y::lexeme_type::character_map symbols;


lagrangian_block::lagrangian_block(symbol_factory& s, version_policy& vp, error_context err_ctx)
  : sym_factory(s),
    policy(vp),
    M_Planck(sym_factory.get_real_symbol(MPLANCK_SYMBOL, MPLANCK_LATEX_SYMBOL))
  {
    // manufacture fake lexeme representing 'location' of Planck mass declaration
    lexeme::lexeme_buffer MPlanck_buffer(MPLANCK_TEXT_NAME, lexeme::lexeme_buffer::type::string_literal, nullptr, 0);
    y::lexeme_type::minus_context mctx = y::lexeme_type::minus_context::unary;

    y::lexeme_type fake_MPlanck_lexeme(MPlanck_buffer, mctx, 0, err_ctx, keywords, symbols);

    // set up attributes for Planck mass symbol
    std::shared_ptr<attributes> Mp_attrs = std::make_shared<attributes>();
    Mp_attrs->set_latex(MPLANCK_LATEX_SYMBOL, fake_MPlanck_lexeme);

    // emplace faked symbol table entry
    reserved.emplace(std::make_pair(MPLANCK_TEXT_NAME,
                                    std::make_unique<parameter_declaration>(MPLANCK_TEXT_NAME, M_Planck,
                                                                            fake_MPlanck_lexeme, Mp_attrs)));
  }


// SYMBOL SERVICES


bool lagrangian_block::add_field(const std::string& n, symbol_wrapper& s, const y::lexeme_type& l,
                                 std::shared_ptr<attributes> a)
  {
    if(this->symbols_frozen) return this->report_frozen(l);
    
    auto check = [&](auto& name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto& name, auto& symbol, auto& lexeme, auto& attr) -> auto
      {
        // add declaration to list
        this->fields.emplace(std::make_pair(name, std::make_unique<field_declaration>(name, symbol, lexeme, attr)));

        // also need to generate a symbol for the momentum corresponding to this field
        auto deriv_symbol = this->sym_factory.get_real_symbol(DERIV_PREFIX + symbol.get_name());
        this->deriv_symbols.push_back(deriv_symbol);

        // also need to generate a symbol for the momentum corresponding to this field - CpptSample style
        auto field_deriv = this->sym_factory.get_real_symbol(symbol.get_name() + DERIV_SUFFIX);
        this->fields_deriv.push_back(field_deriv);

        // If the field value and/or prior is provided, it is added to the list to be returned later for CpptSample
        if(a->get_value().length() > 0)
        {
          auto field_val = this->sym_factory.get_real_symbol(symbol.get_name() + INIT_SUFFIX  + " = " + a->get_value());
          this->fields_vals.push_back(field_val);
        }

        if(a->get_derivvalue().length() > 0)
        {
          auto field_derivval = this->sym_factory.get_real_symbol(symbol.get_name() + DERIV_SUFFIX + INIT_SUFFIX + " = " + a->get_derivvalue() );
          this->fields_derivvals.push_back(field_derivval);
        }

        if(a->get_prior().length() > 0)
        {
          auto field_prior = this->sym_factory.get_real_symbol(symbol.get_name() + INIT_SUFFIX + " = " + a->get_prior() );
          this->fields_priors.push_back(field_prior);
        }

        if(a->get_derivprior().length() > 0)
        {
          auto field_derivprior = this->sym_factory.get_real_symbol(symbol.get_name() + DERIV_SUFFIX + INIT_SUFFIX + " = " + a->get_derivprior() );
          this->fields_derivpriors.push_back(field_derivprior);
        }

      };

    return GenericInsertSymbol(check, insert, n, s, l, a, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATE_SYMBOL_DEFN_WAS);
  }


bool lagrangian_block::add_parameter(const std::string& n, symbol_wrapper& s, const y::lexeme_type& l,
                                     std::shared_ptr<attributes> a)
  {
    if(this->symbols_frozen) return this->report_frozen(l);

    auto check = [&](auto& name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto& name, auto& symbol, auto& lexeme, auto& attr) -> auto
      {
        // add declaration to list
        this->parameters.emplace(std::make_pair(name, std::make_unique<parameter_declaration>(name, symbol, lexeme, attr)));

        // If the parameter value and/or prior is provided, it is added to the list to be called later
        if(a->get_value().length() > 0)
        {
          auto param_val = this->sym_factory.get_real_symbol(symbol.get_name() + INIT_SUFFIX  + " = " + a->get_value());
          this->params_values.push_back(param_val);
        }

        if(a->get_prior().length() > 0)
        {
          auto param_prior = this->sym_factory.get_real_symbol(symbol.get_name() + INIT_SUFFIX + " = " + a->get_prior() );
          this->params_priors.push_back(param_prior);
        }
      };

    return GenericInsertSymbol(check, insert, n, s, l, a, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATE_SYMBOL_DEFN_WAS);
  }


bool lagrangian_block::add_subexpr(const std::string& n, symbol_wrapper& s, const y::lexeme_type& l,
                                   std::shared_ptr<subexpr> e)
  {
    if(this->symbols_frozen) return this->report_frozen(l);

    auto check = [&](auto& name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto& name, auto& symbol, auto& lexeme, auto& expr) -> auto
      {
        // add declaration to list
        this->subexprs.emplace(std::make_pair(name, std::make_unique<subexpr_declaration>(name, symbol, lexeme, expr)));
      };

    return GenericInsertSymbol(check, insert, n, s, l, e, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATE_SYMBOL_DEFN_WAS);
  }


boost::optional<const declaration&> lagrangian_block::check_symbol_exists(const std::string& nm) const
  {
    // check user-defined symbols

    auto f_it = this->fields.find(nm);
    if(f_it != this->fields.end()) return *f_it;

    auto p_it = this->parameters.find(nm);
    if(p_it != this->parameters.end()) return *p_it;

    p_it = this->reserved.find(nm);
    if(p_it != this->reserved.end()) return *p_it;

    auto s_it = this->subexprs.find(nm);
    if(s_it != this->subexprs.end()) return *s_it;

    // didn't find anything -- assume symbol does not exist
    return(boost::none);
  }


// LAGRANGIAN MANAGEMENT


bool lagrangian_block::set_potential(const y::lexeme_type& l, std::shared_ptr<GiNaC::ex> V)
  {
    // symbol tables should be frozen whenever we start to specify the model
    this->freeze_tables(l);

    return SetContextedValue(this->potential, V, l, ERROR_POTENTIAL_REDECLARATION);
  }


boost::optional< contexted_value< std::shared_ptr<GiNaC::ex> > > lagrangian_block::get_potential() const
  {
    if(this->potential) return *this->potential; else return boost::none;
  }


bool lagrangian_block::set_metric(const y::lexeme_type& l, std::shared_ptr<field_metric> f)
  {
    if(!this->type || this->type->get() != model_type::nontrivial_metric)
      {
        l.error(ERROR_METRIC_REQUIRES_NONTRIVIAL);
        
        if(this->type)
          {
            this->type->get_declaration_point().error(ERROR_METRIC_SETTING_WAS);
          }

        throw parse_error(ERROR_METRIC_REQUIRES_NONTRIVIAL);
      }
    
    // symbol tables should be frozen whenever we start to specify the model
    this->freeze_tables(l);
    
    return SetContextedValue(this->metric, f, l, ERROR_METRIC_REDECLARATION);
  }


boost::optional< contexted_value< std::shared_ptr<field_metric> > > lagrangian_block::get_metric() const
  {
    if(this->metric) return *this->metric; else return boost::none;
  }


// FIELDS AND PARAMETERS


unsigned int lagrangian_block::get_number_fields() const
  {
    return(static_cast<unsigned int>(this->fields.size()));
  }


unsigned int lagrangian_block::get_number_params() const
  {
    return(static_cast<unsigned int>(this->parameters.size()));
  }


std::vector<std::string> lagrangian_block::get_field_name_list() const
  {
    auto Get = [&](const field_symbol_table::record_type& v) -> auto { return v.get_name(); };
    return UnzipSortedZipList(this->fields, Get);
  }


std::vector<std::string> lagrangian_block::get_field_latex_list() const
  {
    auto Get = [&](const field_symbol_table::record_type& v) -> auto { return v.get_latex_name(); };
    return UnzipSortedZipList(this->fields, Get);
  }


std::vector<std::string> lagrangian_block::get_param_name_list() const
  {
    auto Get = [&](const parameter_symbol_table::record_type& v) -> auto { return v.get_name(); };
    return UnzipSortedZipList(this->parameters, Get);
  }


std::vector<std::string> lagrangian_block::get_param_latex_list() const
  {
    auto Get = [&](const parameter_symbol_table::record_type& v) -> auto { return v.get_latex_name(); };
    return UnzipSortedZipList(this->parameters, Get);
  }


symbol_list lagrangian_block::get_field_symbols() const
  {
    auto Get = [&](const field_symbol_table::record_type& v) -> auto { return v.get_ginac_symbol(); };
    return UnzipSortedZipList(this->fields, Get);
  }


symbol_list lagrangian_block::get_deriv_symbols() const
  {
    return(this->deriv_symbols);
  }

// Customised function to get my way of doing field derivatives
symbol_list lagrangian_block::get_field_deriv() const
{
  return(this->fields_deriv);
}

// Four new functions that gets field values & priors, both normal and derivatives for CpptSample/cosmosis
symbol_list lagrangian_block::get_field_val() const
{
  return(this->fields_vals);
}

symbol_list lagrangian_block::get_field_derivval() const
{
  return(this->fields_derivvals);
}

symbol_list lagrangian_block::get_field_prior() const
{
  return(this->fields_priors);
}

symbol_list lagrangian_block::get_field_derivprior() const
{
  return(this->fields_derivpriors);
}

symbol_list lagrangian_block::get_param_symbols() const
  {
    auto Get = [&](const parameter_symbol_table::record_type& v) -> auto { return v.get_ginac_symbol(); };
    return UnzipSortedZipList(this->parameters, Get);
  }

// Get the list of parameter values for CpptSample/cosmosis
symbol_list lagrangian_block::get_param_values() const
{
  return(this->params_values);
}

// Get the list of parameter priors for CpptSample/cosmosis
symbol_list lagrangian_block::get_param_priors() const
{
  return(this->params_priors);
}

const symbol_wrapper& lagrangian_block::get_Mp_symbol() const
  {
    return(this->M_Planck);
  }


validation_exceptions lagrangian_block::validate() const
  {
    validation_exceptions list;
    
    if(!this->type || this->type->get() == model_type::canonical)
      {
        if(!this->potential) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_POTENTIAL));
      }
    else if(this->type->get() == model_type::nontrivial_metric)
      {
        if(!this->potential) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_POTENTIAL));
        if(!this->metric) list.push_back(std::make_unique<validation_message>(true, ERROR_NO_METRIC));
      }
    
    return list;
  }


void lagrangian_block::set_lagrangian_type(model_type t, const y::lexeme_type& l)
  {
    SetContextedValue(this->type, t, l, ERROR_LAGRANGIAN_TYPE_REDECLARATION);
  }


model_type lagrangian_block::get_lagrangian_type() const
  {
    if(this->type) return *this->type;
    return model_type::canonical;
  }


void lagrangian_block::freeze_tables(const y::lexeme_type& l)
  {
    // nothing to do if already frozen
    if(this->symbols_frozen) return;
    
    // set token contexted value to remember where freezing-point occurred
    SetContextedValue(this->symbols_frozen, 1u, l, "");
  }


bool lagrangian_block::report_frozen(const y::lexeme_type& l)
  {
    l.get_error_context().error(ERROR_MODEL_SPECIFICATION_STARTED);
    if(this->symbols_frozen)
      {
        auto& v = *this->symbols_frozen;
        v.get_declaration_point().warn(WARNING_SPECIFICATION_START_WAS);
      }

    throw parse_error(ERROR_MODEL_SPECIFICATION_STARTED);
  }


std::shared_ptr<field_metric_base> lagrangian_block::make_field_metric_base() const
  {
    return std::make_shared<field_metric_base>(this->fields);
  }
