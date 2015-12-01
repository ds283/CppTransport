//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <sstream>
#include <assert.h>
#include <algorithm>

#include "lexical.h"
#include "script.h"

#define DEFAULT_MODEL_NAME "inflationary_model"
#define DERIV_PREFIX       "__d"


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


std::vector<std::string>     fake_keyword_table;
std::vector<keyword_type>    fake_keyword_map;
std::vector<std::string>     fake_character_table;
std::vector<character_type>  fake_character_map;
std::vector<bool>            fake_context_table;


script::script(symbol_factory& s, error_context err_ctx)
  : potential_set(false),
    errors_encountered(false),
    model(DEFAULT_MODEL_NAME),
    order(indexorder::right),
		sym_factory(s)
  {
		// set up reserved symbol for Planck mass
    M_Planck = sym_factory.get_symbol(MPLANCK_SYMBOL, MPLANCK_LATEX_SYMBOL);

    // set up attributes for Planck mass symbol
		attributes Mp_attrs;
		Mp_attrs.set_latex(MPLANCK_LATEX_SYMBOL);

    // manufacture fake lexeme
    std::string MPlanck_buffer(MPLANCK_TEXT_NAME);
    lexeme::minus_context mctx = lexeme::minus_context::unary;
    fake_MPlanck_lexeme = std::make_unique<y::lexeme_type>(MPlanck_buffer, lexeme::buffer_type::string_literal, mctx,
                                                           0, err_ctx,
                                                           fake_keyword_table, fake_keyword_map, fake_character_table,
                                                           fake_character_map, fake_context_table);

    // emplace faked symbol table entry
		reserved.emplace(std::make_pair(MPLANCK_TEXT_NAME, std::make_unique<parameter_declaration>(MPLANCK_TEXT_NAME, M_Planck, *fake_MPlanck_lexeme, &Mp_attrs)));

    // set up default values for the steppers
    this->background_stepper.abserr    = DEFAULT_ABS_ERR;
    this->background_stepper.relerr    = DEFAULT_REL_ERR;
    this->background_stepper.name      = DEFAULT_STEPPER;

    this->perturbations_stepper.abserr = DEFAULT_ABS_ERR;
    this->perturbations_stepper.relerr = DEFAULT_REL_ERR;
    this->perturbations_stepper.name   = DEFAULT_STEPPER;
  }


boost::optional<declaration&> script::check_symbol_exists(const std::string& nm) const
	{
		// check user-defined symbols

    field_symbol_table::const_iterator f_it = this->fields.find(nm);
		if(f_it != this->fields.end()) return *f_it->second;

    parameter_symbol_table::const_iterator p_it = this->parameters.find(nm);
		if(p_it != this->parameters.end()) return *p_it->second;

		p_it = this->reserved.find(nm);
		if(p_it != this->reserved.end()) return *p_it->second;

    subexpr_symbol_table::const_iterator s_it = this->subexprs.find(nm);
		if(s_it != this->subexprs.end()) return *s_it->second;

		// didn't find anything
		return boost::optional<declaration&>();
	}


void script::set_name(const std::string n)
  {
    this->name = n;
  }


const std::string& script::get_name() const
  {
    return(this->name);
  }


void script::set_author(const std::string a)
  {
    this->author = a;
  }


const std::string& script::get_author() const
  {
    return(this->author);
  }


void script::set_tag(const std::string t)
  {
    this->tag = t;
  }


const std::string& script::get_tag() const
  {
    return(this->tag);
  }


void script::set_core(const std::string c)
  {
    this->core = c;
  }


const std::string& script::get_core() const
  {
    return(this->core);
  }


void script::set_implementation(const std::string i)
  {
    this->implementation = i;
  }


const std::string& script::get_implementation() const
  {
    return(this->implementation);
  }


void script::set_model(const std::string m)
  {
    this->model = m;
  }


const std::string& script::get_model() const
  {
    return(this->model);
  }


void script::set_indexorder(enum indexorder o)
  {
    this->order = o;
  }


enum indexorder script::get_indexorder() const
  {
    return(this->order);
  }


