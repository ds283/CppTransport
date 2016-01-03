//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SHARED_RESOURCES_H
#define CPPTRANSPORT_SHARED_RESOURCES_H


#include <memory>
#include <vector>
#include <stdexcept>

#include "resource_manager.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "indices.h"
#include "index_flatten.h"
#include "abstract_index.h"

#include "language_printer.h"

#include "ginac/ginac.h"


class shared_resources
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    shared_resources(translator_data& p, resource_manager& m, expression_cache& c);

    //! destructor is default
    ~shared_resources() = default;


    // INTERFACE - QUERY DATA

  public:

    //! get number of parameter indices
    param_index get_number_parameters() const { return(this->num_params); }

    //! get number of field-space indices
    field_index get_number_field() const { return(this->num_fields); }

    //! get number of phase-space indices
    phase_index get_number_phase() const { return(this->num_phase); }


    // INTERFACE -- GENERATE RESOURCES

  public:

    //! generate Mp resource
    GiNaC::symbol generate_Mp() const { return this->M_Planck; }

    //! generate concrete parameter label resource
    std::unique_ptr<symbol_list> generate_parameters(const language_printer& printer) const;

    //! generate concrete field-space coordinate label resource
    std::unique_ptr<symbol_list> generate_fields(const language_printer& printer) const;

    //! generate concrete field-space derivative label resource
    std::unique_ptr<symbol_list> generate_derivs(const language_printer& printer) const;

    //! generate abstract parameter label resource
    GiNaC::symbol generate_parameters(const abstract_index& idx, const language_printer& printer) const;

    //! generate abstract field-space coordinate label resource
    GiNaC::symbol generate_fields(const abstract_index& idx, const language_printer& printer) const;

    //! generate abstract fields-space derivative label resource
    GiNaC::symbol generate_derivs(const abstract_index& idx, const language_printer& printer) const;

    //! generate string representing working type for lambda objects
    //! (this could be double, but may be something else (eg. number) if the implementation allows templated objects)
    std::string generate_working_type() const;


    // INTERFACE -- QUERY ROLL/UNROLL AVAILABILITY

  public:

    //! query whether parameters can be rolled-up into loops
    bool roll_parameters() const;

    //! query whether coordinate labels can be rolled-up into loops
    bool roll_coordinates() const;


    // INTERFACE -- MAKE INDICES

  public:

    //! make a field-space index
    GiNaC::idx generate_index(const field_index& i);

    //! make a phase-space index
    GiNaC::idx generate_index(const phase_index& i);

    //! make an abstract index
    GiNaC::idx generate_index(const abstract_index& i);


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
    GiNaC::symbol M_Planck;


    // LOCAL COPIES OF MODEL DATA

    //! number of parameters
    param_index num_params;

    //! number of field-space indices
    field_index num_fields;

    //! number of phase-space indices
    phase_index num_phase;


    // AGENTS

    //! index flattener
    index_flatten fl;

  };


class resource_failure: public std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    resource_failure(std::string x)
      : std::runtime_error(std::move(x))
      {
      }

    //! destructor
    ~resource_failure() = default;

  };


#endif //CPPTRANSPORT_SHARED_RESOURCES_H
