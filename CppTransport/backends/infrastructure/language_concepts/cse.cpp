//
// Created by David Seery on 13/11/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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
#include <assert.h>
#include <functional>

#include "msg_en.h"

#include "cse.h"


void cse::clear()
  {
    this->symbols.clear();
    this->decls.clear();

    this->serial_number++;
    this->symbol_counter = 0;
  }


void cse::parse(const GiNaC::ex& expr)
  {
		timer.resume();

    for(GiNaC::const_postorder_iterator t = expr.postorder_begin(); t != expr.postorder_end(); ++t)
      {
        // print this expression without use counting (false means that print will use get_symbol_without_use_count)
        std::string e = this->print(*t, false);

        // does this expression already exist in the lookup table?
        symbol_table::iterator u = this->symbols.find(e);

        // if not, we should insert it
        if(u == this->symbols.end())
          {
            this->symbols.emplace(std::make_pair( e, cse_impl::symbol_record( e, this->make_symbol() ) ));
          }
      }

		timer.stop();
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

        error_context err_ctx(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        err_ctx.error(msg.str());
        ok = false;
      }
    if((rhs_pos = t.find("$2")) == std::string::npos)
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_RHS << " '" << t << "'";

        error_context err_ctx(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        err_ctx.error(msg.str());
        ok = false;
      }

    std::string left  = t.substr(0, lhs_pos);
    std::string mid   = t.substr(lhs_pos+2, rhs_pos-lhs_pos-2);
    std::string right = t.substr(rhs_pos+2, std::string::npos);

    if(ok)
      {
        // deposit each declaration into the output stream
        for(const std::pair<std::string, std::string>& decl: this->decls)
          {
            // replace LHS and RHS macros in the template
            out << left << decl.first << mid << decl.second << right << '\n';
          }
      }
    else
      {
        out << t << '\n';
      }

    return(out.str());
  }


std::string cse::get_symbol_without_use_count(const GiNaC::ex& expr)
  {
    // print expression using ourselves as the lookup function (false means that print doesn't count uses via recursively calling ourselves)
    std::string e = this->print(expr, false);

    // search for this expression in the lookup table
    symbol_table::iterator t = this->symbols.find(e);

    // was it present? if not, return the plain expression
    if(t == this->symbols.end()) return e;

    // otherwise, return whatever the expression resolves to
    return t->second.get_symbol();
  }


std::string cse::get_symbol_with_use_count(const GiNaC::ex& expr)
  {
    // print expression using ourselves as the lookup function (true means that print will count uses via recursively calling ourselves)
    std::string e = this->print(expr, true);

    // search for this expression in the lookup table
    symbol_table::iterator t = this->symbols.find(e);

    // was it present? if not, return the plain expression
    if(t == this->symbols.end()) return e;

    // if it was present, check whether this symbol has been written into the list
    // of declarations

    if(!t->second.is_written())
      {
        this->decls.push_back(std::make_pair(t->second.get_symbol(), t->second.get_target()));
        t->second.set_written();
      }

    return t->second.get_symbol();
  }


std::string cse::make_symbol()
  {
    std::ostringstream s;

    s << this->temporary_name_kernel << "_" << serial_number << "_" << symbol_counter;
    symbol_counter++;

    return(s.str());
  }
