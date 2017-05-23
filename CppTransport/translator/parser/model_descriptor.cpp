//
// Created by David Seery on 18/06/2013.
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
#include <assert.h>
#include <algorithm>

#include "lexical.h"
#include "model_descriptor.h"

const auto DERIV_PREFIX = "__d";


// ******************************************************************


declaration::declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l)
  : name(n),
    symbol(s),
    declaration_point(l),
		my_id(current_id++)
  {
  }

// initialize static member
unsigned int declaration::current_id = 0;


// ******************************************************************


field_declaration::field_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  : declaration(n, s, l)
  {
		attrs = std::make_unique<attributes>(*a);
  }


std::string field_declaration::get_latex_name() const
	{
    std::string latex_name = this->attrs->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
	}


void field_declaration::print(std::ostream& stream) const
  {
    stream << "Field declaration for symbol '" << this->get_name()
           << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
  }


// ******************************************************************


parameter_declaration::parameter_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  : declaration(n, s, l)
  {
		attrs = std::make_unique<attributes>(*a);
  }


std::string parameter_declaration::get_latex_name() const
	{
    std::string latex_name = this->attrs->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
	}


void parameter_declaration::print(std::ostream& stream) const
  {
    stream << "Parameter declaration for symbol '" << this->get_name()
      << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
  }


// ******************************************************************


subexpr_declaration::subexpr_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr* e)
	: declaration(n, s, l)
	{
		sexpr = std::make_unique<subexpr>(*e);
	}


std::string subexpr_declaration::get_latex_name() const
	{
    std::string latex_name = this->sexpr->get_latex();

    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
	}


GiNaC::ex subexpr_declaration::get_value() const
	{
		return this->sexpr->get_value();
	}


void subexpr_declaration::print(std::ostream& stream) const
	{
    stream << "Subexpression declaration for symbol '" << this->get_name()
	    << "', GiNaC symbol '" << this->get_ginac_symbol() << "'" << '\n';
	}


// ******************************************************************


author_declaration::author_declaration(const std::string& n, const y::lexeme_type& l, author* a)
  : declaration_point(l)
  {
    auth = std::make_unique<author>(*a);
  }


std::string author_declaration::get_name() const
  {
    return this->auth->get_name();
  }


std::string author_declaration::get_email() const
  {
    return this->auth->get_email();
  }


std::string author_declaration::get_institute() const
  {
    return this->auth->get_institute();
  }


void author_declaration::print(std::ostream& stream) const
  {
    stream << "Author declaration for '" << this->get_name() << "', email = '" << this->get_email() << "', institute = '" << this->get_institute() << "'" << '\n';
  }


// ******************************************************************


std::vector<std::string>     fake_keyword_table;
std::vector<keyword_type>    fake_keyword_map;
std::vector<std::string>     fake_character_table;
std::vector<character_type>  fake_character_map;
std::vector<bool>            fake_context_table;


model_descriptor::model_descriptor(symbol_factory& s, error_context err_ctx)
  : errors_encountered(false),
    order(index_order::right),
		sym_factory(s)
  {
		// set up reserved symbol for Planck mass
    M_Planck = sym_factory.get_symbol(MPLANCK_SYMBOL, MPLANCK_LATEX_SYMBOL);

    // manufacture fake lexeme representing 'location' of Planck mass decalaration
    lexeme::lexeme_buffer MPlanck_buffer(MPLANCK_TEXT_NAME, lexeme::lexeme_buffer::type::string_literal);
    lexeme::minus_context mctx = lexeme::minus_context::unary;
    fake_MPlanck_lexeme = std::make_unique<y::lexeme_type>(MPlanck_buffer, mctx, 0, err_ctx,
                                                           fake_keyword_table, fake_keyword_map, fake_character_table,
                                                           fake_character_map, fake_context_table);

    // set up attributes for Planck mass symbol
    attributes Mp_attrs;
    Mp_attrs.set_latex(MPLANCK_LATEX_SYMBOL, *fake_MPlanck_lexeme);

    // emplace faked symbol table entry
		reserved.emplace(std::make_pair(MPLANCK_TEXT_NAME, std::make_unique<parameter_declaration>(MPLANCK_TEXT_NAME, M_Planck, *fake_MPlanck_lexeme, &Mp_attrs)));
  }


// SYMBOL SERVICES


