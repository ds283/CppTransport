//
// Created by David Seery on 12/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_GAUGE_XFM_GADGET_H_
#define __CPP_TRANSPORT_GAUGE_XFM_GADGET_H_

#include <vector>

namespace transport
  {
    // GAUGE TRANSFORMATION GADGET

    // abstract case - must be overridden by each model
    template <typename number>
    class gauge_xfm_gadget
      {
      public:
        gauge_xfm_gadget(number Mp, const std::vector<number>& ps) : M_Planck(Mp), parameters(ps)
          {
          }

        virtual gauge_xfm_gadget *clone() = 0;

        virtual void compute_gauge_xfm_1(const std::vector<number>& __state,
                                         std::vector<number>& __dN)                 = 0;

        virtual void compute_gauge_xfm_2(const std::vector<number>& __state,
                                         std::vector< std::vector<number> >& __ddN) = 0;

      protected:
        std::vector<number> parameters;
        number              M_Planck;
      };

  }   // namespace transport


#endif // __CPP_TRANSPORT_GAUGE_XFM_GADGET_H
