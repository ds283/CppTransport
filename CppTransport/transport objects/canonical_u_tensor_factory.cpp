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
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u1(Hsq, eps));
  }


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector<GiNaC::ex> rval;

    // set up GiNaC symbols to represent epsilon = -dot(H)/H^2 and H^2

    for(int i = 0; i < this->num_fields; i++)
      {
        rval.push_back(this->deriv_list[i]);
      }
    for(int i = 0; i < this-> num_fields; i++)
      {
        GiNaC::ex d_deriv = -(3-eps)*this->deriv_list[i] - diff(this->V, this->field_list[i])/Hsq;
        rval.push_back(d_deriv);
      }

    return(rval);
  }


std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_u2(GiNaC::symbol& k, GiNaC::symbol& a)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u2(k, a, Hsq, eps));
  }

std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_u2(GiNaC::symbol& k, GiNaC::symbol& a,
  GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector<GiNaC::ex> > rval(2*this->num_fields);

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        rval[i].resize(2*this->num_fields);
      }

    // set up GiNaC symbols to represent epsilon and H^2

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
                        c -= (k*k) / (a*a * Hsq);
                      }

                    GiNaC::ex Vab = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i-this->num_fields]), this->field_list[j]);
                    GiNaC::ex Va  = GiNaC::diff(this->V, this->field_list[i-this->num_fields]);
                    GiNaC::ex Vb  = GiNaC::diff(this->V, this->field_list[j]);

                    c -= Vab/Hsq;
                    c -= (3-eps)*this->deriv_list[i-this->num_fields]*this->deriv_list[j]/pow(this->M_Planck,2);
                    c -= 1/(pow(this->M_Planck,2)*Hsq)*(this->deriv_list[i-this->num_fields]*Vb + this->deriv_list[j]*Va);
                  }
                else
                  {
                    if(i == j)
                      {
                        c = (eps - 3);
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


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_u3(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u3(k1, k2, k3, a, Hsq, eps));
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_u3(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
  GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval(2*this->num_fields);

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        rval[i].resize(2*this->num_fields);
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            rval[i][j].resize(2*this->num_fields);
          }
      }

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            for(int k = 0; k < 2*this->num_fields; k++)
              {
                GiNaC::ex c = 0;

                if(i < this->num_fields)            // first index is a field
                  {
                    if(j < this->num_fields)          // second index is a field
                      {
                        if(k < this->num_fields)        // and third index is a field
                          {
                            c = -(1/2) * this->compute_B(j, k2, k, k3, i, k1);
                          }
                        else                            // third index is a momentum
                          {
                            c = -(1/2) * this->compute_C(i, k1, j, -k2, k, k3);
                          }
                      }
                    else                              // second index is a momentum
                      {
                        if(k < this->num_fields)        // and third index is a field
                          {
                            c = -(1/2) * this->compute_C(i, k1, k, -k3, j, k2);
                          }
                        else                            // third index is a momentum
                          {
                            c = 0;
                          }
                      }
                  }
                else                                // first index is a momentum
                  {
                    if(j < this->num_fields)          // second index is a field
                      {
                        if(k < this->num_fields)        // and third index is a field
                          {
                            c = (3/2) * this->compute_A(i, -k1, j, k2, k, k3);
                          }
                        else                            // third index is a momentum
                          {
                            c = (1/2) * this->compute_B(i, -k1, j, k2, k, -k3);
                          }
                      }
                    else                              // second index is a momentum
                      {
                        if(k < this->num_fields)        // and third index is a field
                          {
                            c = (1/2) * this->compute_B(i, -k1, k, k3, j, -k2);
                          }
                        else                            // third index is a momentum
                          {
                            c = (1/2) * this->compute_C(j, -k2, k, -k3, i, -k1);
                          }
                      }
                  }

                rval[i][j][k] = c;
              }
          }
      }

    return(rval);
  }


