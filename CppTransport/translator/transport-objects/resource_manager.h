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


#include <map>
#include <array>
#include <string>
#include <limits>

#include "contexted_value.h"
#include "index_literal.h"

#include "msg_en.h"

#include "boost/optional.hpp"


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


namespace resource_manager_impl
  {

    // distance function on two index variances;
    // returns infinity is either is missing variance information but the other has it
    // otherwise returns 1 if the variances differ, and 0 if they agree
    inline unsigned int variance_distance(variance a, variance b)
      {
        if(a == variance::none || b == variance::none)
          {
            if(a == b) return 0;
            return std::numeric_limits<unsigned int>::infinity();
          }

        if(a != b) return 1;
        return 0;
      }

    // taxicab metric on an array of variances
    template <unsigned int Indices>
    class variance_metric
      {

      public:

        //! constructor is default
        variance_metric() = default;

        //! destructor is default
        ~variance_metric() = default;

        //! measure the distance between two arrays
        unsigned int operator()(const std::array<variance, Indices>& a, const std::array<variance, Indices>& b) const
          {
            unsigned int v = 0;
            for(unsigned int i = 0; i < Indices; ++i)
              {
                v += variance_distance(a[i], b[i]);
              }

            return v;
          }

      };

  }   // namespace resource_manager_impl


//! represents a resource that can be indexed with indices of different variance.
//! therefore we may have several versions
template <unsigned int Indices, typename DataType = std::string, typename DistanceMetric = resource_manager_impl::variance_metric<Indices> >
class indexed_resource
  {
    
    // TYPES
    
  public:
    
    //! type to represent the index variances
    using variance_list = std::array<variance, Indices>;
    
    //! use a std::map to associate a variance list with a data item
    //! no benefit from using a std::unordered_map since these arrays will typically be quite small
    using database = std::map< variance_list, std::unique_ptr< contexted_value<DataType> > >;
    
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor is default
    indexed_resource() = default;
    
    //! destructor is default
    ~indexed_resource() = default;
    
    
    // INTERFACE
    
  public:
    
    //! assign a value associated with the given variance list
    indexed_resource& assign(const contexted_value<DataType>& d, variance_list v);
    
    //! lookup a resource assignment by variance
    //! if exact is false then the nearest match is found, no matter what its variance is
    boost::optional< std::pair< std::array<variance, Indices>, contexted_value<DataType> > > find(variance_list v, bool exact=true) const;

    //! clear any stored values
    indexed_resource& reset() { this->labels.clear(); return *this; }

  protected:

    //! lookup a resource assignment by value
    typename database::const_iterator find(DataType d) const;

    
    // INTERNAL DATA
    
  private:
    
    //! database of resource labels
    database labels;

    //! distance metric agent -- measures distance between two variance lists
    DistanceMetric metric;
    
  };


