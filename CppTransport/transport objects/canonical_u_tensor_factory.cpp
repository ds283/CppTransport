//
// Created by David Seery on 01/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include "u_tensor_factory.h"


// *****************************************************************************


void canonical_u_tensor_factory::compute_sr_u(std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(1));

    for(int i = 0; i < this->num_fields; i++)
      {
        v[fl->flatten(i)] = - GiNaC::diff(this->V, this->field_list[i]) * GiNaC::pow(this->M_Planck, 2) / (this->V);
      }
  }


void canonical_u_tensor_factory::compute_u1(std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u1(Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(1));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        if(this->is_field(i))
          {
            v[fl->flatten(i)] = this->deriv_list[this->species(i)];
          }
        else if(this->is_momentum(i))
          {
            v[fl->flatten(i)] = -(3-eps)*this->deriv_list[this->species(i)] - diff(this->V, this->field_list[this->species(i)])/Hsq;
          }
        else
          {
            assert(false);
          }
      }
  }


void canonical_u_tensor_factory::compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u2(k, a, Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_u2(GiNaC::symbol& k, GiNaC::symbol& a,
                                            GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(2));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            GiNaC::ex c = 0;

            if(this->is_field(i) && this->is_field(j))
              {
                c = 0;
              }
            else if(this->is_field(i) && this->is_momentum(j))
              {
                c = (this->species(i) == this->species(j) ? 1 : 0);
              }
            else if(this->is_momentum(i) && this->is_field(j))
              {
                c = (this->species(i) == this->species(j) ? -(k*k) / (a*a * Hsq) : 0 );

                GiNaC::ex Vab = GiNaC::diff(GiNaC::diff(this->V, this->field_list[this->species(i)]), this->field_list[this->species(j)]);
                GiNaC::ex Va  = GiNaC::diff(this->V, this->field_list[this->species(i)]);
                GiNaC::ex Vb  = GiNaC::diff(this->V, this->field_list[this->species(j)]);

                c -= Vab/Hsq;
                c -= (3-eps)*this->deriv_list[this->species(i)]*this->deriv_list[this->species(j)]/pow(this->M_Planck,2);
                c -= 1/(pow(this->M_Planck,2)*Hsq)*(this->deriv_list[this->species(i)]*Vb + this->deriv_list[this->species(j)]*Va);
              }
            else if(this->is_momentum(i) && this->is_momentum(j))
              {
                c = (this->species(i) == this->species(j) ? (eps-3) : 0);
              }
            else
              {
                assert(false);
              }

            v[fl->flatten(i,j)] = c;
          }
      }
  }


