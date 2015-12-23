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

    //! constructor
    lambda(const abstract_index_list& list)
      : index_list(list)
      {
      }

    //! destructor is default
    virtual ~lambda() = default;


    // INTERNAL DATA

  protected:

    //! index list associated with this lambda
    const abstract_index_list& index_list;

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

    //! destructor is default
    ~atomic_lambda() = default;


    // INTERNAL DATA

  protected:

    //! GiNaC expression associated with this lambda
    GiNaC::ex expr;

  };


#endif //CPPTRANSPORT_LAMBDAS_H
