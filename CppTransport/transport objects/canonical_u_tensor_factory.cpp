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

    // set up GiNaC symbols to represent epsilon = -dot(H)/H^2 and H^2

    GiNaC::ex epsilon = this->epsilon();
    GiNaC::ex Hsq     = this->Hubble_square();

    for(int i = 0; i < this->num_fields; i++)
      {
        rval.push_back(this->deriv_list[i]);
      }
    for(int i = 0; i < this-> num_fields; i++)
      {
        GiNaC::ex d_deriv = -(3-epsilon)*this->deriv_list[i] - diff(this->V, this->field_list[i])/Hsq;
        rval.push_back(d_deriv);
      }

    return(rval);
  }


std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_u2()
  {
    std::vector< std::vector<GiNaC::ex> > rval(2*this->num_fields);
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        rval[i].resize(2*this->num_fields);
      }

    // set up GiNaC symbols to represent epsilon and H^2

    GiNaC::ex epsilon = this->epsilon();
    GiNaC::ex Hsq     = this->Hubble_square();

    GiNaC::symbol k("__k");
    GiNaC::symbol a("__a");

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            GiNaC::ex c = 0;

            if(i < this->num_fields)  // a field
              {
                if(j < this->num_fields)
                  {
                    c = 0;
                  }
                else
                  {
                    if(i == j - this->num_fields)
                      {
                        c = 1;
                      }
                    else
                      {
                        c = 0;
                      }
                  }
              }
            else                      // a momentum
              {
                if(j < this->num_fields)
                  {
                    c = 0;
                    if(i - this->num_fields == j)
                      {
                        c += (k*k) / (a*a * Hsq);
                      }

                    GiNaC::ex Vab = diff(diff(this->V, this->field_list[i - this->num_fields]), this->field_list[j]);
                    GiNaC::ex Va  = diff(this->V, this->field_list[i - this->num_fields]);
                    GiNaC::ex Vb  = diff(this->V, this->field_list[j]);

                    c += Vab/Hsq;
                    c += (3-epsilon)*this->deriv_list[i - this->num_fields]*this->deriv_list[j]/pow(this->M_Planck,2);
                    c += 1/(pow(this->M_Planck,2)*Hsq)*(this->deriv_list[i - this->num_fields]*Vb + this->deriv_list[j]*Va);
                  }
                else
                  {
                    if(i == j)
                      {
                        c = (3 - epsilon);
                      }
                    else
                      {
                        c = 0;
                      }
                  }
              }

            (rval[i])[j] = c;
          }
      }

    return(rval);
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_u3()
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval;

    return(rval);
  }


GiNaC::ex canonical_u_tensor_factory::compute_Hsq()
  {
    return(this->Hubble_square());
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


GiNaC::ex canonical_u_tensor_factory::Hubble_square()
  {
    GiNaC::ex sum_momenta_sq(0);

    for(int i = 0; i < this->deriv_list.size(); i++)
      {
        sum_momenta_sq += GiNaC::pow(this->deriv_list[i], 2);
      }

    GiNaC::ex Hsq = (sum_momenta_sq /2 + this->V)/(3*GiNaC::pow(this->M_Planck, 2));

    return(Hsq);
  }
