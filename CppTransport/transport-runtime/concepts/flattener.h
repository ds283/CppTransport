//
// Created by David Seery on 28/12/2013.
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


#ifndef CPPTRANSPORT_FLATTENER_H
#define CPPTRANSPORT_FLATTENER_H


namespace transport
  {

    class abstract_flattener
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! declare virtual destructor so derived class destructors get called correctly
        virtual ~abstract_flattener() = default;

        // INTERFACE: INDEX-FLATTENING FUNCTIONS

      public:

        virtual unsigned int flatten(unsigned int a)                                          const = 0;
        virtual unsigned int flatten(unsigned int a, unsigned int b)                          const = 0;
        virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)          const = 0;

        virtual unsigned int fields_flatten(unsigned int a)                                   const = 0;
        virtual unsigned int fields_flatten(unsigned int a, unsigned int b)                   const = 0;
        virtual unsigned int fields_flatten(unsigned int a, unsigned int b, unsigned int c)   const = 0;

        virtual unsigned int tensor_flatten(unsigned int a, unsigned int b)                   const = 0;

        // INTERFACE: INDEX TRAITS

        virtual unsigned int species(unsigned int a)                                          const = 0;
        virtual unsigned int momentum(unsigned int a)                                         const = 0;
        virtual unsigned int is_field(unsigned int a)                                         const = 0;
        virtual unsigned int is_momentum(unsigned int a)                                      const = 0;

      };


    namespace flatten_impl
      {

        constexpr unsigned int flatten(unsigned int a, unsigned int N)
          {
            return(a);
          }

        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int N)
          {
            return(2*N*a + b);
          }

        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c, unsigned int N)
          {
            return(2*N*2*N*a + 2*N*b + c);
          }

        constexpr unsigned int fields_flatten(unsigned int a, unsigned int N)
          {
            return(a);
          }

        constexpr unsigned int fields_flatten(unsigned int a, unsigned int b, unsigned int N)
          {
            return(N*a + b);
          }

        constexpr unsigned int fields_flatten(unsigned int a, unsigned int b, unsigned int c, unsigned int N)
          {
            return(N*N*a + N*b + c);
          }

        constexpr unsigned int tensor_flatten(unsigned int a, unsigned int b)
          {
            return(2*a + b);
          }

        constexpr unsigned int species(unsigned int a, unsigned int N)
          {
            return(a >= N ? a-N : a);
          }

        constexpr unsigned int momentum(unsigned int a, unsigned int N)
          {
            return(a >= N ? a : a+N);
          }

        constexpr bool is_field(unsigned int a, unsigned int N)
          {
            return(a < N);
          }

        constexpr bool is_momentum(unsigned int a, unsigned int N)
          {
            return(a >= N && a < 2*N);
          }

      }   // namespace flatten_impl


  }   // namespace transport


#endif //CPPTRANSPORT_FLATTENER_H
