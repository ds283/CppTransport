//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __canonical_model_H_
#define __canonical_model_H_


#include <string>
#include <vector>

#include "transport/models/model.h"


namespace transport
  {

    // a canonical model: allows an arbitrary number of fields, but flat field-space metric.
    // has a potential but no other structure
    template <typename number>
    class canonical_model : public model<number>
      {
      public:
        canonical_model(const std::string& n, const std::string &a, const std::string& t, number Mp,
                        unsigned int N_f, unsigned int N_p,
                        const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                        const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                        const std::vector<std::string>& s_names,
                        const std::vector<number>& ps);

        // calculate potential, given a field configuration. Pure virtual, so must be overridden by derived class
        virtual number                  V(std::vector<number> coords) = 0;
      };


    //  IMPLEMENTATION -- CLASS CANONICAL_MODEL


    template <typename number>
    canonical_model<number>::canonical_model(const std::string& n, const std::string& a, const std::string& t, number Mp,
                                             unsigned int N_f, unsigned int N_p,
                                             const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                                             const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                                             const std::vector<std::string>& s_names,
                                             const std::vector<number>& ps)
      : model<number>(n, a, t, Mp, N_f, N_p, f_names, fl_names, p_names, pl_names, s_names, ps)
      // invoke constructor of parent class to initialize const members
      {
      }

  }


#endif //__canonical_model_H_
