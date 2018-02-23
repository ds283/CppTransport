//
// Created by David Seery on 20/06/2017.
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

#ifndef CPPTRANSPORT_INDEXED_RESOURCE_H
#define CPPTRANSPORT_INDEXED_RESOURCE_H


#include <map>
#include <array>
#include <string>
#include <limits>
#include <memory>

#include "contexted_value.h"

#include "msg_en.h"


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
    
    //! type to represent a label assignment; bool represents a usage flag to track labels that are
    //! declared but not used
    using label_record = std::pair< bool, std::unique_ptr< contexted_value<DataType> > >;
    
    //! use a std::map to associate a variance list with a data item
    //! no benefit from using a std::unordered_map since these arrays will typically be quite small
    using database = std::map< variance_list, label_record >;
    
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    indexed_resource(bool dw)
      : dev_warn(dw)
      {
      }
    
    //! destructor checks whether any labels are unused
    ~indexed_resource();
    
    
    // INTERFACE
    
  public:
    
    //! assign a value associated with the given variance list
    indexed_resource& assign(const contexted_value<DataType>& d, variance_list v);
    
    //! lookup a resource assignment by variance
    //! if exact is false then the nearest match is found, no matter what its variance is
    boost::optional< std::pair< std::array<variance, Indices>, contexted_value<DataType> > >
    find(variance_list v, bool exact=true);

    //! clear any stored values
    indexed_resource& reset();

  protected:

    //! lookup a resource assignment by value
    typename database::const_iterator find(DataType d) const;
    
    //! check for unused resource labels and issue an appropriate warning
    void warn_unused() const;

    
    // INTERNAL DATA
    
  private:
    
    //! database of resource labels
    database labels;

    //! distance metric agent -- measures distance between two variance lists
    DistanceMetric metric;

    //! show developer warnings?
    bool dev_warn;
    
  };


template <unsigned int Indices, typename DataType, typename DistanceMetric>
indexed_resource<Indices, DataType, DistanceMetric>::~indexed_resource()
  {
    this->warn_unused();
  }


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

            const error_context& u_ctx = u->second.second->get_declaration_point();
            u_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
          }

        this->labels.emplace(std::make_pair(std::move(v), std::move(std::make_pair(false, std::move(dp)))));
        return *this;
      }

    // otherwise, issue a warning that we are overwriting an unreleased value
    if(this->dev_warn)
      {
        const error_context& ctx = d.get_declaration_point();
        ctx.warn(NOTIFY_RESOURCE_REDECLARATION);

        const error_context& p_ctx = t->second.second->get_declaration_point();
        p_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
      }

    // check whether label is already in use by an assignment with different variance,
    // and issue an error if it is
    auto u = this->find(d);
    if(u != this->labels.end() && t->first != v)
      {
        const error_context& ctx = d.get_declaration_point();
        ctx.error(ERROR_RESOURCE_LABEL_IN_USE);

        const error_context& u_ctx = u->second.second->get_declaration_point();
        u_ctx.warn(NOTIFY_RESOURCE_DECLARATION_WAS);
      }

    // swap old and new pointers; old pointer will lapse and be destroyed
    // when dp goes out of scope
    t->second.second.swap(dp);
    
    return *this;
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
void indexed_resource<Indices, DataType, DistanceMetric>::warn_unused() const
  {
    if(!this->dev_warn) return;

    for(const auto& r : this->labels)
      {
        const label_record& rec = r.second;
        if(!rec.first)
          {
            const error_context& ctx = rec.second->get_declaration_point();
            ctx.warn(NOTIFY_RESOURCE_DECLARED_NOT_USED);
          }
      }
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
boost::optional< std::pair< typename indexed_resource<Indices, DataType, DistanceMetric>::variance_list, contexted_value<DataType> > >
indexed_resource<Indices, DataType, DistanceMetric>::find(variance_list v, bool exact)
  {
    using rtype = boost::optional< std::pair< variance_list, contexted_value<DataType> > >;
    using record_type = std::pair< unsigned int, typename database::iterator >;
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
            t->second.first = true;
            rtype rval = std::make_pair(t->first, *t->second.second);
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
    top_choice.second->second.first = true;

    rtype rval = std::make_pair(top_choice.second->first, *top_choice.second->second.second);
    return rval;
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
typename indexed_resource<Indices, DataType, DistanceMetric>::database::const_iterator
indexed_resource<Indices, DataType, DistanceMetric>::find(DataType d) const
  {
    auto comparator = [&](const typename database::value_type& a) -> bool
      {
        const DataType& v = a.second.second->get();
        return v == d;
      };

    return std::find_if(this->labels.begin(), this->labels.end(), comparator);
  }


template <unsigned int Indices, typename DataType, typename DistanceMetric>
indexed_resource<Indices, DataType, DistanceMetric>&
indexed_resource<Indices, DataType, DistanceMetric>::reset()
  {
    this->warn_unused();
    this->labels.clear();
    return *this;
  }


#endif //CPPTRANSPORT_INDEXED_RESOURCE_H
