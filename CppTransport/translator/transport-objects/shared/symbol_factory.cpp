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


#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "symbol_factory.h"

#include "msg_en.h"


template <bool real_valued>
symbol_wrapper symbol_factory::get_symbol(const std::string& name, std::string LaTeX)
	{
    // symbol type is realsymbol or symbol, depending which is required
    using symbol_type = typename std::conditional<real_valued, GiNaC::realsymbol, GiNaC::symbol>::type;
    using symbol_factory_impl::symbol_record;

    auto t = this->cache.find(name);

		if(t != this->cache.end())
      {
        const auto& record = t->second;

        // check whether symbol was previously defined as real
        if(real_valued && !record.is_real_valued())
          {
            std::ostringstream msg;
            msg << "'" << name << "': " << ERROR_SYMBOL_PREVIOUSLY_COMPLEX;
            throw std::runtime_error(msg.str());
          }
        else if(!real_valued && !record.is_complex_valued())
          {
            std::ostringstream msg;
            msg << "'" << name << "': " << ERROR_SYMBOL_PREVIOUSLY_REAL;
            throw std::runtime_error(msg.str());
          }

        // explicit cast makes me uneasy but alternative solutions are just as unpalatable
        return symbol_wrapper{record.get_symbol<symbol_type>()};
      }

    symbol_type rval{name, std::move(LaTeX)};

    auto u = this->cache.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(name),
                                 std::forward_as_tuple(name, rval, real_valued));
    if(!u.second)
      {
        std::ostringstream msg;
        msg << "'" << name << "': " << ERROR_SYMBOL_RECORD_INSERTION;
        throw std::runtime_error(msg.str());
      }

    return symbol_wrapper{u.first->second.template get_symbol<symbol_type>()};
	}


symbol_wrapper symbol_factory::get_real_symbol(const std::string& name, std::string LaTeX)
  {
    return this->get_symbol<true>(name, std::move(LaTeX));
  }


symbol_wrapper symbol_factory::get_complex_symbol(const std::string& name, std::string LaTeX)
  {
    return this->get_symbol<false>(name, std::move(LaTeX));
  }


template <typename SymbolType>
symbol_factory_impl::symbol_record::symbol_record(std::string n, SymbolType& s, bool rv)
  : name(std::move(n)),
    symbol(std::make_unique<SymbolType>(s)),
    real_valued(rv)
  {
  }


symbol_wrapper::symbol_wrapper(GiNaC::symbol& s)
  : symbol(s)
  {
  }


GiNaC::ex operator-(const symbol_wrapper& a)
  {
    return -a.get();
  }


GiNaC::ex operator+(const symbol_wrapper& a, const symbol_wrapper& b)
  {
    return a.get() + b.get();
  }


GiNaC::ex operator+(const symbol_wrapper& a, const GiNaC::ex& b)
  {
    return a.get() + b;
  }


GiNaC::ex operator+(const GiNaC::ex& a, const symbol_wrapper& b)
  {
    return a + b.get();
  }


GiNaC::ex operator-(const symbol_wrapper& a, const symbol_wrapper& b)
  {
    return a.get() - b.get();
  }


GiNaC::ex operator-(const symbol_wrapper& a, const GiNaC::ex& b)
  {
    return a.get() - b;
  }


GiNaC::ex operator-(const GiNaC::ex& a, const symbol_wrapper& b)
  {
    return a - b.get();
  }


GiNaC::ex operator*(const symbol_wrapper& a, const symbol_wrapper& b)
  {
    return a.get() * b.get();
  }


GiNaC::ex operator*(const symbol_wrapper& a, const GiNaC::ex& b)
  {
    return a.get() * b;
  }


GiNaC::ex operator*(const GiNaC::ex& a, const symbol_wrapper& b)
  {
    return a * b.get();
  }


GiNaC::ex operator/(const symbol_wrapper& a, const symbol_wrapper& b)
  {
    return a.get() / b.get();
  }


GiNaC::ex operator/(const symbol_wrapper& a, const GiNaC::ex& b)
  {
    return a.get() / b;
  }


GiNaC::ex operator/(const GiNaC::ex& a, const symbol_wrapper& b)
  {
    return a / b.get();
  }
