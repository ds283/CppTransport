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

#ifndef CPPTRANSPORT_FACTORY_H
#define CPPTRANSPORT_FACTORY_H


#include <memory>

#include "resource_manager.h"

#include "language_printer.h"
#include "cse.h"
#include "expression_cache.h"
#include "shared_resources.h"
#include "translator_data.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "tensors/A.h"
#include "tensors/Atilde.h"
#include "tensors/B.h"
#include "tensors/C.h"
#include "tensors/connexion.h"
#include "tensors/coordinates.h"
#include "tensors/dV.h"
#include "tensors/ddV.h"
#include "tensors/dddV.h"
#include "tensors/dN1.h"
#include "tensors/dN2.h"
#include "tensors/fields.h"
#include "tensors/Hubble.h"
#include "tensors/M.h"
#include "tensors/metric.h"
#include "tensors/metric_inverse.h"
#include "tensors/parameters.h"
#include "tensors/Riemann_A2.h"
#include "tensors/Riemann_A3.h"
#include "tensors/Riemann_B3.h"
#include "tensors/SR_velocity.h"
#include "tensors/u1.h"
#include "tensors/u2.h"
#include "tensors/u3.h"
#include "tensors/zeta1.h"
#include "tensors/zeta2.h"

#include "boost/timer/timer.hpp"


//! tensor factory is the most generic factory concept.
//! It can manufacture tensor classes for the transport tensors that are common to all models.
class tensor_factory
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    tensor_factory(translator_data& p, expression_cache& c)
      : payload(p),
        cache(c),
        fl(p.model.get_number_params(), p.model.get_number_fields()),
        traits(p.model.get_number_fields())
      {
      }

    //! destructor
    virtual ~tensor_factory() = default;
    
    
    // SERVICES
    
  public:
    
    //! make an index flatten object
    index_flatten make_flatten() const { return fl; }

    
    // GENERIC TENSOR CLASSES

  public:

    //! obtain a Hubble-object
    virtual std::unique_ptr<Hubble> make_Hubble(language_printer& p, cse& cw) = 0;

    //! obtain a parameter object
    virtual std::unique_ptr<parameters> make_parameters(language_printer& p, cse& cw) = 0;
    
    //! obtain a fields object
    virtual std::unique_ptr<fields> make_fields(language_printer& p, cse& cw) = 0;
    
    //! obtain a coordinates object
    virtual std::unique_ptr<coordinates> make_coordinates(language_printer& p, cse& cw) = 0;
    
    //! obtain a dV-tensor
    virtual std::unique_ptr<dV> make_dV(language_printer& p, cse& cw) = 0;

    //! obtain a ddV-tensor
    virtual std::unique_ptr<ddV> make_ddV(language_printer& p, cse& cw) = 0;

    //! obtain a dddV-tensor
    virtual std::unique_ptr<dddV> make_dddV(language_printer& p, cse& cw) = 0;

    //! obtain SR tensor
    virtual std::unique_ptr<SR_velocity> make_SR_velocity(language_printer& p, cse& cw) = 0;

    //! obtain an A-tensor
    virtual std::unique_ptr<A> make_A(language_printer& p, cse& cw) = 0;
    
    //! obtain an Atilde-tensor
    virtual std::unique_ptr<Atilde> make_Atilde(language_printer& p, cse& cw) = 0;

    //! obtain a B-tensor
    virtual std::unique_ptr<B> make_B(language_printer& p, cse& cw) = 0;

    //! obtain a C-tensor
    virtual std::unique_ptr<C> make_C(language_printer& p, cse& cw) = 0;

    //! obtain an M-tensor
    virtual std::unique_ptr<M> make_M(language_printer& p, cse& cw) = 0;

    //! obtain a u1-tensor
    virtual std::unique_ptr<u1> make_u1(language_printer& p, cse& cw) = 0;

    //! obtain a u2-tensor
    virtual std::unique_ptr<u2> make_u2(language_printer& p, cse& cw) = 0;

    //! obtain a u3-tensor
    virtual std::unique_ptr<u3> make_u3(language_printer& p, cse& cw) = 0;

    //! obtain a zeta1-tensor
    virtual std::unique_ptr<zeta1> make_zeta1(language_printer& p, cse& cw) = 0;

    //! obtain a zeta2-tensor
    virtual std::unique_ptr<zeta2> make_zeta2(language_printer& p, cse& cw) = 0;

    //! obtain a dN1-tensor
    virtual std::unique_ptr<dN1> make_dN1(language_printer& p, cse& cw) = 0;

    //! obtain a dN2-tensor
    virtual std::unique_ptr<dN2> make_dN2(language_printer& p, cse& cw) = 0;


    // PROVIDE ACCESS TO RESOURCE MANAGER

  public:

    //! link to resource manager object
    virtual resource_manager& get_resource_manager() = 0;
    
    
    // QUERY FOR PERFORMANCE

  public:

    //! get time spent doing symbolic computation
    boost::timer::nanosecond_type get_symbolic_compute_time() const { return(this->compute_timer.elapsed().wall); }


    // INTERNAL DATA

  protected:

    // CACHES

    //! data payload provided by parent translation unit
    translator_data& payload;

    //! GiNaC expression cache
    expression_cache& cache;


    // RESOURCES


    // AGENTS

    //! index flattener
    index_flatten fl;

    //! index traits
    index_traits traits;


    // TIMERS

    //! compute timer
    boost::timer::cpu_timer compute_timer;

  };


//! curvature_tensor_factory inherits from tensor_factory, but provides APIs to manufacture
//! transport tensors for curvature quantities
class curvature_tensor_factory: public tensor_factory
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    curvature_tensor_factory(translator_data& p, expression_cache& c)
      : tensor_factory(p, c)
      {
      }
    
    //! destructor is default
    ~curvature_tensor_factory() = default;
    
    
    // CURVATURE TENSOR CLASSES
  
  public:
    
    //! obtain a connexion-"tensor" object
    virtual std::unique_ptr<connexion> make_connexion(language_printer& p, cse& cw) = 0;
    
    //! obtain a metric tensor object
    virtual std::unique_ptr<metric> make_metric(language_printer& p, cse& cw) = 0;
    
    //! obtain an inverse metric tensor object
    virtual std::unique_ptr<metric_inverse> make_metric_inverse(language_printer& p, cse& cw) = 0;
    
    //! obtain a Riemann-A2 tensor object
    virtual std::unique_ptr<Riemann_A2> make_Riemann_A2(language_printer& p, cse& cw) = 0;
    
    //! obtain a Riemann-A3 tensor object
    virtual std::unique_ptr<Riemann_A3> make_Riemann_A3(language_printer& p, cse& cw) = 0;
    
    //! obtain a Riemann-B3 tensor object
    virtual std::unique_ptr<Riemann_B3> make_Riemann_B3(language_printer& p, cse& cw) = 0;
    
  };


#endif //CPPTRANSPORT_FACTORY_H
