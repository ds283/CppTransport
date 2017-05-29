//
// Created by David Seery on 28/05/2017.
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

#ifndef CPPTRANSPORT_SYMBOL_DATABASE_H
#define CPPTRANSPORT_SYMBOL_DATABASE_H


#include <map>

#include "generic_iterator.h"

#include "msg_en.h"


template <typename RecordType>
class symbol_database
  {
    
    // TYPES
    
  public:
    
    //! type of record being stored
    typedef RecordType record_type;
    
  protected:
    
    //! alias for database type
    typedef std::map< std::string, std::unique_ptr<RecordType> > database_type;
    
    
    // ITERATORS
  
  public:
    
    // specialize to obtain intended iterators
    typedef generic_iterator< typename database_type::iterator, typename database_type::const_iterator, RecordType, false > iterator;
    typedef generic_iterator< typename database_type::iterator, typename database_type::const_iterator, RecordType, true >  const_iterator;
    
    typedef generic_iterator< typename database_type::reverse_iterator, typename database_type::const_reverse_iterator, RecordType, false > reverse_iterator;
    typedef generic_iterator< typename database_type::reverse_iterator, typename database_type::const_reverse_iterator, RecordType, true >  const_reverse_iterator;
    
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    symbol_database() = default;
    
    //! destructor is default
    ~symbol_database() = default;
    
    
    // ITERATORS
  
  public:
    
    iterator               begin()         { return iterator(this->database.begin()); }
    iterator               end()           { return iterator(this->database.end()); }
    const_iterator         begin()   const { return const_iterator(this->database.begin()); }
    const_iterator         end()     const { return const_iterator(this->database.end()); }
    
    const_iterator         cbegin()  const { return const_iterator(this->database.cbegin()); }
    const_iterator         cend()    const { return const_iterator(this->database.cend()); }
    
    reverse_iterator       rbegin()        { return reverse_iterator(this->database.rbegin()); }
    reverse_iterator       rend()          { return reverse_iterator(this->database.rend()); }
    const_reverse_iterator rbegin()  const { return const_reverse_iterator(this->database.crbegin()); }
    const_reverse_iterator rend()    const { return const_reverse_iterator(this->database.crend()); }
    
    const_reverse_iterator crbegin() const { return const_reverse_iterator(this->database.crbegin()); }
    const_reverse_iterator crend()   const { return const_reverse_iterator(this->database.crend()); }
  
  
  public:
    
    typedef std::pair< iterator, bool > emplace_return_type;
    
    
    // INTERFACE
  
  public:
    
    //! find a symbol in the database -- non-const version
    iterator find(const std::string& key);
    
    //! find a symbol in the database -- const version
    const_iterator find(const std::string& key) const;
    
    //! emplace an item at the front of the database
    //! arguments are forwarded to the emplace method for the underlying database type,
    //! so should be a std::pair< char, std::shared_ptr<> >
    //!
    //! Returns std::pair< iterator, bool > with iterator an iterator to the emplaced item
    //! and the bool indicating whether emplacement occurred
    template <typename ... Args>
    emplace_return_type emplace(Args&& ... args);
    
    //! get number of elements in database
    size_t size() const { return(this->database.size()); }
    
    
    // INTERNAL DATA
    
  private:
    
    //! symbol database
    database_type database;
  
  };


template <typename RecordType>
template <typename ... Args>
typename symbol_database<RecordType>::emplace_return_type symbol_database<RecordType>::emplace(Args&& ... args)
  {
    std::pair< typename database_type::iterator, bool > result = this->database.emplace(std::forward<Args>(args) ...);
    
    // want to return an iterator to the emplaced item, or to the existing item if an entry
    // with this key was already present
    typename database_type::iterator it = this->database.find(result.first->first);
    
    if(it == this->database.end()) throw std::runtime_error(ERROR_SYMBOL_DATABASE_EMPLACE_FAIL);
    
    return std::make_pair(iterator(result.first), result.second);
  }


template<typename RecordType>
typename symbol_database<RecordType>::iterator symbol_database<RecordType>::find(const std::string& key)
  {
    typename database_type::iterator it = this->database.find(key);
    
    return iterator(it);
  }


template<typename RecordType>
typename symbol_database<RecordType>::const_iterator symbol_database<RecordType>::find(const std::string& key) const
  {
    typename database_type::const_iterator it = this->database.find(key);
    
    return const_iterator(it);
  }


#endif //CPPTRANSPORT_SYMBOL_DATABASE_H
