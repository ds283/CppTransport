//
// Created by David Seery on 27/06/2013.
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


#ifndef CPPTRANSPORT_INDEX_ASSIGNMENT_H
#define CPPTRANSPORT_INDEX_ASSIGNMENT_H


#include <iostream>
#include <list>
#include <string>
#include <deque>
#include <map>

#include "model_settings.h"
#include "index_class.h"
#include "abstract_index.h"
#include "index_literal.h"

#include "msg_en.h"


namespace index_assignment_impl
  {
    class assignment_set_iterator;
  }


//! represents a set of index assignments constructed from a list of abstract indices.
//! the set may be implemented by eg. for-loop type constructions or by index
//! unrolling.
//! For unrolling, provides an iterator which iterates through the individual assignments.
class assignment_set
  {

  public:

    typedef index_assignment_impl::assignment_set_iterator iterator;

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! construct an assignment set from a given list of abstract indices
    assignment_set(const abstract_index_database& s, index_order o = index_order::right);

    //! destructor is default
    ~assignment_set() = default;


    // ITERATORS

  public:

    //! return iterator to initial assignment
    iterator begin();

    //! return interator past the final assignment
    iterator end();


    // INTERFACE

  public:

    //! get size of assignment set
    unsigned int size() const { return(this->assignments_size); }


    // make iterator class a friend
    friend class index_assignment_impl::assignment_set_iterator;


    // ITERATORS TO UNDERLYING ABSTRACT INDICES

  public:

    //! begin
    abstract_index_database::const_iterator idx_set_begin() const { return(this->source_set.cbegin()); }

    //! end
    abstract_index_database::const_iterator idx_set_end() const { return(this->source_set.cend()); }


    // INTERNAL DATA

  private:

    //! reference to abstract index list used to construct this assignment set
    const abstract_index_database& source_set;

    //! size of assignment set
    unsigned int assignments_size;

    //! cache desired index order
    index_order order;

  };


//! index_value represents the assignment of some specific
//! numerical value to an abstract index
class index_value
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_value(const abstract_index& a, unsigned int v)
      : abstract(a),
        value(v)
      {
      }

    //! destructor is default
    ~index_value() = default;


    // INTERFACE -- REQUIRED FOR INDEX_DATABASE

  public:

    //! return label
    char get_label() const { return(this->abstract.get_label()); }

    //! return classification
    index_class get_class() const { return(this->abstract.get_class()); }


    // INTERFACE -- SPECIFIC TO ASSIGNMENT_RECORD

  public:

    //! get numerical value
    unsigned int get_numeric_value() const { return(this->value); }

    //! identify as species (no error flagged if index is really a parameter)
    bool is_field() const { return(this->value < this->abstract.get_number_fields()); }

    //! identify as momentum (no error flagged if index is really a parameter)
    bool is_momentum() const { return(this->value >= this->abstract.get_number_fields() && this->value < 2*this->abstract.get_number_fields()); }

    //! get underlying species (no error flagged if index is really a parameter)
    unsigned int species() const { return(this->is_momentum() ? this->value - this->abstract.get_number_fields() : this->value); }


    // INTERNAL DATA

  private:

    //! reference to associated abstract index
    const abstract_index& abstract;

    //! numerical value
    unsigned int value;

  };


//! set up index_assignment as a database of assignment records
typedef index_database<index_value> indices_assignment;

//! an index literal value is a map between an index literal and a numerical index value
typedef std::pair< std::reference_wrapper<const index_literal>, std::reference_wrapper<const index_value> > index_literal_value;

//! an index literal assignment is a list of index literal values, one for each index in the set
typedef std::vector< index_literal_value > index_literal_assignment;


namespace index_traits_impl
  {
    
    template <typename RecordType>
    index_class get_index_class(RecordType item);
    
    template <typename RecordType>
    char get_index_label(RecordType item);
    
    template <typename RecordType>
    const error_context& get_index_declaration(RecordType item);
    
    template<>
    inline index_class get_index_class<const index_literal_value&>(const index_literal_value& item)
      {
        return item.second.get().get_class();
      }
    
    template<>
    inline char get_index_label<const index_literal_value&>(const index_literal_value& item)
      {
        return item.second.get().get_label();
      }
    
    template<>
    inline const error_context& get_index_declaration<const index_literal_value&>(const index_literal_value& item)
      {
        return item.first.get().get_declaration_point();
      }
    
  }   // index_traits


namespace index_assignment_impl
  {

    //! iterate through an index assigment set, constructing assignment lists
    //! when dereferenced
    class assignment_set_iterator
      {
    
        // CONSTRUCTOR, DESTRUCTOR
  
      public:
    
        //! empty constructor
        assignment_set_iterator()
          : parent(boost::none),
            pos(0),
            max_pos(0)
          {
          }
    
        //! value constructor
        assignment_set_iterator(assignment_set& p, unsigned int cp)
          : parent(p),
            pos(cp),
            max_pos(p.size())
          {
          }
    
        //! destructor is default
        ~assignment_set_iterator() = default;
    
    
        // COMPARISON
  
      public:
    
        //! equality comparison
        bool operator==(const assignment_set_iterator& obj) const
          {
            if(this->pos != obj.pos) return false;
            if(this->parent && !obj.parent) return false;
            if(!this->parent && obj.parent) return false;
        
            return &(*this->parent) == &(*obj.parent);
          }
    
        //! inequality comparison
        bool operator!=(const assignment_set_iterator& obj) const
          {
            if(this->pos != obj.pos) return true;
            if(this->parent && !obj.parent) return true;
            if(!this->parent && obj.parent) return true;
        
            return &(*this->parent) != &(*obj.parent);
          }
    
    
        // INCREMENT, DECREMENT
  
      public:
    
        //! prefix decrement
        assignment_set_iterator& operator--()
          {
            if(!this->parent) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == 0) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_DECREMENT);
        
            --this->pos;
            return(*this);
          }
    
        //! postfix decrement
        assignment_set_iterator operator--(int)
          {
            if(!this->parent) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == 0) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_DECREMENT);
        
            const assignment_set_iterator old(*this);
            --this->pos;
            return(old);
          }
    
        //! prefix increment
        assignment_set_iterator& operator++()
          {
            if(!this->parent) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == this->max_pos) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_INCREMENT);
        
            ++this->pos;
            return(*this);
          }
    
        //! postfix increment
        assignment_set_iterator operator++(int)
          {
            if(!this->parent) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == this->max_pos) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_INCREMENT);
        
            const assignment_set_iterator old(*this);
            ++this->pos;
            return(old);
          }
    
    
        // DEREFERENCE
  
      public:
    
        //! dereference to get the assignment group corresponding to our current position within the
        //! assignment set
        std::unique_ptr<indices_assignment> operator*() const;
    
    
        // INTERNAL API
  
      private:
    
        //! construct an assignment group given an iterator range over abstract indices
        template <typename IteratorType, typename Inserter>
        void construct_assignment(IteratorType begin, IteratorType end, Inserter ins) const;
    
    
        // INTERNAL DATA
  
      private:
    
        //! parent assignment set
        boost::optional<assignment_set&> parent;
    
        //! current position in the set of index assignments;
        //! used to compute the value of each index when constructing a particular assignment
        unsigned int pos;
    
        //! total size of set
        unsigned int max_pos;
    
      };
    
  }   // namespace index_assignment_impl


#endif //CPPTRANSPORT_INDEX_ASSIGNMENT_H
