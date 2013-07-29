//
// Created by David Seery on 01/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include "u_tensor_factory.h"


// *****************************************************************************


#define SPECIES(z)     (z >= this->num_fields ? z - this->num_fields : z)
#define COORDINATE(z)  (z < this->num_fields ? this->field_list[z] : this->deriv_list[z-this->num_fields])

#define IS_FIELD(z)    (z >= 0 && z < this->num_fields)
#define IS_MOMENTUM(z) (z >= this->num_fields && z < 2*this->num_fields)

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

    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            GiNaC::ex c = 0;

            if(IS_FIELD(i) && IS_FIELD(j))
              {
                c = 0;
              }
            else if(IS_FIELD(i) && IS_MOMENTUM(j))
              {
                c = (SPECIES(i) == SPECIES(j) ? 1 : 0);
              }
            else if(IS_MOMENTUM(i) && IS_FIELD(j))
              {
                c = (SPECIES(i) == SPECIES(j) ? -(k*k) / (a*a * Hsq) : 0 );

                GiNaC::ex Vab = GiNaC::diff(GiNaC::diff(this->V, this->field_list[SPECIES(i)]), this->field_list[SPECIES(j)]);
                GiNaC::ex Va  = GiNaC::diff(this->V, this->field_list[SPECIES(i)]);
                GiNaC::ex Vb  = GiNaC::diff(this->V, this->field_list[SPECIES(j)]);

                c -= Vab/Hsq;
                c -= (3-eps)*this->deriv_list[SPECIES(i)]*this->deriv_list[SPECIES(j)]/pow(this->M_Planck,2);
                c -= 1/(pow(this->M_Planck,2)*Hsq)*(this->deriv_list[SPECIES(i)]*Vb + this->deriv_list[SPECIES(j)]*Va);

              }
            else if(IS_MOMENTUM(i) && IS_MOMENTUM(j))
              {
                c = (SPECIES(i) == SPECIES(j) ? (eps-3) : 0);
              }
            else
              {
                assert(false);
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

                // note that we flip the sign of momenta attached to the j, k components
                // compared to the analytic definition of the tensor
                // this accounts for integrating out the delta-functions when
                // contracting u3 with something else
                if(IS_FIELD(i) && IS_FIELD(j) && IS_FIELD(k))
                  {
                    c = -this->compute_B_component(SPECIES(j), k2, SPECIES(k), k3, SPECIES(i), k1, a, Hsq, eps, +1, -1, -1)/2;
                  }
                else if(IS_FIELD(i) && IS_FIELD(j) && IS_MOMENTUM(k))
                  {
                    c = -this->compute_C_component(SPECIES(i), k1, SPECIES(j), k2, SPECIES(k), k3, a, Hsq, eps, +1, +1, -1)/2;
                  }
                else if(IS_FIELD(i) && IS_MOMENTUM(j) && IS_FIELD(k))
                  {
                    c = -this->compute_C_component(SPECIES(i), k1, SPECIES(k), k3, SPECIES(j), k2, a, Hsq, eps, +1, +1, -1)/2;
                  }
                else if(IS_FIELD(i) && IS_MOMENTUM(j) && IS_MOMENTUM(k))
                  {
                    c = 0;
                  }
                else if(IS_MOMENTUM(i) && IS_FIELD(j) && IS_FIELD(k))
                  {
                    c = 3*this->compute_A_component(SPECIES(i), k1, SPECIES(j), k2, SPECIES(k), k3, a, Hsq, eps, -1, -1, -1)/2;
                  }
                else if(IS_MOMENTUM(i) && IS_FIELD(j) && IS_MOMENTUM(k))
                  {
                    c = this->compute_B_component(SPECIES(i), k1, SPECIES(j), k2, SPECIES(k), k3, a, Hsq, eps, -1, -1, +1)/2;
                  }
                else if(IS_MOMENTUM(i) && IS_MOMENTUM(j) && IS_FIELD(k))
                  {
                    c = this->compute_B_component(SPECIES(i), k1, SPECIES(k), k3, SPECIES(j), k2, a, Hsq, eps, -1, -1, +1)/2;
                  }
                else if(IS_MOMENTUM(i) && IS_MOMENTUM(j) && IS_MOMENTUM(k))
                  {
                    c = this->compute_C_component(SPECIES(j), k2, SPECIES(k), k3, SPECIES(i), k1, a, Hsq, eps, -1, +1, +1)/2;
                  }
                else
                  {
                    assert(false);
                  }

                rval[i][j][k] = c;
              }
          }
      }

    return(rval);
  }


