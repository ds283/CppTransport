//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __flattener_H_
#define __flattener_H_


namespace transport
  {

    template <unsigned int N>
    class flattener
      {
        // CONSTRUCTOR, DESTRUCTOR

      public:
        virtual ~flattener()
          {
          }

        // INDEX-FLATTENING FUNCTIONS -- used by container classes

      protected:
        constexpr unsigned int flatten(unsigned int a)                                  const { return(a); };
        constexpr unsigned int flatten(unsigned int a, unsigned int b)                  const { return(2*N*a + b); };
        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)  const { return(2*N*2*N*a + 2*N*b + c); };

        // INDEX TRAITS -- used by container classes

        constexpr unsigned int species(unsigned int a)     const { return((a >= N) ? a-N : a); };
        constexpr unsigned int momentum(unsigned int a)    const { return((a >= N) ? a : a+N); };
        constexpr unsigned int is_field(unsigned int a)    const { return(a < N); }
        constexpr unsigned int is_momentum(unsigned int a) const { return(a >= N && a <= 2*N); }

        // macros to simplify application of these functions
        #define SPECIES(a)     this->species(a)
        #define MOMENTUM(a)    this->momentum(a)
        #define IS_FIELD(a)    this->is_field(a)
        #define IS_MOMENTUM(a) this->is_momentum(a)
        #define FLATTEN        this->flatten

      };

  }


#endif //__flattener_H_
