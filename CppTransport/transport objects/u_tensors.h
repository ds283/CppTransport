//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __u_tensors_H_
#define __u_tensors_H_

#include <iostream>

#include "parse_tree.h"

class u_tensors
  {
    public:
      u_tensors(script* r) : root(r) {}

      //  CALCULATE DERIVED QUANTITIES

      std::vector<GiNaC::ex>                                compute_sr_u();

      std::vector< std::vector<GiNaC::ex> >                 compute_u2  ();

      std::vector< std::vector< std::vector<GiNaC::ex> > >  compute_u3  ();

    private:
      const script* root;
  };


#endif //__u_tensors_H_
