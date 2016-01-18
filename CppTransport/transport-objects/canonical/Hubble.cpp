//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#include "Hubble.h"


namespace canonical
  {

    GiNaC::ex canonical_Hubble::compute_V()
      {
        return this->res.V_resource(this->cse_worker, this->printer);
      }


    GiNaC::ex canonical_Hubble::compute_Hsq()
      {
        return this->res.Hsq_resource(this->cse_worker, this->printer);
      }


    GiNaC::ex canonical_Hubble::compute_eps()
      {
        return this->res.eps_resource(this->cse_worker, this->printer);
      }

  }   // namespace canonical

