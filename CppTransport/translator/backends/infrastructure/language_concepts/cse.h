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


#ifndef CPPTRANSPORT_CSE_H
#define CPPTRANSPORT_CSE_H

#include <string>
#include <unordered_map>
#include <set>
#include <utility>
#include <stdexcept>

#include "disable_warnings.h"
#include "ginac/ginac.h"

#include "language_printer.h"
#include "translator_data.h"
#include "msg_en.h"

#include "boost/optional.hpp"
#include "boost/timer/timer.hpp"


namespace cse_impl
  {

    class symbol_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        symbol_record(std::string t, std::string s)
          : target(std::move(t)),
            symbol(std::move(s))
          {
          }

        //! destuctor is default
        ~symbol_record() = default;


      public:

        //! return printed expression that this symbol resolves to
        const std::string get_target() const { return(this->target); }

        //! return symbol name
        const std::string get_symbol() const { return(this->symbol); }

        //! has this symbol record been written to the declaration stream?
        bool is_written() const { return(this->written); }

        //! mark this symbol as written
        void mark_written() { this->written = true; }


        // INTERNAL DATA

      protected:

        //! expression this symbol resolves to
        std::string target;

        //! symbol name
        std::string symbol;

        //! writeen flag
        bool written{false};

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
    cse(unsigned int s, language_printer& p, translator_data& pd, std::string k = OUTPUT_DEFAULT_CSE_TEMPORARY_NAME)
      : serial_number(s),
        printer(p),
        temporary_name_kernel(std::move(k)),
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

    //! parse a given GiNaC expression, building up a list of temporaries as we go.
    //! Nothing is marked as requiring deposition in a temporary pool, though, until
    //! it is tagged using get_symbol_with_use_count()
    //! the second argument is optional and can be used to assign a fixed temporary
    //! name to the entire expression, if desired
    void parse(const GiNaC::ex& expr, const boost::optional<std::string> name = boost::none);

    //! obtain list of temporary definitions which should be deposited in the current pool
    std::unique_ptr< std::list<std::string> >
    temporaries(const std::string& left, const std::string& mid, const std::string& right) const;

    //! two methods for getting the symbol corresponding to a GiNaC expression
    //! -- get_symbol_without_use_count() just returns the symbol and is used during the parsing phase
    //! -- get_symbol_with_use_count() marks each temporary as 'used', and injects it into the declarations.
    //!    this method is used when actually outputting symbols
    std::string get_symbol_with_use_count(const GiNaC::ex& expr);

    //! get number of temporary definitions
    size_t number_temporaries() const { return this->decls.size(); }

    //! clear all current temporary definitions;
    //! typically called when closing one pool and opening another
    void clear();

  protected:

    //! deposit a symbol record to the declaration list
    void deposit(cse_impl::symbol_record& record);


		// INTERFACE - GET/SET NAME USED FOR TEMPORARIES

  public:

    //! get current temporary name kernel
    const std::string& get_temporary_kernel() const { return (this->temporary_name_kernel); }

    //! set current temporary name kernel
    void set_temporary_kernel(const std::string& k) { this->temporary_name_kernel = k; }


		// INTERFACE - METADATA

  public:

		// get CSE active flag; indicates whether CSE will actually be performed
    bool do_cse() const { return (this->data_payload.do_cse()); }

		// get raw language printer associated with this CSE worker
    language_printer& get_ginac_printer() { return (this->printer); }


		// INTERFACE - STATISTICS

  public:

		// get cumulative time spent performing CSE
		boost::timer::nanosecond_type get_cse_time() const { return(this->timer.elapsed().wall); }


		// INTERNAL API

  protected:

    // these functions are abstract and must be implemented by any derived classes
    // typically they will vary depending on the target language

    //! print a GiNaC expression; if use_count is set then any temporaries which are
    //! used will be marked for deposition
    virtual std::string print(const GiNaC::ex& expr, bool use_count) = 0;

    //! print the operands to a GiNaC expression; if use_count is set then any temporaries
    //! which are used will be marked for deposition
    virtual std::string print_operands(const GiNaC::ex& expr, std::string op, bool use_count) = 0;


  protected:

    //! get symbol corresponding to a GiNaC expression, without tagging it (and any temporaries it depends on)
    //! for deposition
    std::string get_symbol_without_use_count(const GiNaC::ex& expr);

		// !make a temporary symbol
    std::string make_symbol();


		// INTERNAL DATA

  protected:

    //! reference to supplied language printer
    language_printer& printer;

    //! reference to supplied payload from parent translator
    translator_data& data_payload;

    //! serial number of current temporary pool
    unsigned int serial_number;

    //! unique symbol counter within current temporary pool
    unsigned int symbol_counter;

    //! current kernel for making names of temporaries
    std::string temporary_name_kernel;

    //! symbol table type
    using symbol_table = std::unordered_map< std::string, cse_impl::symbol_record >;

    //! declaration list type (note we use a vector because we want to preserve insertion order)
    using declaration_table = std::vector< std::pair<std::string, std::string> >;

    //! named symbol list type
    using named_symbol_table = std::set< std::string >;

    //! symbol table: maps printed GiNaC expressions to temporary definitions
    symbol_table symbols;

    //! declaration table: maps temporary names to printed GiNaC expressions
    declaration_table decls;

    //! named symbols: tracks named symbols that have been declared but aren't in the symbol table
    //! this is to avoid duplicate declarations
    named_symbol_table named_symbols;

		// work timer
		boost::timer::cpu_timer timer;

  };


class cse_exception: public std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    cse_exception(std::string arg)
      : std::runtime_error(std::move(arg))
      {
      }

    virtual ~cse_exception() = default;

  };


#endif //CPPTRANSPORT_CSE_H
