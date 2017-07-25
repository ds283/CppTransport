//
// Created by David Seery on 13/11/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// @contributor: Doug Baker <cape1232@yahoo.com>
// --@@
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
#include "timing_instrument.h"


void cse::clear()
  {
    this->symbols.clear();
    this->decls.clear();

    this->serial_number++;
    this->symbol_counter = 0;
  }


void cse::parse(const GiNaC::ex& expr, std::string name)
  {
    // no effect if CSE is disabled
    if(!this->data_payload.do_cse()) return;

    timing_instrument instrument(timer);

    // was a name supplied?
    // (eg. "name" might be __InternalHsq or __InternalEps, for when we are trying to simplify
    // expressions used in client code
    const bool use_name = !name.empty();
    
    // if a name was supplied, find iterator to entire expression
    // technically this is --expr.postorder_end(), however these iterators
    // are forward-directed only so we have to do a search
    GiNaC::const_postorder_iterator last;
    if(use_name)    // avoid performing possibly expensive search if no name supplied
      {
        for(auto t = expr.postorder_begin(); t != expr.postorder_end(); ++t) last = t;
      }

    for(auto t = expr.postorder_begin(); t != expr.postorder_end(); ++t)
      {
        // print this expression without use counting (false means that print will use get_symbol_without_use_count)
        std::string e = this->print(*t, false);

        // does this expression already exist in the lookup table?
        auto u = this->symbols.find(e);

        // if not, we should insert it
        if(u != this->symbols.end()) continue;
    
        // get a name for this symbol. If it is the top-level expression and a name was supplied then we use it,
        // otherwise we make a temporary
        std::string nm;
        if(use_name && t == last) nm = name;
        else nm = this->make_symbol();
    
        // perform insertion
        auto result = this->symbols.emplace(e, cse_impl::symbol_record{e, nm});
        
        // check whether insertion took place; failure could be due to aliasing
        if(!result.second) throw cse_exception(name);
    
        // if a name was supplied, we automatically deposit everything to the pool, because typically clients
        // further up the stack will only get a GiNaC symbol corresponding to this name;
        // they won't have an explicit expression to print which could cause these temporaries to be deposited
        cse_impl::symbol_record& record = result.first->second;
        if(use_name && !record.is_written())
          {
            this->decls.emplace_back(record.get_symbol(), record.get_target());
            record.set_written();
          }
      }
  }


std::unique_ptr< std::list<std::string> >
cse::temporaries(const std::string& left, const std::string& mid, const std::string& right) const
  {
    auto rval = std::make_unique< std::list<std::string> >();

    // deposit each declaration into the output stream
    for(const auto& decl: this->decls)
      {
        std::string temp = left;
        temp.append(decl.first);
        temp.append(mid);
        temp.append(decl.second);
        temp.append(right);
        temp.append("\n");

        rval->push_back(temp);
      }

    return(rval);
  }


std::string cse::get_symbol_without_use_count(const GiNaC::ex& expr)
  {
    // if CSE disabled, return raw expression
    if(!this->data_payload.do_cse()) return this->printer.ginac(expr);

    // print expression using ourselves as the lookup function (false means that print doesn't count uses via recursively calling ourselves)
    std::string e = this->print(expr, false);

    // search for this expression in the lookup table
    auto t = this->symbols.find(e);

    // was it present? if not, return the plain expression
    if(t == this->symbols.end()) return e;

    // otherwise, return whatever the expression resolves to
    return t->second.get_symbol();
  }


std::string cse::get_symbol_with_use_count(const GiNaC::ex& expr)
  {
    // if CSE disabled, return raw expression
    if(!this->data_payload.do_cse()) return this->printer.ginac(expr);

    // print expression using ourselves as the lookup function (true means that print will count uses via recursively calling ourselves)
    std::string e = this->print(expr, true);

    // search for this expression in the lookup table
    auto t = this->symbols.find(e);

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
    std::string s{this->temporary_name_kernel};
    
    s.append("_");
    s.append(std::to_string(serial_number));
    s.append("_");
    s.append(std::to_string(symbol_counter));

    ++symbol_counter;

    return s;
  }
