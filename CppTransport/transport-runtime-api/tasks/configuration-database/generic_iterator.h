//
// Created by David Seery on 17/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __generic_iterator_H_
#define __generic_iterator_H_


#include <type_traits>
#include <iterator>


namespace transport
  {

    namespace configuration_database
      {

        template <typename DatabaseType, typename RecordType, bool is_const_iterator=true >
        class generic_record_iterator: public std::iterator< std::bidirectional_iterator_tag, RecordType >
          {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const RecordType&, RecordType& >::type record_reference_type;

            //! set up an alias for a pointer to each element
            typedef typename std::conditional< is_const_iterator, const RecordType*, RecordType* >::type record_pointer_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, typename DatabaseType::const_iterator, typename DatabaseType::iterator>::type record_iterator_type;

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
              : it(i)
              {
              }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_record_iterator(const generic_record_iterator<DatabaseType, RecordType, false>& obj)
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
            friend class generic_record_iterator<DatabaseType, RecordType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            record_iterator_type it;

          };


        // CONFIGURATION ITERATOR (differs only in dereference behaviour)


        template <typename DatabaseType, typename ConfigType, bool is_const_iterator=true>
        class generic_config_iterator: public std::iterator< std::bidirectional_iterator_tag, ConfigType >
          {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const ConfigType&, ConfigType& >::type config_reference_type;

            //! set up an alias for a pointer to each element
            typedef typename std::conditional< is_const_iterator, const ConfigType*, ConfigType* >::type config_pointer_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, typename DatabaseType::const_iterator, typename DatabaseType::iterator>::type config_iterator_type;

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
              : it(i)
              {
              }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_config_iterator(const generic_config_iterator<DatabaseType, ConfigType, false>& obj)
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
            friend class generic_config_iterator<DatabaseType, ConfigType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            config_iterator_type it;

          };


		    // VALUE ITERATOR


        template <typename DatabaseType, typename ValueType, bool is_const_iterator=true >
        class generic_value_iterator: public std::iterator< std::bidirectional_iterator_tag, ValueType >
	        {

          private:

            //! set up an alias for a reference to each element.
            //! there are const- and non-const-versions
            typedef typename std::conditional< is_const_iterator, const ValueType&, ValueType& >::type value_reference_type;

            //! set up an alias for a pointer to each element
            typedef typename std::conditional< is_const_iterator, const ValueType*, ValueType* >::type value_pointer_type;

            //! set up an alias for the underlying iterator
            typedef typename std::conditional< is_const_iterator, typename DatabaseType::const_iterator, typename DatabaseType::iterator>::type record_iterator_type;

          public:

            //! setup an alias for the value type
            typedef typename std::conditional< is_const_iterator, const ValueType, ValueType >::type type;

          public:

            //! default constructor
            generic_value_iterator()
	            {
	            }

            //! value constructor
            generic_value_iterator(record_iterator_type i)
	            : it(i)
	            {
	            }

            //! copy constructor - allows for implicit conversion from a regular iterator to a const iterator
            generic_value_iterator(const generic_value_iterator<DatabaseType, ValueType, false>& obj)
	            : it(obj.it)
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
            value_reference_type operator*()
	            {
                return((*this->it->second).get_value());
	            }

            //! provide member access
            value_pointer_type operator->()
	            {
                return(&((*this->it->second).get_value()));
	            }

            //! deference pointer to get value (const version)
            const value_reference_type operator*() const
              {
                return((*this->it->second).get_value());
              }

            //! provide member access (const version)
            const value_pointer_type operator->() const
              {
                return(&((*this->it->second).get_value()));
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
            friend class generic_value_iterator<DatabaseType, ValueType, true>;


            // INTERNAL DATA

          private:

            //! current value of iterator
            record_iterator_type it;

	        };

      }

  }


#endif //__generic_iterator_H_
