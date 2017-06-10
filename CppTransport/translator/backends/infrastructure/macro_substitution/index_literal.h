//
// Created by David Seery on 06/06/2017.
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


#ifndef CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H
#define CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H


#include <memory>
#include <vector>
#include <stdexcept>
#include <map>

#include "abstract_index.h"
#include "error_context.h"
#include "hash_combine.h"

#include "boost/optional.hpp"


//! enumeration representing variance type (co-, contra-, or unset)
enum class variance { covariant, contravariant, none };


class duplicate_index : public std::out_of_range
  {
    
    // CONSTRUCTOR, DESTRUCTOR
  
  public:
    
    //! constructor
    duplicate_index(std::string m, error_context c)
      : std::out_of_range(std::move(m)),
        ctx(c)
      {
      }
    
    //! destructor
    ~duplicate_index() = default;
    
    
    // INTERFACE
  
  public:
    
    //! get error context
    const error_context& get_error_point() const { return this->ctx; }
    
    
    // INTERNAL DATA
  
  public:
    
    //! error context
    error_context ctx;
    
  };


//! index literal represents an instance of an abstract index, and carries context information
//! such as the variance type
class index_literal
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor; defaults to unset variance
    index_literal(abstract_index& i, error_context ec, variance t)
      : idx(i),
        ctx(std::move(ec)),
        type(t)
      {
      }

    //! destructor is default
    ~index_literal() = default;


    // CONVERSIONS

  public:

    //! get underlying an abstract_index object (const version)
    const abstract_index& get() const { return this->idx; }

    //! allow conversion to abstract index (const version)
    operator const abstract_index&() const { return this->get(); }


    // INTERROGATE PROPERTIES

  public:

    //! get index variance
    variance get_variance() const { return this->type; }
    
    //! set index variance
    index_literal& set_variance(variance v) { this->type = v; return *this; }
    
    //! get error context
    const error_context& get_declaration_point() const { return this->ctx; }


    // FORMAT

  public:

    //! convert to string
    std::string to_string() const;


    // DATABASE SERVICES

  public:

    //! reassign parent abstract_index record
    void reassign(const abstract_index& i) { this->idx = std::cref(i); }


    // INTERNAL DATA

  private:

    //! link to abstract index of which this is an instance;
    //! held as a std::reference_wrapper<> so that we can reassign
    //! it later if needed
    std::reference_wrapper<const abstract_index> idx;
    
    //! context for this instance
    error_context ctx;

    //! variance
    variance type;

  };


namespace std
  {

    // provide std::equal_to<> and std::hash<> implementations for index_literal

    template <>
    struct equal_to<index_literal>
      {

        bool operator()(const index_literal& lhs, const index_literal& rhs) const
          {
            const abstract_index& lhs_idx = lhs;
            const abstract_index& rhs_idx = rhs;

            // not equal if the underlying label is different
            if(lhs_idx.get_label() != rhs_idx.get_label()) return false;

            // not equal if the underlying class is different, even if the labels agree
            if(lhs_idx.get_class() != rhs_idx.get_class()) return false;

            // not equal if the index variance does not agree
            if(lhs.get_variance() != rhs.get_variance()) return false;

            return true;
          }

      };
    
    
    // provide std::hash implementation for abstract_index, so it can be used in a std::unordered_map
    template <>
    struct hash<index_literal>
      {
        
        size_t operator()(const index_literal& idx) const
          {
            const abstract_index& a = idx;

            size_t hash = static_cast<size_t>(idx.get_variance());
            hash_combine(hash, a);
            
            return hash;
          }
        
      };

  }   // namespace std


//! typedef for list of index literals
//! use std::vector so it provides a subscript operator
typedef std::vector< std::shared_ptr<index_literal> > index_literal_list;

//! typedef for database of index literals
typedef std::map< char, std::shared_ptr<index_literal> > index_literal_database;


namespace index_literal_impl
  {

    // accessors for different container types
    template <typename Container>
    index_literal& get_index_literal(typename Container::value_type& item);

    template <>
    inline index_literal& get_index_literal<index_literal_list>(index_literal_list::value_type& item)
      {
        return *item;
      }

    template <>
    inline index_literal& get_index_literal<index_literal_database>(index_literal_database::value_type& item)
      {
        return *item.second;
      }

  }   // namespace index_literal_impl


namespace index_traits_impl
  {
    
    template <typename RecordType>
    index_class get_index_class(RecordType item);
    
    template <typename RecordType>
    char get_index_label(RecordType item);
    
    template <typename RecordType>
    const error_context& get_index_declaration(RecordType item);
    
    template <>
    inline index_class get_index_class< const std::shared_ptr<index_literal>& >(const std::shared_ptr<index_literal>& item)
      {
        return item->get().get_class();
      }
    
    template <>
    inline char get_index_label< const std::shared_ptr<index_literal>& >(const std::shared_ptr<index_literal>& item)
      {
        return item->get().get_label();
      }
    
    template <>
    inline const error_context& get_index_declaration< const std::shared_ptr<index_literal>& >(const std::shared_ptr<index_literal>& item)
      {
        return item->get_declaration_point();
      }
    
  }   // index_traits


//! convert between index_literal_list and index_literal_database;
//! throws std::runtime_error if the conversion cannot be done because
//! index_literal_list contains more than a single instance of an idex
std::unique_ptr<index_literal_database> to_database(const index_literal_list& indices);

//! change the references held in an index_literal_list or index_literal_database
//! from one database to another
template <typename Container>
void replace_database(Container& ctr, const abstract_index_database& db)
  {
    // loop over container, extracting records
    for(auto& item : ctr)
      {
        // get reference to index_literal object from container record
        index_literal& rec = index_literal_impl::get_index_literal<Container>(item);
        const abstract_index& idx = rec;
        
        // find associated abstract_index record from new database
        auto t = db.find(idx.get_label());
        
        if(t == db.end())
          {
            std::ostringstream msg;
            msg << ERROR_INDEX_LITERAL_REASSIGN << " '" << idx.get_label() << "'";
            throw std::out_of_range(msg.str());
          }
        
        rec.reassign(*t);
      }
  }


#endif //CPPTRANSPORT_ABSTRACT_INDEX_INSTANCE_H
