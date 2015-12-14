//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef __u_tensor_factory_H_
#define __u_tensor_factory_H_

#include <iostream>

#include "flatten.h"
#include "ginac_cache.h"

#include "ginac/ginac.h"
#include "translator_data.h"
#include "expression_types.h"

#include "boost/timer/timer.hpp"


// abstract u-tensor factory class
class u_tensor_factory
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    u_tensor_factory(translator_data& p,  ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& c);

    virtual ~u_tensor_factory() = default;


		// INTERFACE - COMPUTE FLOW TENSORS AND RELATIVES

  public:

    virtual void compute_sr_u(std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u1(std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    // the A, B and C tensors are associated with the Hamiltonian for
    // scalar field fluctuations [see written notes]
    // actually, these compute A/H^2, B/H and C
    // all of these have mass dimension -1

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    // compute M tensor

    virtual void compute_M(std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;


    // INTERFACE - CALCULATE GAUGE TRANSFORMATIONS

  public:

    virtual void compute_zeta_xfm_1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

    virtual void compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) = 0;

		virtual void compute_deltaN_xfm_1(std::vector<GiNaC::ex>& v, flattener& fl) = 0;

		virtual void compute_deltaN_xfm_2(std::vector<GiNaC::ex>& v, flattener& fl) = 0;


		// INTERFACE - COMPUTE BACKGROUND OBJECTS

  public:

    virtual GiNaC::ex compute_Hsq() = 0;

    virtual GiNaC::ex compute_eps() = 0;
    
    
    // INTERFACE -- COMPUTE DERIVATIVES OF THE POTENTIAL
    
  public:
    
    virtual void compute_dV(std::vector<GiNaC::ex>& v, flattener& fl) = 0;
    
    virtual void compute_ddV(std::vector<GiNaC::ex>& v, flattener& fl) = 0;
    
    virtual void compute_dddV(std::vector<GiNaC::ex>& v, flattener& fl) = 0;


		// INTERFACE - UTILITY FUNCTIONS

  protected:

    unsigned int species(unsigned int z)
      {
        return (z >= this->num_fields ? z - this->num_fields : z);
      }

    unsigned int momentum(unsigned int z)
      {
        return (z >= this->num_fields ? z : z + this->num_fields);
      }

    const GiNaC::symbol& coordinate(unsigned int z)
      {
        return ((z < this->num_fields) ? this->field_list[z] : this->deriv_list[z - this->num_fields]);
      }

    bool is_field(unsigned int z)
      {
        return (z < this->num_fields);
      }

    bool is_momentum(unsigned int z)
      {
        return (z >= this->num_fields && z < 2 * this->num_fields);
      }


		// INTERFACE - STATISTICS

  public:

		// get total time spent doing symbolic computations
		boost::timer::nanosecond_type get_symbolic_compute_time() const { return(this->compute_timer.elapsed().wall); }


		// INTERNAL DATA

  protected:

		// parent translation unit
    translator_data& data_payload;

		// basic model details
    const unsigned int num_fields;
    const GiNaC::symbol& M_Planck;
    const GiNaC::ex V;

		// list of symbols representing fields and momenta in the model
    const std::vector<GiNaC::symbol> field_list;
    const std::vector<GiNaC::symbol> deriv_list;

    // reference to GiNaC expression cache, used to cache the output from our calculations.
    // the cache is intended to avoid expensive recomputation where possible
    ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache;

		// timer
		boost::timer::cpu_timer compute_timer;

  };


class canonical_u_tensor_factory: public u_tensor_factory
  {

  private:


		// CONSTRUCTOR

  public:

    canonical_u_tensor_factory(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& c)
	    : u_tensor_factory(p, c)
      {
      }
    
    virtual ~canonical_u_tensor_factory() = default;
    
    
    // INTERFACE - COMPUTE FLOW TENSORS AND RELATIVES

  public:

    virtual void compute_sr_u(std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u1(std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    // the A, B and C tensors are associated with the Hamiltonian for
    // scalar field fluctuations [see written notes]
    // actually, these compute A/H^2, B/H and C
    // all of these have mass dimension -1

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;
    
    // compute M tensor

    virtual void compute_M(std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;
    
    
    // INTERFACE - CALCULATE GAUGE TRANSFORMATIONS

  public:

    virtual void compute_zeta_xfm_1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

    virtual void compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v, flattener& fl) override;

		virtual void compute_deltaN_xfm_1(std::vector<GiNaC::ex>& v, flattener& fl) override;

		virtual void compute_deltaN_xfm_2(std::vector<GiNaC::ex>& v, flattener& fl) override;
    
    
    // INTERFACE - COMPUTE BACKGROUND OBJECTS
    
  public:

    virtual GiNaC::ex compute_Hsq() override;

    virtual GiNaC::ex compute_eps() override;
    
    
    // INTERFACE -- COMPUTE DERIVATIVES OF THE POTENTIAL
  
  public:
    
    virtual void compute_dV(std::vector<GiNaC::ex>& v, flattener& fl) override;
    
    virtual void compute_ddV(std::vector<GiNaC::ex>& v, flattener& fl) override;
    
    virtual void compute_dddV(std::vector<GiNaC::ex>& v, flattener& fl) override;


		// INTERNAL API

  protected:

    //! Compute field-field part of the 2nd-order gauge xfm
    GiNaC::ex compute_zeta_xfm_2_ff(unsigned int m, unsigned int n, GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

    //! Compute field-momentum part of the 2nd-order gauge xfm
    GiNaC::ex compute_zeta_xfm_2_fp(unsigned int m, unsigned int n, GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

    // compute A/H^2 [in the notation of the written notes]
    GiNaC::ex compute_A_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

    // compute B/H [in the notation of the written notes]
    GiNaC::ex compute_B_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

    // compute C [in the notation of the written notes]
    GiNaC::ex compute_C_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps);

    // compute xi/H^2 [in the notation of the written notes]
    GiNaC::ex compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps);

    // compute M-tensor component
    GiNaC::ex compute_M_component(unsigned int i, unsigned int j, GiNaC::ex& Hsq, GiNaC::ex& eps);

  };


// factory function to manufacture a u_tensor_factory instance
std::unique_ptr<u_tensor_factory> make_u_tensor_factory(translator_data& u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);


#endif //__u_tensor_factory_H_
