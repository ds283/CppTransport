//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include "quantity.h"


// ******************************************************************


attributes::attributes()
  : latex_set(false), latex("")
  {
    return;
  }

attributes::~attributes()
  {
    return;
  }

bool attributes::get_latex(std::string& l)
  {
    if(this->latex_set)
      {
        l = this->latex;
      }
    return(this->latex_set);
  }

void attributes::set_latex(const std::string l)
  {
    this->latex_set = true;
    this->latex     = l;
  }

void attributes::unset_latex()
  {
    this->latex_set = false;
    this->latex     = "";
  }


// ******************************************************************


quantity::quantity(std::string n, unsigned int l, filestack* p)
  : name(n), line(l), path(p)
  {
    assert(path != nullptr);
    ginac_symbol = GiNaC::symbol(n);
  }

quantity::quantity(std::string n, attributes& a, unsigned int l, filestack* p)
  : name(n), attrs(a), line(l), path(p)
  {
    assert(path != nullptr);

    std::string latex_name;
    bool        latex_set = attrs.get_latex(latex_name);

    if(latex_set)
      {
        ginac_symbol = GiNaC::symbol(n, latex_name);
      }
    else
      {
        ginac_symbol = GiNaC::symbol(n);
      }
  }

quantity::quantity(std::string n, attributes&a, GiNaC::symbol&s)
  : name(n), attrs(a), line(0), ginac_symbol(s)
  {
  }

quantity::~quantity()
  {
  }

void quantity::print(std::ostream& stream)
  {
    stream << "Quantity '" << this->name << "'" << std::endl;

    stream << "  GiNaC symbol = '" << this->ginac_symbol << "'" << std::endl;

    std::string latex_name;
    bool        latex_set = this->attrs.get_latex(latex_name);

    if(latex_set)
      {
        stream << "  LaTeX name = '" << latex_name << "'" << std::endl;
      }
  }

unsigned int quantity::get_line()
  {
    return(this->line);
  }

filestack* quantity::get_path()
  {
    return(this->path);
  }

const std::string quantity::get_name()
  {
    return(this->name);
  }

const std::string quantity::get_latex_name()
  {
    std::string latex_name;
    bool        ok = this->attrs.get_latex(latex_name);

    if(!ok)
      {
        latex_name = this->name;
      }

    return(latex_name);
  }

const GiNaC::symbol* quantity::get_ginac_symbol()
  {
    return(&this->ginac_symbol);
  }