template <unsigned int Indices, typename DataType, typename DistanceMetric>
indexed_resource<Indices, DataType, DistanceMetric>&
indexed_resource<Indices, DataType, DistanceMetric>::assign(const contexted_value<DataType>& d, variance_list v)
  {
    auto t = this->labels.find(v);
    
    // make a copy of the value we wish to store
    auto dp = std::make_unique< contexted_value<DataType> >(d);
    
    // if there is no existing value associated with this variance list, move our copy into the database and return
    if(t == this->labels.end())
      {
        // check whether label is already in use, and issue an error if it is
        auto u = this->find(d);
        if(u != this->labels.end())
          {
            const error_context& ctx = d.get_declaration_point();
            ctx.error(ERROR_RESOURCE_LABEL_IN_USE);

            const error_context& u_ctx = u->second->get_declaration_point();
            u_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
          }

        this->labels.emplace(std::make_pair(std::move(v), std::move(dp)));
        return *this;
      }
    
    // otherwise, issue a warning that we are overwriting an unreleased value
    const error_context& ctx = d.get_declaration_point();
    ctx.warn(NOTIFY_RESOURCE_REDECLARATION);
    
    const error_context& p_ctx = t->second->get_declaration_point();
    p_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);

    // check whether label is already in use by an assignment with different variance,
    // and issue an error if it is
    auto u = this->find(d);
    if(u != this->labels.end() && t->first != v)
      {
        ctx.error(ERROR_RESOURCE_LABEL_IN_USE);

        const error_context& u_ctx = u->second->get_declaration_point();
        u_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
      }

    // swap old and new pointers; old pointer will lapse and be destroyed
    // when dp goes out of scope
    t->second.swap(dp);
    
    return *this;
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
boost::optional< std::pair< typename indexed_resource<Indices, DataType, DistanceMetric>::variance_list, contexted_value<DataType> > >
indexed_resource<Indices, DataType, DistanceMetric>::find(variance_list v, bool exact) const
  {
    using rtype = boost::optional< std::pair< variance_list, contexted_value<DataType> > >;
    using record_type = std::pair< unsigned int, typename database::const_iterator >;
    using record_list = std::vector<record_type>;

    // if no records, nothing to do
    if(this->labels.empty()) return boost::none;

    record_list records;

    // compute and store the distance of each label from the required variance list
    for(auto t = this->labels.begin(); t != this->labels.end(); ++t)
      {
        unsigned int dist = this->metric(v, t->first);

        // if exact match, abandon the search and return immediately
        if(dist == 0)
          {
            rtype rval = std::make_pair(t->first, *t->second);
            return rval;
          }

        // push back the distance for this rec
        records.emplace_back(std::make_pair(dist, t));
      }

    // if exact flag is set then return no value, since if we got to this point then we didn't
    // encounter any record with an exact match for the variance list
    if(exact) return boost::none;

    auto sort_comparator = [&](const record_type& a, const record_type& b) -> bool
      {
        return a.first < b.first;
      };

    // sort into order of distance from the desired variance list
    std::sort(records.begin(), records.end(), sort_comparator);

    // pick the record with least distance to the desired variance assignment and return
    const record_type& top_choice = records.front();

    rtype rval = std::make_pair(top_choice.second->first, *top_choice.second->second);
    return rval;
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
typename indexed_resource<Indices, DataType, DistanceMetric>::database::const_iterator
indexed_resource<Indices, DataType, DistanceMetric>::find(DataType d) const
  {
    auto comparator = [&](const typename database::value_type& a) -> bool
      {
        const DataType& v = a.second->get();
        return v == d;
      };

    return std::find_if(this->labels.begin(), this->labels.end(), comparator);
  }


template <typename DataType = std::string>
class simple_resource
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor is default
    simple_resource() = default;
    
    //! destructor is default
    ~simple_resource() = default;
    
    
    // INTERFACE
    
  public:
    
    //! assign a value
    simple_resource& assign(const contexted_value<DataType>& d);
    
    //! lookup value
    const boost::optional< contexted_value<DataType> >& find() const;
    
    //! clear any stored values
    simple_resource& reset() { this->label.reset(); return *this; }
    
    
    // INTERNAL DATA
    
  private:
    
    //! resource label, if set
    boost::optional< contexted_value<DataType> > label;
    
  };


template <typename DataType>
simple_resource<DataType>& simple_resource<DataType>::assign(const contexted_value<DataType>& d)
  {
    // if there is no existing value, assign and return
    if(!this->label)
      {
        this->label = d;
        return *this;
      }
    
    // otherwise, issue a warning that we are overwriting an unreleased value
    const error_context& ctx = d.get_declaration_point();
    ctx.warn(NOTIFY_RESOURCE_REDECLARATION);
    
    const error_context& p_ctx = this->label.get().get_declaration_point();
    p_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
    
    this->label = d;
    
    return *this;
  }


