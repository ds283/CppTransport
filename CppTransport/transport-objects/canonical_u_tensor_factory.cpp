//
// Created by David Seery on 01/07/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>

#include "canonical_u_tensor_factory.h"


// *****************************************************************************


void canonical_u_tensor_factory::compute_sr_u(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields, std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(1));

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));

    GiNaC::ex subs_V;
    bool cached = false;

    for(int i = 0; i < this->num_fields; ++i)
      {
		    unsigned int index = field_fl.flatten(i);

		    if(!this->cache.query(expression_item_types::sr_U_item, index, args, v[index]))
			    {
            bool is_stopped = this->compute_timer.is_stopped();
				    if(is_stopped) this->compute_timer.resume();

            if(!cached)
              {
                subs_V = this->substitute_V(params, fields, args);
                cached = true;
              }

		        v[index] = - GiNaC::diff(subs_V, fields[i]) * GiNaC::pow(this->M_Planck, 2) / (subs_V);

		        if(is_stopped) this->compute_timer.stop();
				    this->cache.store(expression_item_types::sr_U_item, index, args, v[index]);
			    }
      }
  }


void canonical_u_tensor_factory::compute_u1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                            std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(fl.get_flattened_size(1));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));

    for(int i = 0; i < 2*this->num_fields; ++i)
      {
		    unsigned int index = fl.flatten(i);

		    if(!this->cache.query(expression_item_types::U1_item, index, args, v[index]))
			    {
            bool is_stopped = this->compute_timer.is_stopped();
				    if(is_stopped) this->compute_timer.resume();

		        if(this->is_field(i))
			        {
		            v[index] = derivs[this->species(i)];
			        }
		        else if(this->is_momentum(i))
			        {
		            v[index] = -(3-eps)*derivs[this->species(i)] - dV[this->species(i)]/Hsq;
			        }
		        else
			        {
		            assert(false);
			        }

		        if(is_stopped) this->compute_timer.stop();
				    this->cache.store(expression_item_types::U1_item, index, args, v[index]);
			    }
      }
  }


void canonical_u_tensor_factory::compute_u2(GiNaC::symbol& k, GiNaC::symbol& a,
                                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                            const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(fl.get_flattened_size(2));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    args.push_back(k);
    args.push_back(a);
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));
    std::copy(ddV.begin(), ddV.end(), std::back_inserter(args));

    for(int i = 0; i < 2*this->num_fields; ++i)
      {
        for(int j = 0; j < 2*this->num_fields; ++j)
          {
		        unsigned int index = fl.flatten(i,j);

		        if(!this->cache.query(expression_item_types::U2_item, index, args, v[index]))
			        {
                bool is_stopped = this->compute_timer.is_stopped();
				        if(is_stopped) this->compute_timer.resume();

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

		                GiNaC::ex Vab = ddV[this->field_fl.flatten(this->species(i), this->species(j))];
		                GiNaC::ex Va  = dV[this->field_fl.flatten(this->species(i))];
		                GiNaC::ex Vb  = dV[this->field_fl.flatten(this->species(j))];

		                c -= Vab/Hsq;
		                c -= (3-eps)*derivs[this->species(i)]*derivs[this->species(j)]/pow(this->M_Planck,2);
		                c -= 1/(pow(this->M_Planck,2)*Hsq)*(derivs[this->species(i)]*Vb + derivs[this->species(j)]*Va);
			            }
		            else if(this->is_momentum(i) && this->is_momentum(j))
			            {
		                c = (this->species(i) == this->species(j) ? (eps-3) : 0);
			            }
		            else
			            {
		                assert(false);
			            }

		            v[index] = c;
		            if(is_stopped) this->compute_timer.stop();
				        this->cache.store(expression_item_types::U2_item, index, args, v[index]);
			        }
          }
      }
  }


