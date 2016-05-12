//
// Created by David Seery on 13/08/15.
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


#ifndef CPPTRANSPORT_GENERIC_CONFIG_ITERATOR_H
#define CPPTRANSPORT_GENERIC_CONFIG_ITERATOR_H


#include <type_traits>
#include <iterator>


namespace transport
  {

    namespace configuration_database
      {

        // CONFIGURATION ITERATOR

        template <typename Iterator, typename ConstIterator, typename ConfigType, bool is_const_iterator=true>
        class generic_config_iterator: public std::iterator< std::bidirectional_iterator_tag, ConfigType >
          {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const ConfigType&, ConfigType& >::type config_reference_type;

            //! set up an alias for a pointer to each element
            typedef typename std::conditional< is_const_iterator, const ConfigType*, ConfigType* >::type config_pointer_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, ConstIterator, Iterator >::type config_iterator_type;

          public:

            //! setup an alias for the value type
            typedef typename std::conditional< is_const_iterator, const ConfigType, ConfigType >::type type;

          public:

            //! default constructor
            generic_config_iterator()
              {
              }

            //! value constructor
            generic_config_iterator(config_iterator_type i)
              : it(std::move(i))
              {
              }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_config_iterator(const generic_config_iterator<Iterator, ConstIterator, ConfigType, false>& obj)
              : it(obj.it)
              {
              }

            //! equality comparison
            bool operator==(const generic_config_iterator& obj) const
              {
                return(this->it == obj.it);
              }

            //! inequality comparison
            bool operator!=(const generic_config_iterator& obj) const
              {
                return(this->it != obj.it);
              }

            //! deference pointer to get value
            config_reference_type operator*()
              {
                return(*(this->it->second));
              }

            //! provide member access
            config_pointer_type operator->()
              {
                return(&(*(this->it->second)));
              }

            //! deference pointer to get value (const version)
            const config_reference_type operator*() const
              {
                return(*this->it->second);
              }

            //! provide member access (const version)
            const config_pointer_type operator->() const
              {
                return(&(*(this->it->second)));
              }

            //! prefix decrement
            generic_config_iterator& operator--()
              {
                --this->it;
                return(*this);
              }

            //! postfix decrement
            generic_config_iterator operator--(int)
              {
                const generic_config_iterator old(*this);
                --this->it;
                return(old);
              }

            //! prefix increment
            generic_config_iterator& operator++()
              {
                ++this->it;
                return(*this);
              }

            //! postfix increment
            generic_config_iterator operator++(int)
              {
                const generic_config_iterator old(*this);
                ++this->it;
                return(old);
              }

            // make generic_record_iterator<true> a friend class of generic_record_iterator<false>, so that
            // the copy constructor can access its private member variables
            friend class generic_config_iterator<Iterator, ConstIterator, ConfigType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            config_iterator_type it;

          };

      }   // namespace configuration_database

  }   // namespace transport


#endif //CPPTRANSPORT_GENERIC_CONFIG_ITERATOR_H