void script::print(std::ostream& stream) const
  {
    stream << "Script summary:" << '\n';
    stream << "===============" << '\n';
    stream << "  Name           = '" << this->name << "'" << '\n';
    stream << "  Model          = '" << this->model << "'" << '\n';
    stream << "  Author         = '" << this->author << "'" << '\n';
    stream << "  Tag            = '" << this->tag << "'" << '\n';
    stream << "  Core           = '" << this->core << "'" << '\n';
    stream << "  Implementation = '" << this->implementation << "'" << '\n';
    stream << '\n';

    stream << "Fields:" << '\n';
    stream << "=======" << '\n';
    for(field_symbol_table::const_iterator ptr = this->fields.begin(); ptr != this->fields.end(); ++ptr)
      {
        ptr->second->print(stream);
      }
    stream << '\n';

    stream << "Parameters:" << '\n';
    stream << "===========" << '\n';
    for(parameter_symbol_table::const_iterator ptr = this->parameters.begin(); ptr != this->parameters.end(); ++ptr)
      {
        ptr->second->print(stream);
      }
    stream << '\n';

		stream << "Subexpressions:" << '\n';
		stream << "===============" << '\n';
		for(subexpr_symbol_table::const_iterator ptr = this->subexprs.begin(); ptr != this->subexprs.end(); ++ptr)
			{
		    ptr->second->print(stream);
			}

    if(this->potential_set)
      {
        stream << "** Potential = " << this->potential << '\n';
      }
    else
      {
        stream << "Potential unset" << '\n';
      }
    stream << '\n';
  }


bool script::add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  {
    // search for an existing entry in the symbol table
    boost::optional<declaration&> record = this->check_symbol_exists(n);

    if(record)
      {
        std::ostringstream msg;
        msg << ERROR_SYMBOL_EXISTS << " '" << n << "'";
        l.error(msg.str());
        this->errors_encountered = true;
      }
    else
      {
        // add declaration to list
        this->fields.emplace(std::make_pair(n, std::make_unique<field_declaration>(n, s, l, a)));

        // also need to generate a symbol for the momentum corresponding to this field
        GiNaC::symbol deriv_symbol(DERIV_PREFIX + s.get_name());
        this->deriv_symbols.push_back(deriv_symbol);
      }

    return(!record);
  }


bool script::add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a)
  {
    // search for an existing entry in the symbol table
    boost::optional<declaration&> record = this->check_symbol_exists(n);

    if(record)
      {
        std::ostringstream msg;
        msg << ERROR_SYMBOL_EXISTS << " '" << n << "'";
        l.error(msg.str());
        this->errors_encountered = true;
      }
    else
      {
        // add declaration to list
        this->parameters.emplace(std::make_pair(n, std::make_unique<parameter_declaration>(n, s, l, a)));
      }

    return(!record);
  }


bool script::add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr* e)
	{
		// search for an existing entry in the symbol table
    boost::optional<declaration&> record = this->check_symbol_exists(n);

		if(record)
			{
		    std::ostringstream msg;
				msg << ERROR_SYMBOL_EXISTS << " '" << n << "'";
				l.error(msg.str());
        this->errors_encountered = true;
			}
		else
			{
				// add declaration to list
				this->subexprs.emplace(std::make_pair(n, std::make_unique<subexpr_declaration>(n, s, l, e)));
			}

		return(!record);
	}


void script::set_background_stepper(stepper*s)
  {
    this->background_stepper = *s;
  }


void script::set_perturbations_stepper(stepper *s)
  {
    this->perturbations_stepper = *s;
  }


const struct stepper& script::get_background_stepper() const
  {
    return(this->background_stepper);
  }


const struct stepper& script::get_perturbations_stepper() const
  {
    return(this->perturbations_stepper);
  }


unsigned int script::get_number_fields() const
  {
    return(static_cast<unsigned int>(this->fields.size()));
  }


unsigned int script::get_number_params() const
  {
    return(static_cast<unsigned int>(this->parameters.size()));
  }