void canonical_u_tensor_factory::compute_u3(GiNaC::symbol& k1,
                                            GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_u3(k1, k2, k3, a, Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_u3(GiNaC::symbol& k1,
                                            GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                                            GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(3));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            for(int k = 0; k < 2*this->num_fields; k++)
              {
                GiNaC::ex c = 0;

                // note that we flip the sign of momenta attached to the i, j, k components compared to the
                // analytic definition
                // this accounts for integrating out the delta-functions when contracting u3 with something else

                // factor of 2 from definition of 2nd order term in transport eq: dX/dN = u2.X + (1/2) u3.X.X + ...
                if(this->is_field(i) && this->is_field(j) && this->is_field(k))
                  {
                    c = -this->compute_B_component(this->species(j), k2, this->species(k), k3, this->species(i), k1, a, Hsq, eps);
                  }
                else if(this->is_field(i) && this->is_field(j) && this->is_momentum(k))
                  {
                    c = -this->compute_C_component(this->species(i), k1, this->species(k), k3, this->species(j), k2, a, Hsq, eps);
                  }
                else if(this->is_field(i) && this->is_momentum(j) && this->is_field(k))
                  {
                    c = -this->compute_C_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, Hsq, eps);
                  }
                else if(this->is_field(i) && this->is_momentum(j) && this->is_momentum(k))
                  {
                    c = 0;
                  }
                else if(this->is_momentum(i) && this->is_field(j) && this->is_field(k))
                  {
                    c = 3*this->compute_A_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, Hsq, eps);
                  }
                else if(this->is_momentum(i) && this->is_field(j) && this->is_momentum(k))
                  {
                    c = this->compute_B_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, Hsq, eps);
                  }
                else if(this->is_momentum(i) && this->is_momentum(j) && this->is_field(k))
                  {
                    c = this->compute_B_component(this->species(i), k1, this->species(k), k3, this->species(j), k2, a, Hsq, eps);
                  }
                else if(this->is_momentum(i) && this->is_momentum(j) && this->is_momentum(k))
                  {
                    c = this->compute_C_component(this->species(j), k2, this->species(k), k3, this->species(i), k1, a, Hsq, eps);
                  }
                else
                  {
                    assert(false);
                  }

                v[fl->flatten(i,j,k)] = c;
              }
          }
      }
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_A(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_A(k1, k2, k3, a, Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_A(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                           std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(3));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                v[fl->flatten(i,j,k)] = this->compute_A_component(i, k1, j, k2, k, k3, a, Hsq, eps);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_B(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_B(k1, k2, k3, a, Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_B(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                           std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(3));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                v[fl->flatten(i,j,k)] = this->compute_B_component(i, k1, j, k2, k, k3, a, Hsq, eps);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_C(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_C(k1, k2, k3, a, Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_C(GiNaC::symbol& k1,
                                           GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                           std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(3));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            for(int k = 0; k < this->num_fields; k++)
              {
                v[fl->flatten(i,j,k)] = this->compute_C_component(i, k1, j, k2, k, k3, a, Hsq, eps);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_M(std::vector<GiNaC::ex>& v, flattener* fl)
  {
    GiNaC::ex Hsq = this->compute_Hsq();
    GiNaC::ex eps = this->compute_eps();

    return(this->compute_M(Hsq, eps, v, fl));
  }


void canonical_u_tensor_factory::compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(2));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < this->num_fields; i++)
      {
        for(int j = 0; j < this->num_fields; j++)
          {
            v[fl->flatten(i,j)] = this->compute_M_component(i, j, Hsq, eps);
          }
      }
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_zeta_xfm_1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(1));

    // formulae from DS calculation '10 - UDG matrices summary', 19 April 2014

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        if(this->is_field(i))
          {
            v[fl->flatten(i)] = -this->deriv_list[i] / (2*pow(this->M_Planck,2)*eps);
          }
        else if(this->is_momentum(i))
          {
            v[fl->flatten(i)] = 0;
          }
        else
          {
            assert(false);
          }
      }
  }


void canonical_u_tensor_factory::compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                    GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                    std::vector<GiNaC::ex>& v, flattener* fl)
  {
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(2));

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
      {
        for(int j = 0; j < 2*this->num_fields; j++)
          {
            GiNaC::ex c = 0;

            if(this->is_field(i) && this->is_field(j))
              {
                c = this->compute_zeta_xfm_2_ff(i, j, k, k1, k2, a, Hsq, eps);
              }
            else if(this->is_field(i) && this->is_momentum(j))
              {
                c = this->compute_zeta_xfm_2_fp(i, this->species(j), k, k1, k2, a, Hsq, eps);
              }
            else if(this->is_momentum(i) && this->is_field(j))
              {
                c = this->compute_zeta_xfm_2_fp(j, this->species(i), k, k2, k1, a, Hsq, eps);
              }
            else if(this->is_momentum(i) && this->is_momentum(j))
              {
                c = 0;
              }
            else
              {
                assert(false);
              }

            v[fl->flatten(i,j)] = c;
          }
      }
  }


GiNaC::ex canonical_u_tensor_factory::compute_zeta_xfm_2_ff(unsigned int m, unsigned int n,
                                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                            GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(m < this->num_fields);
    assert(n < this->num_fields);

    // formulae from DS calculation 28 May 2014

    GiNaC::ex p = 0;

		for(unsigned int i = 0; i < this->num_fields; i++)
			{
				p += diff(this->V, this->field_list[i])*this->deriv_list[i];
			}
		p = p / (Hsq*pow(this->M_Planck,2));

    GiNaC::ex c = (-GiNaC::ex(1)/2 + 3/(2*eps) + p/(4*pow(eps,2)))*this->deriv_list[m]*this->deriv_list[n] / (pow(this->M_Planck,4)*eps);

    return(c);
  }


