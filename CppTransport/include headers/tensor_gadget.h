//
// Created by David Seery on 29/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//
#ifndef __CPP_TRANSPORT_TENSOR_GADGET_H_
#define __CPP_TRANSPORT_TENSOR_GADGET_H_

#include <vector>

namespace transport
  {
      // TENSOR GADGET
      // -- provide A, B & C tensors to the data container classes

      // abstract case - must be overridden by each model
      template <typename number>
      class tensor_gadget
        {
          public:
            tensor_gadget(number Mp, const std::vector<number>& ps) : M_Planck(Mp), parameters(ps) {}

            virtual std::vector< std::vector< std::vector<number> > >
              A(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N) = 0;
            virtual std::vector< std::vector< std::vector<number> > >
              B(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N) = 0;
            virtual std::vector< std::vector< std::vector<number> > >
              C(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N) = 0;

          protected:
            std::vector<number>   parameters;
            number                M_Planck;
        };

  }   // namespace transport


#endif // __CPP_TRANSPORT_TENSOR_GADGET_H
