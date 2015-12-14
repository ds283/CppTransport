//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_INDEX_ASSIGNMENT_H_
#define CPPTRANSPORT_INDEX_ASSIGNMENT_H_


#include <iostream>
#include <list>
#include <string>
#include <deque>
#include <map>

#include "indexorder.h"
#include "index_traits.h"
#include "abstract_index.h"

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
    assignment_set(const abstract_index_list& s, enum indexorder o = indexorder::right);

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
    abstract_index_list::const_iterator idx_set_begin() const { return(this->source_set.cbegin()); }

    //! end
    abstract_index_list::const_iterator idx_set_end() const { return(this->source_set.cend()); }


    // INTERNAL DATA

  private:

    //! reference to abstract index list used to construct this assignment set
    const abstract_index_list& source_set;

    //! size of assignment set
    unsigned int assignments_size;

    //! cache desired index order
    enum indexorder order;

  };


//! assignment_record represents the assignment of some specific
//! numerical value to an abstract index
class assignment_record
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    assignment_record(const abstract_index& a, unsigned int v)
      : abstract(a),
        value(v)
      {
      }

    //! destructor is default
    ~assignment_record() = default;


    // INTERFACE -- REQUIRED FOR INDEX_DATABASE

  public:

    //! return label
    char get_label() const { return(this->abstract.get_label()); }

    //! return classification
    enum index_class get_class() const { return(this->abstract.get_class()); }


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


//! set up assignment_list as a database of assignment records
typedef index_database<assignment_record> assignment_list;


namespace index_assignment_impl
  {

    //! iterate through an index assignent set, constructing assignment lists
    //! when dereferenced
    class assignment_set_iterator
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! empty constructor
        assignment_set_iterator()
          : parent(nullptr),
            pos(0),
            max_pos(0)
          {
          }

        //! value constructor
        assignment_set_iterator(assignment_set* p, unsigned int cp)
          : parent(p),
            pos(cp),
            max_pos(p != nullptr ? p->size() : 0)
          {
          }

        //! destructor is default
        ~assignment_set_iterator() = default;


        // COMPARISON

      public:

        //! equality comparison
        bool operator=(const assignment_set_iterator& obj) const
          {
            return(this->parent == obj.parent && this->pos == obj.pos);
          }

        //! inequality comparison
        bool operator!=(const assignment_set_iterator& obj) const
          {
            return(this->parent != obj.parent || this->pos != obj.pos);
          }


        // INCREMENT, DECREMENT

      public:

        //! prefix decrement
        assignment_set_iterator& operator--()
          {
            if(this->parent == nullptr)    throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == 0)             throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_DECREMENT);

            --this->pos;
            return(*this);
          }

        //! postfix decrement
        assignment_set_iterator operator--(int)
          {
            if(this->parent == nullptr)    throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == 0)             throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_DECREMENT);

            const assignment_set_iterator old(*this);
            --this->pos;
            return(old);
          }

        //! prefix increment
        assignment_set_iterator& operator++()
          {
            if(this->parent == nullptr)    throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == this->max_pos) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_INCREMENT);

            ++this->pos;
            return(*this);
          }

        //! postfix increment
        assignment_set_iterator operator++(int)
          {
            if(this->parent == nullptr)    throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_NO_PARENT);
            if(this->pos == this->max_pos) throw std::runtime_error(ERROR_ASSIGNMENT_ITERATOR_INCREMENT);

            const assignment_set_iterator old(*this);
            ++this->pos;
            return(old);
          }


        // DEREFERENCE

      public:

        //! dereference to get the assignment group corresponding to our current position within the
        //! assignment set
        std::unique_ptr<assignment_list> operator*() const;


        // INTERNAL API

      private:

        //! construct an assignment group given an iterator range over abstract indices
        template <typename IteratorType, typename Inserter>
        void construct_assignment(IteratorType begin, IteratorType end, Inserter ins) const;


        // INTERNAL DATA

      private:

        //! parent assignment set
        assignment_set* parent;

        //! current position
        unsigned int pos;

        //! total size of set
        unsigned int max_pos;

      };

  }   // namespace index_assignment_impl


#endif //CPPTRANSPORT_INDEX_ASSIGNMENT_H_
