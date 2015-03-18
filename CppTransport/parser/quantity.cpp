//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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

const std::string& attributes::get_latex() const
  {
		return(this->latex);
  }

void attributes::set_latex(const std::string l)
  {
    this->latex_set = true;
    this->latex = l;
  }

void attributes::unset_latex()
  {
    this->latex_set = false;
    this->latex.clear();
  }


// ******************************************************************


quantity::quantity(std::string n, attributes& a, const filestack* p, GiNaC::symbol& s)
  : name(n), attrs(a), path(p), ginac_symbol(s)
  {
  }

quantity::~quantity()
  {
  }

void quantity::print(std::ostream& stream) const
  {
    stream << "Quantity '" << this->name << "'" << std::endl;

    stream << "  GiNaC symbol = '" << this->ginac_symbol << "'" << std::endl;

    std::string latex_name = this->attrs.get_latex();

    if(latex_name.length() > 0)
      {
        stream << "  LaTeX name = '" << latex_name << "'" << std::endl;
      }
  }

const filestack* quantity::get_path() const
  {
    return(this->path);
  }

const std::string quantity::get_name() const
  {
    return(this->name);
  }

const std::string quantity::get_latex_name() const
  {
    std::string latex_name = this->attrs.get_latex();
 
    if(latex_name.length() == 0) latex_name = this->name;

    return(latex_name);
  }

const GiNaC::symbol& quantity::get_ginac_symbol() const
  {
    return(this->ginac_symbol);
  }
