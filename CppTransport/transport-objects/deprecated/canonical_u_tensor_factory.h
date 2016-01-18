//
// Created by David Seery on 16/12/2015.
//

#ifndef CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H
#define CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H


#include "u_tensor_factory.h"


class canonical_u_tensor_factory: public u_tensor_factory
  {

		// CONSTRUCTOR

  public:

    canonical_u_tensor_factory(translator_data& p, expression_cache& c)
	    : u_tensor_factory(p, c)
      {
      }

    virtual ~canonical_u_tensor_factory() = default;


    // INTERFACE - COMPUTE FLOW TENSORS AND RELATIVES

  public:

    virtual void compute_sr_u(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            std::vector<GiNaC::ex>& v) override;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a,
                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v) override;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                            std::vector<GiNaC::ex>& v) override;

    // the A, B and C tensors are associated with the Hamiltonian for
    // scalar field fluctuations [see written notes]
    // actually, these compute A/H^2, B/H and C
    // all of these have mass dimension -1

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                           std::vector<GiNaC::ex>& v) override;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           std::vector<GiNaC::ex>& v) override;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v) override;

    // compute M tensor

    virtual void compute_M(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v) override;


    // INTERFACE - CALCULATE GAUGE TRANSFORMATIONS

  public:

    virtual void compute_zeta_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                    std::vector<GiNaC::ex>& v) override;

    virtual void compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a,
                                    const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                    std::vector<GiNaC::ex>& v) override;

		virtual void compute_deltaN_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                      const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v) override;

		virtual void compute_deltaN_xfm_2(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                      const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                      std::vector<GiNaC::ex>& v) override;


    // INTERFACE - COMPUTE BACKGROUND OBJECTS

  public:

    virtual GiNaC::ex compute_V(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields) override;

    virtual GiNaC::ex compute_Hsq(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                  const std::vector<GiNaC::symbol>& derivs) override;

    virtual GiNaC::ex compute_eps(const std::vector<GiNaC::symbol>& derivs) override;


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
    GiNaC::ex compute_zeta_xfm_2_ff(unsigned int m, unsigned int n,
                                    GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                    GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV);

    //! Compute field-momentum part of the 2nd-order gauge xfm
    GiNaC::ex compute_zeta_xfm_2_fp(unsigned int m, unsigned int n,
                                    GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2,
                                    GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                    const std::vector<GiNaC::symbol>& derivs);

    // compute A/H^2 [in the notation of the written notes]
    GiNaC::ex compute_A_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3,
                                  GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, const std::vector<GiNaC::symbol>& derivs,
                                  const std::vector<GiNaC::symbol>& dV, const std::vector<GiNaC::symbol>& ddV,
                                  const std::vector<GiNaC::symbol>& dddV, const std::vector<GiNaC::ex>& args);

    // compute B/H [in the notation of the written notes]
    GiNaC::ex compute_B_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3,
                                  GiNaC::symbol& a, GiNaC::ex& Hsq, GiNaC::ex& eps, const std::vector<GiNaC::symbol>& derivs,
                                  const std::vector<GiNaC::symbol>& dV, const std::vector<GiNaC::ex>& args);

    // compute C [in the notation of the written notes]
    GiNaC::ex compute_C_component(unsigned int i, GiNaC::symbol& k1, unsigned int j, GiNaC::symbol& k2, unsigned int k, GiNaC::symbol& k3,
                                  GiNaC::symbol& a, const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::ex>& args);

    // compute xi/H^2 [in the notation of the written notes]
    GiNaC::ex compute_xi(unsigned int i, GiNaC::ex& Hsq, GiNaC::ex& eps,
                         const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV);

    // compute M-tensor component
    GiNaC::ex compute_M_component(unsigned int i, unsigned int j, GiNaC::ex& Hsq, GiNaC::ex& eps,
                                  const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                  const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::ex>& args);

  };


#endif //CPPTRANSPORT_CANONICAL_U_TENSOR_FACTORY_H
