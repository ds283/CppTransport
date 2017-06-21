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
    constexpr auto METRIC_INDICES = 2;
    constexpr auto INVERSE_METRIC_INDICES = 2;
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


    // QUERY FOR RESOURCE LABELS

  public:

    //! get parameters label
    virtual const boost::optional< contexted_value<std::string> >&
    parameters() = 0;

    //! get phase-space coordinates label
    //! if exact is false then the closest possible match is returned, if one is found
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES>, contexted_value<std::string> > >
    coordinates(std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES> v = { variance::none }, bool exact=true) = 0;

    //! get V,i label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DV_INDICES>, contexted_value<std::string> > >
    dV(std::array<variance, RESOURCE_INDICES::DV_INDICES> v = { variance::none }, bool exact=true) = 0;

    //! get V,ij label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDV_INDICES>, contexted_value<std::string> > >
    ddV(std::array<variance, RESOURCE_INDICES::DDV_INDICES> v = { variance::none, variance::none }, bool exact=true) = 0;

    //! get V,ijk label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDDV_INDICES>, contexted_value<std::string> > >
    dddV(std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v = { variance::none, variance::none, variance::none }, bool exact=true) = 0;
    
    
    //! get phase-space flattening function
    virtual const boost::optional< contexted_value<std::string> >&
    phase_flatten() = 0;

    //! get field-space flattening function
    virtual const boost::optional< contexted_value<std::string> >&
    field_flatten() = 0;


    //! get working type
    virtual const boost::optional< contexted_value<std::string> >&
    working_type() = 0;


    // ASSIGN RESOURCE LABELS

  public:

    //! assign parameter resource label
    virtual void assign_parameters(const contexted_value<std::string>& p) = 0;

    //! assign phase-space coordinate resource label
    virtual void assign_coordinates(const contexted_value<std::string>& c,
                            std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES> v) = 0;
    
    //! assign V,i resource label
    virtual void assign_dV(const contexted_value<std::string>& d,
                   std::array<variance, RESOURCE_INDICES::DV_INDICES> v) = 0;
    
    //! assign V,ij resource label
    virtual void assign_ddV(const contexted_value<std::string>& d,
                    std::array<variance, RESOURCE_INDICES::DDV_INDICES> v) = 0;
    
    //! assign V,ijk resource label
    virtual void assign_dddV(const contexted_value<std::string>& d,
                     std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v) = 0;
    
    
    //! assign phase-space flattening function
    virtual void assign_phase_flatten(const contexted_value<std::string>& f) = 0;

    //! assign field-space flattening function
    virtual void assign_field_flatten(const contexted_value<std::string>& f) = 0;


    //! assign working type
    virtual void assign_working_type(const contexted_value<std::string>& t) = 0;


    // RELEASE INDIVIDUAL RESOURCE LABELS

  public:

    //! release parameter resource
    virtual void release_parameter() = 0;

    //! release phase-space coordinate resource
    virtual void release_coordinates() = 0;

    //! release V,i resource
    virtual void release_dV() = 0;

    //! release V,ij resource
    virtual void release_ddV() = 0;

    //! release V,ijk resource
    virtual void release_dddV() = 0;


    //! release phase-space flattening function
    virtual void release_phase_flatten() = 0;

    //! release field-space flattening function
    virtual void release_field_flatten() = 0;


    //! release working type
    virtual void release_working_type() = 0;


    // GLOBAL RELEASE OF RESOURCE LABELS

  public:

    //! release all resources, but not flatteners (this is the most common use case;
    //! we wish to release resource labels at the end of a function, but not the flattener labels)
    virtual void release() = 0;

    //! release flatteners
    virtual void release_flatteners() = 0;

  };


class curvature_resource_manager: public resource_manager
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    curvature_resource_manager() = default;
    
    //! destructor
    ~curvature_resource_manager() = default;
    
    
    // QUERY FOR RESOURCE LABELS
  
  public:
    
    //! get connexion label
    virtual const boost::optional< contexted_value<std::string> >&
    connexion() = 0;
    
    //! get metric label
    virtual const boost::optional< contexted_value<std::string> >&
    metric() = 0;
    
    //! get inverse metric label
    virtual const boost::optional< contexted_value<std::string> >&
    metric_inverse() = 0;
    
    //! get Riemann A2 label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES>, contexted_value<std::string> > >
    Riemann_A2(std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v, bool exact=true) = 0;
    
    //! get Riemann A3 label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES>, contexted_value<std::string> > >
    Riemann_A3(std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v, bool exact=true) = 0;
    
    //! get Riemann B3 label
    virtual boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES>, contexted_value<std::string> > >
    Riemann_B3(std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v, bool exact=true) = 0;
    
    
    // ASSIGN RESOURCE LABELS
  
  public:
    
    //! assign connexion resource label
    virtual void assign_connexion(const contexted_value<std::string>& c) = 0;
    
    //! assign metric resource label
    virtual void assign_metric(const contexted_value<std::string>& c) = 0;
    
    //! assign inverse resource label
    virtual void assign_metric_inverse(const contexted_value<std::string>& c) = 0;
    
    //! assign Riemann A2 resource label
    virtual void assign_Riemann_A2(const contexted_value<std::string>& R,
                                   std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v) = 0;
    
    //! assign Riemann A3 resource label
    virtual void assign_Riemann_A3(const contexted_value<std::string>& R,
                                   std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v) = 0;
    
    //! assign Riemann B3 resource label
    virtual void assign_Riemann_B3(const contexted_value<std::string>& R,
                                   std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v) = 0;
    
    
    // RELEASE INDIVIDUAL RESOURCE LABELS
  
  public:
    
    //! release connexion resource
    virtual void release_connexion() = 0;
    
    //! release metric resource
    virtual void release_metricn() = 0;
    
    //! release connexion resource
    virtual void release_metric_inverse() = 0;
    
    //! release Riemann A2 resource
    virtual void release_Riemann_A2() = 0;
    
    //! release Riemann A3 resource
    virtual void release_Riemann_A3() = 0;
    
    //! release Riemann B3 resource
    virtual void release_Riemann_B3() = 0;
    
  };


#endif //CPPTRANSPORT_RESOURCE_MANAGER_H
