//
// Created by David Seery on 08/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_INDEX_DATABASE_H
#define CPPTRANSPORT_INDEX_DATABASE_H


#include <iostream>

#include <string>
#include <memory>
#include <map>
#include <list>
#include <iterator>
#include <stdexcept>

#include "msg_en.h"


//#define DEBUG_INDEX_DATABASE


class index_exception: public std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_exception(char index)
      : std::runtime_error(std::string(1, index))
      {
      }

    //! destructor is default
    virtual ~index_exception() = default;

  };


namespace index_database_impl
  {

    template <typename Iterator, typename ConstIterator, typename RecordType, bool is_const_iterator=true>
    class database_iterator: public std::iterator< std::bidirectional_iterator_tag, RecordType >
      {

      private:

        //! set up aliases for a reference to an element
        //! there are const- and non-const-versions
        typedef typename std::conditional< is_const_iterator, const RecordType&, RecordType& >::type reference_type;

        //! set up an aliad for underlying managed pointer
        typedef typename std::conditional< is_const_iterator, const std::shared_ptr<RecordType>&, std::shared_ptr<RecordType>& >::type pointer_type;

        //! set up an alias for the underlying iterator
        typedef typename std::conditional< is_const_iterator, ConstIterator, Iterator >::type iterator_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! default constructor
        database_iterator()
          {
          }

        //! value constructor
        database_iterator(iterator_type i)
          : it(std::move(i))
          {
          }

        //! copy constructor: allows for implicit conversion from a regular iterator to a const iterator
        database_iterator(const database_iterator<Iterator, ConstIterator, RecordType, false>& obj)
          : it(obj.it)
          {
          }


        // COMPARISON

      public:

        //! equality comparison
        bool operator==(const database_iterator& obj) const
          {
            return(this->it == obj.it);
          }

        //! inequality comparison
        bool operator!=(const database_iterator& obj) const
          {
            return(this->it != obj.it);
          }


        // DEREFERENCE

      public:

        //! dereference to get underlying record
        reference_type operator*()
          {
            pointer_type& rec_ptr = (*this->it)->second;
            return(*rec_ptr);
          }

        //! dereference to provide member access
        pointer_type operator->()
          {
            pointer_type& rec_ptr = (*this->it)->second;
            return(rec_ptr);
          }


        // INCREMENT, DECREMENT

      public:

        //! prefix decrement
        database_iterator& operator--()
          {
            --this->it;
            return(*this);
          }

        //! postfix decrement
        database_iterator operator--(int)
          {
            const database_iterator old(*this);
            --this->it;
            return(old);
          }

        //! prefix increment
        database_iterator& operator++()
          {
            ++this->it;
            return(*this);
          }

        //! postfix incremnet
        database_iterator operator++(int)
          {
            const database_iterator old(*this);
            ++this->it;
            return(old);
          }


        // make index_abstract_list_iterator<true> a friend class of index_abstract_list_iterator<false>,
        // so that the copy constructor can access its private member variables
        friend class database_iterator<Iterator, ConstIterator, RecordType, true>;


      private:

        //! underlying iterator
        iterator_type it;

      };


  }   // namespace index_database_impl


