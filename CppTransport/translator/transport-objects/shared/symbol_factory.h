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
    GiNaC::realsymbol& get_real_symbol(const std::string& name, std::string LaTeX = std::string{});

    //! get a complex-valued symbol, optionally specifying a LaTeX label
    GiNaC::symbol& get_complex_symbol(const std::string& name, std::string LaTeX = std::string{});


    // INTERNAL API

  protected:

    //! generic implementation of get_symbol
    template <bool real_valued>
    typename std::conditional<real_valued, GiNaC::realsymbol&, GiNaC::symbol&>::type
    get_symbol(const std::string& name, std::string LaTeX);


		// INTERNAL DATA

  private:

    //! define cache type
    using cache_type = std::unordered_map< std::string, symbol_factory_impl::symbol_record >;

    //! symbol cache
		cache_type cache;

	};


#endif //CPPTRANSPORT_SYMBOL_FACTORY_H
