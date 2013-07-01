//
// Created by David Seery on 01/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include "u_tensor_factory.h"


// *****************************************************************************


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_sr_u()
  {
    std::vector<GiNaC::ex> rval;

    for(int i = 0; i < this->num_fields; i++)
      {
        GiNaC::ex sr_u = - GiNaC::diff(this->V, this->field_list[i]) * GiNaC::pow(this->M_Planck, 2) / (this->V);

        rval.push_back(sr_u);
      }

    return(rval);
  }


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_u1()
  {
    std::vector<GiNaC::ex> rval;

    // set up a GiNaC symbol to represent epsilon = -dot(H)/H^2

    GiNaC::ex epsilon = this->epsilon();

    for(int i = 0; i < this->num_fields; i++)
      {
        rval.push_back(this->deriv_list[i]);
      }
    for(int i = 0; i < this-> num_fields; i++)
      {
        GiNaC::ex d_deriv = -(3-epsilon)*this->deriv_list[i] - diff(this->V, this->field_list[i]);
        rval.push_back(d_deriv);
      }

    return(rval);
  }


std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_u2()
  {
    std::vector< std::vector<GiNaC::ex> > rval;

    return(rval);
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_u3()
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval;

    return(rval);
  }


// *****************************************************************************


GiNaC::ex canonical_u_tensor_factory::epsilon()
  {
    GiNaC::ex sum_momenta_sq(0);

    for(int i = 0; i < this->deriv_list.size(); i++)
      {
        sum_momenta_sq += GiNaC::pow(this->deriv_list[i], 2);
      }

    GiNaC::ex Hsq = (sum_momenta_sq /2 + this->V)/(3*GiNaC::pow(this->M_Planck, 2));
    GiNaC::ex eps = 3 * (sum_momenta_sq /2) / (sum_momenta_sq /2 + this->V/Hsq);

    return(eps);
  }
