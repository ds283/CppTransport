//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <sstream>
#include <assert.h>

#include "parse_tree.h"

#define DEFAULT_MODEL_NAME "inflationary_model"
#define DERIV_PREFIX       "__d"

// ******************************************************************


declaration::declaration(const quantity& o, const filestack* p)
  : path(p)
  {
    assert(path != nullptr);
    this->obj = new quantity(o);
  }


const quantity* declaration::get_quantity() const
  {
    return(this->obj);
  }


field_declaration::field_declaration(const quantity& o, const filestack* p)
  : declaration(o, p)
  {
  }


field_declaration::~field_declaration()
  {
    delete this->obj;

    return;
  }


void field_declaration::print(std::ostream& stream) const
  {
    stream << "Field declaration for symbol '" << this->obj->get_name()
           << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'" << std::endl;

    stream << "  defined at line " << this->path->write();
  }


parameter_declaration::parameter_declaration(const quantity& o, const filestack* p)
  : declaration(o, p)
  {
  }


parameter_declaration::~parameter_declaration()
  {
    delete this->obj;

    return;
  }


void parameter_declaration::print(std::ostream& stream) const
  {
    stream << "Parameter declaration for symbol '" << this->obj->get_name()
      << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'" << std::endl;

    stream << "  defined at line " << this->path->write();
  }


// ******************************************************************


script::script()
  : potential_set(false), potential(nullptr), model(DEFAULT_MODEL_NAME), M_Planck(MPLANCK_SYMBOL, MPLANCK_LATEX_SYMBOL), order(indexorder_right)
  {
    this->table = new symbol_table<const quantity>(SYMBOL_TABLE_SIZE);

    // insert M_Planck symbol into the symbol table
    attributes attrs;
    attrs.set_latex(MPLANCK_LATEX_SYMBOL);

    quantity Mp(MPLANCK_TEXT_NAME, attrs, M_Planck);

    this->table->insert(&Mp);

    // set up default values for the steppers
    this->background_stepper.abserr    = DEFAULT_ABS_ERR;
    this->background_stepper.relerr    = DEFAULT_REL_ERR;
    this->background_stepper.name      = DEFAULT_STEPPER;
    this->perturbations_stepper.abserr = DEFAULT_ABS_ERR;
    this->perturbations_stepper.relerr = DEFAULT_REL_ERR;
    this->perturbations_stepper.name   = DEFAULT_STEPPER;
  }

script::~script()
  {
    // the only record of the various declarations which have been
    // set up is via our list, so we must delete them:
    for(int i = 0; i < this->fields.size(); i++)
      {
        delete this->fields[i];
      }
    for(int i = 0; i < this->parameters.size(); i++)
      {
        delete this->parameters[i];
      }

    // delete symbol table
    delete this->table;

    // delete potential
    if(this->potential_set)
      {
        assert(this->potential != nullptr);
        if(this->potential != nullptr)
          {
            delete this->potential;
          }
      }
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
    stream << "Script summary:" << std::endl;
    stream << "===============" << std::endl;
    stream << "  Name           = '" << this->name << "'" << std::endl;
    stream << "  Model          = '" << this->model << "'" << std::endl;
    stream << "  Author         = '" << this->author << "'" << std::endl;
    stream << "  Tag            = '" << this->tag << "'" << std::endl;
    stream << "  Core           = '" << this->core << "'" << std::endl;
    stream << "  Implementation = '" << this->implementation << "'" << std::endl;
    stream << std::endl;

    stream << "Fields:" << std::endl;
    stream << "=======" << std::endl;
    for(std::deque<field_declaration*>::const_iterator ptr = this->fields.begin();
        ptr != this->fields.end(); ptr++)
      {
        (*ptr)->print(stream);
      }
    stream << std::endl;

    stream << "Parameters:" << std::endl;
    stream << "===========" << std::endl;
    for(std::deque<parameter_declaration*>::const_iterator ptr = this->parameters.begin();
        ptr != this->parameters.end(); ptr++)
      {
        (*ptr)->print(stream);
      }
    stream << std::endl;

    stream << "Symbol table:" << std::endl;
    stream << "=============" << std::endl;
    this->table->print(stream);
    stream << std::endl;

    if(this->potential_set)
      {
        assert(this->potential != nullptr);
        stream << "** Potential = " << *this->potential << std::endl;
      }
    else
      {
        stream << "Potential unset" << std::endl;
      }
    stream << std::endl;
  }