// n is the momentum index
GiNaC::ex canonical_u_tensor_factory::compute_zeta_xfm_2_fp(unsigned int m, unsigned int n,
                                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                            GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(m < this->num_fields);
    assert(n < this->num_fields);

    // formulae from DS calculation 28 May 2014

    GiNaC::ex k1dotk2 = (k*k - k1*k1 - k2*k2)/2;

    GiNaC::ex k12sq = k*k;

    GiNaC::ex c = this->deriv_list[m]*this->deriv_list[n] / (2*pow(this->M_Planck,4)*pow(eps,2));

		if(m == n)
			{
				c += -(k1dotk2 / k12sq) / (2*pow(this->M_Planck,2)*eps);
				c += -(k1*k1 / k12sq)   / (2*pow(this->M_Planck,2)*eps);
			}

    return(c);
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_deltaN_xfm_1(std::vector<GiNaC::ex>& v, flattener* fl)
	{
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(1));

    GiNaC::ex Hsq  = this->compute_Hsq();
    GiNaC::ex eps  = this->compute_eps();

    GiNaC::ex dotH = -eps*Hsq;

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
	    {
        if(this->is_field(i))
	        {
            v[fl->flatten(i)] = -(1/(2*dotH)) * diff(Hsq, this->field_list[this->species(i)]);
	        }
        else if(this->is_momentum(i))
	        {
            v[fl->flatten(i)] =  -(1/(2*dotH)) * diff(Hsq, this->deriv_list[this->species(i)]);
	        }
        else
	        {
            assert(false);
	        }
	    }
	}


void canonical_u_tensor_factory::compute_deltaN_xfm_2(std::vector<GiNaC::ex>& v, flattener* fl)
	{
    fl->set_size(2*this->num_fields);
    v.clear();
    v.resize(fl->get_flattened_size(2));

    GiNaC::ex Hsq  = this->compute_Hsq();
    GiNaC::ex eps  = this->compute_eps();

    GiNaC::ex dotH = -eps*Hsq;

    // formulae from arXiv:1302.3842, Eqs. (6.4) and (6.5)

    GiNaC::ex p_sum(0);
    for(int i = 0; i < this->num_fields; i++)
	    {
        p_sum += diff(1/(2*dotH), this->field_list[i]) * this->deriv_list[i];
	    }
    for(int i = 0; i < this->num_fields; i++)
	    {
        GiNaC::ex dXdN  = (eps-3)*this->deriv_list[i] - diff(this->V, this->field_list[i])/Hsq;
        p_sum          += diff(1/(2*dotH), this->deriv_list[i]) * dXdN;
	    }

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < 2*this->num_fields; i++)
	    {
        for(int j = 0; j < 2*this->num_fields; j++)
	        {
            v[fl->flatten(i,j)] = -1/(2*dotH) * diff(diff(Hsq, this->coordinate(i)), this->coordinate(j))
	            - diff(1/(2*dotH), this->coordinate(i)) * diff(Hsq, this->coordinate(j))
	            - diff(1/(2*dotH), this->coordinate(j)) * diff(Hsq, this->coordinate(i))
	            + 1/(2*dotH) * p_sum * diff(Hsq, this->coordinate(i)) * diff(Hsq, this->coordinate(j));
	        }
	    }
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