void canonical_u_tensor_factory::compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                            const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                                            std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(fl.get_flattened_size(3));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    // build argument block for each of the k1, k2, k3 permutations we need
    // we need different blocks for each permutation to prevent cache aliasing

    std::vector<GiNaC::ex> args;
    args.push_back(k1);
    args.push_back(k2);
    args.push_back(k3);
    args.push_back(a);
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));
    std::copy(ddV.begin(), ddV.end(), std::back_inserter(args));
    std::copy(dddV.begin(), dddV.end(), std::back_inserter(args));

    std::vector<GiNaC::ex> a_args;
    a_args.push_back(k1);
    a_args.push_back(k2);
    a_args.push_back(k3);
    a_args.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(a_args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(a_args));
    std::copy(ddV.begin(), ddV.end(), std::back_inserter(a_args));
    std::copy(dddV.begin(), dddV.end(), std::back_inserter(a_args));

    std::vector<GiNaC::ex> b_args_123;
    b_args_123.push_back(k1);
    b_args_123.push_back(k2);
    b_args_123.push_back(k3);
    b_args_123.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(b_args_123));
    std::copy(dV.begin(), dV.end(), std::back_inserter(b_args_123));

    std::vector<GiNaC::ex> c_args_123;
    c_args_123.push_back(k1);
    c_args_123.push_back(k2);
    c_args_123.push_back(k3);
    c_args_123.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(c_args_123));

    std::vector<GiNaC::ex> b_args_231;
    b_args_231.push_back(k2);
    b_args_231.push_back(k3);
    b_args_231.push_back(k1);
    b_args_231.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(b_args_231));
    std::copy(dV.begin(), dV.end(), std::back_inserter(b_args_231));

    std::vector<GiNaC::ex> c_args_231;
    c_args_231.push_back(k2);
    c_args_231.push_back(k3);
    c_args_231.push_back(k1);
    c_args_231.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(c_args_231));

    std::vector<GiNaC::ex> b_args_132;
    b_args_132.push_back(k1);
    b_args_132.push_back(k3);
    b_args_132.push_back(k2);
    b_args_132.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(b_args_132));
    std::copy(dV.begin(), dV.end(), std::back_inserter(b_args_132));

    std::vector<GiNaC::ex> c_args_132;
    c_args_132.push_back(k1);
    c_args_132.push_back(k3);
    c_args_132.push_back(k2);
    c_args_132.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(c_args_132));

    for(int i = 0; i < 2*this->num_fields; ++i)
      {
        for(int j = 0; j < 2*this->num_fields; ++j)
          {
            for(int k = 0; k < 2*this->num_fields; ++k)
              {
		            unsigned int index = fl.flatten(i,j,k);

		            if(!this->cache.query(expression_item_types::U3_item, index, args, v[index]))
			            {
                    bool is_stopped = this->compute_timer.is_stopped();
				            if(is_stopped) this->compute_timer.resume();

		                GiNaC::ex c = 0;

		                // note that we flip the sign of momenta attached to the i, j, k components compared to the
		                // analytic definition
		                // this accounts for integrating out the delta-functions when contracting u3 with something else

		                // factor of 2 from definition of 2nd order term in transport eq: dX/dN = u2.X + (1/2) u3.X.X + ...
		                if(this->is_field(i) && this->is_field(j) && this->is_field(k))
			                {
		                    c = -this->compute_B_component(this->species(j), k2, this->species(k), k3, this->species(i), k1, a, Hsq, eps, derivs, dV, b_args_231);
			                }
		                else if(this->is_field(i) && this->is_field(j) && this->is_momentum(k))
			                {
		                    c = -this->compute_C_component(this->species(i), k1, this->species(k), k3, this->species(j), k2, a, derivs, c_args_132);
			                }
		                else if(this->is_field(i) && this->is_momentum(j) && this->is_field(k))
			                {
		                    c = -this->compute_C_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, derivs, c_args_123);
			                }
		                else if(this->is_field(i) && this->is_momentum(j) && this->is_momentum(k))
			                {
		                    c = 0;
			                }
		                else if(this->is_momentum(i) && this->is_field(j) && this->is_field(k))
			                {
		                    c = 3*this->compute_A_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, Hsq, eps, derivs, dV, ddV, dddV, a_args);
			                }
		                else if(this->is_momentum(i) && this->is_field(j) && this->is_momentum(k))
			                {
		                    c = this->compute_B_component(this->species(i), k1, this->species(j), k2, this->species(k), k3, a, Hsq, eps, derivs, dV, b_args_123);
			                }
		                else if(this->is_momentum(i) && this->is_momentum(j) && this->is_field(k))
			                {
		                    c = this->compute_B_component(this->species(i), k1, this->species(k), k3, this->species(j), k2, a, Hsq, eps, derivs, dV, b_args_132);
			                }
		                else if(this->is_momentum(i) && this->is_momentum(j) && this->is_momentum(k))
			                {
		                    c = this->compute_C_component(this->species(j), k2, this->species(k), k3, this->species(i), k1, a, derivs, c_args_231);
			                }
		                else
			                {
		                    assert(false);
			                }

		                v[index] = c;
		                if(is_stopped) this->compute_timer.stop();
		                this->cache.store(expression_item_types::U3_item, index, args, v[index]);
			            }
              }
          }
      }
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                           const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                                           std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(3));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> a_args;
    a_args.push_back(k1);
    a_args.push_back(k2);
    a_args.push_back(k3);
    a_args.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(a_args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(a_args));
    std::copy(ddV.begin(), ddV.end(), std::back_inserter(a_args));
    std::copy(dddV.begin(), dddV.end(), std::back_inserter(a_args));

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            for(int k = 0; k < this->num_fields; ++k)
              {
                v[field_fl.flatten(i,j,k)] = this->compute_A_component(i, k1, j, k2, k, k3, a, Hsq, eps, derivs, dV, ddV, dddV, a_args);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                           std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(3));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> b_args;
    b_args.push_back(k1);
    b_args.push_back(k2);
    b_args.push_back(k3);
    b_args.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(b_args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(b_args));

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            for(int k = 0; k < this->num_fields; ++k)
              {
                v[field_fl.flatten(i,j,k)] = this->compute_B_component(i, k1, j, k2, k, k3, a, Hsq, eps, derivs, dV, b_args);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                           const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(3));

    std::vector<GiNaC::ex> c_args;
    c_args.push_back(k1);
    c_args.push_back(k2);
    c_args.push_back(k3);
    c_args.push_back(a);
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(c_args));

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            for(int k = 0; k < this->num_fields; ++k)
              {
                v[field_fl.flatten(i,j,k)] = this->compute_C_component(i, k1, j, k2, k, k3, a, derivs, c_args);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_M(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                           const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(2));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));
    std::copy(ddV.begin(), ddV.end(), std::back_inserter(args));

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            v[field_fl.flatten(i,j)] = this->compute_M_component(i, j, Hsq, eps, derivs, dV, ddV, args);
          }
      }
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_zeta_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                                    std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(fl.get_flattened_size(1));

    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));

    // formulae from DS calculation '10 - UDG matrices summary', 19 April 2014

    for(int i = 0; i < 2*this->num_fields; ++i)
      {
		    unsigned int index = fl.flatten(i);

		    if(!this->cache.query(expression_item_types::zxfm1_item, index, args, v[index]))
			    {
            bool is_stopped = this->compute_timer.is_stopped();
				    if(is_stopped) this->compute_timer.resume();

		        if(this->is_field(i))
			        {
		            v[index] = -derivs[i] / (2*pow(this->M_Planck,2)*eps);
			        }
		        else if(this->is_momentum(i))
			        {
		            v[index] = 0;
			        }
		        else
			        {
		            assert(false);
			        }

				    if(is_stopped) this->compute_timer.stop();
				    this->cache.store(expression_item_types::zxfm1_item, index, args, v[index]);
			    }
      }
  }


