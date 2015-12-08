//
// Created by David Seery on 17/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __generic_record_iterator_H_
#define __generic_record_iterator_H_


#include <type_traits>
#include <iterator>


namespace transport
  {

    namespace configuration_database
      {

        // RECORD ITERATOR

        template <typename Iterator, typename ConstIterator, typename RecordType, bool is_const_iterator=true >
        class generic_record_iterator: public std::iterator< std::bidirectional_iterator_tag, RecordType >
          {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const RecordType&, RecordType& >::type record_reference_type;

            //! set up an alias for a pointer to each element
            typedef typename std::conditional< is_const_iterator, const RecordType*, RecordType* >::type record_pointer_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, ConstIterator, Iterator >::type record_iterator_type;

          public:

            //! setup an alias for the value type
            typedef typename std::conditional< is_const_iterator, const RecordType, RecordType >::type type;

          public:

            //! default constructor
            generic_record_iterator()
              {
              }

            //! value constructor
            generic_record_iterator(record_iterator_type i)
              : it(std::move(i))
              {
              }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_record_iterator(const generic_record_iterator<Iterator, ConstIterator, RecordType, false>& obj)
              : it(obj.it)
              {
              }

            //! equality comparison
            bool operator==(const generic_record_iterator& obj) const
              {
                return(this->it == obj.it);
              }

            //! inequality comparison
            bool operator!=(const generic_record_iterator& obj) const
              {
                return(this->it != obj.it);
              }

            //! deference pointer to get value
            record_reference_type operator*()
              {
                return(this->it->second);
              }

            //! provide member access
            record_pointer_type operator->()
              {
                return(&this->it->second);
              }

            //! deference pointer to get value (const version)
            const record_reference_type operator*() const
              {
                return(this->it->second);
              }

            //! provide member access (const version)
            const record_pointer_type operator->() const
              {
                return(&this->it->second);
              }

            //! prefix decrement
            generic_record_iterator& operator--()
              {
                --this->it;
                return(*this);
              }

            //! postfix decrement
            generic_record_iterator operator--(int)
              {
                const generic_record_iterator old(*this);
                --this->it;
                return(old);
              }

            //! prefix increment
            generic_record_iterator& operator++()
              {
                ++this->it;
                return(*this);
              }

            //! postfix increment
            generic_record_iterator operator++(int)
              {
                const generic_record_iterator old(*this);
                ++this->it;
                return(old);
              }

            // make generic_record_iterator<true> a friend class of generic_record_iterator<false>, so that
            // the copy constructor can access its private member variables
            friend class generic_record_iterator<Iterator, ConstIterator, RecordType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            record_iterator_type it;

          };

      }   // namespace configuration_database

  }   // namespace transport


#endif //__generic_record_iterator_H_
