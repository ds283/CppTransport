//
// Created by David Seery on 18/12/2015.
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

#ifndef CPPTRANSPORT_RESOURCE_MANAGER_H
#define CPPTRANSPORT_RESOURCE_MANAGER_H


#include "simple_resource.h"
#include "indexed_resource.h"


namespace RESOURCE_INDICES
  {
    
    constexpr auto COORDINATES_INDICES = 1;
    constexpr auto DV_INDICES = 1;
    constexpr auto DDV_INDICES = 2;
    constexpr auto DDDV_INDICES = 3;
    constexpr auto CONNEXION_INDICES = 3;
    constexpr auto RIEMANN_A2_INDICES = 2;
    constexpr auto RIEMANN_A3_INDICES = 3;
    constexpr auto RIEMANN_B3_INDICES = 3;
    
  }   // namespace RESOURCE_INDICES


//! resource_manager is a simple cache that records the names of indexable arrays
//! declared to the translator as repositories for the expressions needed to
//! construct tensors using for-loops. For example, we often need the potential
//! derivative V,i available. This is achieved by declaring an array name such as dV
//! that can be indexed inside the for-loop as dV[i] to extract the required value.
//! Since these indexable quantities can have indices of different variance, we need
//! to keep track of the variances declared with each label.
class resource_manager
  {

    //! CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    resource_manager() = default;

    //! destructor
    virtual ~resource_manager() = default;


    // INTERFACE - QUERY FOR RESOURCE LABELS

  public:

    //! get parameters label
    const boost::optional< contexted_value<std::string> >&
    parameters()
      { return this->parameters_cache.find(); }

    //! get phase-space coordinates label
    //! if exact is false then the closest possible match is returned, if one is found
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES>, contexted_value<std::string> > >
    coordinates(std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES> v = { variance::none }, bool exact=true)
      { return this->coordinates_cache.find(v, exact); }

    //! get V,i label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DV_INDICES>, contexted_value<std::string> > >
    dV(std::array<variance, RESOURCE_INDICES::DV_INDICES> v = { variance::none }, bool exact=true)
      { return this->dV_cache.find(v, exact); }

    //! get V,ij label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDV_INDICES>, contexted_value<std::string> > >
    ddV(std::array<variance, RESOURCE_INDICES::DDV_INDICES> v = { variance::none, variance::none }, bool exact=true)
      { return this->ddV_cache.find(v, exact); }

    //! get V,ijk label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDDV_INDICES>, contexted_value<std::string> > >
    dddV(std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v = { variance::none, variance::none, variance::none }, bool exact=true)
      { return this->dddV_cache.find(v, exact); }


    //! get phase-space flattening function
    const boost::optional< contexted_value<std::string> >&
    phase_flatten()
      { return this->phase_flatten_cache.find(); }

    //! get field-space flattening function
    const boost::optional< contexted_value<std::string> >&
    field_flatten()
      { return this->field_flatten_cache.find(); }


    //! get working type
    const boost::optional< contexted_value<std::string> >&
    working_type()
      { return this->working_type_cache.find(); }


    // INTERFACE - ASSIGN RESOURCE LABELS

  public:

    //! assign parameter resource label
    void assign_parameters(const contexted_value<std::string>& p)
      { this->parameters_cache.assign(p); }

    //! assign phase-space coordinate resource label
    void assign_coordinates(const contexted_value<std::string>& c,
                            std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES> v)
      { this->coordinates_cache.assign(c, v); }
    
    //! assign V,i resource label
    void assign_dV(const contexted_value<std::string>& d,
                   std::array<variance, RESOURCE_INDICES::DV_INDICES> v)
      { this->dV_cache.assign(d, v); }
    
    //! assign V,ij resource label
    void assign_ddV(const contexted_value<std::string>& d,
                    std::array<variance, RESOURCE_INDICES::DDV_INDICES> v)
      { this->ddV_cache.assign(d, v); }
    
    //! assign V,ijk resource label
    void assign_dddV(const contexted_value<std::string>& d,
                     std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v)
      { this->dddV_cache.assign(d, v); }
    
    
    //! assign phase-space flattening function
    void assign_phase_flatten(const contexted_value<std::string>& f)
      { this->phase_flatten_cache.assign(f); }

    //! assign field-space flattening function
    void assign_field_flatten(const contexted_value<std::string>& f)
      { this->field_flatten_cache.assign(f); }


    //! assign working type
    void assign_working_type(const contexted_value<std::string>& t)
      { this->working_type_cache.assign(t); }


    // INTERFACE - RELEASE INDIVIDUAL RESOURCE LABELS

  public:

    //! release parameter resource
    void release_parameter()
      { this->parameters_cache.reset(); }

    //! release phase-space coordinate resource
    void release_coordinates()
      { this->coordinates_cache.reset(); }

    //! release V,i resource
    void release_dV()
      { this->dV_cache.reset(); }

    //! release V,ij resource
    void release_ddV()
      { this->ddV_cache.reset(); }

    //! release V,ijk resource
    void release_dddV()
      { this->dddV_cache.reset(); }


    //! release phase-space flattening function
    void release_phase_flatten()
      { this->phase_flatten_cache.reset(); }

    //! release field-space flattening function
    void release_field_flatten()
      { this->field_flatten_cache.reset(); }


    //! release working type
    void release_working_type()
      { this->working_type_cache.reset(); }


    // INTERFACE - GLOBAL RELEASE OF RESOURCE LABELS

  public:

    //! release all resources, but not flatteners (this is the most common use case;
    //! we wish to release resource labels at the end of a function, but not the flattener labels)
    void release();

    //! release flatteners
    void release_flatteners();


    // INTERNAL DATA

  protected:

    //! cache parameters resource label
    simple_resource<std::string> parameters_cache;

    //! cache parameters resource labels
    indexed_resource<RESOURCE_INDICES::COORDINATES_INDICES, std::string> coordinates_cache;

    //! cache V, resource labels
    indexed_resource<RESOURCE_INDICES::DV_INDICES, std::string> dV_cache;

    //! cache V,ij resource labels
    indexed_resource<RESOURCE_INDICES::DDV_INDICES, std::string> ddV_cache;

    //! cache V,ijk resource labels
    indexed_resource<RESOURCE_INDICES::DDDV_INDICES, std::string> dddV_cache;


    //! cache flattening function - full phase-space coordinates
    simple_resource<std::string> phase_flatten_cache;

    //! cache flattening function - field space coordinates
    simple_resource<std::string> field_flatten_cache;


    //! cache working type
    simple_resource<std::string> working_type_cache;

  };


#endif //CPPTRANSPORT_RESOURCE_MANAGER_H