template <typename RecordType>
class index_database
  {

  public:

    typedef RecordType underlying_type;

  private:

    //! basic index database, organized as a map with lookup via key value;
    //! we use a shared pointer to manage the lifetime of the database entries,
    //! so ownership is shared with any copies of ourselves
    typedef std::map< char, std::shared_ptr<RecordType> > database_type;

    //! however, we also need to retain a memory of the order in which indexes were
    //! added to the database, in order that we can respect the 'index order' setting.
    //! we do that by maintaining an auxiliary list of iterators into the database.
    //!
    //! use std::list so that iterators held in the index do not become invalidated.
    //! This has performance implications for subscripted lookup, but we just have to
    //! keep that to a minimum
    typedef std::list< typename database_type::iterator > ordered_list_type;

    //! finally, maintain an index into the ordered_list!
    //! this so that we can quickly look up iterators into the ordered list rather than
    //! having to search for them
    typedef std::map< char, typename ordered_list_type::iterator > ordered_list_index_type;


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    index_database() = default;

    //! over-ride copy constructor to rebuild ordering database
    index_database(const index_database& obj);

    //! destructor is default
    ~index_database() = default;


    // ASSIGNMENT

  public:

    //! supply move assignment operator to handle rebuild of indices
    index_database& operator=(index_database&& obj);


    // ITERATORS

  public:

    // specialize to obtain iterators
    typedef index_database_impl::database_iterator< typename ordered_list_type::iterator, typename ordered_list_type::const_iterator, RecordType, false > iterator;
    typedef index_database_impl::database_iterator< typename ordered_list_type::iterator, typename ordered_list_type::const_iterator, RecordType, true > const_iterator;

    typedef index_database_impl::database_iterator< typename ordered_list_type::reverse_iterator, typename ordered_list_type::const_reverse_iterator, RecordType, false > reverse_iterator;
    typedef index_database_impl::database_iterator< typename ordered_list_type::reverse_iterator, typename ordered_list_type::const_reverse_iterator, RecordType, true > const_reverse_iterator;

    iterator               begin()         { return iterator(this->order_db.begin()); }
    iterator               end()           { return iterator(this->order_db.end()); }

    const_iterator         begin()   const { return const_iterator(this->order_db.cbegin()); }
    const_iterator         end()     const { return const_iterator(this->order_db.cend()); }

    const_iterator         cbegin()  const { return const_iterator(this->order_db.cbegin()); }
    const_iterator         cend()    const { return const_iterator(this->order_db.cend()); }

    reverse_iterator       rbegin()        { return reverse_iterator(this->order_db.rbegin()); }
    reverse_iterator       rend()          { return reverse_iterator(this->order_db.rend()); }

    const_reverse_iterator rbegin()  const { return const_reverse_iterator(this->order_db.crbegin()); }
    const_reverse_iterator rend()    const { return const_reverse_iterator(this->order_db.crend()); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(this->order_db.crbegin()); }
    const_reverse_iterator crend()   const { return const_reverse_iterator(this->order_db.crend()); }


  public:

    typedef std::pair< iterator, bool > emplace_return_type;


    // INTERFACE

  public:

    //! find an item in the database -- const version
    const_iterator find(char key) const;

    //! find an item in the database -- non-const version
    iterator find(char key);

    //! emplace an item at the front of the database
    //! arguments are forwarded to the emplace method for the underlying database type,
    //! so should be a std::pair< char, std::shared_ptr<> >
    //!
    //! Returns std::pair< iterator, bool > with iterator an iterator to the emplaced item
    //! and the bool indicating whether emplacement occurred
    template <typename ... Args>
    emplace_return_type emplace_front(Args&& ... args);

    //! emplace an item at the back of the database
    //! arguments are forwarded to the emplace method for the underlying database type,
    //! so should be a std::pair< char, std::shared_ptr<> >
    //!
    //! Returns std::pair< iterator, bool > with iterator an iterator to the emplaced item
    //! and the bool indicating whether emplacement occurred
    template <typename ... Args>
    emplace_return_type emplace_back(Args&& ... args);

    //! get number of elements in database
    size_t size() const { return(this->db.size()); }

    //! subscripted access into database
    //! slow -- because means iterating over the std::list holding the ordered lookup table
    //! non-const version
    RecordType& operator[](std::size_t i);

    //! subscripted access into database
    //! slow -- because means iterating over the std::list holding the ordered lookup table
    //! non-const version
    const RecordType& operator[](std::size_t i) const;


    // DEBUGGING API

  public:

    void write_database() const;


    // INTERNAL DATA

  private:

    //! database
    database_type db;

    //! ordering memory
    //! note this will be invalidated on copy, so we have to rebuild it in the copy constructor
    ordered_list_type order_db;

    //! index into ordered list
    //! note this will be invalidated on copy, so we have to rebuild it in the copy constructor
    ordered_list_index_type order_index;

  };


template <typename RecordType>
index_database<RecordType>::index_database(const index_database<RecordType>& obj)
  : db(obj.db)
  {
#ifdef DEBUG_INDEX_DATABASE
    std::cout << '\n' << "CALLING COPY CONSTRUCTOR" << '\n';
#endif

    // work through ordered_db in object, finding iterators into the database as we go
    for(typename database_type::iterator t : obj.order_db)
      {
        // get key
        char key = t->first;

        typename database_type::iterator u = db.find(key);
        if(u != db.end())
          {
            order_db.push_back(u);
            order_index.emplace(std::make_pair(key, --order_db.end()));
          }
        else
          {
            throw index_exception(key);
          }
      }

#ifdef DEBUG_INDEX_DATABASE
    std::cout << "ORIGINAL DATABASE" << '\n';
    this->write_database();

    std::cout << '\n' << "COPIED DATABASE" << '\n';
    obj.write_database();
#endif
  }


