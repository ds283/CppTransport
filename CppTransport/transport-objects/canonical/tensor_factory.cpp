//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "tensor_factory.h"

#include "Hubble.h"
#include "dV.h"
#include "ddV.h"
#include "dddV.h"
#include "SR_velocity.h"
#include "A.h"
#include "B.h"
#include "C.h"
#include "M.h"
#include "u1.h"
#include "u2.h"
#include "u3.h"
#include "zeta1.h"
#include "zeta2.h"
#include "dN1.h"
#include "dN2.h"


namespace canonical
  {

    std::unique_ptr<Hubble> tensor_factory::make_Hubble(language_printer& p)
      {
        std::unique_ptr<canonical_Hubble> obj =  std::make_unique<canonical_Hubble>(p, this->cres);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<dV> tensor_factory::make_dV(language_printer& p)
      {
        std::unique_ptr<canonical_dV> obj = std::make_unique<canonical_dV>(p, this->cres, this->shared, this->fl);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<ddV> tensor_factory::make_ddV(language_printer& p)
      {
        std::unique_ptr<canonical_ddV> obj = std::make_unique<canonical_ddV>(p, this->cres, this->shared, this->fl);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<dddV> tensor_factory::make_dddV(language_printer& p)
      {
        std::unique_ptr<canonical_dddV> obj = std::make_unique<canonical_dddV>(p, this->cres, this->shared, this->fl);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<SR_velocity> tensor_factory::make_SR_velocity(language_printer& p)
      {
        std::unique_ptr<canonical_SR_velocity> obj = std::make_unique<canonical_SR_velocity>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl);
        return std::move(obj);
      }


    std::unique_ptr<A> tensor_factory::make_A(language_printer& p)
      {
        std::unique_ptr<canonical_A> obj = std::make_unique<canonical_A>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<B> tensor_factory::make_B(language_printer& p)
      {
        std::unique_ptr<canonical_B> obj = std::make_unique<canonical_B>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<C> tensor_factory::make_C(language_printer& p)
      {
        std::unique_ptr<canonical_C> obj = std::make_unique<canonical_C>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<M> tensor_factory::make_M(language_printer& p)
      {
        std::unique_ptr<canonical_M> obj = std::make_unique<canonical_M>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<u1> tensor_factory::make_u1(language_printer& p)
      {
        std::unique_ptr<canonical_u1> obj = std::make_unique<canonical_u1>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<u2> tensor_factory::make_u2(language_printer& p)
      {
        std::unique_ptr<canonical_u2> obj = std::make_unique<canonical_u2>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);     // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<u3> tensor_factory::make_u3(language_printer& p)
      {
        std::unique_ptr<canonical_u3> obj = std::make_unique<canonical_u3>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);     // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<zeta1> tensor_factory::make_zeta1(language_printer& p)
      {
        std::unique_ptr<canonical_zeta1> obj = std::make_unique<canonical_zeta1>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<zeta2> tensor_factory::make_zeta2(language_printer& p)
      {
        std::unique_ptr<canonical_zeta2> obj = std::make_unique<canonical_zeta2>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);     // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<dN1> tensor_factory::make_dN1(language_printer& p)
      {
        std::unique_ptr<canonical_dN1> obj = std::make_unique<canonical_dN1>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);      // require std::move() here because there is no implicit conversion for upcast
      }


    std::unique_ptr<dN2> tensor_factory::make_dN2(language_printer& p)
      {
        std::unique_ptr<canonical_dN2> obj = std::make_unique<canonical_dN2>(p, this->cache, this->cres, this->shared, this->compute_timer, this->fl, this->traits);
        return std::move(obj);     // require std::move() here because there is no implicit conversion for upcast
      }

  }
