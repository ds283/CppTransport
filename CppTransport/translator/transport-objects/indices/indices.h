//
// Created by David Seery on 19/12/2015.
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

#ifndef CPPTRANSPORT_INDICES_H
#define CPPTRANSPORT_INDICES_H


namespace index_impl
  {

    template <unsigned int type>
    struct index_type
      {
        enum { IndexTypeIdentifier = type };
      };


    // declare template functions which will need to be friended in index declaration
    template <typename IndexType> class index;

    // allow < comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator<(const index<IndexType>& l, const index<IndexType>& r);

    //! allow <= comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator<=(const index<IndexType>& l, const index<IndexType>& r);

    //! allow equality comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator==(const index<IndexType>& l, const index<IndexType>& r);


    template <typename IndexType>
    class index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! value constructor
        constexpr index(unsigned int v)
          : index_value(v)
          {
          }


        // CONVERSIONS

      public:

        //! allow explicit cast to unsigned int
        explicit constexpr operator unsigned int() const { return(this->index_value); }


        // INCREMENT, DECREMENT

      public:

        //! preincrement
        index& operator++()
          {
            ++this->index_value;
            return(*this);
          }

        //! predecrement
        index& operator--()
          {
            --this->index_value;
            return(*this);
          }

        //! postincrement
        index operator++(int)
          {
            const index old(*this);
            ++this->index_value;
            return(old);
          }

        //! postdecrement
        index operator--(int)
          {
            const index old(*this);
            --this->index_value;
            return(old);
          }


        // FRIEND DECLARATION FOR COMPARISON OPERATORS

      public:

        //! < comparison operator
        friend bool operator< <> (const index& l, const index& r);

        //! <= comparison operator
        friend bool operator<= <> (const index& l, const index& r);

        //! == comparison operator
        friend bool operator== <> (const index& l, const index& r);


        // INTERNAL DATA

      private:

        // value of index
        unsigned int index_value;

      };


    template <typename IndexType>
    constexpr bool operator<(const index<IndexType>& l, const index<IndexType>& r)
      {
        return(l.index_value < r.index_value);
      }


    template <typename IndexType>
    constexpr bool operator<=(const index<IndexType>& l, const index<IndexType>& r)
      {
        return(l.index_value <= r.index_value);
      }


    template <typename IndexType>
    constexpr bool operator==(const index<IndexType>& l, const index<IndexType>& r)
      {
        return(l.index_value == r.index_value);
      }


    using ParameterIndexClass  = index_impl::index_type<3>;
    using FieldSpaceIndexClass = index_impl::index_type<1>;
    using PhaseSpaceIndexClass = index_impl::index_type<2>;

  }   // namespace index_impl


using param_index = index_impl::index<index_impl::ParameterIndexClass>;
using field_index = index_impl::index<index_impl::FieldSpaceIndexClass>;
using phase_index = index_impl::index<index_impl::PhaseSpaceIndexClass>;


#endif //CPPTRANSPORT_INDICES_H
