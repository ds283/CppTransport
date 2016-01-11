//
// Created by David Seery on 13/08/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __generic_value_iterator_H_
#define __generic_value_iterator_H_


#include <type_traits>
#include <iterator>


namespace transport
  {

    namespace configuration_database
      {

		    // VALUE ITERATOR

        template <typename Iterator, typename ConstIterator, typename ValueType, bool is_const_iterator=true >
        class generic_value_iterator: public std::iterator< std::bidirectional_iterator_tag, ValueType >
	        {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const ValueType, ValueType >::type value_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, ConstIterator, Iterator >::type value_iterator_type;

          public:

            //! setup an alias for the value type
            typedef typename std::conditional< is_const_iterator, const ValueType, ValueType >::type type;

          public:

            //! default constructor
            generic_value_iterator()
              : offset(0.0)
	            {
	            }

            //! value constructor
            generic_value_iterator(value_iterator_type i, ValueType o)
	            : it(std::move(i)),
                offset(o)
	            {
	            }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_value_iterator(const generic_value_iterator<Iterator, ConstIterator, ValueType, false>& obj)
	            : it(obj.it),
                offset(obj.offset)
	            {
	            }

            //! equality comparison
            bool operator==(const generic_value_iterator& obj) const
	            {
                return(this->it == obj.it);
	            }

            //! inequality comparison
            bool operator!=(const generic_value_iterator& obj) const
	            {
                return(this->it != obj.it);
	            }

            //! deference pointer to get value
            value_type operator*()
	            {
                return((*this->it->second).get_value() - this->offset);
	            }

            //! deference pointer to get value (const version)
            const value_type operator*() const
              {
                return((*this->it->second).get_value() - this->offset);
              }

            //! prefix decrement
            generic_value_iterator& operator--()
	            {
                --this->it;
                return(*this);
	            }

            //! postfix decrement
            generic_value_iterator operator--(int)
	            {
                const generic_value_iterator old(*this);
                --this->it;
                return(old);
	            }

            //! prefix increment
            generic_value_iterator& operator++()
	            {
                ++this->it;
                return(*this);
	            }

            //! postfix increment
            generic_value_iterator operator++(int)
	            {
                const generic_value_iterator old(*this);
                ++this->it;
                return(old);
	            }

            // make generic_record_iterator<true> a friend class of generic_record_iterator<false>, so that
            // the copy constructor can access its private member variables
            friend class generic_value_iterator<Iterator, ConstIterator, ValueType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            value_iterator_type it;

		        //! offset to apply to dereferenced values
		        const ValueType offset;

	        };

      }   // namespace configuration_database

  }   // namespace transport


#endif //__generic_value_iterator_H_
