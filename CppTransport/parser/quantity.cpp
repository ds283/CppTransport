//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


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


quantity::quantity(std::string n, unsigned int l, std::deque<struct inclusion>& p)
  : name(n), line(l), path(p)
  {
    ginac_symbol = GiNaC::symbol(n);
  }

quantity::quantity(std::string n, attributes& a, unsigned int l, std::deque<struct inclusion>& p)
  : name(n), attrs(a), line(l), path(p)
  {
    std::string latex_name;
    bool        latex_set = attrs.get_latex(latex_name);

    GiNaC::symbol phi("phi", "\\phi");

    if(latex_set)
      {
        ginac_symbol = GiNaC::symbol(n, latex_name);
      }
    else
      {
        ginac_symbol = GiNaC::symbol(n);
      }
  }

quantity::~quantity()
  {
    return;
  }

unsigned int quantity::get_line()
  {
    return(this->line);
  }

std::deque<struct inclusion>& quantity::get_path()
  {
    return(this->path);
  }

std::string quantity::get_name()
  {
    return(this->name);
  }

GiNaC::symbol* quantity::get_ginac_symbol()
  {
    return(&this->ginac_symbol);
  }
