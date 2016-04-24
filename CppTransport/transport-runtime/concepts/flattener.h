//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef __flattener_H_
#define __flattener_H_


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


#endif //__flattener_H_
