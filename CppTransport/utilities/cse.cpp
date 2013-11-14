//
// Created by David Seery on 13/11/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//

// based on GinacPrint by Doug Baker
// original copyright notice:

//***************************************************************************
//* @file GinacPrint.cc
//*
// * Source file for a class that prints GiNaC expressions in a form that
//* is efficient to compile and efficient to run.  The main work this
//* class does is identifying shared sub-expressions to generate code
//* that only computes them once.
//*
//* See GinacPrint.hh for details on usage.
//*
//* Copyright 2010
//*
//* National Robotics Engineering Center, Carnegie Mellon University
//* 10 40th Street, Pittsburgh, PA 15201
//* www.rec.ri.cmu.edu
//*
//* This program is free software; you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation; either version 2 of the License, or
//* (at your option) any later version.
//*
//* This program is distributed in the hope that it will be useful, but
//* WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//* General Public License for more details.
//*
//* This notice must appear in all copies of this file and its derivatives.
//*
//* History of Significant Contributions (don't put commit logs here):
//* 2010-05-20 Doug Baker, cape1232@yahoo.com: Created.
//*
//***************************************************************************


#include <iostream>
#include <sstream>
#include <map>
#include <utility>

#include "msg_en.h"

#include "error.h"
#include "cse.h"


// **********************************************************************

void cse::clear()
  {
    this->symbols.clear();
    this->decls.clear();

    this->serial_number++;
  }

// **********************************************************************

void cse::parse(const GiNaC::ex& expr)
  {
    for(GiNaC::const_postorder_iterator it = expr.postorder_begin(); it != expr.postorder_end(); it++)
      {
        std::string e = this->print(*it);

        if(this->symbols[e] == "")
          {
            std::string sym = this->make_symbol();
            this->symbols[e] = sym;
            this->decls.push_back(std::make_pair(sym, e));
          }
      }
  }

std::string cse::temporaries(const std::string& t)
  {
    std::ostringstream out;
    bool ok = true;

    size_t lhs_pos;
    size_t rhs_pos;

    if((lhs_pos = t.find("$1")) == std::string::npos)
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_LHS << " '" << t << "'";
        error(msg.str());
        ok = false;
      }
    if((rhs_pos = t.find("$2")) == std::string::npos)
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_RHS << " '" << t << "'";
        error(msg.str());
        ok = false;
      }

    if(ok)
      {
        for(size_t i = 0; i < this->decls.size(); i++)
          {
            std::string line = t;

            line.replace(lhs_pos, (size_t)2, this->decls[i].first);
            line.replace(rhs_pos, (size_t)2, this->decls[i].second);
            out << line << std::endl;
          }
      }
    else
      {
        out << t << std::endl;
      }

    return(out.str());
  }

// **********************************************************************

std::string cse::symbol(const GiNaC::ex& expr)
  {
    std::string e = this->print(expr);
    return(this->symbols[e]);
  }

std::string cse::make_symbol()
  {
    std::ostringstream s;

    s << this->kernel_name << "_" << serial_number << "_" << symbol_counter;
    symbol_counter++;

    return(s.str());
  }

// **********************************************************************

std::string cse::print(const GiNaC::ex& expr)
  {
    std::ostringstream out;
    std::string        name;

    if(GiNaC::is_a<GiNaC::function>(expr)) name = GiNaC::ex_to<GiNaC::function>(expr).get_name();
    else                                   name = GiNaC::ex_to<GiNaC::basic>(expr).class_name();

    if     (name == "numeric") out << expr;
    else if(name == "symbol")  out << expr;
    else if(name == "add")     out << this->print_operands(expr, "+");
    else if(name == "mul")     out << this->print_operands(expr, "*");
    else if(name == "power")   out << "pow(" << this->print_operands(expr, ",") << ")";
    else                       out << name << "(" << this->print_operands(expr, ",") << ")";

    return(out.str());
  }

std::string cse::print_operands(const GiNaC::ex& expr, std::string op)
  {
    std::ostringstream out;
    size_t             n = expr.nops();

    if(n > 0)
      {
        for(size_t i = 0; i < n; i++)
          {
            out << (i > 0 ? op : "") << this->symbol(expr.op(i));
          }
      }
    else
      {
        out << expr;
      }

    return(out.str());
  }