template <typename RecordType>
index_database<RecordType>& index_database<RecordType>::operator=(index_database<RecordType>&& obj)
  {
    if(this != &obj)
      {
        this->db.clear();
        this->order_db.clear();
        this->order_index.clear();

        // move main database
        this->db = std::move(obj.db);

#ifdef DEBUG_INDEX_DATABASE
        std::cout << '\n' << "CALLING MOVE ASSIGNMENT OPERATOR" << '\n';
#endif

        // work through ordered_db in object, finding iterators into the database as we go
        for(typename database_type::iterator t : obj.order_db)
          {
            // get key
            char key = t->first;

            typename database_type::iterator u = db.find(key);
            if(u != db.end())
              {
                order_db.push_back(u);
                order_index.emplace(std::make_pair(key, --order_db.end()));
              }
            else
              {
                throw index_exception(key);
              }
          }

        obj.order_db.clear();
        obj.order_index.clear();

#ifdef DEBUG_INDEX_DATABASE
        std::cout << "ASSIGNED DATABASE" << '\n';
        this->write_database();

        std::cout << '\n' << "ORIGINAL DATABASE" << '\n';
        obj.write_database();
#endif
      }

    return(*this);
  }


template <typename RecordType>
template <typename ... Args>
typename index_database<RecordType>::emplace_return_type index_database<RecordType>::emplace_front(Args&& ... args)
  {
    std::pair< typename database_type::iterator, bool > result = this->db.emplace(std::forward<Args>(args) ...);

    if(result.second)    // emplacement occurred
      {
        // store result in ordering database
        this->order_db.push_front(result.first);

        // store reference to ordering database element in index
        this->order_index.emplace(std::make_pair(result.first->second->get_label(), this->order_db.begin()));
      }

    // want to return an iterator to the emplaced item, or to the existing item if an entry
    // with this key was already present
    typename ordered_list_index_type::iterator it = this->order_index.find(result.first->second->get_label());

    if(it == this->order_index.end()) throw std::runtime_error(ERROR_INDEX_DATABASE_EMPLACE_FAIL);

    return std::make_pair(iterator(it->second), result.second);
  }


template <typename RecordType>
template <typename ... Args>
typename index_database<RecordType>::emplace_return_type index_database<RecordType>::emplace_back(Args&& ... args)
  {
    std::pair< typename database_type::iterator, bool > result = this->db.emplace(std::forward<Args>(args) ...);

    if(result.second)    // emplacement occurred
      {
        // store result in ordering database
        this->order_db.push_back(result.first);

        // store reference to ordering database element in index
        this->order_index.emplace(std::make_pair(result.first->second->get_label(), --this->order_db.end()));
      }

    // want to return an iterator to the emplaced item, or to the existing item if an entry
    // with this key was already present
    typename ordered_list_index_type::iterator it = this->order_index.find(result.first->second->get_label());

    if(it == this->order_index.end()) throw std::runtime_error(ERROR_INDEX_DATABASE_EMPLACE_FAIL);

    return std::make_pair(iterator(it->second), result.second);
  }


template<typename RecordType>
typename index_database<RecordType>::iterator index_database<RecordType>::find(char key)
  {
    typename ordered_list_index_type::iterator it = this->order_index.find(key);

    if(it == this->order_index.end()) return iterator(this->order_db.end());

    typename ordered_list_type::iterator oit = it->second;
    iterator rval(oit);

    return rval;
  }


template<typename RecordType>
typename index_database<RecordType>::const_iterator index_database<RecordType>::find(char key) const
  {
    typename ordered_list_index_type::const_iterator it = this->order_index.find(key);

    if(it == this->order_index.end()) return const_iterator(this->order_db.end());

    typename ordered_list_type::const_iterator oit = it->second;
    const_iterator rval(oit);

    return rval;
  }


template <typename RecordType>
const RecordType& index_database<RecordType>::operator[](std::size_t i) const
  {
    if(i > this->db.size()) throw std::out_of_range(ERROR_INDEX_DATABASE_OUT_OF_RANGE);

    const_iterator it = this->cbegin();
    while(i > 0)
      {
        ++it;
        --i;
      }

    return(*it);
  }