// *****************************************************************************


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_A(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_A(k1, k2, k3, a, Hsq, eps));
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_A(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval(this->num_fields);

    for(int i = 0; i < this->num_fields; i++)
      {
        rval[i].resize(this->num_fields);
        for(int j = 0; j < this->num_fields; j++)
          {
            rval[i][j].resize(this->num_fields);
          }
      }

    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                rval[i][j][k] = this->compute_A_component(i, k1, j, k2, k, k3, a, Hsq, eps, +1, +1, +1);
              }
          }
      }

    return(rval);
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_B(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_B(k1, k2, k3, a, Hsq, eps));
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_B(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval(this->num_fields);

    for(int i = 0; i < this->num_fields; i++)
      {
        rval[i].resize(this->num_fields);
        for(int j = 0; j < this->num_fields; j++)
          {
            rval[i][j].resize(this->num_fields);
          }
      }

    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                rval[i][j][k] = this->compute_B_component(i, k1, j, k2, k, k3, a, Hsq, eps, +1, +1, +1);
              }
          }
      }

    return(rval);

  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_C(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_C(k1, k2, k3, a, Hsq, eps));
  }


std::vector< std::vector< std::vector<GiNaC::ex> > > canonical_u_tensor_factory::compute_C(GiNaC::symbol& k1,
  GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    std::vector< std::vector< std::vector<GiNaC::ex> > > rval(this->num_fields);

    for(int i = 0; i < this->num_fields; i++)
      {
        rval[i].resize(this->num_fields);
        for(int j = 0; j < this->num_fields; j++)
          {
            rval[i][j].resize(this->num_fields);
          }
      }

    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                rval[i][j][k] = this->compute_C_component(i, k1, j, k2, k, k3, a, Hsq, eps, +1, +1, +1);
              }
          }
      }

    return(rval);

  }


// *****************************************************************************