void canonical_u_tensor_factory::compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a,
                                                    const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                                    std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(fl.get_flattened_size(2));

    GiNaC::ex Hsq = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    args.push_back(k1);
    args.push_back(k2);
    args.push_back(a);
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));

    for(int i = 0; i < 2*this->num_fields; ++i)
      {
        for(int j = 0; j < 2*this->num_fields; ++j)
          {
		        unsigned int index = fl.flatten(i,j);

		        if(!this->cache.query(expression_item_types::zxfm2_item, index, args, v[index]))
			        {
                bool is_stopped = this->compute_timer.is_stopped();
				        if(is_stopped) this->compute_timer.resume();

		            GiNaC::ex c = 0;

		            if(this->is_field(i) && this->is_field(j))
			            {
		                c = this->compute_zeta_xfm_2_ff(i, j, k, k1, k2, a, Hsq, eps, derivs, dV);
			            }
		            else if(this->is_field(i) && this->is_momentum(j))
			            {
		                c = this->compute_zeta_xfm_2_fp(i, this->species(j), k, k1, k2, a, Hsq, eps, derivs);
			            }
		            else if(this->is_momentum(i) && this->is_field(j))
			            {
		                c = this->compute_zeta_xfm_2_fp(j, this->species(i), k, k2, k1, a, Hsq, eps, derivs);
			            }
		            else if(this->is_momentum(i) && this->is_momentum(j))
			            {
		                c = 0;
			            }
		            else
			            {
		                assert(false);
			            }

		            v[index] = c;

				        if(is_stopped) this->compute_timer.stop();
				        this->cache.store(expression_item_types::zxfm2_item, index, args, v[index]);
			        }

          }
      }
  }


