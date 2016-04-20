//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
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

#include "Hubble.h"
#include "SR_velocity.h"
#include "dV.h"
#include "ddV.h"
#include "dddV.h"
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

#include "boost/timer/timer.hpp"


class tensor_factory
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    tensor_factory(translator_data& p, expression_cache& c)
      : payload(p),
        cache(c),
        shared(p, mgr, c),
        fl(p.get_number_parameters(), p.get_number_fields()),
        traits(p.get_number_fields())
      {
      }

    //! destructor
    virtual ~tensor_factory() = default;


    // INTERFACE -- MANUFACTURE TENSOR OBJECTS

  public:

    //! obtain a Hubble-object
    virtual std::unique_ptr<Hubble> make_Hubble(language_printer& p, cse& cw) = 0;

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


    // INTERFACE -- PROVIDE ACCESS TO AGENTS

  public:

    //! link to resource manager object
    resource_manager& get_resource_manager() { return(this->mgr); }

    //! link to shared resources object
    shared_resources& get_shared_resources() { return(this->shared); }


    // INTERFACE -- QUERY FOR PERFORMANCE

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

    //! resource manager
    resource_manager mgr;


    // RESOURCES

    //! shared resources; must be constructed *after* resource_manager
    shared_resources shared;


    // AGENTS

    //! index flattener
    index_flatten fl;

    //! index traits
    index_traits traits;


    // TIMERS

    //! compute timer
    boost::timer::cpu_timer compute_timer;

  };


#endif //CPPTRANSPORT_FACTORY_H
