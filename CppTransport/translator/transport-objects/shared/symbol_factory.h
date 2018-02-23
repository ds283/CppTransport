//
// Created by David Seery on 12/03/15.
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
// --@@
//


#ifndef CPPTRANSPORT_SYMBOL_FACTORY_H
#define CPPTRANSPORT_SYMBOL_FACTORY_H


#include <unordered_map>
#include <functional>

#include "disable_warnings.h"
#include "ginac/ginac.h"


namespace symbol_factory_impl
  {

    class symbol_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor accepts a symbol name, a reference to a GiNaC symbol, and a flag indicating real-valued status.
        //! The template on SymbolType prevents derived classes of GiNaC::symbol from being sliced
        //! during copying
        template <typename SymbolType>
        symbol_record(std::string n, SymbolType& s, bool rv);

        //! destructor is default
        ~symbol_record() = default;


        // INTERFACE

      public:

        //! is real valued?
        bool is_real_valued() const { return this->real_valued; }

        //! is complex valued
        bool is_complex_valued() const { return !this->real_valued; }

        //! get symbol
        template <typename SymbolType>
        SymbolType& get_symbol() const { return dynamic_cast<SymbolType&>(*this->symbol); }

        //! get symbol name
        const std::string& get_name() const { return this->name; }


        // INTERNAL DATA

      private:

        //! name
        const std::string name;

        //! symbol instance
        std::unique_ptr<GiNaC::symbol> symbol;

        //! real valued flag
        bool real_valued;

      };

  }


class symbol_wrapper
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor accepts a GiNaC symbol reference; this may resolve to a subclass of GiNaC::symbol
    //! and the point of the symbol_wrapper class is to prevent any slicing occurring during copying
    symbol_wrapper(GiNaC::symbol& s);

    //! destructor is default
    ~symbol_wrapper() = default;


    // INTERFACE

  public:

    //! allow implicit conversion to GiNaC::symbol&
    operator GiNaC::symbol&() { return this->symbol.get(); }

    //! allow implicit conversion to GiNaC::symbol& (const version)
    operator const GiNaC::symbol&() const { return this->symbol.get(); }

    //! allow implicit conversion to GiNaC::ex
    operator GiNaC::ex() const { return GiNaC::ex{this->symbol.get()}; }

    //! get() returns reference to underlying symbol
    GiNaC::symbol& get() { return this->symbol.get(); }

    //! const version
    const GiNaC::symbol& get() const { return this->symbol.get(); }

    //! return symbol name
    std::string get_name() const { return this->symbol.get().get_name(); };


    // INTERNAL DATA

  private:

    //! reference to symbol object; use std::reference wrapper to allow copying
    std::reference_wrapper<GiNaC::symbol> symbol;

  };


//! unary minus
GiNaC::ex operator-(const symbol_wrapper& a);

//! allow symbol_wrapper objects to be summed to a GiNaC expression
GiNaC::ex operator+(const symbol_wrapper& a, const symbol_wrapper& b);

//! allow symbol_wrapper objects to be summed with GiNaC::ex objects
GiNaC::ex operator+(const symbol_wrapper& a, const GiNaC::ex& b);
GiNaC::ex operator+(const GiNaC::ex& a, const symbol_wrapper& b);

//! allow symbol_wrapper objects to be subtracted to a GiNaC expression
GiNaC::ex operator-(const symbol_wrapper& a, const symbol_wrapper& b);

//! allow symbol_wrapper objects to be subtracted with GiNaC::ex objects
GiNaC::ex operator-(const symbol_wrapper& a, const GiNaC::ex& b);
GiNaC::ex operator-(const GiNaC::ex& a, const symbol_wrapper& b);

//! allow symbol wrapper objects to be multiplied to a GiNaC expression
GiNaC::ex operator*(const symbol_wrapper& a, const symbol_wrapper& b);

//! allow symbol_wrapper objects to be multiplied with GiNaC::ex objects
GiNaC::ex operator*(const symbol_wrapper& a, const GiNaC::ex& b);
GiNaC::ex operator*(const GiNaC::ex& a, const symbol_wrapper& b);

//! allow symbol wrapper objects to be divided to a GiNaC expression
GiNaC::ex operator/(const symbol_wrapper& a, const symbol_wrapper& b);

//! allow symbol_wrapper objects to be divided with GiNaC::ex objects
GiNaC::ex operator/(const symbol_wrapper& a, const GiNaC::ex& b);
GiNaC::ex operator/(const GiNaC::ex& a, const symbol_wrapper& b);


class symbol_factory
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
		symbol_factory() = default;

    //! destructor is default
		~symbol_factory() = default;


		// INTERFACE

  public:

		//! get a real-valued symbol, optionally specifying a LaTeX label
    symbol_wrapper get_real_symbol(const std::string& name, std::string LaTeX = std::string{});

    //! get a complex-valued symbol, optionally specifying a LaTeX label
    symbol_wrapper get_complex_symbol(const std::string& name, std::string LaTeX = std::string{});


    // INTERNAL API

  protected:

    //! generic implementation of get_symbol
    template <bool real_valued>
    symbol_wrapper get_symbol(const std::string& name, std::string LaTeX);


		// INTERNAL DATA

  private:

    //! define cache type
    using cache_type = std::unordered_map< std::string, symbol_factory_impl::symbol_record >;

    //! symbol cache
		cache_type cache;

	};


#endif //CPPTRANSPORT_SYMBOL_FACTORY_H