GiNaC::ex canonical_u_tensor_factory::compute_zeta_xfm_2_ff(unsigned int m, unsigned int n,
                                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                            GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV)
  {
    assert(m < this->num_fields);
    assert(n < this->num_fields);

    // formulae from DS calculation 28 May 2014

    bool is_stopped = this->compute_timer.is_stopped();
    if(is_stopped) this->compute_timer.resume();

    GiNaC::ex p = 0;

		for(unsigned int i = 0; i < this->num_fields; ++i)
			{
				p += dV[i]*derivs[i];
			}
		p = p / (Hsq*pow(this->M_Planck,2));

    GiNaC::ex c = (-GiNaC::ex(1)/2 + 3/(2*eps) + p/(4*pow(eps,2)))*derivs[m]*derivs[n] / (pow(this->M_Planck,4)*eps);

    if(is_stopped) this->compute_timer.stop();

    return(c);
  }


// n is the momentum index
GiNaC::ex canonical_u_tensor_factory::compute_zeta_xfm_2_fp(unsigned int m, unsigned int n,
                                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                                            GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                            const std::vector<GiNaC::symbol>& derivs)
  {
    assert(m < this->num_fields);
    assert(n < this->num_fields);

    // formulae from DS calculation 28 May 2014

    bool is_stopped = this->compute_timer.is_stopped();
    if(is_stopped) this->compute_timer.resume();

    GiNaC::ex k1dotk2 = (k*k - k1*k1 - k2*k2)/2;

    GiNaC::ex k12sq = k*k;

    GiNaC::ex c = derivs[m]*derivs[n] / (2*pow(this->M_Planck,4)*pow(eps,2));

		if(m == n)
			{
				c += -(k1dotk2 / k12sq) / (2*pow(this->M_Planck,2)*eps);
				c += -(k1*k1 / k12sq)   / (2*pow(this->M_Planck,2)*eps);
			}

    if(is_stopped) this->compute_timer.stop();

    return(c);
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_deltaN_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                                      const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v)
	{
    v.clear();
    v.resize(fl.get_flattened_size(1));

    GiNaC::ex Hsq  = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps  = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));

    GiNaC::ex dotH;
    bool cached = false;

    for(int i = 0; i < 2*this->num_fields; ++i)
	    {
		    unsigned int index = fl.flatten(i);

		    if(!this->cache.query(expression_item_types::dN1_item, index, v[index]))
			    {
            bool is_stopped = this->compute_timer.is_stopped();
				    if(is_stopped) this->compute_timer.resume();

            if(!cached)
              {
                dotH = -eps*Hsq;
                cached = true;
              }

		        if(this->is_field(i))
			        {
		            v[index] = -(1/(2*dotH)) * diff(Hsq, fields[this->species(i)]);
			        }
		        else if(this->is_momentum(i))
			        {
		            v[index] =  -(1/(2*dotH)) * diff(Hsq, fields[this->species(i)]);
			        }
		        else
			        {
		            assert(false);
			        }

						if(is_stopped) this->compute_timer.stop();
				    this->cache.store(expression_item_types::dN1_item, index, v[index]);
			    }
	    }
	}


