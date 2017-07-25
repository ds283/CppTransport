//
// Created by David Seery on 29/05/17.
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


#ifndef CPPTRANSPORT_GENERIC_ITERATOR_H
#define CPPTRANSPORT_GENERIC_ITERATOR_H


#include <type_traits>
#include <iterator>


template <typename Iterator, typename ConstIterator, typename RecordType, bool is_const_iterator = true>
class generic_iterator : public std::iterator<std::bidirectional_iterator_tag, RecordType>
  {
  
  private:
    
    //! set up an alias for a reference to each element.
    //! there are const- and non-const-versions
    typedef typename std::conditional<is_const_iterator, const RecordType&, RecordType&>::type reference_type;
    
    //! set up an alias for a pointer to each element
    typedef typename std::conditional<is_const_iterator, const RecordType*, RecordType*>::type pointer_type;
    
    //! set up an alias for the underlying iterator
    typedef typename std::conditional<is_const_iterator, ConstIterator, Iterator>::type iterator_type;
  
  public:
    
    //! setup an alias for the value type
    typedef typename std::conditional<is_const_iterator, const RecordType, RecordType>::type type;
  
  public:
    
    //! default constructor
    generic_iterator()
      {
      }
    
    
    //! value constructor
    generic_iterator(iterator_type i)
      : it(std::move(i))
      {
      }
    
    
    //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
    generic_iterator(const generic_iterator<Iterator, ConstIterator, RecordType, false>& obj)
      : it(obj.it)
      {
      }
    
    
    //! equality comparison
    bool operator==(const generic_iterator& obj) const
      {
        return (this->it == obj.it);
      }
    
    
    //! inequality comparison
    bool operator!=(const generic_iterator& obj) const
      {
        return (this->it != obj.it);
      }
    
    
    //! deference pointer to get value
    reference_type operator*()
      {
        // iterator points to a std::pair< Key, std::unique_ptr<RecordType> >
        return (*this->it->second);
      }
    
    
    //! provide member access
    pointer_type operator->()
      {
        return (this->it->second.get());
      }
    
    
    //! deference pointer to get value (const version)
    const reference_type operator*() const
      {
        return (*this->it->second);
      }
    
    
    //! provide member access (const version)
    const pointer_type operator->() const
      {
        return (this->it->second.get());
      }
    
    
    //! prefix decrement
    generic_iterator& operator--()
      {
        --this->it;
        return (*this);
      }
    
    
    //! postfix decrement
    generic_iterator operator--(int)
      {
        const generic_iterator old(*this);
        --this->it;
        return (old);
      }
    
    
    //! prefix increment
    generic_iterator& operator++()
      {
        ++this->it;
        return (*this);
      }
    
    
    //! postfix increment
    generic_iterator operator++(int)
      {
        const generic_iterator old(*this);
        ++this->it;
        return (old);
      }
    
    
    // make generic_iterator<true> a friend class of generic_iterator<false>, so that
    // the copy constructor can access its private member variables
    friend class generic_iterator<Iterator, ConstIterator, RecordType, true>;
    
    
    // INTERNAL DATA
  
  private:
    
    //! current value of iterator
    iterator_type it;
    
  };


#endif //CPPTRANSPORT_GENERIC_ITERATOR_H
