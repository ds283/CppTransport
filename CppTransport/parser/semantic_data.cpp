//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#include <assert.h>

#include "semantic_data.h"


// ******************************************************************


attributes::attributes()
  : latex_set(false), latex("")
  {
    return;
  }

const std::string& attributes::get_latex() const
  {
		return(this->latex);
  }

void attributes::set_latex(const std::string& l)
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


subexpr::subexpr()
	: latex_set(false), latex(""), value_set(false), value(0)
	{
		return;
	}


const std::string& subexpr::get_latex() const
	{
		return(this->latex);
	}


void subexpr::set_latex(const std::string& l)
	{
		this->latex = l;
		this->latex_set = true;
	}


void subexpr::unset_latex()
	{
		this->latex_set = false;
		this->latex.clear();
	}


GiNaC::ex subexpr::get_value() const
	{
		return(this->value);
	}


void subexpr::set_value(GiNaC::ex v)
	{
    this->value     = v;
    this->value_set = true;
	}


void subexpr::unset_value()
	{
    this->value_set = false;
    this->value     = 0;
	}