template <typename RecordType>
RecordType& index_database<RecordType>::operator[](std::size_t i)
  {
    if(i > this->db.size()) throw std::out_of_range(ERROR_INDEX_DATABASE_OUT_OF_RANGE);

    iterator it = this->begin();
    while(i > 0)
      {
        ++it;
        --i;
      }

    return(*it);
  }


template <typename RecordType>
void index_database<RecordType>::write_database() const
  {
    std::cout << "  DATABASE: " << '\n';
    for(const std::pair< char, std::shared_ptr<RecordType> >& v : this->db)
      {
        std::cout << "    -- key = '" << v.first << "', shared pointer OK = " << static_cast<bool>(v.second) << ", shared pointer value = " << v.second.get() << '\n';
      }

    std::cout << "  ORDERED LIST: " << '\n';
    for(typename database_type::const_iterator t : this->order_db)
      {
        std::cout << "    -- key = '" << t->first << "', shared pointer OK = " << static_cast<bool>(t->second) << ", shared pointer value = " << t->second.get() << '\n';
      }

    std::cout << "  LOOKUP INDEX: " << '\n';
    for(const std::pair< char, typename ordered_list_type::iterator >& v : this->order_index)
      {
        std::cout << "    -- key = '" << v.first << "', ";

        typename database_type::const_iterator t = *v.second;
        std::cout << "iterator points at: key = '" << t->first << "', shared pointer OK = " << static_cast<bool>(t->second) << ", shared pointer value = " << t->second.get() << '\n';
      }
  }


template <typename Container>
class database_front_inserter
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    database_front_inserter(Container& c)
      : parent(c)
      {
      }

    //! destructor is default
    ~database_front_inserter() = default;


    // INTERFACE

  public:

    template <typename ... Args>
    typename Container::emplace_return_type insert(Args&& ... args)
      {
        return this->parent.emplace_front(std::forward<Args>(args) ...);
      }


    // INTERNAL DATA

  private:

    Container& parent;

  };


template <typename Container>
class database_back_inserter
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    database_back_inserter(Container& c)
      : parent(c)
      {
      }

    //! destructor is default
    ~database_back_inserter() = default;


    // INTERFACE

  public:

    template <typename ... Args>
    typename Container::emplace_return_type insert(Args&& ... args)
      {
        return this->parent.emplace_back(std::forward<Args>(args) ...);
      }


    // INTERNAL DATA

  private:

    Container& parent;

  };


//! Construct the difference of two index databases, ie. all elements in l which are not also in r.
//! Ordering in l is preserved.

template <typename RecordType>
index_database<RecordType> operator-(const index_database<RecordType>& l, const index_database<RecordType>& r)
  {
    index_database<RecordType> rval;

    // loop through elements in l, checking whether they are also in r
    // this means that ordering in l is preserved
    for(const RecordType& idx : l)
      {
        typename index_database<RecordType>::const_iterator it = r.find(idx.get_label());

        if(it == r.end())
          {
            rval.emplace_back(std::make_pair(idx.get_label(), std::make_shared<RecordType>(idx)));
          }
        else if(idx.get_class() != it->get_class())
          {
            throw index_exception(idx.get_label());
          }
      }

    return(rval);
  }


//! Merge two index databases together.
//! Ordering is preserved, in the sequence l then r.

template <typename RecordType>
index_database<RecordType> operator+(const index_database<RecordType>& l, const index_database<RecordType>& r)
  {
#ifdef DEBUG_INDEX_DATABASE
    std::cout << '\n' << "********" << '\n';
    std::cout << '\n' << "COMBINING DATABASES" << '\n';
    std::cout << '\n' << "LHS DATABASE:" << '\n';
    l.write_database();
    std::cout << '\n' << "RHS DATABASE:" << '\n';
    r.write_database();
#endif

    index_database<RecordType> rval = l;

    for(const RecordType& rec : r)
      {
        typename index_database<RecordType>::const_iterator it = rval.find(rec.get_label());

        if(it != rval.end()) throw index_exception(rec.get_label());

        rval.emplace_back(std::make_pair(rec.get_label(), std::make_shared<RecordType>(rec)));
      }

#ifdef DEBUG_INDEX_DATABASE
    std::cout << '\n' << "COMBINED DATABASE:" << '\n';
    rval.write_database();
    std::cout << '\n' << "********" << '\n';
#endif

    return (rval);
  }


#endif //CPPTRANSPORT_INDEX_DATABASE_H
