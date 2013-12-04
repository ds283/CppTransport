//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "background.h"
#include "macro.h"


namespace macro_packages
  {

    std::string background::replace_V(const std::vector<std::string> &args)
      {
        GiNaC::ex potential = this->data.source->get_potential();

        std::ostringstream out;
        out << GiNaC::csrc << potential;

        return(out.str());
      }

    std::string background::replace_Hsq(const std::vector<std::string> &args)
      {
        GiNaC::ex Hsq = this->data.u_factory->compute_Hsq();

        std::ostringstream out;
        out << GiNaC::csrc << Hsq;

        return(out.str());
      }

    std::string background::replace_eps(const std::vector<std::string> &args)
      {
        GiNaC::ex eps = this->data.u_factory->compute_eps();

        std::ostringstream out;
        out << GiNaC::csrc << eps;

        return(out.str());
      }

  } // namespace macro_packages