void canonical_u_tensor_factory::compute_deltaN_xfm_2(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                                      const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                                      std::vector<GiNaC::ex>& v)
	{
    v.clear();
    v.resize(fl.get_flattened_size(2));

    GiNaC::ex Hsq  = this->compute_Hsq(params, fields, derivs);
    GiNaC::ex eps  = this->compute_eps(derivs);

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));
    std::copy(dV.begin(), dV.end(), std::back_inserter(args));

    // formulae from arXiv:1302.3842, Eqs. (6.4) and (6.5)

    GiNaC::ex p_sum(0);
    GiNaC::ex dotH;
    bool cached = false;

    for(int i = 0; i < 2*this->num_fields; ++i)
	    {
        for(int j = 0; j < 2*this->num_fields; ++j)
	        {
		        unsigned int index = fl.flatten(i,j);

		        if(!this->cache.query(expression_item_types::dN2_item, index, v[index]))
			        {
                bool is_stopped = this->compute_timer.is_stopped();
				        if(is_stopped) this->compute_timer.resume();

                if(!cached)
                  {
                    dotH = -eps*Hsq;

                    for(int m = 0; m < this->num_fields; ++m)
                      {
                        p_sum += diff(1/(2*dotH), fields[m]) * derivs[m];
                      }
                    for(int m = 0; m < this->num_fields; ++m)
                      {
                        GiNaC::ex dXdN  = (eps-3)*derivs[m] - dV[m]/Hsq;
                        p_sum          += diff(1/(2*dotH), derivs[m]) * dXdN;
                      }
                  }

                auto coord_i = this->is_field(i) ? fields[i] : derivs[this->species(i)];
                auto coord_j = this->is_field(j) ? fields[j] : derivs[this->species(j)];

		            v[index] = -1/(2*dotH) * diff(diff(Hsq, coord_i), coord_j)
			            - diff(1/(2*dotH), coord_i) * diff(Hsq, coord_i)
			            - diff(1/(2*dotH), coord_j) * diff(Hsq, coord_j)
			            + 1/(2*dotH) * p_sum * diff(Hsq, coord_i) * diff(Hsq, coord_j);

				        if(is_stopped) this->compute_timer.stop();
				        this->cache.store(expression_item_types::dN2_item, index, v[index]);
			        }
	        }
	    }
	}


// *****************************************************************************


GiNaC::ex canonical_u_tensor_factory::compute_V(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields)
  {
    return this->substitute_V(params, fields);
  }


GiNaC::ex canonical_u_tensor_factory::compute_Hsq(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                                  const std::vector<GiNaC::symbol>& derivs)
  {
    GiNaC::ex Hsq;

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));
    std::copy(derivs.begin(), derivs.end(), std::back_inserter(args));

    if(!this->cache.query(expression_item_types::Hubble2_item, 0, args, Hsq))
      {
        bool is_stopped = this->compute_timer.is_stopped();
        if(is_stopped) this->compute_timer.resume();

        GiNaC::ex subs_V = this->substitute_V(params, fields);

        GiNaC::ex eps = this->compute_eps(derivs);
        Hsq = subs_V / (GiNaC::pow(this->M_Planck,2)*(3-eps));

        if(is_stopped) this->compute_timer.stop();
        this->cache.store(expression_item_types::Hubble2_item, 0, args, Hsq);
      }

    return(Hsq);
  }


GiNaC::ex canonical_u_tensor_factory::compute_eps(const std::vector<GiNaC::symbol>& derivs)
  {
    GiNaC::ex sum_momenta_sq(0);

    // presumably not worth going to the cache for a calculation as short as this
    bool is_stopped = this->compute_timer.is_stopped();
    if(is_stopped) this->compute_timer.resume();

    for(const GiNaC::symbol& sym : derivs)
      {
        sum_momenta_sq += sym*sym;
      }

    GiNaC::ex eps = sum_momenta_sq/(2*GiNaC::pow(this->M_Planck,2));

    if(is_stopped) this->compute_timer.stop();

    return(eps);
  }