// *****************************************************************************


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_zeta_xfm_1()
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_zeta_xfm_1(Hsq, eps));
  }


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_zeta_xfm_1(GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector<GiNaC::ex> rval;

    GiNaC::ex dotH    = -eps*Hsq;

    for(int i = 0; i < this->num_fields; i++)
      {
        rval.push_back( -(1/(2*dotH)) * diff(Hsq, this->field_list[i]) );
      }
    for(int i = 0; i < this->num_fields; i++)
      {
        rval.push_back( -(1/(2*dotH)) * diff(Hsq, this->deriv_list[i]) );
      }

    return(rval);
  }


std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_zeta_xfm_2()
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_zeta_xfm_2(Hsq, eps));
  }


std::vector< std::vector<GiNaC::ex> > canonical_u_tensor_factory::compute_zeta_xfm_2(GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector<GiNaC::ex> > rval(2*this->num_fields);

    GiNaC::ex dotH    = -eps*Hsq;

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        rval[i].resize(2*this->num_fields);
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            rval[i][j] = 0;
          }
      }

    GiNaC::ex p_sum(0);
    for(int i = 0; i < this->num_fields; i++)
      {
        p_sum += 1/(2*dotH) * diff(1/(2*dotH), this->field_list[i]) * this->deriv_list[i];
      }
    for(int i = 0; i < this->num_fields; i++)
      {
        GiNaC::ex dXdN = (eps-3)*this->deriv_list[i] - diff(this->V, this->field_list[i])/Hsq;
        p_sum += 1/(2*dotH) * diff(1/(2*dotH), this->deriv_list[i]) * dXdN;
      }

    #define COORDINATE(i) (i < this->num_fields ? this->field_list[i] : this->deriv_list[i-this->num_fields])
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            rval[i][j] = -1/(2*dotH) * diff(diff(Hsq, COORDINATE(i)), COORDINATE(j))
              - diff(1/(2*dotH), COORDINATE(i)) * diff(Hsq, COORDINATE(j))
              - diff(1/(2*dotH), COORDINATE(j)) * diff(Hsq, COORDINATE(i))
              + p_sum * diff(Hsq, COORDINATE(i)) * diff(Hsq, COORDINATE(j));
          }
      }

    return(rval);
  }


// *****************************************************************************


GiNaC::ex canonical_u_tensor_factory::compute_Hsq()
  {
    GiNaC::ex eps = this->compute_eps();

    GiNaC::ex Hsq = this->V / (GiNaC::pow(this->M_Planck,2)*(3-eps));

    return(Hsq);
  }


GiNaC::ex canonical_u_tensor_factory::compute_eps()
  {
    GiNaC::ex sum_momenta_sq(0);

    for(int i = 0; i < this->deriv_list.size(); i++)
      {
        sum_momenta_sq += GiNaC::pow(this->deriv_list[i], 2);
      }

    GiNaC::ex eps = sum_momenta_sq/(2*GiNaC::pow(this->M_Planck,2));

    return(eps);
  }


// *****************************************************************************


GiNaC::ex canonical_u_tensor_factory::compute_A(unsigned int i, GiNaC::symbol& k1,
  unsigned int j, GiNaC::symbol& k2,
  unsigned int k, GiNaC::symbol& k3,
  GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    GiNaC::ex Vijk = GiNaC::diff(GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[j]), this->field_list[k]);
    GiNaC::ex Vij  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[j]);
    GiNaC::ex Vik  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[k]);
    GiNaC::ex Vjk  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[j]), this->field_list[k]);

    GiNaC::ex xi_i = this->compute_xi(i);
    GiNaC::ex xi_j = this->compute_xi(j);
    GiNaC::ex xi_k = this->compute_xi(k);
  }


GiNaC::ex canonical_u_tensor_factory::compute_B(unsigned int i, GiNaC::symbol& k1,
  unsigned int j, GiNaC::symbol& k2,
  unsigned int k, GiNaC::symbol& k3,
  GiNaC::ex& Hsq, GiNaC::ex& eps)
  {

  }


GiNaC::ex canonical_u_tensor_factory::compute_C(unsigned int i, GiNaC::symbol& k1,
  unsigned int j, GiNaC::symbol& k2,
  unsigned int k, GiNaC::symbol& k3,
  GiNaC::ex& Hsq, GiNaC::ex& eps)
  {

  }