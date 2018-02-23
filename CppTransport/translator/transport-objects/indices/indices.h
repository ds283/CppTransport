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


#include <stdexcept>

#include "index_literal.h"

#include "msg_en.h"


class tensor_index_error : public std::runtime_error
  {
    
    // CONSTRUCTOR, DESTRUCTOR
  
  public:
    
    //! constructor
    tensor_index_error(std::string msg)
      : std::runtime_error(std::move(msg))
      {
      }
    
    //! destructor is default
    ~tensor_index_error() = default;
    
  };


namespace tensor_index_impl
  {

    template <unsigned int type>
    struct tensor_index_type
      {
        enum { IndexTypeIdentifier = type };
      };


    // declare template functions which will need to be friended in index declaration
    
    // forward-declare index class
    template <typename IndexType> class tensor_index;

    //! allow < comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator<(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r);

    //! allow <= comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator<=(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r);

    //! allow equality comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator==(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r);

    //! allow inequality comparison between two different indices of the same class
    template <typename IndexType>
    constexpr bool operator!=(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r);


    template <typename IndexType>
    class tensor_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! value constructor
        constexpr tensor_index(unsigned int N, variance v = variance::none)
          : index_value(N),
            index_variance(v)
          {
            // TODO: consider validating that variance is non for parameter indices
          }


        // CONVERSIONS

      public:

        //! allow explicit cast to unsigned int
        explicit constexpr operator unsigned int() const { return(this->index_value); }

        //! get value
        unsigned int get() const { return this->index_value; }

        //! get variance
        variance get_variance() const { return this->index_variance; }
        
        //! convert to string
        std::string to_string() const;


        // INCREMENT, DECREMENT

      public:

        //! preincrement
        tensor_index& operator++()
          {
            ++this->index_value;
            return(*this);
          }

        //! predecrement
        tensor_index& operator--()
          {
            --this->index_value;
            return(*this);
          }

        //! postincrement
        tensor_index operator++(int)
          {
            const tensor_index old(*this);
            ++this->index_value;
            return(old);
          }

        //! postdecrement
        tensor_index operator--(int)
          {
            const tensor_index old(*this);
            --this->index_value;
            return(old);
          }


        // FRIEND DECLARATION FOR COMPARISON OPERATORS

      public:

        //! < comparison operator
        friend bool operator< <> (const tensor_index& l, const tensor_index& r);

        //! <= comparison operator
        friend bool operator<= <> (const tensor_index& l, const tensor_index& r);

        //! == comparison operator
        friend bool operator== <> (const tensor_index& l, const tensor_index& r);


        // INTERNAL DATA

      private:

        //! numerical value of index
        unsigned int index_value;
        
        //! variance of index
        variance index_variance;

      };


    template <typename IndexType>
    constexpr bool operator<(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r)
      {
        if(l.index_variance != r.index_variance) throw tensor_index_error(ERROR_MISMATCHED_TENSOR_INDICES);
        return l.index_value < r.index_value;
      }


    template <typename IndexType>
    constexpr bool operator<=(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r)
      {
        if(l.index_variance != r.index_variance) throw tensor_index_error(ERROR_MISMATCHED_TENSOR_INDICES);
        return l.index_value <= r.index_value;
      }


    template <typename IndexType>
    constexpr bool operator==(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r)
      {
        if(l.index_variance != r.index_variance) throw tensor_index_error(ERROR_MISMATCHED_TENSOR_INDICES);
        return l.index_value == r.index_value;
      }
    
    
    template <typename IndexType>
    std::string tensor_index<IndexType>::to_string() const
      {
        return ::to_string(this->index_variance) + std::to_string(this->index_value);
      }
    
    
    template <typename IndexType>
    constexpr bool operator!=(const tensor_index<IndexType>& l, const tensor_index<IndexType>& r)
      {
        return !(l==r);
      }


    using ParameterIndexClass  = tensor_index_impl::tensor_index_type<3>;
    using FieldSpaceIndexClass = tensor_index_impl::tensor_index_type<1>;
    using PhaseSpaceIndexClass = tensor_index_impl::tensor_index_type<2>;

  }   // namespace tensor_index_impl


using param_index = tensor_index_impl::tensor_index<tensor_index_impl::ParameterIndexClass>;
using field_index = tensor_index_impl::tensor_index<tensor_index_impl::FieldSpaceIndexClass>;
using phase_index = tensor_index_impl::tensor_index<tensor_index_impl::PhaseSpaceIndexClass>;


#endif //CPPTRANSPORT_INDICES_H
