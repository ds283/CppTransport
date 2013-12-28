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
    template <typename number, unsigned int Nf, unsigned int Np>
    class canonical_model : public model<number, Nf, Np>
      {
      public:
        canonical_model(instance_manager<number>* mgr, const std::string& uid,
                        const std::string& n, const std::string &a, const std::string& t,
                        const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                        const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                        const std::vector<std::string>& s_names)
          : model<number, Nf, Np>(mgr, uid, n, a, t, f_names, fl_names, p_names, pl_names, s_names)
          {
          }

        // calculate potential, given a field configuration. Pure virtual, so must be overridden by derived class
        virtual number V(const parameters<number>& p, const std::vector<number>& coords) = 0;
      };

  }


#endif //__canonical_model_H_
