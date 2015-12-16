//
// Created by David Seery on 16/12/2015.
//

#ifndef CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H
#define CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H


#include "u_tensor_factory.h"


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

    virtual void compute_sr_u(std::vector<GiNaC::ex>& v) override;

    virtual void compute_u1(std::vector<GiNaC::ex>& v) override;

    virtual void compute_u1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    // the A, B and C tensors are associated with the Hamiltonian for
    // scalar field fluctuations [see written notes]
    // actually, these compute A/H^2, B/H and C
    // all of these have mass dimension -1

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v) override;

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v) override;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, std::vector<GiNaC::ex>& v) override;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    // compute M tensor

    virtual void compute_M(std::vector<GiNaC::ex>& v) override;

    virtual void compute_M(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;


    // INTERFACE - CALCULATE GAUGE TRANSFORMATIONS

  public:

    virtual void compute_zeta_xfm_1(GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

    virtual void compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, std::vector<GiNaC::ex>& v) override;

		virtual void compute_deltaN_xfm_1(std::vector<GiNaC::ex>& v) override;

		virtual void compute_deltaN_xfm_2(std::vector<GiNaC::ex>& v) override;


    // INTERFACE - COMPUTE BACKGROUND OBJECTS

  public:

    virtual GiNaC::ex compute_Hsq() override;

    virtual GiNaC::ex compute_eps() override;


    // INTERFACE -- COMPUTE DERIVATIVES OF THE POTENTIAL

  public:

    virtual void compute_dV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            std::vector<GiNaC::ex>& v) override;

    virtual void compute_ddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                             std::vector<GiNaC::ex>& v) override;

    virtual void compute_dddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                              std::vector<GiNaC::ex>& v) override;


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
inline std::unique_ptr<u_tensor_factory> make_u_tensor_factory(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  {
    // at the moment, nothing to do - only canonical models implemented
    return std::make_unique<canonical_u_tensor_factory>(p, cache);
  }


#endif //CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H
