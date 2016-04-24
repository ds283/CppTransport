//
// Created by David Seery on 29/06/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_U_TENSOR_FACTORY_H
#define CPPTRANSPORT_U_TENSOR_FACTORY_H

#include <iostream>
#include <stdexcept>

#include "flatten.h"
#include "ginac_cache.h"

#include "ginac/ginac.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "resource_manager.h"

#include "boost/timer/timer.hpp"


// abstract u-tensor factory class
class u_tensor_factory
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    u_tensor_factory(translator_data& p, expression_cache& c);

    virtual ~u_tensor_factory() = default;


    // INTERFACE -- GET RESOURCE MANAGER

  public:

    resource_manager& get_resource_manager() { return(this->res_mgr); }


		// INTERFACE - COMPUTE FLOW TENSORS AND RELATIVES

  public:

    virtual void compute_sr_u(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields, std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_u1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_u2(GiNaC::symbol& k, GiNaC::symbol& a,
                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_u3(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                            const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                            const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                            std::vector<GiNaC::ex>& v) = 0;

    // the A, B and C tensors are associated with the Hamiltonian for
    // scalar field fluctuations [see written notes]
    // actually, these compute A/H^2, B/H and C
    // all of these have mass dimension -1

    virtual void compute_A(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           const std::vector<GiNaC::symbol>& ddV, const std::vector<GiNaC::symbol>& dddV,
                           std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_B(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_C(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a,
                           const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v) = 0;

    // compute M tensor

    virtual void compute_M(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                           const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                           const std::vector<GiNaC::symbol>& ddV, std::vector<GiNaC::ex>& v) = 0;


    // INTERFACE - CALCULATE GAUGE TRANSFORMATIONS

  public:

    virtual void compute_zeta_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                    std::vector<GiNaC::ex>& v) = 0;

    virtual void compute_zeta_xfm_2(GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a,
                                    const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                    const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                    std::vector<GiNaC::ex>& v) = 0;

		virtual void compute_deltaN_xfm_1(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                      const std::vector<GiNaC::symbol>& derivs, std::vector<GiNaC::ex>& v) = 0;

		virtual void compute_deltaN_xfm_2(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                      const std::vector<GiNaC::symbol>& derivs, const std::vector<GiNaC::symbol>& dV,
                                      std::vector<GiNaC::ex>& v) = 0;


		// INTERFACE - COMPUTE BACKGROUND OBJECTS

  public:

    virtual GiNaC::ex compute_V(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields) = 0;

    virtual GiNaC::ex compute_Hsq(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                                  const std::vector<GiNaC::symbol>& derivs) = 0;

    virtual GiNaC::ex compute_eps(const std::vector<GiNaC::symbol>& derivs) = 0;
    
    
    // INTERFACE -- COMPUTE DERIVATIVES OF THE POTENTIAL
    
  public:
    
    virtual void compute_dV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                            std::vector<GiNaC::ex>& v) = 0;
    
    virtual void compute_ddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                             std::vector<GiNaC::ex>& v) = 0;
    
    virtual void compute_dddV(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields,
                              std::vector<GiNaC::ex>& v) = 0;


		// INTERNAL API - UTILITY FUNCTIONS

  protected:

    //! return species index corresponding to a given full-phase-space index
    unsigned int species(unsigned int z);

    //! return momentum index corresponding to a given full-phase-space index
    unsigned int momentum(unsigned int z);

    //! is this full-phase-space index a field?
    bool is_field(unsigned int z);

    //! is this full-phase-space index a coordinate?
    bool is_momentum(unsigned int z);


    // INTERNAL API - GINAC SUBSTITUTION

  protected:

    //! build substitution map for parameters+fields combination
    std::unique_ptr<GiNaC::exmap> substitution_map(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields);

    //! apply a substitution map to the potential
    GiNaC::ex substitute_V(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields);

    //! apply a substitution map to the potential -- argument list already build (prevents a rebuild inside the function)
    GiNaC::ex substitute_V(const std::vector<GiNaC::symbol>& params, const std::vector<GiNaC::symbol>& fields, const std::vector<GiNaC::ex>& args);


		// INTERFACE - STATISTICS

  public:

		// get total time spent doing symbolic computations
		boost::timer::nanosecond_type get_symbolic_compute_time() const { return(this->compute_timer.elapsed().wall); }


		// INTERNAL DATA

  protected:

    // LOCAL COPIES OF BASIC DATA

		//! cache number of fields
    const unsigned int num_fields;

    //! cache number of parameters
    const unsigned int num_params;

    //! cache M_Planck symbol
    const GiNaC::symbol& M_Planck;

    //! cache potential
    const GiNaC::ex V;


    // SYMBOL LISTS

		//! list of symbols representing fields in the model
    const std::vector<GiNaC::symbol> field_list;

    //! list of symbols representing derivatives in the model
    const std::vector<GiNaC::symbol> deriv_list;

    //! list of symbols representing parameters in the model
    const std::vector<GiNaC::symbol> param_list;


    // CACHES

    //! reference to GiNaC expression cache, used to cache the output from our calculations;
    //! the cache is intended to avoid expensive recomputation where possible
    expression_cache& cache;

    //! data payload provided by parent translation unit
    translator_data& data_payload;


    // AGENTS

    //! flattener -- full tensors
    right_order_flattener fl;

    //! flattener -- field-space tensors
    right_order_flattener field_fl;

    //! resource manager
    resource_manager res_mgr;


    // TIMERS

    //! timer
    boost::timer::cpu_timer compute_timer;

  };


class u_tensor_exception: std::runtime_error
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    u_tensor_exception(const std::string& msg)
      : std::runtime_error(msg)
      {
      }

    //! destructor is default
    virtual ~u_tensor_exception() = default;

  };


#endif //CPPTRANSPORT_U_TENSOR_FACTORY_H
