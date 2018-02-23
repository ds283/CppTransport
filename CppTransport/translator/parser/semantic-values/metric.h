//
// Created by David Seery on 28/05/2017.
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

#ifndef CPPTRANSPORT_FIELD_METRIC_H
#define CPPTRANSPORT_FIELD_METRIC_H


#include <unordered_map>
#include <parser/y_common.h>

#include "symbol_tables.h"
#include "contexted_value.h"

#include "msg_en.h"

#include "boost/optional.hpp"

#include "ginac/ginac.h"


//! abstract metric class defines an interface that all concrete
//! instances should implement -- mostly just the ability to
//! extract particular components
class field_metric
  {
    
    // TYPES
  
  public:
    
    //! index type
    typedef tensor_index index_type;

    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor is default
    field_metric();
    
    //! destructor is virtual, default
    virtual ~field_metric() = default;
    
    
    // INTERFACE
    
  public:
    
    //! extract a specific component
    virtual GiNaC::ex operator()(index_type idx) const = 0;
    
    //! multiply by a constant
    field_metric& multiply(const GiNaC::ex& f);
    
    //! set a prefactor
    field_metric& set_prefactor(const GiNaC::ex& expr);
    
    //! clear prefactor
    field_metric& clear_prefactor() { this->prefactor.reset(); return *this; }
    
    
    // INTERNAL DATA
    
  protected:
    
    //! include an optional multiplicative prefactor
    boost::optional<GiNaC::ex> prefactor;
    
  };


//! field_metric_base is an elementary class that implements the field_metric
//! interface.
//! It can store the components of a metric, possibly with an additional
//! common factor
class field_metric_base : public field_metric
  {
    
    // TYPES

  public:

    //! context data type
    typedef std::pair< std::reference_wrapper<y::lexeme_type>, std::reference_wrapper<y::lexeme_type> > context_type;

  protected:
    
    //! type used to store component data
    typedef contexted_value<GiNaC::ex> component_type;
    
    //! type representing a database of components
    typedef std::unordered_map< field_metric::index_type, component_type > component_database;
    
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    field_metric_base(const field_symbol_table& idx);
    
    //! destructor is default
    virtual ~field_metric_base() = default;
    
    
    // INTERFACE
    
  public:
    
    //! extract a specific component
    GiNaC::ex operator()(field_metric::index_type idx) const override;
    
    
    // CLASS-SPECIFIC INTERFACE
    
  public:
    
    //! set component idx to value expr, with context supplied by the lexeme l
    field_metric_base&
    set_component(field_metric::index_type idx, context_type ctx, const GiNaC::ex& expr, const y::lexeme_type& l);

  protected:

    template <typename ErrorHandlerA, typename ErrorHandlerB>
    void check_indices(field_metric::index_type idx, ErrorHandlerA errA, ErrorHandlerB errB) const;
    
    
    // INTERNAL DATA
    
  private:

    //! symbol database representing possible indices
    const field_symbol_table& indices;

    //! store elements of the metric
    component_database components;
    
  };


//! field_metric_binop is a class that implements the field metric
//! interface and captures a binary operation between two metrics.
//! Currently, the allowed binary operations are addition and
//! subtraction
class field_metric_binop : public field_metric
  {
    
    // TYPES
    
  public:
    
    //! possible binary operations
    enum class op { add, sub };

    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    field_metric_binop(op O_, std::shared_ptr<field_metric> A_, std::shared_ptr<field_metric> B_);
    
    //! destructor is default
    virtual ~field_metric_binop() = default;
    
    
    // INTERFACE
    
  public:

    //! extract a specific component
    GiNaC::ex operator()(field_metric::index_type idx) const override;
    
    
    // INTERNAL DATA
    
  private:
    
    //! identify binary operation
    op O;
    
    //! store possible prefactor
    boost::optional<GiNaC::ex> prefactor;
    
    //! store left operand
    std::shared_ptr<field_metric> A;
    
    //! store right operand
    std::shared_ptr<field_metric> B;
    
  };


#endif //CPPTRANSPORT_FIELD_METRIC_H
