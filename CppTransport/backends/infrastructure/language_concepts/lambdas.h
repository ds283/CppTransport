//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_LAMBDAS_H
#define CPPTRANSPORT_LAMBDAS_H


#include "abstract_index.h"

#include "ginac/ginac.h"


//! generic lambda-representable object;
//! concrete realizations are just atomic, ie. a single expression,
//! and case-by-case enumeration
class lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! pre-supplied database constructor
    lambda(const abstract_index_list& list);

    //! one-index lambda constructor
    lambda(const abstract_index& i);

    //! two-index lambda constructor
    lambda(const abstract_index& i, const abstract_index& j);

    //! three-index lambda constructor
    lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k);

    //! destructor is default
    virtual ~lambda() = default;


    // INTERNAL DATA

  protected:

    //! index list associated with this lambda
    abstract_index_list index_list;

  };


//! atomic lambda expression, ie. one with just one symbolic expression no matter
//! what arrangement of indices is involves
class atomic_lambda: public lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    atomic_lambda(const abstract_index_list& list, GiNaC::ex e)
      : lambda(list)
      {
        expr.swap(e);
      }

    //! one-index lambda constructor
    atomic_lambda(const abstract_index& i, GiNaC::ex e)
      : lambda(i)
      {
        expr.swap(e);
      }

    //! two-index lambda constructor
    atomic_lambda(const abstract_index& i, const abstract_index& j, GiNaC::ex e)
      : lambda(i, j)
      {
        expr.swap(e);
      }

    //! three-index lambda constructor
    atomic_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, GiNaC::ex e)
      : lambda(i, j, k)
      {
        expr.swap(e);
      }

    //! destructor is default
    ~atomic_lambda() = default;


    // INTERFACE

  public:

    //! dereference to get GiNaC expression
    const GiNaC::ex& operator*() { return(this->expr); }


    // INTERNAL DATA

  protected:

    //! GiNaC expression associated with this lambda
    GiNaC::ex expr;

  };


typedef std::vector<GiNaC::ex> map_lambda_table;


//! lambda-map expression, with a collection of different symbolic expressions for
//! different combinations of field/momentum indices
class map_lambda: public lambda
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    map_lambda(const abstract_index_list& list, map_lambda_table t)
      : lambda(list),
        map(t)
      {
      }

    //! one-index lambda constructor
    map_lambda(const abstract_index& i, map_lambda_table& t)
      : lambda(i),
        map(t)
      {
      }

    //! two-index lambda constructor
    map_lambda(const abstract_index& i, const abstract_index& j, map_lambda_table& t)
      : lambda(i, j),
        map(t)
      {
      }

    //! three-index lambda constructor
    map_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, map_lambda_table& t)
      : lambda(i, j, k),
        map(t)
      {
      }

    //! destructor is default
    ~map_lambda() = default;


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
    unsigned int size = 1;
    while(d > 0)
      {
        size *= 2;
        --d;
      }

    return(size);
  }


#endif //CPPTRANSPORT_LAMBDAS_H