bool script::add_field(field_declaration* d)
  {
    // search for an existing entry in the symbol table
    const quantity* p;
    bool            exists = this->table->find(d->get_quantity()->get_name(), p);

    if(exists)
      {
        std::ostringstream msg;

        msg << ERROR_SYMBOL_EXISTS << " '" << d->get_quantity()->get_name() << "'";
        error(msg.str());
      }
    else
      {
        // insert symbol in symbol table
        this->table->insert(d->get_quantity());

        // add declaration to list
        this->fields.push_back(d);

        // also need to generate a symbol for the momentum corresponding to this field
        GiNaC::symbol deriv_symbol(DERIV_PREFIX + d->get_quantity()->get_ginac_symbol()->get_name());
        this->deriv_symbols.push_back(deriv_symbol);
      }

    return(!exists);  // caller must delete d explicitly if returns false
  }


bool script::add_parameter(parameter_declaration* d)
  {
    // search for an existing entry in the symbol table
    const quantity* p;
    bool            exists = this->table->find(d->get_quantity()->get_name(), p);

    if(exists)
      {
        std::ostringstream msg;

        msg << ERROR_SYMBOL_EXISTS << " '" << d->get_quantity()->get_name() << "'";
        error(msg.str());
      }
    else
      {
        // insert symbol in symbol table
        this->table->insert(d->get_quantity());

        // add declaration to list
        this->parameters.push_back(d);
      }

    return(!exists);  // caller must delete d explicitly if returns false
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


bool script::lookup_symbol(std::string id, const quantity*& s) const
  {
    return(this->table->find(id, s));
  }


unsigned int script::get_number_fields() const
  {
    return(this->fields.size());
  }


unsigned int script::get_number_params() const
  {
    return(this->parameters.size());
  }


std::vector<std::string> script::get_field_list() const
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(this->fields[i]->get_quantity()->get_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_latex_list() const
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(this->fields[i]->get_quantity()->get_latex_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_param_list() const
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(this->parameters[i]->get_quantity()->get_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_platx_list() const
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(this->parameters[i]->get_quantity()->get_latex_name());
      }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_field_symbols() const
  {
    std::vector<GiNaC::symbol> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(*(this->fields[i]->get_quantity()->get_ginac_symbol()));
      }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_deriv_symbols() const
  {
    return(this->deriv_symbols);
  }


std::vector<GiNaC::symbol> script::get_param_symbols() const
  {
    std::vector<GiNaC::symbol> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(*(this->parameters[i]->get_quantity()->get_ginac_symbol()));
      }

    return(rval);
  }


const GiNaC::symbol& script::get_Mp_symbol() const
  {
    return(this->M_Planck);
  }


void script::set_potential(GiNaC::ex* V)
  {
    if(this->potential_set)
      {
        assert(this->potential != nullptr);
        delete this->potential;
      }
    this->potential     = V;
    this->potential_set = true;

//    std::cerr << "Set potential to be V = " << *this->potential << std::endl;
  }


GiNaC::ex script::get_potential() const
  {
    GiNaC::ex V = GiNaC::numeric(0);    // returned in case no potential has been set

    if(this->potential_set)
      {
        V = *this->potential;
      }

    return(V);
  }


void script::unset_potential()
  {
    if(this->potential_set)
      {
        assert(this->potential != nullptr);
        delete this->potential;
      }
    this->potential     = nullptr;
    this->potential_set = false;
  }