bool model_descriptor::add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  {
    auto check = [&](auto name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto name, auto symbol, auto lexeme, auto attr) -> auto
      {
        // add declaration to list
        this->fields.emplace(std::make_pair(name, std::make_unique<field_declaration>(name, symbol, lexeme, attr)));
    
        // also need to generate a symbol for the momentum corresponding to this field
        GiNaC::symbol deriv_symbol(DERIV_PREFIX + symbol.get_name());
        this->deriv_symbols.push_back(deriv_symbol);
      };
    
    if(GenericInsertSymbol(check, insert, n, s, l, a, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATION_DEFINITION_WAS)) return true;

    this->errors_encountered = true;
    return false;
  }


bool model_descriptor::add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  {
    auto check = [&](auto name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto name, auto symbol, auto lexeme, auto attr) -> auto
      {
        // add declaration to list
        this->parameters.emplace(std::make_pair(name, std::make_unique<parameter_declaration>(name, symbol, lexeme, attr)));
      };
    
    if(GenericInsertSymbol(check, insert, n, s, l, a, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATION_DEFINITION_WAS)) return true;
    
    this->errors_encountered = true;
    return false;
  }


bool model_descriptor::add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr* e)
  {
    auto check = [&](auto name) -> auto { return this->check_symbol_exists(name); };
    auto insert = [&](auto name, auto symbol, auto lexeme, auto expr) -> auto
      {
        // add declaration to list
        this->subexprs.emplace(std::make_pair(name, std::make_unique<subexpr_declaration>(name, symbol, lexeme, expr)));
      };
    
    if(GenericInsertSymbol(check, insert, n, s, l, e, ERROR_SYMBOL_EXISTS, NOTIFY_DUPLICATION_DEFINITION_WAS)) return true;
    
    this->errors_encountered = true;
    return false;
  }


boost::optional<declaration&> model_descriptor::check_symbol_exists(const std::string& nm) const
	{
		// check user-defined symbols
    
    auto f_it = this->fields.find(nm);
		if(f_it != this->fields.end()) return *f_it->second;
    
    auto p_it = this->parameters.find(nm);
		if(p_it != this->parameters.end()) return *p_it->second;

		p_it = this->reserved.find(nm);
		if(p_it != this->reserved.end()) return *p_it->second;
    
    auto s_it = this->subexprs.find(nm);
		if(s_it != this->subexprs.end()) return *s_it->second;

		// didn't find anything -- assume symbol does not exist
		return(boost::none);
	}


// BASIC METADATA


