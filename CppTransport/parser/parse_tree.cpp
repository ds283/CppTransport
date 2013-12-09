//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <sstream>

#include "parse_tree.h"

#define DEFAULT_MODEL_NAME "inflationary_model"
#define DERIV_PREFIX       "__d"

// ******************************************************************


declaration::declaration(const quantity& o, unsigned int l, filestack& p)
  : line(l), path(p)
  {
    this->obj = new quantity(o);
  }


quantity* declaration::get_quantity()
  {
    return(this->obj);
  }


field_declaration::field_declaration(const quantity& o, unsigned int l, filestack& p)
  : declaration(o, l, p)
  {
  }


field_declaration::~field_declaration()
  {
    delete this->obj;

    return;
  }


void field_declaration::print(std::ostream& stream)
  {
    stream << "Field declaration for symbol '" << this->obj->get_name()
           << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'" << std::endl;

    stream << "  defined at line " << line;
    if(this->path.size() >= 1)
      {
        stream << " of '" << path[0].name << "'";
      }
    stream << std::endl;

    for(int i = 1; i < this->path.size(); i++)
      {
        stream << "  included from line " << this->path[i].line << " of file '" << this->path[i].name << "'" << std::endl;
      }
  }


parameter_declaration::parameter_declaration(const quantity& o, unsigned int l, filestack& p)
  : declaration(o, l, p)
  {
  }


parameter_declaration::~parameter_declaration()
  {
    delete this->obj;

    return;
  }


void parameter_declaration::print(std::ostream& stream)
  {
    stream << "Parameter declaration for symbol '" << this->obj->get_name()
      << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'" << std::endl;

    stream << "  defined at line " << line;
    if(this->path.size() >= 1)
      {
        stream << " of '" << path[0].name << "'";
      }
    stream << std::endl;

    for(int i = 1; i < this->path.size(); i++)
      {
        stream << "  included from line " << this->path[i].line << " of file '" << this->path[i].name << "'" << std::endl;
      }
  }


// ******************************************************************


script::script()
  : potential_set(false), potential(NULL), model(DEFAULT_MODEL_NAME), M_Planck(MPLANCK_SYMBOL, MPLANCK_LATEX_SYMBOL), order(indexorder_right)
  {
    this->table = new symbol_table<quantity>(SYMBOL_TABLE_SIZE);

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
        assert(this->potential != NULL);
        if(this->potential != NULL)
          {
            delete this->potential;
          }
      }
  }


void script::set_name(const std::string n)
  {
    this->name = n;
  }


const std::string& script::get_name()
  {
    return(this->name);
  }


void script::set_author(const std::string a)
  {
    this->author = a;
  }


const std::string& script::get_author()
  {
    return(this->author);
  }


void script::set_tag(const std::string t)
  {
    this->tag = t;
  }


const std::string& script::get_tag()
  {
    return(this->tag);
  }


void script::set_core(const std::string c)
  {
    this->core = c;
  }


const std::string& script::get_core()
  {
    return(this->core);
  }


void script::set_implementation(const std::string i)
  {
    this->implementation = i;
  }


const std::string& script::get_implementation()
  {
    return(this->implementation);
  }


void script::set_model(const std::string m)
  {
    this->model = m;
  }


const std::string& script::get_model()
  {
    return(this->model);
  }


void script::set_indexorder(enum indexorder o)
  {
    this->order = o;
  }


enum indexorder script::get_indexorder()
  {
    return(this->order);
  }


void script::print(std::ostream& stream)
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
    for(std::deque<field_declaration*>::iterator ptr = this->fields.begin();
        ptr != this->fields.end(); ptr++)
      {
        (*ptr)->print(stream);
      }
    stream << std::endl;

    stream << "Parameters:" << std::endl;
    stream << "===========" << std::endl;
    for(std::deque<parameter_declaration*>::iterator ptr = this->parameters.begin();
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
        assert(this->potential != NULL);
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
    quantity* p;
    bool      exists = this->table->find(d->get_quantity()->get_name(), p);

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
    quantity* p;
    bool      exists = this->table->find(d->get_quantity()->get_name(), p);

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


const struct stepper& script::get_background_stepper()
  {
    return(this->background_stepper);
  }


const struct stepper& script::get_perturbations_stepper()
  {
    return(this->perturbations_stepper);
  }


bool script::lookup_symbol(std::string id, quantity *& s)
  {
    return(this->table->find(id, s));
  }


unsigned int script::get_number_fields()
  {
    return(this->fields.size());
  }


unsigned int script::get_number_params()
  {
    return(this->parameters.size());
  }


std::vector<std::string> script::get_field_list()
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(this->fields[i]->get_quantity()->get_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_latex_list()
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(this->fields[i]->get_quantity()->get_latex_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_param_list()
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(this->parameters[i]->get_quantity()->get_name());
      }

    return(rval);
  }


std::vector<std::string> script::get_platx_list()
  {
    std::vector<std::string> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(this->parameters[i]->get_quantity()->get_latex_name());
      }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_field_symbols()
  {
    std::vector<GiNaC::symbol> rval;

    for(int i = 0; i < this->fields.size(); i++)
      {
        rval.push_back(*(this->fields[i]->get_quantity()->get_ginac_symbol()));
      }

    return(rval);
  }


std::vector<GiNaC::symbol> script::get_deriv_symbols()
  {
    return(this->deriv_symbols);
  }


std::vector<GiNaC::symbol> script::get_param_symbols()
  {
    std::vector<GiNaC::symbol> rval;

    for(int i = 0; i < this->parameters.size(); i++)
      {
        rval.push_back(*(this->parameters[i]->get_quantity()->get_ginac_symbol()));
      }

    return(rval);
  }


const GiNaC::symbol& script::get_Mp_symbol()
  {
    return(this->M_Planck);
  }


void script::set_potential(GiNaC::ex* V)
  {
    if(this->potential_set)
      {
        assert(this->potential != NULL);
        delete this->potential;
      }
    this->potential     = V;
    this->potential_set = true;

    // std::cerr << "Set potential to be V = " << *this->potential << std::endl;
  }


GiNaC::ex script::get_potential()
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
        assert(this->potential != NULL);
        delete this->potential;
      }
    this->potential     = NULL;
    this->potential_set = false;
  }
