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

#include "Hubble.h"
#include "fields.h"
#include "coordinates.h"
#include "dV.h"
#include "ddV.h"
#include "dddV.h"
#include "SR_velocity.h"
#include "A.h"
#include "Atilde.h"
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


namespace nontrivial_metric
  {

    std::unique_ptr<Hubble> tensor_factory::make_Hubble(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_Hubble>(p, cw, this->res);
      }


    std::unique_ptr<dV> tensor_factory::make_dV(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_dV>(p, cw, this->res, this->shared, this->fl);
      }


    std::unique_ptr<ddV> tensor_factory::make_ddV(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_ddV>(p, cw, this->res, this->shared, this->fl);
      }


    std::unique_ptr<dddV> tensor_factory::make_dddV(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_dddV>(p, cw, this->res, this->shared, this->fl);
      }


    std::unique_ptr<SR_velocity> tensor_factory::make_SR_velocity(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_SR_velocity>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl);
      }


    std::unique_ptr<A> tensor_factory::make_A(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_A>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }
    
    
    std::unique_ptr<Atilde> tensor_factory::make_Atilde(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_Atilde>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<B> tensor_factory::make_B(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_B>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<C> tensor_factory::make_C(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_C>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<M> tensor_factory::make_M(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_M>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<u1> tensor_factory::make_u1(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_u1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<u2> tensor_factory::make_u2(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_u2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<u3> tensor_factory::make_u3(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_u3>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<zeta1> tensor_factory::make_zeta1(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_zeta1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<zeta2> tensor_factory::make_zeta2(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_zeta2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<dN1> tensor_factory::make_dN1(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_dN1>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }


    std::unique_ptr<dN2> tensor_factory::make_dN2(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_dN2>(p, cw, this->cache, this->res, this->shared, this->compute_timer, this->fl, this->traits);
      }
    
    
    std::unique_ptr<fields> tensor_factory::make_fields(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_fields>(p, cw, this->res, this->shared, this->fl, this->traits);
      }
    
    
    std::unique_ptr<coordinates> tensor_factory::make_coordinates(language_printer& p, cse& cw)
      {
        return std::make_unique<nontrivial_metric_coordinates>(p, cw, this->res, this->shared, this->fl, this->traits);
      }
    
    
    std::unique_ptr<parameters> tensor_factory::make_parameters(language_printer& p, cse& cw)
      {
        return std::make_unique<parameters>(p, cw, this->shared, this->fl, this->traits);
      }
    
  }