bool model_descriptor::set_name(const std::string n, const y::lexeme_type& l)
  {
    return SetContextedValue(this->name, n, l, ERROR_NAME_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_name() const
  {
    if(this->name) return(*this->name); else return(boost::none);
  }


bool model_descriptor::set_citeguide(const std::string t, const y::lexeme_type& l)
  {
    return SetContextedValue(this->citeguide, t, l, ERROR_CITEGUIDE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_citeguide() const
  {
    if(this->citeguide) return(*this->citeguide); else return(boost::none);
  }


bool model_descriptor::set_description(const std::string d, const y::lexeme_type& l)
  {
    return SetContextedValue(this->description, d, l, ERROR_DESCRIPTION_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_description() const
  {
    if(this->description) return *this->description; else return boost::none;
  }


bool model_descriptor::set_revision(int r, const y::lexeme_type& l)
  {
    return SetContextedValue(this->revision, static_cast<unsigned>(std::abs(r)), l, ERROR_REVISION_DECLARATION);
  }


boost::optional< contexted_value<unsigned int>& > model_descriptor::get_revision() const
  {
    if(this->revision) return *this->revision; else return boost::none;
  }


bool model_descriptor::set_license(const std::string lic, const y::lexeme_type& l)
  {
    return SetContextedValue(this->license, lic, l, ERROR_LICENSE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_license() const
  {
    if(this->license) return *this->license; else return boost::none;
  }


bool model_descriptor::set_references(const std::vector< contexted_value<std::string> >& refs)
  {
    return SetContextedValue(this->references, refs, ERROR_REFERENCES_REDECLARATION);
  }


boost::optional< std::vector< contexted_value<std::string> >& > model_descriptor::get_references() const
  {
    if(this->references) return *this->references; else return boost::none;
  }


bool model_descriptor::set_urls(const std::vector<contexted_value<std::string> >& urls)
  {
    return SetContextedValue(this->urls, urls, ERROR_URLS_REDECLARATION);
  }


boost::optional< std::vector< contexted_value<std::string> >& > model_descriptor::get_urls() const
  {
    if(this->urls) return *this->urls; else return boost::none;
  }


bool model_descriptor::add_author(const std::string& n, const y::lexeme_type& l, author* a)
  {
    auto check = [&](auto name) -> auto
      {
        boost::optional<author_table::mapped_type::element_type&> rval;
    
        author_table::const_iterator t = this->authors.find(n);
        if(t != this->authors.cend()) rval = *t->second;
        
        return rval;
      };
    auto insert = [&](auto name, auto lexeme, auto attr) -> auto
      {
        // add declaration to list
        this->authors.emplace(std::make_pair(name, std::make_unique<author_declaration>(name, lexeme, attr)));
      };
    
    if(GenericInsertSymbol(check, insert, n, l, a, ERROR_AUTHOR_EXISTS, NOTIFY_DUPLICATE_AUTHOR_WAS)) return true;
    
    this->errors_encountered = true;
    return false;
  }


const author_table& model_descriptor::get_author() const
  {
    return this->authors;
  }


// TEMPLATE SPECIFICATION


bool model_descriptor::set_core(const std::string c, const y::lexeme_type& l)
  {
    return SetContextedValue(this->core, c, l, ERROR_CORE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_core() const
  {
    if(this->core) return(*this->core); else return(boost::none);
  }


bool model_descriptor::set_implementation(const std::string i, const y::lexeme_type& l)
  {
    return SetContextedValue(this->implementation, i, l, ERROR_IMPLEMENTATION_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_implementation() const
  {
    if(this->implementation) return(*this->implementation); else return(boost::none);
  }


bool model_descriptor::set_model(const std::string m, const y::lexeme_type& l)
  {
    return SetContextedValue(this->model, m, l, ERROR_MODEL_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > model_descriptor::get_model() const
  {
    if(this->model) return(*this->model); else return(boost::none);
  }


bool model_descriptor::set_background_stepper(stepper* s, const y::lexeme_type& l)
  {
    return SetContextedValue(this->background_stepper, *s, l, ERROR_BACKGROUND_REDECLARATION);
  }


bool model_descriptor::set_perturbations_stepper(stepper *s, const y::lexeme_type& l)
  {
    return SetContextedValue(this->perturbations_stepper, *s, l, ERROR_PERTURBATIONS_REDECLARATION);
  }


boost::optional< contexted_value<stepper>& > model_descriptor::get_background_stepper() const
  {
    if(this->background_stepper) return *this->background_stepper; else return boost::none;
  }


boost::optional< contexted_value<stepper>& > model_descriptor::get_perturbations_stepper() const
  {
    if(this->perturbations_stepper) return *this->perturbations_stepper; else return boost::none;
  }


// MISCELLANEOUS SETTINGS


void model_descriptor::set_indexorder(enum index_order o)
  {
    this->order = o;
  }


enum index_order model_descriptor::get_indexorder() const
  {
    return(this->order);
  }


// LAGRANGIAN MANAGEMENT


bool model_descriptor::set_potential(GiNaC::ex V, const y::lexeme_type& l)
  {
    return SetContextedValue(this->potential, V, l, ERROR_POTENTIAL_REDECLARATION);
  }


boost::optional< contexted_value<GiNaC::ex>& > model_descriptor::get_potential() const
  {
    if(this->potential) return *this->potential; else return boost::none;
  }


void model_descriptor::unset_potential()
  {
    this->potential.release();
  }


// FIELDS AND PARAMETERS


unsigned int model_descriptor::get_number_fields() const
  {
    return(static_cast<unsigned int>(this->fields.size()));
  }


unsigned int model_descriptor::get_number_params() const
  {
    return(static_cast<unsigned int>(this->parameters.size()));
  }


std::vector<std::string> model_descriptor::get_field_name_list() const
  {
    auto Get = [&](const field_symbol_table::mapped_type::element_type& v) -> auto { return v.get_name(); };
    return UnzipSortedZipList(this->fields, Get);
  }


std::vector<std::string> model_descriptor::get_field_latex_list() const
  {
    auto Get = [&](const field_symbol_table::mapped_type::element_type& v) -> auto { return v.get_latex_name(); };
    return UnzipSortedZipList(this->fields, Get);
  }


std::vector<std::string> model_descriptor::get_param_name_list() const
  {
    auto Get = [&](const parameter_symbol_table::mapped_type::element_type& v) -> auto { return v.get_name(); };
    return UnzipSortedZipList(this->parameters, Get);
  }


std::vector<std::string> model_descriptor::get_param_latex_list() const
  {
    auto Get = [&](const parameter_symbol_table::mapped_type::element_type& v) -> auto { return v.get_latex_name(); };
    return UnzipSortedZipList(this->parameters, Get);
  }


symbol_list model_descriptor::get_field_symbols() const
  {
    auto Get = [&](const field_symbol_table::mapped_type::element_type& v) -> auto { return v.get_ginac_symbol(); };
    return UnzipSortedZipList(this->fields, Get);
  }


symbol_list model_descriptor::get_deriv_symbols() const
  {
    return(this->deriv_symbols);
  }


symbol_list model_descriptor::get_param_symbols() const
  {
    auto Get = [&](const parameter_symbol_table::mapped_type::element_type& v) -> auto { return v.get_ginac_symbol(); };
    return UnzipSortedZipList(this->parameters, Get);
  }


const GiNaC::symbol& model_descriptor::get_Mp_symbol() const
  {
    return(this->M_Planck);
  }


// WRITE SELF TO STREAM


void model_descriptor::print(std::ostream& stream) const
  {
    stream << "Script summary:" << '\n';
    stream << "===============" << '\n';
    if(this->name)           stream << "  Name           = '" << static_cast<std::string>(*this->name) << "'" << '\n';
    if(this->model)          stream << "  Model          = '" << static_cast<std::string>(*this->model) << "'" << '\n';

    if(this->citeguide)      stream << "  Citeguide      = '" << static_cast<std::string>(*this->citeguide) << "'" << '\n';
    if(this->description)    stream << "  Description    = '" << static_cast<std::string>(*this->description) << "'" << '\n';
    if(this->license)        stream << "  License        = '" << static_cast<std::string>(*this->license) << "'" << '\n';
    if(this->revision)       stream << "  Revision       = '" << static_cast<unsigned int>(*this->revision) << "'" << '\n';

    if(!this->authors.empty())
      {
        stream << "  Authors:" << '\n';
        for(const author_table::value_type& item : this->authors)
          {
            const author_declaration& decl = *item.second;
            stream << "    " << decl.get_name() << ": <" << decl.get_email() << "> " << decl.get_institute() << '\n';
          }
      }

    if(this->references)
      {
        stream << "  References:" << '\n';
        for(const contexted_value<std::string>& v : *this->references)
          {
            stream << "    '" << static_cast<std::string>(v) << "'" << '\n';
          }
      }

    if(this->urls)
      {
        stream << "  URLs:" << '\n';
        for(const contexted_value<std::string>& v : *this->urls)
          {
            stream << "    '" << static_cast<std::string>(v) << "'" << '\n';
          }
      }

    if(this->core)           stream << "  Core           = '" << static_cast<std::string>(*this->core) << "'" << '\n';
    if(this->implementation) stream << "  Implementation = '" << static_cast<std::string>(*this->implementation) << "'" << '\n';
    stream << '\n';

    stream << "Fields:" << '\n';
    stream << "=======" << '\n';
    for(auto ptr = this->fields.cbegin(); ptr != this->fields.cend(); ++ptr)
      {
        ptr->second->print(stream);
      }
    stream << '\n';

    stream << "Parameters:" << '\n';
    stream << "===========" << '\n';
    for(auto ptr = this->parameters.cbegin(); ptr != this->parameters.cend(); ++ptr)
      {
        ptr->second->print(stream);
      }
    stream << '\n';

		stream << "Subexpressions:" << '\n';
		stream << "===============" << '\n';
		for(auto ptr = this->subexprs.cbegin(); ptr != this->subexprs.cend(); ++ptr)
			{
		    ptr->second->print(stream);
			}

    if(this->potential)
      {
        GiNaC::ex V = *this->potential;
        stream << "** Potential = " << V << '\n';
      }
    else
      {
        stream << "Potential unset" << '\n';
      }
    stream << '\n';
  }
