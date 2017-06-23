//
// Created by David Seery on 23/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#include "cache_tags.h"


cache_tags& cache_tags::operator+=(GiNaC::symbol sym)
  {
    this->symbols.emplace_back(std::make_shared<GiNaC::symbol>(std::move(sym)));
    return *this;
  }


cache_tags& cache_tags::operator+=(std::initializer_list<GiNaC::symbol> syms)
  {
    for(const auto& sym : syms)
      {
        this->symbols.emplace_back(std::make_shared<GiNaC::symbol>(sym));
      }
    return *this;
  }


cache_tags& cache_tags::operator+=(GiNaC::varidx idx)
  {
    this->var_indices.emplace_back(std::make_shared<GiNaC::varidx>(std::move(idx)));
    return *this;
  }


cache_tags& cache_tags::operator+=(std::initializer_list<GiNaC::varidx> idxs)
  {
    for(const auto& idx : idxs)
      {
        this->var_indices.emplace_back(std::make_shared<GiNaC::varidx>(idx));
      }
    return *this;
  }


cache_tags& cache_tags::operator+=(GiNaC::idx idx)
  {
    this->indices.emplace_back(std::make_shared<GiNaC::idx>(std::move(idx)));
    return *this;
  }


cache_tags& cache_tags::operator+=(std::initializer_list<GiNaC::idx> idxs)
  {
    for(const auto& idx : idxs)
      {
        this->indices.emplace_back(std::make_shared<GiNaC::idx>(idx));
      }
    return *this;
  }


bool operator==(const cache_tags& A, const cache_tags& B)
  {
    if(A.symbols.size() != B.symbols.size()) return false;
    if(A.indices.size() != B.indices.size()) return false;
    if(A.var_indices.size() != B.var_indices.size()) return false;

    for(unsigned int i = 0; i < A.symbols.size(); ++i)
      {
        GiNaC::ex test = (*A.symbols[i] - *B.symbols[i]).eval();
        if(!static_cast<bool>(test == 0)) return false;
      }

    for(unsigned int i = 0; i < A.indices.size(); ++i)
      {
        GiNaC::ex test = (*A.indices[i] - *B.indices[i]).eval();
        if(!static_cast<bool>(test == 0)) return false;
      }

    for(unsigned int i = 0; i < A.var_indices.size(); ++i)
      {
        GiNaC::ex test = (*A.var_indices[i] - *B.var_indices[i]).eval();
        if(!static_cast<bool>(test == 0)) return false;
      }

    return true;
  }


bool operator!=(const cache_tags& A, const cache_tags& B)
  {
    return !(A == B);
  }