std::vector<GiNaC::ex> canonical_u_tensor_factory::compute_zeta_xfm_1()
  {
    std::vector<GiNaC::ex> rval;

    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

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
    std::vector< std::vector<GiNaC::ex> > rval(2*this->num_fields);

    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

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


GiNaC::ex canonical_u_tensor_factory::compute_A_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sign, int k3_sign)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    GiNaC::ex Vijk = GiNaC::diff(GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[j]), this->field_list[k]);
    GiNaC::ex Vij  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[j]);
    GiNaC::ex Vik  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[k]);
    GiNaC::ex Vjk  = GiNaC::diff(GiNaC::diff(this->V, this->field_list[j]), this->field_list[k]);

    GiNaC::ex xi_i = this->compute_xi(i, Hsq, eps);
    GiNaC::ex xi_j = this->compute_xi(j, Hsq, eps);
    GiNaC::ex xi_k = this->compute_xi(k, Hsq, eps);

    GiNaC::ex k1dotk2 = k1_sign*k2_sign * (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = k1_sign*k3_sign * (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = k2_sign*k3_sign * (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = -(Vijk / Hsq)/3;

    c -=   (this->deriv_list[i]/(2*pow(this->M_Planck,2))) * (Vjk / Hsq)/3
         + (this->deriv_list[j]/(2*pow(this->M_Planck,2))) * (Vik / Hsq)/3
         + (this->deriv_list[k]/(2*pow(this->M_Planck,2))) * (Vij / Hsq)/3;

    c +=   (this->deriv_list[i]*this->deriv_list[j]*xi_k)/(8*pow(this->M_Planck,4))/3
         + (this->deriv_list[i]*this->deriv_list[k]*xi_j)/(8*pow(this->M_Planck,4))/3
         + (this->deriv_list[j]*this->deriv_list[k]*xi_i)/(8*pow(this->M_Planck,4))/3;

    c +=   (this->deriv_list[i]*xi_j*xi_k)/(32*pow(this->M_Planck,4))/3
         + (this->deriv_list[j]*xi_i*xi_k)/(32*pow(this->M_Planck,4))/3
         + (this->deriv_list[k]*xi_i*xi_j)/(32*pow(this->M_Planck,4))/3;

    c += this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k]/(8*pow(this->M_Planck,4)) * (6 - 2*eps);

    if(j == k) c += this->deriv_list[i]/(2*pow(this->M_Planck,2)) * k2dotk3/(pow(a,2)*Hsq)/3;
    if(i == k) c += this->deriv_list[j]/(2*pow(this->M_Planck,2)) * k1dotk3/(pow(a,2)*Hsq)/3;
    if(i == j) c += this->deriv_list[k]/(2*pow(this->M_Planck,2)) * k1dotk2/(pow(a,2)*Hsq)/3;

    c -=   (this->deriv_list[i]*xi_j*xi_k)/(32*pow(this->M_Planck,2)) * k2dotk3 / (k2*k2*k3*k3) /3
         + (this->deriv_list[j]*xi_i*xi_k)/(32*pow(this->M_Planck,2)) * k1dotk3 / (k1*k1*k3*k3) /3
         + (this->deriv_list[k]*xi_i*xi_j)/(32*pow(this->M_Planck,2)) * k1dotk2 / (k1*k1*k2*k2) /3;

    return(c);
  }


GiNaC::ex canonical_u_tensor_factory::compute_B_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sign, int k3_sign)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    GiNaC::ex xi_i = this->compute_xi(i, Hsq, eps);
    GiNaC::ex xi_j = this->compute_xi(j, Hsq, eps);

    GiNaC::ex k1dotk2 = k1_sign*k2_sign * (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = k1_sign*k3_sign * (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = k2_sign*k3_sign * (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k]/(4*pow(this->M_Planck,4));

    c -=   this->deriv_list[i]*xi_j*this->deriv_list[k]/(8*pow(this->M_Planck,4))/2
         + this->deriv_list[j]*xi_i*this->deriv_list[k]/(8*pow(this->M_Planck,4))/2;

    if(j == k) c -= (xi_i / (2*pow(this->M_Planck,2))) * k1dotk2 / (k1*k1) /2;
    if(i == k) c -= (xi_j / (2*pow(this->M_Planck,2))) * k1dotk2 / (k2*k2) /2;

    c +=   this->deriv_list[i]*xi_j*this->deriv_list[k]/(8*pow(this->M_Planck,4)) * pow(k2dotk3/(k2*k3),2) /2
         + this->deriv_list[j]*xi_i*this->deriv_list[k]/(8*pow(this->M_Planck,4)) * pow(k1dotk3/(k1*k3),2) /2;

    return(c);
  }


GiNaC::ex canonical_u_tensor_factory::compute_C_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sign, int k3_sign)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    GiNaC::ex k1dotk2 = k1_sign*k2_sign * (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = k1_sign*k3_sign * (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = k2_sign*k3_sign * (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = 0;

    if (i == j) c -= this->deriv_list[k] / (2*pow(this->M_Planck, 2));

    c += this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k] / (8*pow(this->M_Planck,4));

    c -= this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k] / (8*pow(this->M_Planck,4)) * pow(k1dotk2 / (k1*k2), 2);

    if (j == k) c += (this->deriv_list[i] / pow(this->M_Planck, 2)) * k1dotk3 / (k1*k1) /2;
    if (i == k) c += (this->deriv_list[j] / pow(this->M_Planck, 2)) * k2dotk3 / (k2*k2) /2;

    return (c);
  }


GiNaC::ex canonical_u_tensor_factory::compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    GiNaC::ex c  = -6 * this->deriv_list[i];
    GiNaC::ex Vi = GiNaC::diff(this->V, this->field_list[i]);

    c -= 2 * Vi/Hsq;
    c += this->deriv_list[i] * 2*eps;

    return(c);
  }
