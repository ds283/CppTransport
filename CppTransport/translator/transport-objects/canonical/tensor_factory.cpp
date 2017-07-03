//
// Created by David Seery on 19/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#include "tensor_factory.h"

#include "tensors/Hubble.h"
#include "tensors/fields.h"
#include "tensors/coordinates.h"
#include "tensors/momenta.h"
#include "tensors/dV.h"
#include "tensors/ddV.h"
#include "tensors/dddV.h"
#include "tensors/SR_velocity.h"
#include "tensors/A.h"
#include "tensors/Atilde.h"
#include "tensors/B.h"
#include "tensors/C.h"
#include "tensors/M.h"
#include "tensors/u1.h"
#include "tensors/u2.h"
#include "tensors/u3.h"
#include "tensors/zeta1.h"
#include "tensors/zeta2.h"
#include "tensors/dN1.h"
#include "tensors/dN2.h"


namespace canonical
  {
    
    std::unique_ptr<::Hubble> tensor_factory::make_Hubble(language_printer& p, cse& cw)
      {
        return std::make_unique<Hubble>(p, cw, this->res);
      }
    
    
    std::unique_ptr<::dV> tensor_factory::make_dV(language_printer& p, cse& cw)
      {
        return std::make_unique<dV>(p, cw, this->res, this->shared, this->fl);
      }
    
    
    std::unique_ptr<::ddV> tensor_factory::make_ddV(language_printer& p, cse& cw)
      {
        return std::make_unique<ddV>(p, cw, this->res, this->shared, this->fl);
      }
    
    
    std::unique_ptr<::dddV> tensor_factory::make_dddV(language_printer& p, cse& cw)
      {
        return std::make_unique<dddV>(p, cw, this->res, this->shared, this->fl);
      }
    
    
    std::unique_ptr<::SR_velocity> tensor_factory::make_SR_velocity(language_printer& p, cse& cw)
      {
        return std::make_unique<SR_velocity>(p, cw, this->cache, this->res, this->shared, this->compute_timer,
                                             this->fl);
      }
    
    
    std::unique_ptr<::A> tensor_factory::make_A(language_printer& p, cse& cw)
      {
        return std::make_unique<A>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                   this->traits);
      }
    
    
    std::unique_ptr<::Atilde> tensor_factory::make_Atilde(language_printer& p, cse& cw)
      {
        return std::make_unique<Atilde>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                        this->traits);
      }
    
    
    std::unique_ptr<::B> tensor_factory::make_B(language_printer& p, cse& cw)
      {
        return std::make_unique<B>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                   this->traits);
      }
    
    
    std::unique_ptr<::C> tensor_factory::make_C(language_printer& p, cse& cw)
      {
        return std::make_unique<C>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                   this->traits);
      }
    
    
    std::unique_ptr<::M> tensor_factory::make_M(language_printer& p, cse& cw)
      {
        return std::make_unique<M>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                   this->traits);
      }
    
    
    std::unique_ptr<::u1> tensor_factory::make_u1(language_printer& p, cse& cw)
      {
        return std::make_unique<u1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                    this->traits);
      }
    
    
    std::unique_ptr<::u2> tensor_factory::make_u2(language_printer& p, cse& cw)
      {
        return std::make_unique<u2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                    this->traits);
      }
    
    
    std::unique_ptr<::u3> tensor_factory::make_u3(language_printer& p, cse& cw)
      {
        return std::make_unique<u3>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                    this->traits);
      }
    
    
    std::unique_ptr<::zeta1> tensor_factory::make_zeta1(language_printer& p, cse& cw)
      {
        return std::make_unique<zeta1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                       this->traits);
      }
    
    
    std::unique_ptr<::zeta2> tensor_factory::make_zeta2(language_printer& p, cse& cw)
      {
        return std::make_unique<zeta2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                       this->traits);
      }
    
    
    std::unique_ptr<::dN1> tensor_factory::make_dN1(language_printer& p, cse& cw)
      {
        return std::make_unique<dN1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                     this->traits);
      }
    
    
    std::unique_ptr<::dN2> tensor_factory::make_dN2(language_printer& p, cse& cw)
      {
        return std::make_unique<dN2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl,
                                     this->traits);
      }
    
    
    std::unique_ptr<::fields> tensor_factory::make_fields(language_printer& p, cse& cw)
      {
        return std::make_unique<fields>(p, cw, this->res, this->shared, this->fl, this->traits);
      }
    
    
    std::unique_ptr<::momenta> tensor_factory::make_momenta(language_printer& p, cse& cw)
      {
        return std::make_unique<momenta>(p, cw, this->res, this->shared, this->fl, this->traits);
      }
    
    
    std::unique_ptr<::coordinates> tensor_factory::make_coordinates(language_printer& p, cse& cw)
      {
        return std::make_unique<coordinates>(p, cw, this->res, this->shared, this->fl, this->traits);
      }
    
    
    std::unique_ptr<::parameters> tensor_factory::make_parameters(language_printer& p, cse& cw)
      {
        return std::make_unique<parameters>(p, cw, this->shared, this->fl, this->traits);
      }
    
  }