namespace script_impl
	{
		typedef std::vector< std::pair<unsigned int, std::string> > zipped_string_list;

		struct zipped_string_sorter
			{
				bool operator()(const std::pair<unsigned int, std::string>& a, const std::pair<unsigned int, std::string>& b)
					{
						return(a.first < b.first);
					}
			};

    typedef std::vector< std::pair<unsigned int, GiNaC::symbol> > zipped_symbol_list;

    struct zipped_symbol_sorter
	    {
        bool operator()(const std::pair<unsigned int, GiNaC::symbol>& a, const std::pair<unsigned int, GiNaC::symbol>& b)
	        {
            return(a.first < b.first);
	        }
	    };
	}


std::vector<std::string> script::get_field_list() const
  {
    script_impl::zipped_string_list zipped_list;
    std::vector<std::string> rval;

		for(field_symbol_table::const_iterator t = this->fields.begin(); t != this->fields.end(); ++t)
      {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_name()));
      }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_string_sorter());

		for(script_impl::zipped_string_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
			{
				rval.push_back(t->second);
			}

    return(rval);
  }


std::vector<std::string> script::get_latex_list() const
  {
    script_impl::zipped_string_list zipped_list;
    std::vector<std::string> rval;

    for(field_symbol_table::const_iterator t = this->fields.begin(); t != this->fields.end(); ++t)
	    {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_latex_name()));
	    }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_string_sorter());

    for(script_impl::zipped_string_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
	    {
        rval.push_back(t->second);
	    }

    return(rval);
  }


std::vector<std::string> script::get_param_list() const
  {
    script_impl::zipped_string_list zipped_list;
    std::vector<std::string> rval;

    for(parameter_symbol_table::const_iterator t = this->parameters.begin(); t != this->parameters.end(); ++t)
	    {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_name()));
	    }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_string_sorter());

    for(script_impl::zipped_string_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
	    {
        rval.push_back(t->second);
	    }

    return(rval);
  }


std::vector<std::string> script::get_platx_list() const
  {
    script_impl::zipped_string_list zipped_list;
    std::vector<std::string> rval;

    for(parameter_symbol_table::const_iterator t = this->parameters.begin(); t != this->parameters.end(); ++t)
	    {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_latex_name()));
	    }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_string_sorter());

    for(script_impl::zipped_string_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
	    {
        rval.push_back(t->second);
	    }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_field_symbols() const
  {
    script_impl::zipped_symbol_list zipped_list;
    std::vector<GiNaC::symbol> rval;

    for(field_symbol_table::const_iterator t = this->fields.begin(); t != this->fields.end(); ++t)
	    {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_ginac_symbol()));
	    }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_symbol_sorter());

    for(script_impl::zipped_symbol_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
	    {
        rval.push_back(t->second);
	    }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_deriv_symbols() const
  {
    return(this->deriv_symbols);
  }


std::vector<GiNaC::symbol> script::get_param_symbols() const
  {
    script_impl::zipped_symbol_list zipped_list;
    std::vector<GiNaC::symbol> rval;

    for(parameter_symbol_table::const_iterator t = this->parameters.begin(); t != this->parameters.end(); ++t)
	    {
        zipped_list.push_back(std::make_pair(t->second->get_unique_id(), t->second->get_ginac_symbol()));
	    }

    std::sort(zipped_list.begin(), zipped_list.end(), script_impl::zipped_symbol_sorter());

    for(script_impl::zipped_symbol_list::const_iterator t = zipped_list.begin(); t != zipped_list.end(); ++t)
	    {
        rval.push_back(t->second);
	    }

    return(rval);
  }


const GiNaC::symbol& script::get_Mp_symbol() const
  {
    return(this->M_Planck);
  }


void script::set_potential(GiNaC::ex V)
  {
    this->potential     = V;
    this->potential_set = true;

//    std::cerr << "Set potential to be V = " << this->potential << '\n';
  }


GiNaC::ex script::get_potential() const
  {
    GiNaC::ex V = GiNaC::numeric(0);    // returned in case no potential has been set

    if(this->potential_set)
      {
        V = this->potential;
      }

    return(V);
  }


void script::unset_potential()
  {
    this->potential     = GiNaC::numeric(0);
    this->potential_set = false;
  }
