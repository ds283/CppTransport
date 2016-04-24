//
// Created by David Seery on 22/12/2015.
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

#ifndef CPPTRANSPORT_LAMBDAS_H
#define CPPTRANSPORT_LAMBDAS_H


#include <stdexcept>

#include "abstract_index.h"
#include "expression_cache.h"

#include "ginac/ginac.h"


// forward-declare language_printer class
class language_printer;


//! generic lambda-representable object;
//! concrete realizations are just atomic, ie. a single expression,
//! and case-by-case enumeration
class generic_lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! pre-supplied database constructor
    generic_lambda(const abstract_index_list& list, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty);

    //! one-index lambda constructor
    generic_lambda(const abstract_index& i, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty);

    //! two-index lambda constructor
    generic_lambda(const abstract_index& i, const abstract_index& j, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty);

    //! three-index lambda constructor
    generic_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty);

    //! destructor is default
    virtual ~generic_lambda() = default;


    // INTERFACE

  public:

    //! get lambda type
    enum expression_item_types get_type() const { return(this->type); }

    //! get tags
    const ginac_cache_tags& get_tags() const { return(this->tags); }

    //! get index list
    const abstract_index_list& get_index_list() const { return(this->index_list); }


    // INTERNAL DATA

  protected:

    //! index list associated with this lambda
    abstract_index_list index_list;

    //! transport object with which this lambda is associated
    enum expression_item_types type;

    //! tag list identifying this lambda
    ginac_cache_tags tags;

    //! C++ expression type
    std::string working_type;

  };


//! atomic lambda expression, ie. one with just one symbolic expression no matter
//! what arrangement of indices is involves
class atomic_lambda: public generic_lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    atomic_lambda(const abstract_index_list& list, GiNaC::ex e, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(list, t, tg, ty)
      {
        expr.swap(e);
      }

    //! one-index lambda constructor
    atomic_lambda(const abstract_index& i, GiNaC::ex e, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, t, tg, ty)
      {
        expr.swap(e);
      }

    //! two-index lambda constructor
    atomic_lambda(const abstract_index& i, const abstract_index& j, GiNaC::ex e, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, j, t, tg, ty)
      {
        expr.swap(e);
      }

    //! three-index lambda constructor
    atomic_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, GiNaC::ex e, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, j, k, t, tg, ty)
      {
        expr.swap(e);
      }

    //! destructor is default
    ~atomic_lambda() = default;


    // INTERFACE

  public:

    //! dereference to get GiNaC expression
    const GiNaC::ex& operator*() const { return(this->expr); }

    //! format for deposition as a temporary
    std::string make_temporary(language_printer& printer, unsigned int) const;


    // INTERNAL DATA

  protected:

    //! GiNaC expression associated with this lambda
    GiNaC::ex expr;

  };


typedef std::vector<GiNaC::ex> map_lambda_table;


//! lambda-map expression, with a collection of different symbolic expressions for
//! different combinations of field/momentum indices
class map_lambda: public generic_lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    map_lambda(const abstract_index_list& list, map_lambda_table t, enum expression_item_types tp, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(list, tp, tg, ty),
        map(t)
      {
        unsigned int size = 1;
        for(unsigned int i = 0; i < list.size(); ++i)
          {
            size *= 2;
          }

        if(t.size() != size) throw std::runtime_error(ERROR_INCONSISTENT_LAMBDA_MAP);
      }

    //! one-index lambda constructor
    map_lambda(const abstract_index& i, map_lambda_table& t, enum expression_item_types tp, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, tp, tg, ty),
        map(t)
      {
        if(t.size() != 2) throw std::runtime_error(ERROR_INCONSISTENT_LAMBDA_MAP);
      }

    //! two-index lambda constructor
    map_lambda(const abstract_index& i, const abstract_index& j, map_lambda_table& t, enum expression_item_types tp, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, j, tp, tg, ty),
        map(t)
      {
        if(t.size() != 4) throw std::runtime_error(ERROR_INCONSISTENT_LAMBDA_MAP);
      }

    //! three-index lambda constructor
    map_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, map_lambda_table& t, enum expression_item_types tp, const ginac_cache_tags& tg, std::string ty)
      : generic_lambda(i, j, k, tp, tg, ty),
        map(t)
      {
        if(t.size() != 8) throw std::runtime_error(ERROR_INCONSISTENT_LAMBDA_MAP);
      }

    //! destructor is default
    ~map_lambda() = default;


    // INTERFACE

  public:

    //! format for deposition as a temporary
    std::string make_temporary(language_printer& printer, unsigned int num_fields) const;


    // INTERNAL DATA

  protected:

    //! map of symbolic expressions for different field/momentum combinations
    map_lambda_table map;

  };


constexpr unsigned int LAMBDA_FIELD = 0;
constexpr unsigned int LAMBDA_MOMENTUM = 1;


constexpr unsigned int lambda_flatten(unsigned int a)
  {
    return(a);
  }


constexpr unsigned int lambda_flatten(unsigned int a, unsigned int b)
  {
    return(2*a + b);
  }


constexpr unsigned int lambda_flatten(unsigned int a, unsigned int b, unsigned int c)
  {
    return(2*2*a + 2*b + c);
  }


constexpr unsigned int lambda_flattened_map_size(unsigned int d)
  {
    return(d > 0 ? 2*lambda_flattened_map_size(d-1) : 1);
  }


#endif //CPPTRANSPORT_LAMBDAS_H