// *****************************************************************************


void canonical_u_tensor_factory::compute_dV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                            std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(1));

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));

    GiNaC::ex subs_V;
    bool cached = false;

    for(int i = 0; i < this->num_fields; ++i)
      {
        unsigned int index = field_fl.flatten(i);

        if(!this->cache.query(expression_item_types::dV_item, index, args, v[index]))
          {
            bool is_stopped = this->compute_timer.is_stopped();
            if(is_stopped) this->compute_timer.resume();

            if(!cached)
              {
                subs_V = this->substitute_V(params, fields, args);
                cached = true;
              }

            v[index] = GiNaC::diff(subs_V, fields[i]);

            if(is_stopped) this->compute_timer.stop();
            this->cache.store(expression_item_types::dV_item, index, args, v[index]);
          }
      }
  }


void canonical_u_tensor_factory::compute_ddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                             std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(2));

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));

    GiNaC::ex subs_V;
    bool cached = false;

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            unsigned int index = field_fl.flatten(i,j);

            if(!this->cache.query(expression_item_types::ddV_item, index, args, v[index]))
              {
                bool is_stopped = this->compute_timer.is_stopped();
                if(is_stopped) this->compute_timer.resume();

                if(!cached)
                  {
                    subs_V = this->substitute_V(params, fields, args);
                    cached = true;
                  }

                v[index] = GiNaC::diff(GiNaC::diff(subs_V, fields[j]), fields[i]);

                if(is_stopped) this->compute_timer.stop();
                this->cache.store(expression_item_types::ddV_item, index, args, v[index]);
              }
          }
      }
  }


void canonical_u_tensor_factory::compute_dddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                              std::vector<GiNaC::ex>& v)
  {
    v.clear();
    v.resize(field_fl.get_flattened_size(3));

    std::vector<GiNaC::ex> args;
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(params.begin(), params.end(), std::back_inserter(args));
    std::copy(fields.begin(), fields.end(), std::back_inserter(args));

    GiNaC::ex subs_V;
    bool cached = false;

    for(int i = 0; i < this->num_fields; ++i)
      {
        for(int j = 0; j < this->num_fields; ++j)
          {
            for(int k = 0; k < this->num_fields; ++k)
              {
                unsigned int index = field_fl.flatten(i,j,k);

                if(!this->cache.query(expression_item_types::dddV_item, index, args, v[index]))
                  {
                    bool is_stopped = this->compute_timer.is_stopped();
                    if(is_stopped) this->compute_timer.resume();

                    if(!cached)
                      {
                        subs_V = this->substitute_V(params, fields, args);
                        cached = true;
                      }

                    v[index] = GiNaC::diff(GiNaC::diff(GiNaC::diff(subs_V, fields[k]), fields[j]), fields[i]);

                    if(is_stopped) this->compute_timer.stop();
                    this->cache.store(expression_item_types::dddV_item, index, args, v[index]);
                  }
              }
          }
      }
  }


// *****************************************************************************


