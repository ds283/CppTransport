//
// Created by David Seery on 19/12/2015.
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

#ifndef CPPTRANSPORT_SHARED_RESOURCES_H
#define CPPTRANSPORT_SHARED_RESOURCES_H


#include <memory>
#include <vector>
#include <stdexcept>

#include "concepts/resource_manager.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "indices.h"
#include "index_flatten.h"
#include "abstract_index.h"
#include "resource_failure.h"

#include "language_printer.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


class shared_resources
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    shared_resources(translator_data& p, resource_manager& m, expression_cache& c);

    //! destructor is default
    ~shared_resources() = default;


    // INTERFACE - GENERATE MAXIMUM INDEX VALUES

  public:

    //! get number of parameter indices
    param_index get_max_param_index() const { return param_index(this->num_params); }

    //! get number of field-space indices
    field_index get_max_field_index(variance v) const { return field_index(this->num_fields, v); }

    //! get number of phase-space indices
    phase_index get_max_phase_index(variance v) const { return phase_index(this->num_phase, v); }


    // SYMBOL SERVICES

  public:

    //! generate named symbol resource
    GiNaC::symbol generate_symbol(const std::string& name) const { return this->sym_factory.get_real_symbol(name); }


    // GENERATE RESOURCES -- SCALARS

  public:

    //! generate Mp symbol resource
    GiNaC::symbol generate_Mp() const { return this->M_Planck; }

    //! generate Hsq symbol resource
    GiNaC::symbol generate_Hsq() const { return this->sym_factory.get_real_symbol(HSQ_SYMBOL_NAME); }

    //! generate epsilon symbol resource
    GiNaC::symbol generate_eps() const { return this->sym_factory.get_real_symbol(EPS_SYMBOL_NAME); }

    //! generate V symbol resource
    GiNaC::symbol generate_V() const { return this->sym_factory.get_real_symbol(V_SYMBOL_NAME); }


    // GENERATE RESOURCES -- PARAMETERS

  public:

    //! generate concrete parameter label resource
    //! note that parameter indices have no variance
    std::unique_ptr<symbol_list> generate_parameter_symbols(const language_printer& printer) const;

    //! generate abstract parameter label resource
    //! note that parameter indices have no variance, and can therefore be sensibly handled
    //! by shared_resources (which knows nothing about model-dependent details)
    //! even though the equivalent field_# and deriv_# versions have to be handled
    //! by a model-specific resource manager
    GiNaC::symbol generate_parameter_vector(const abstract_index& idx, const language_printer& printer) const;


    // GENERATE RESOURCES -- FIELD- AND PHASE-SPACE COORDINATE SYMBOLS

  public:

    //! generate concrete field-space coordinate label resource
    //! note that these are the literal symbols that label the fields; they should be regarded
    //! as the components of the *contravariant* field-space coordinate in the case of a
    //! nontrivial field-space metric
    std::unique_ptr<symbol_list> generate_field_symbols(const language_printer& printer) const;

    //! generate concrete field-space derivative label resource
    //! note that these are the literal symbols that label the fields; they should be regarded
    //! as components of the *contravariant* phase-space coordinate in the case of a
    //! nontrivial field-space metric
    std::unique_ptr<symbol_list> generate_deriv_symbols(const language_printer& printer) const;


    // GENERATE RESOURCES -- LAMBDA RETURN TYPE

  public:

    //! generate string representing working type for lambda objects
    //! (this could be double, but may be something else (eg. number) if the implementation allows templated objects)
    std::string generate_working_type() const;


    // QUERY ROLL/UNROLL AVAILABILITY

  public:

    //! query whether parameters can be rolled-up into loops
    bool can_roll_parameters() const;

    //! query whether coordinate labels can be rolled-up into loops
    bool can_roll_coordinates() const;


    // MANUFACTURE GINAC INDICES

  public:

    //! make a field-space index
    template <typename IndexType>
    IndexType generate_index(const field_index& i);

    //! make a phase-space index
    template <typename IndexType>
    IndexType generate_index(const phase_index& i);

    //! make an abstract index
    template <typename IndexType>
    IndexType generate_index(const index_literal& i);


    // INTERNAL DATA

  private:

    // CACHES

    //! resource manager object
    resource_manager& mgr;

    //! expression cache
    expression_cache& cache;

    //! symbol factory
    symbol_factory& sym_factory;

    //! data payload provided by parent translation unit
    translator_data& payload;


    // SYMBOL LISTS

    //! list of symbols representing fields in the model
    const symbol_list field_list;

    //! list of symbols representing derivatives in the model
    const symbol_list deriv_list;

    //! list of symbols representing parameters in the model
    const symbol_list param_list;

    //! Mp symbol
    const GiNaC::symbol M_Planck;


    // LOCAL COPIES OF MODEL DATA

    //! number of parameters
    const unsigned int num_params;

    //! number of field-space indices
    const unsigned int num_fields;

    //! number of phase-space indices
    const unsigned int num_phase;


    // AGENTS

    //! index flattener
    index_flatten fl;

  };


#endif //CPPTRANSPORT_SHARED_RESOURCES_H
