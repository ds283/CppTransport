//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <sstream>

#include "parse_tree.h"


// ******************************************************************


declaration::declaration(const quantity& o, unsigned int l, const std::deque<struct inclusion>& p)
  : line(l), path(p)
  {
    this->obj = new quantity(o);
  }


quantity* declaration::get_quantity()
  {
    return(this->obj);
  }


field_declaration::field_declaration(const quantity& o, unsigned int l, const std::deque<struct inclusion>& p)
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
           << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'\n";

    stream << "  defined at line " << line;
    if(this->path.size() >= 1)
      {
        stream << " of '" << path[0].name << "'";
      }
    stream << "\n";

    for(int i = 1; i < this->path.size(); i++)
      {
        stream << "  included from line " << this->path[i].line << " of file '" << this->path[i].name << "'\n";
      }
  }

parameter_declaration::parameter_declaration(const quantity& o, unsigned int l, const std::deque<struct inclusion>& p)
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
      << "', GiNaC symbol '" << *this->obj->get_ginac_symbol() << "'\n";

    stream << "  defined at line " << line;
    if(this->path.size() >= 1)
      {
        stream << " of '" << path[0].name << "'";
      }
    stream << "\n";

    for(int i = 1; i < this->path.size(); i++)
      {
        stream << "  included from line " << this->path[i].line << " of file '" << this->path[i].name << "'\n";
      }
  }


// ******************************************************************


script::script()
  : potential_set(false), potential(NULL)
  {
    this->table = new symbol_table<quantity>(SYMBOL_TABLE_SIZE);
  }

script::~script()
  {
    // the only record of the various declarations which have been
    // set up is via our list, so we must delete them:
    for(int i = 0; i < this->decls.size(); i++)
      {
        delete this->decls[i];
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


const std::string script::get_name()
  {
    return(this->name);
  }


void script::set_author(const std::string a)
  {
    this->author = a;
  }


const std::string script::get_author()
  {
    return(this->author);
  }


void script::set_tag(const std::string t)
  {
    this->tag = t;
  }


void script::print(std::ostream& stream)
  {
    stream << "Script summary:\n";
    stream << "===============\n";
    stream << "  Name   = '" << this->name << "'\n";
    stream << "  Author = '" << this->author << "'\n";
    stream << "  Tag    = '" << this->tag << "'\n";

    stream << "\nDeclarations:\n";
    stream <<   "=============\n";
    for(std::deque<declaration*>::iterator ptr = this->decls.begin();
        ptr != this->decls.end(); ptr++)
      {
        (*ptr)->print(stream);
      }

    stream << "\nSymbol table:\n";
    stream <<   "=============\n";
    this->table->print(stream);

    if(this->potential_set)
      {
        assert(this->potential != NULL);
        stream << "** Potential = " << *this->potential << "\n";
      }
    else
      {
        stream << "Potential unset\n";
      }
  }


const std::string script::get_tag()
  {
    return(this->tag);
  }


bool script::add_declaration(declaration* d)
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
        this->decls.push_back(d);
      }

    return(!exists);  // caller must delete d explicitly if returns false
  }

bool script::lookup_symbol(std::string id, quantity *& s)
  {
    return(this->table->find(id, s));
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

    // std::cerr << "Set potential to be V = " << *this->potential << "\n";
  }


bool script::get_potential(GiNaC::ex*& V)
  {
    if(this->potential_set)
      {
        V = this->potential;
      }

    return(this->potential_set);
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
