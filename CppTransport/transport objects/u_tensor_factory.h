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
      u_tensor_factory(script *r);

      virtual std::vector<GiNaC::ex> compute_sr_u() = 0;

      virtual std::vector<GiNaC::ex> compute_u1() = 0;

      virtual std::vector<GiNaC::ex> compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      virtual std::vector< std::vector<GiNaC::ex> > compute_u2(GiNaC::symbol& k, GiNaC::symbol& a) = 0;

      virtual std::vector< std::vector<GiNaC::ex> > compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps)= 0;

      // the A, B and C tensors are associated with the Hamiltonian for
      // scalar field fluctuations [see written notes]
      // actually, these compute A/H^2, B/H and C
      // all of these have mass dimension -1
      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) = 0;

      virtual std::vector< std::vector< std::vector<GiNaC::ex> > > compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      // compute M tensor
      virtual std::vector< std::vector<GiNaC::ex> > compute_M() = 0;

      virtual std::vector< std::vector<GiNaC::ex> > compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps) = 0;

      //  CALCULATE GAUGE TRANSFORMATIONS
      virtual std::vector<GiNaC::ex> compute_zeta_xfm_1() = 0;

      virtual std::vector< std::vector<GiNaC::ex> > compute_zeta_xfm_2() = 0;

      virtual GiNaC::ex compute_Hsq() = 0;

      virtual GiNaC::ex compute_eps() = 0;

protected:
      script*                          root;

      const unsigned int               num_fields;
      const GiNaC::symbol              M_Planck;
      const GiNaC::ex                  V;

      const std::vector<GiNaC::symbol> field_list;
      const std::vector<GiNaC::symbol> deriv_list;
  };


class canonical_u_tensor_factory : public u_tensor_factory
  {
    public:
      canonical_u_tensor_factory(script *r) : u_tensor_factory(r)
        {
        }

      //  CALCULATE DERIVED QUANTITIES
      std::vector<GiNaC::ex> compute_sr_u();

      std::vector<GiNaC::ex> compute_u1();

      std::vector<GiNaC::ex> compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps);

      std::vector< std::vector<GiNaC::ex> > compute_u2(GiNaC::symbol& k, GiNaC::symbol& a);

      std::vector< std::vector<GiNaC::ex> > compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

      // the A, B and C tensors are associated with the Hamiltonian for
      // scalar field fluctuations [see written notes]
      // actually, these compute A/H^2, B/H and C
      // all of these have mass dimension -1
      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a);

      std::vector< std::vector< std::vector<GiNaC::ex> > > compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

      // compute M tensor
      std::vector< std::vector<GiNaC::ex> > compute_M();

      std::vector< std::vector<GiNaC::ex> > compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps);

      //  CALCULATE GAUGE TRANSFORMATIONS
      std::vector<GiNaC::ex> compute_zeta_xfm_1();

      std::vector< std::vector<GiNaC::ex> > compute_zeta_xfm_2();

      GiNaC::ex compute_Hsq();

      GiNaC::ex compute_eps();

    private:

      // compute A/H^2 [in the notation of the written notes]
      GiNaC::ex compute_A_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sign, int k3_sign);

      // compute B/H [in the notation of the written notes]
      GiNaC::ex compute_B_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sign, int k3_sign);

      // compute C [in the notation of the written notes]
      GiNaC::ex compute_C_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, int k1_sign, int k2_sing, int k3_sign);

      // compute xi/H^2 [in the notation of the written notes]
      GiNaC::ex compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps);

      // compute M-tensor component
      GiNaC::ex compute_M_component(unsigned int i, unsigned int j, GiNaC::ex& Hsq, GiNaC::ex& eps);
  };


// factory function to manufacture a u_tensor_factory instance
u_tensor_factory* make_u_tensor_factory(script* script);


#endif //__u_tensor_factory_H_