// compute A/H^2 tensor
GiNaC::ex canonical_u_tensor_factory::compute_A_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                          const std::vector<GiNaC::symbol>& derivs,
                                                          const std::vector<GiNaC::symbol>& dV,
                                                          const std::vector<GiNaC::symbol>& ddV,
                                                          const std::vector<GiNaC::symbol>& dddV,
                                                          const std::vector<GiNaC::ex>& args)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    unsigned int index = this->field_fl.flatten(i,j,k);

    GiNaC::ex c = 0;

		if(!this->cache.query(expression_item_types::A_item, index, args, c))
			{
        bool is_stopped = this->compute_timer.is_stopped();
				if(is_stopped) this->compute_timer.resume();

		    GiNaC::ex Vijk = dddV[this->field_fl.flatten(i,j,k)];
		    GiNaC::ex Vij  = ddV[this->field_fl.flatten(i,j)];
		    GiNaC::ex Vik  = ddV[this->field_fl.flatten(i,k)];
		    GiNaC::ex Vjk  = ddV[this->field_fl.flatten(j,k)];

		    GiNaC::ex xi_i = this->compute_xi(i, Hsq, eps, derivs, dV);
		    GiNaC::ex xi_j = this->compute_xi(j, Hsq, eps, derivs, dV);
		    GiNaC::ex xi_k = this->compute_xi(k, Hsq, eps, derivs, dV);

		    GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
		    GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
		    GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

		    c = - (Vijk / Hsq)/3;

		    c += - (derivs[i]/(2*pow(this->M_Planck,2))) * (Vjk / Hsq)/3
			       - (derivs[j]/(2*pow(this->M_Planck,2))) * (Vik / Hsq)/3
			       - (derivs[k]/(2*pow(this->M_Planck,2))) * (Vij / Hsq)/3;

		    c +=   ((derivs[i]*derivs[j]*xi_k)/(8*pow(this->M_Planck,4)))/3
			       + ((derivs[i]*derivs[k]*xi_j)/(8*pow(this->M_Planck,4)))/3
			       + ((derivs[j]*derivs[k]*xi_i)/(8*pow(this->M_Planck,4)))/3;

		    c +=   (derivs[i]*xi_j*xi_k)/(32*pow(this->M_Planck,4)) * ( 1 - k2dotk3*k2dotk3 / (k2*k2*k3*k3) ) /3
			       + (derivs[j]*xi_i*xi_k)/(32*pow(this->M_Planck,4)) * ( 1 - k1dotk3*k1dotk3 / (k1*k1*k3*k3) ) /3
			       + (derivs[k]*xi_i*xi_j)/(32*pow(this->M_Planck,4)) * ( 1 - k1dotk2*k1dotk2 / (k1*k1*k2*k2) ) /3;

		    c += (derivs[i]*derivs[j]*derivs[k]/(8*pow(this->M_Planck,4))) * (6 - 2*eps);

		    if(j == k) c += ((derivs[i]/(2*pow(this->M_Planck,2))) * k2dotk3/(pow(a,2)*Hsq))/3;
		    if(i == k) c += ((derivs[j]/(2*pow(this->M_Planck,2))) * k1dotk3/(pow(a,2)*Hsq))/3;
		    if(i == j) c += ((derivs[k]/(2*pow(this->M_Planck,2))) * k1dotk2/(pow(a,2)*Hsq))/3;

				if(is_stopped) this->compute_timer.stop();

				this->cache.store(expression_item_types::A_item, index, args, c);
			}

    return(c);
  }


// compute B/H tensor
GiNaC::ex canonical_u_tensor_factory::compute_B_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                          const std::vector<GiNaC::symbol>& derivs,
                                                          const std::vector<GiNaC::symbol>& dV,
                                                          const std::vector<GiNaC::ex>& args)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    unsigned int index = this->field_fl.flatten(i,j,k);

    GiNaC::ex c = 0;

		if(!this->cache.query(expression_item_types::B_item, index, args, c))
			{
        bool is_stopped = this->compute_timer.is_stopped();
				if(is_stopped) this->compute_timer.resume();

		    GiNaC::ex xi_i = this->compute_xi(i, Hsq, eps, derivs, dV);
		    GiNaC::ex xi_j = this->compute_xi(j, Hsq, eps, derivs, dV);

		    GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
		    GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
		    GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

		    c = derivs[i]*derivs[j]*derivs[k]/(4*pow(this->M_Planck,4));

		    c += - (derivs[i]*xi_j*derivs[k]/(8*pow(this->M_Planck,4))) * ( 1 - k2dotk3*k2dotk3 / (k2*k2*k3*k3) ) /2
             - (derivs[j]*xi_i*derivs[k]/(8*pow(this->M_Planck,4))) * ( 1 - k1dotk3*k1dotk3 / (k1*k1*k3*k3) ) /2;

		    if(j == k) c += - (xi_i / (2*pow(this->M_Planck,2))) * k1dotk2 / (k1*k1) /2;
		    if(i == k) c += - (xi_j / (2*pow(this->M_Planck,2))) * k1dotk2 / (k2*k2) /2;

				if(is_stopped) this->compute_timer.stop();

				this->cache.store(expression_item_types::B_item, index, args, c);
			}

    return(c);
  }