// compute A/H^2 tensor
GiNaC::ex canonical_u_tensor_factory::compute_A_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
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

    GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = - (Vijk / Hsq)/3;

    c += - (this->deriv_list[i]/(2*pow(this->M_Planck,2))) * (Vjk / Hsq)/3
         - (this->deriv_list[j]/(2*pow(this->M_Planck,2))) * (Vik / Hsq)/3
         - (this->deriv_list[k]/(2*pow(this->M_Planck,2))) * (Vij / Hsq)/3;

    c +=   ((this->deriv_list[i]*this->deriv_list[j]*xi_k)/(8*pow(this->M_Planck,4)))/3
         + ((this->deriv_list[i]*this->deriv_list[k]*xi_j)/(8*pow(this->M_Planck,4)))/3
         + ((this->deriv_list[j]*this->deriv_list[k]*xi_i)/(8*pow(this->M_Planck,4)))/3;

    c +=   (this->deriv_list[i]*xi_j*xi_k)/(32*pow(this->M_Planck,4)) * ( 1 - k2dotk3*k2dotk3 / (k2*k2*k3*k3) ) /3
         + (this->deriv_list[j]*xi_i*xi_k)/(32*pow(this->M_Planck,4)) * ( 1 - k1dotk3*k1dotk3 / (k1*k1*k3*k3) ) /3
         + (this->deriv_list[k]*xi_i*xi_j)/(32*pow(this->M_Planck,4)) * ( 1 - k1dotk2*k1dotk2 / (k1*k1*k2*k2) ) /3;

    c += (this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k]/(8*pow(this->M_Planck,4))) * (6 - 2*eps);

    if(j == k) c += ((this->deriv_list[i]/(2*pow(this->M_Planck,2))) * k2dotk3/(pow(a,2)*Hsq))/3;
    if(i == k) c += ((this->deriv_list[j]/(2*pow(this->M_Planck,2))) * k1dotk3/(pow(a,2)*Hsq))/3;
    if(i == j) c += ((this->deriv_list[k]/(2*pow(this->M_Planck,2))) * k1dotk2/(pow(a,2)*Hsq))/3;

    return(c);
  }


// compute B/H tensor
GiNaC::ex canonical_u_tensor_factory::compute_B_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    GiNaC::ex xi_i = this->compute_xi(i, Hsq, eps);
    GiNaC::ex xi_j = this->compute_xi(j, Hsq, eps);

    GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k]/(4*pow(this->M_Planck,4));

    c += - (this->deriv_list[i]*xi_j*this->deriv_list[k]/(8*pow(this->M_Planck,4))) * ( 1 - k2dotk3*k2dotk3 / (k2*k2*k3*k3) ) /2
         - (this->deriv_list[j]*xi_i*this->deriv_list[k]/(8*pow(this->M_Planck,4))) * ( 1 - k1dotk3*k1dotk3 / (k1*k1*k3*k3) ) /2;

    if(j == k) c += - (xi_i / (2*pow(this->M_Planck,2))) * k1dotk2 / (k1*k1) /2;
    if(i == k) c += - (xi_j / (2*pow(this->M_Planck,2))) * k1dotk2 / (k2*k2) /2;

    return(c);
  }


// compute C-tensor
GiNaC::ex canonical_u_tensor_factory::compute_C_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
    GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
    GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

    GiNaC::ex c = 0;

    if (i == j) c += - this->deriv_list[k] / (2*pow(this->M_Planck,2));

    c += (this->deriv_list[i]*this->deriv_list[j]*this->deriv_list[k] / (8*pow(this->M_Planck,4))) * ( 1 - k1dotk2*k1dotk2 / (k1*k1*k2*k2) );

    if (j == k) c += (this->deriv_list[i] / pow(this->M_Planck, 2)) * (k1dotk3 / (k1*k1)) /2;
    if (i == k) c += (this->deriv_list[j] / pow(this->M_Planck, 2)) * (k2dotk3 / (k2*k2)) /2;

    return (c);
  }


GiNaC::ex canonical_u_tensor_factory::compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(i < this->num_fields);

    GiNaC::ex Vi = GiNaC::diff(this->V, this->field_list[i]);

    GiNaC::ex c = -2*(3-eps)*this->deriv_list[i] - 2*Vi/Hsq;

    return(c);
  }


// compute M tensor
GiNaC::ex canonical_u_tensor_factory::compute_M_component(unsigned int i, unsigned int j, GiNaC::ex& Hsq, GiNaC::ex& eps)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);

    GiNaC::ex Vab = GiNaC::diff(GiNaC::diff(this->V, this->field_list[i]), this->field_list[j]);
    GiNaC::ex Va  = GiNaC::diff(this->V, this->field_list[i]);
    GiNaC::ex Vb  = GiNaC::diff(this->V, this->field_list[j]);

    GiNaC::ex c = (i == j ? eps : 0);

    GiNaC::ex u = -Vab/Hsq;
    u -= (3-eps)*this->deriv_list[i]*this->deriv_list[j]/pow(this->M_Planck,2);
    u -= 1/(pow(this->M_Planck,2)*Hsq)*(this->deriv_list[i]*Vb + this->deriv_list[j]*Va);

    return(c+u/3);
  }
