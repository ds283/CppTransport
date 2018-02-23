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


void cse::deposit(cse_impl::symbol_record& record)
  {
    if(!record.is_written())
      {
        this->decls.emplace_back(record.get_symbol(), record.get_target());
        record.mark_written();
      }
  }


void cse::parse(const GiNaC::ex& expr, const boost::optional<std::string> name)
  {
    // no effect if CSE is disabled
    if(!this->data_payload.do_cse()) return;

    // enable parsing timer
    timing_instrument instrument(timer);

    // loop over all subexpressions in the tree, beginning with the leaves and building up to
    // more complex expressions
    for(auto t = expr.postorder_begin(); t != expr.postorder_end(); ++t)
      {
        // print this subexpression *without* use counting
        // (false means that print will use get_symbol_without_use_count)
        std::string target_string = this->print(*t, false);

        // does this subexpression already exist in the lookup table?
        auto u = this->symbols.find(target_string);

        // if not, insert a new subexpression

        // if a name was supplied, we automatically deposit everything to the pool, because typically clients
        // further up the stack will only get a GiNaC symbol corresponding to this name;
        // they won't have an explicit expression to print which would cause these temporaries to be deposited

        // work out whether we are looking at the total expression, or just one of the component subexpressions
        auto w = t;
        ++w;
        bool last_subexpr = (w == expr.postorder_end());

        if(u == this->symbols.end())
          {
            // Assign a name for this subexpression.
            // If it is the top-level expression and a name was supplied then we use it,
            // otherwise we set up a label for a new temporary object
            std::string symbol_name{name && last_subexpr ? *name : this->make_symbol()};

            // perform insertion
            auto result = this->symbols.emplace(target_string, cse_impl::symbol_record{target_string, symbol_name});

            // check whether insertion took place; failure could be due to aliasing, so take no risks and
            // raise an exception
            if(!result.second) throw cse_exception(*name);

            if(name) deposit(result.first->second);
          }
        else
          {
            // otherwise, no need to insert, but we should check whether the write flag needs to be set
            // and we should also check
            if(name)
              {
                cse_impl::symbol_record& rec = u->second;
                deposit(rec);

                // if this is the total subexpression, check whether the record we've matched has the assigned name
                if(last_subexpr && rec.get_symbol() != *name)
                  {
                    // check whether we've emitted a declaration for this named symbol before
                    auto v = this->named_symbols.find(*name);

                    if(v == this->named_symbols.end())
                      {
                        // if not, then assign the temporary we've matched to the intended name

                        // *don't* inject into the symbol table, though, otherwise we'll end up with *two*
                        // entries that match the same subexpression -- this is inconsistent
                        this->decls.emplace_back(*name, rec.get_symbol());
                        this->named_symbols.insert(*name);
                      }
                  }
              }
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
        this->decls.emplace_back(t->second.get_symbol(), t->second.get_target());
        t->second.mark_written();
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
