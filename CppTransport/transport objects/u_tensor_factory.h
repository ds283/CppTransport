//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __u_tensor_factory_H_
#define __u_tensor_factory_H_

#include <iostream>

#include "parse_tree.h"
#include "ginac/basic.h"

class u_tensor_factory
  {
    public:
      u_tensor_factory(script* r);

      //  CALCULATE DERIVED QUANTITIES

      std::vector<GiNaC::ex>                                compute_sr_u();

      std::vector<GiNaC::ex>                                compute_u1  ();

      std::vector< std::vector<GiNaC::ex> >                 compute_u2  ();

      std::vector< std::vector< std::vector<GiNaC::ex> > >  compute_u3  ();

    private:
      script*                          root;

      const unsigned int               num_fields;
      const GiNaC::symbol              M_Planck;
      const GiNaC::ex                  V;

      const std::vector<GiNaC::symbol> field_list;
      const std::vector<GiNaC::symbol> deriv_list;

    GiNaC::ex epsilon();
  };


#endif //__u_tensor_factory_H_
