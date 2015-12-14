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


#ifndef CPPTRANSPORT_CSE_H
#define CPPTRANSPORT_CSE_H

#include <string>
#include <unordered_map>
#include <utility>

#include "ginac/ginac.h"

#include "language_printer.h"
#include "translator_data.h"
#include "msg_en.h"

#include "boost/timer/timer.hpp"


namespace cse_impl
  {

    class symbol_record
      {

      public:

        symbol_record(const std::string& t, const std::string& s)
          : target(t),
            symbol(s),
            written(false)
          {
          }


      public:

        const std::string get_target() const { return(this->target); }

        const std::string get_symbol() const { return(this->symbol); }

        bool is_written() const { return(this->written); }

        void set_written() { this->written = true; }

      protected:

        std::string target;
        std::string symbol;
        bool        written;

      };

  }   // namespace cse_impl


class cse
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    //! s  = initial serial number for temporaries, typically 0 for a new translation unit
    //! p  = printer appropriate for language
    //! pd = payload from translation_unit
    //! k  = kernel name for temporary identifiers
    cse(unsigned int s, language_printer& p, translator_data& pd, std::string k=OUTPUT_DEFAULT_CPP_CSE_TEMPORARY_NAME)
      : serial_number(s),
        printer(p),
        temporary_name_kernel(k),
        symbol_counter(0),
        data_payload(pd)
      {
		    // pause timer
		    timer.stop();
      }

    //! destructor is default
    virtual ~cse() = default;


		// INTERFACE - COMMON SUBEXPRESSION ELIMINATION

  public:

    void               parse(const GiNaC::ex& expr);

    std::string        temporaries(const std::string& t);

    // two methods for getting the symbol corresponding to a GiNaC expression
    // get_symbol_without_use_count() just returns the symbol and is used during the parsing phase
    // get_symbol_with_use_count() marks each temporary as 'used', and injects it into the declarations.
    // This method is used when actually outputting symbols
    std::string        get_symbol_with_use_count(const GiNaC::ex& expr);

    void               clear();


		// INTERFACE - GET/SET NAME USED FOR TEMPORARIES

  public:

    const std::string& get_temporary_name() const                { return(this->temporary_name_kernel); }
    void               set_temporary_name(const std::string& k)  { this->temporary_name_kernel = k; }


		// INTERFACE - METADATA

  public:

		// get CSE active flag
    bool               do_cse() const { return(this->data_payload.do_cse()); }

		// get raw GiNaC printer associated with this CSE worker
    language_printer&  get_ginac_printer() { return(this->printer); }


		// INTERFACE - STATISTICS

  public:

		// get time spent performing CSE
		boost::timer::nanosecond_type get_cse_time() const { return(this->timer.elapsed().wall); }


		// INTERNAL API

  protected:

    // these functions are abstract and must be implemented by any derived classes
    // typically they will vary depending on the target language
    virtual std::string print(const GiNaC::ex& expr, bool use_count)                          = 0;
    virtual std::string print_operands(const GiNaC::ex& expr, std::string op, bool use_count) = 0;

    std::string get_symbol_without_use_count(const GiNaC::ex& expr);

		// make a temporary symbol
    std::string make_symbol();


		// INTERNAL DATA

  protected:

    language_printer& printer;

    translator_data& data_payload;

    unsigned int serial_number;
    unsigned int symbol_counter;

    std::string temporary_name_kernel;

    typedef std::unordered_map< std::string, cse_impl::symbol_record > symbol_table;
    typedef std::vector< std::pair<std::string, std::string> > declaration_table;

    symbol_table symbols;
    declaration_table   decls;

		// timer
		boost::timer::cpu_timer timer;

  };


#endif //CPPTRANSPORT_CSE_H
