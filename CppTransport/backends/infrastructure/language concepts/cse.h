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

#include <boost/timer/timer.hpp>

// to be defined below; need a forward reference here
class cse;

// utility class to make using CSE easier
// it takes a vector of GiNaC expressions as input,
// and can be indexed in the same order to produce the equivalent CSE get_symbol
class cse_map
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    cse_map(std::vector<GiNaC::ex>* l, cse* c);
    ~cse_map();


		// INTERFACE

  public:

    // not returning a reference disallows using [] as an lvalue
    std::string operator[](unsigned int index);


		// INTERNAL DATA

  protected:

    cse* cse_worker;

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


		// CONSTRUCTOR, DESTRUCTOR

  public:

    cse(unsigned int s, language_printer& p, bool d=true, std::string k=OUTPUT_DEFAULT_CPP_CSE_TEMPORARY_NAME)
      : serial_number(s), printer(p), do_cse(d), temporary_name_kernel(k), symbol_counter(0)
      {
		    // pause timer
		    timer.stop();
      }

    virtual ~cse() = default;


		// INTERFACE - COMMON SUBEXPRESSION ELIMINATION

  public:

    void               parse(const GiNaC::ex& expr);

    std::string        temporaries(const std::string& t);

    // two methods for getting the symbol corresponding to a GiNaC expression
    // get_symbol_no_tag() just returns the symbol and is used during the parsing phase
    // get_symbol_and_tag() marks each temporary as 'used', and injects it into the declarations.
    // This method is used when actually outputting symbols
    std::string        get_symbol_and_tag(const GiNaC::ex &expr);

    void               clear();


		// INTERFACE - GET/SET NAME USED FOR TEMPORARIES

  public:

    const std::string& get_temporary_name() const                { return(this->temporary_name_kernel); }
    void               set_temporary_name(const std::string& k)  { this->temporary_name_kernel = k; }


		// INTERFACE - CSE MAPS

  public:

    cse_map*           map_factory(std::vector<GiNaC::ex>* l) { return(new cse_map(l, this)); }


		// INTERFACE - METADATA

  public:

    bool               get_do_cse()                           { return(this->do_cse); }
    void               set_do_cse(bool d)                     { this->do_cse = d; }

    language_printer&  get_ginac_printer()                    { return(this->printer); }


		// INTERFACE - STATISTICS

  public:

		boost::timer::nanosecond_type get_cse_time() const { return(this->timer.elapsed().wall); }


		// INTERNAL API

  protected:

    // these functions are abstract and must be implemented by any derived classes
    // typically they will vary depending on the target language
    virtual std::string print           (const GiNaC::ex& expr, symbol_f symf) = 0;
    virtual std::string print_operands  (const GiNaC::ex& expr, std::string op, symbol_f symf) = 0;

    std::string get_symbol_no_tag(const GiNaC::ex& expr);

    std::string make_symbol();


		// INTERNAL DATA

  protected:

    language_printer& printer;
    bool              do_cse;


    unsigned int serial_number;
    unsigned int symbol_counter;

    std::string temporary_name_kernel;

    std::map<std::string, symbol_record>              symbols;
    std::vector<std::pair<std::string, std::string> > decls;

		// timer
		boost::timer::cpu_timer timer;

  };


#endif //__cse_H_
