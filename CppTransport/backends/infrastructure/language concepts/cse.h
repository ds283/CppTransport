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


#ifndef __cse_H_
#define __cse_H_

#include <string>
#include <map>
#include <utility>

#include "ginac/ginac.h"

#include "language_printer.h"
#include "msg_en.h"

// to be defined below; need a forward reference here
class cse;

// utility class to make using CSE easier
// it takes a vector of GiNaC expressions as input,
// and can be indexed in the same order to produce the equivalent CSE get_symbol
class cse_map
  {
  public:
    cse_map(std::vector<GiNaC::ex>* l, cse* c);
    ~cse_map();

    // not returning a reference disallows using [] as an lvalue
    std::string operator[](unsigned int index);

  protected:
    cse*                    cse_worker;

    std::vector<GiNaC::ex>* list;
  };


typedef std::function<std::string(const GiNaC::ex&)> symbol_f;


class cse
  {
  public:

    class symbol_record
      {
      public:
        symbol_record()
          : target(""), written(false), filled(false)
          {
          }

        std::string target;
        std::string symbol;
        bool        written;
        bool        filled;
      };

    cse(unsigned int s, language_printer& p, bool d=true, std::string k=OUTPUT_DEFAULT_CPP_KERNEL_NAME)
      : serial_number(s), printer(p), do_cse(d), kernel_name(k), symbol_counter(0)
      {
      }
    virtual ~cse()
      {
      }

    void               parse(const GiNaC::ex& expr);

    std::string        temporaries(const std::string& t);

    // two methods for getting the symbol corresponding to a GiNaC expression
    // get_symbol_no_tag() just returns the symbol and is used during the parsing phase
    // get_symbol_and_tag() marks each temporary as 'used', and injects it into the declarations
    // it is used when actually outputting symbols
    std::string        get_symbol_and_tag(const GiNaC::ex &expr);

    void               clear();

    const std::string& get_kernel_name()                      { return(this->kernel_name); }
    void               set_kernel_name(const std::string& k)  { this->kernel_name = k; }

    cse_map*           map_factory(std::vector<GiNaC::ex>* l) { return(new cse_map(l, this)); }

    bool               get_do_cse()                           { return(this->do_cse); }
    void               set_do_cse(bool d)                     { this->do_cse = d; }

    language_printer&  get_ginac_printer()                    { return(this->printer); }

  protected:
    language_printer& printer;
    bool              do_cse;

    // these functions are abstract and must be implemented by any derived classes
    // typically they will vary depending on the target language
    virtual std::string print           (const GiNaC::ex& expr, symbol_f symf) = 0;
    virtual std::string print_operands  (const GiNaC::ex& expr, std::string op, symbol_f symf) = 0;

    std::string        get_symbol_no_tag(const GiNaC::ex &expr);

    std::string        make_symbol      ();

    unsigned int serial_number;
    unsigned int symbol_counter;

    std::string  kernel_name;

    std::map<std::string, symbol_record>               symbols;
    std::vector< std::pair<std::string, std::string> > decls;
  };


#endif //__cse_H_
