//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __canonical_model_H_
#define __canonical_model_H_


#include <string>
#include <vector>

#include "transport-runtime-api/models/model.h"


namespace transport
  {

    // a canonical model: allows an arbitrary number of fields, but flat field-space metric.
    // has a potential but no other structure
    template <typename number>
    class canonical_model : public model<number>
      {
      public:
        canonical_model(instance_manager<number>* mgr, const std::string& uid, unsigned int tver)
          : model<number>(mgr, uid, tver)
          {
          }

        // calculate potential, given a field configuration. Pure virtual, so must be overridden by derived class
        virtual number V(const parameters<number>& p, const std::vector<number>& coords) const = 0;
      };

  }


#endif //__canonical_model_H_