template <typename DataType>
const boost::optional<contexted_value<DataType> >& simple_resource<DataType>::find() const
  {
    return this->label;
  }


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
    ~resource_manager() = default;


    // INTERFACE - QUERY FOR RESOURCES

  public:

    //! get parameters label
    const boost::optional< contexted_value<std::string> >&
    parameters() const
      { return this->parameters_cache.find(); }

    //! get phase-space coordinates label
    //! if exact is false then the closest possible match is returned, if one is found
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES>, contexted_value<std::string> > >
    coordinates(std::array<variance, RESOURCE_INDICES::COORDINATES_INDICES> v = { variance::none }, bool exact=true) const
      { return this->coordinates_cache.find(v, exact); }

    //! get V,i label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DV_INDICES>, contexted_value<std::string> > >
    dV(std::array<variance, RESOURCE_INDICES::DV_INDICES> v = { variance::none }, bool exact=true) const
      { return this->dV_cache.find(v, exact); }

    //! get V,ij label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDV_INDICES>, contexted_value<std::string> > >
    ddV(std::array<variance, RESOURCE_INDICES::DDV_INDICES> v = { variance::none, variance::none }, bool exact=true) const
      { return this->ddV_cache.find(v, exact); }

    //! get V,ijk label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDDV_INDICES>, contexted_value<std::string> > >
    dddV(std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v = { variance::none, variance::none, variance::none }, bool exact=true) const
      { return this->dddV_cache.find(v, exact); }

    //! get connexion label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES>, contexted_value<std::string> > >
    connexion(std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES> v, bool exact=true) const
      { return this->connexion_cache.find(v, exact); }

    //! get Riemann A2 label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES>, contexted_value<std::string> > >
    Riemann_A2(std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v, bool exact=true) const
      { return this->Riemann_A2_cache.find(v, exact); }
    
    //! get Riemann A3 label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES>, contexted_value<std::string> > >
    Riemann_A3(std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v, bool exact=true) const
      { return this->Riemann_A3_cache.find(v, exact); }
    
    //! get Riemann B3 label
    boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES>, contexted_value<std::string> > >
    Riemann_B3(std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v, bool exact=true) const
      { return this->Riemann_B3_cache.find(v, exact); }


    //! get phase-space flattening function
    const boost::optional< contexted_value<std::string> >&
    phase_flatten() const
      { return this->phase_flatten_cache.find(); }

    //! get field-space flattening function
    const boost::optional< contexted_value<std::string> >&
    field_flatten() const
      { return this->field_flatten_cache.find(); }


    //! get working type
    const boost::optional< contexted_value<std::string> >&
    working_type() const
      { return this->working_type_cache.find(); }


    // INTERFACE - ASSIGN RESOURCES

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
    
    //! assign connexion resource label
    void assign_connexion(const contexted_value<std::string>& c,
                          std::array<variance, RESOURCE_INDICES::CONNEXION_INDICES> v)
      { this->connexion_cache.assign(c, v); }
    
    //! assign Riemann A2 resource label
    void assign_Riemann_A2(const contexted_value<std::string>& R,
                           std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v)
      { this->Riemann_A2_cache.assign(R, v); }
    
    //! assign Riemann A3 resource label
    void assign_Riemann_A3(const contexted_value<std::string>& R,
                           std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v)
      { this->Riemann_A3_cache.assign(R, v); }
    
    //! assign Riemann B3 resource label
    void assign_Riemann_B3(const contexted_value<std::string>& R,
                           std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v)
      { this->Riemann_B3_cache.assign(R, v); }
    
    
    //! assign phase-space flattening function
    void assign_phase_flatten(const contexted_value<std::string>& f)
      { this->phase_flatten_cache.assign(f); }

    //! assign field-space flattening function
    void assign_field_flatten(const contexted_value<std::string>& f)
      { this->field_flatten_cache.assign(f); }


    //! assign working type
    void assign_working_type(const contexted_value<std::string>& t)
      { this->working_type_cache.assign(t); }


    // INTERFACE - RELEASE RESOURCES

  public:

    //! release parameter resource
    void release_parameter()
      { this->parameters_cache.reset(); }

    //! release phase-space coordinate resource
    void release_coordinates()
      { this->connexion_cache.reset(); }

    //! release V,i resource
    void release_dV()
      { this->dV_cache.reset(); }

    //! release V,ij resource
    void release_ddV()
      { this->ddV_cache.reset(); }

    //! release V,ijk resource
    void release_dddV()
      { this->dddV_cache.reset(); }

    //! release connexion resource
    void release_connexion()
      { this->connexion_cache.reset(); }

    //! release Riemann A2 resource
    void release_Riemann_A2()
      { this->Riemann_A2_cache.reset(); }
    
    //! release Riemann A3 resource
    void release_Riemann_A3()
      { this->Riemann_A3_cache.reset(); }
    
    //! release Riemann B3 resource
    void release_Riemann_B3()
      { this->Riemann_B3_cache.reset(); }


    //! release phase-space flattening function
    void release_phase_flatten()
      { this->phase_flatten_cache.reset(); }

    //! release field-space flattening function
    void release_field_flatten()
      { this->field_flatten_cache.reset(); }


    //! release working type
    void release_working_type()
      { this->working_type_cache.reset(); }


    // INTERFACE - UTILITY FUNCTIONS

  public:

    //! release all resources, but not flatteners
    void release();

    //! release flatteners
    void release_flatteners();


    // INTERNAL DATA

  private:

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

    //! cache connexion resource labels
    indexed_resource<RESOURCE_INDICES::CONNEXION_INDICES, std::string> connexion_cache;

    //! cache Riemann A2 resource labels
    indexed_resource<RESOURCE_INDICES::RIEMANN_A2_INDICES, std::string> Riemann_A2_cache;
    
    //! cache Riemann A3 resource labels
    indexed_resource<RESOURCE_INDICES::RIEMANN_A3_INDICES, std::string> Riemann_A3_cache;
    
    //! cache Riemann B3 resource labels
    indexed_resource<RESOURCE_INDICES::RIEMANN_B3_INDICES, std::string> Riemann_B3_cache;


    //! cache flattening function - full phase-space coordinates
    simple_resource<std::string> phase_flatten_cache;

    //! cache flattening function - field space coordinates
    simple_resource<std::string> field_flatten_cache;


    //! cache working type
    simple_resource<std::string> working_type_cache;

  };


#endif //CPPTRANSPORT_RESOURCE_MANAGER_H