// compute C-tensor
GiNaC::ex canonical_u_tensor_factory::compute_C_component(unsigned int i, GiNaC::symbol& k1,
                                                          unsigned int j, GiNaC::symbol& k2,
                                                          unsigned int k, GiNaC::symbol& k3,
                                                          GiNaC::symbol& a, const std::vector<GiNaC::symbol>& derivs,
                                                          const std::vector<GiNaC::ex>& args)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);
    assert(k < this->num_fields);

    unsigned int index = this->field_fl.flatten(i,j,k);
		
    GiNaC::ex c = 0;

    if(!this->cache.query(expression_item_types::C_item, index, args, c))
	    {
        bool is_stopped = this->compute_timer.is_stopped();
		    if(is_stopped) this->compute_timer.resume();

        GiNaC::ex k1dotk2 = (k3*k3 - k1*k1 - k2*k2)/2;
        GiNaC::ex k1dotk3 = (k2*k2 - k1*k1 - k3*k3)/2;
        GiNaC::ex k2dotk3 = (k1*k1 - k2*k2 - k3*k3)/2;

        if (i == j) c += - derivs[k] / (2*pow(this->M_Planck,2));

        c += (derivs[i]*derivs[j]*derivs[k] / (8*pow(this->M_Planck,4))) * ( 1 - k1dotk2*k1dotk2 / (k1*k1*k2*k2) );

        if (j == k) c += (derivs[i] / pow(this->M_Planck, 2)) * (k1dotk3 / (k1*k1)) /2;
        if (i == k) c += (derivs[j] / pow(this->M_Planck, 2)) * (k2dotk3 / (k2*k2)) /2;

		    if(is_stopped) this->compute_timer.stop();

		    this->cache.store(expression_item_types::C_item, index, args, c);
	    }

    return(c);
  }


GiNaC::ex canonical_u_tensor_factory::compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                 const std::vector<GiNaC::symbol>& derivs,
                                                 const std::vector<GiNaC::symbol>& dV)
  {
    assert(i < this->num_fields);

    bool is_stopped = this->compute_timer.is_stopped();
    if(is_stopped) this->compute_timer.resume();

    GiNaC::ex c = -2*(3-eps)*derivs[i] - 2*dV[i]/Hsq;

    if(is_stopped) this->compute_timer.stop();

    return(c);
  }


// compute M tensor
GiNaC::ex canonical_u_tensor_factory::compute_M_component(unsigned int i, unsigned int j, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                                          const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                                          const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::ex>& args)
  {
    assert(i < this->num_fields);
    assert(j < this->num_fields);

		unsigned int index = this->field_fl.flatten(i,j);

    GiNaC::ex v;

		if(!this->cache.query(expression_item_types::M_item, index, args, v))
			{
        bool is_stopped = this->compute_timer.is_stopped();
				if(is_stopped) this->compute_timer.resume();

		    GiNaC::ex Vab = ddV[this->field_fl.flatten(i,j)];
		    GiNaC::ex Va  = dV[this->field_fl.flatten(i)];
		    GiNaC::ex Vb  = dV[this->field_fl.flatten(j)];

		    GiNaC::ex c = (i == j ? eps : 0);

		    GiNaC::ex u = -Vab/Hsq;
		    u -= (3-eps)*derivs[i]*derivs[j]/pow(this->M_Planck,2);
		    u -= 1/(pow(this->M_Planck,2)*Hsq)*(derivs[i]*Vb + derivs[j]*Va);

				v = c+u/3;

				if(is_stopped) this->compute_timer.stop();

				this->cache.store(expression_item_types::M_item, index, args, v);
			}

    return(v);
  }
