//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_background_H_
#define __macropackage_background_H_


#include <string>

#include "macro.h"

#include "replacement_rule_package.h"

namespace macro_packages
  {

    class flow_tensors : public replacement_rule_package
      {
      public:
        flow_tensors(replacement_data& d, ginac_printer p)
          : replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

      protected:

        std::string replace_V         (const std::vector<std::string> &args);
        std::string replace_Hsq       (const std::vector<std::string> &args);
        std::string replace_eps       (const std::vector<std::string> &args);

        std::string replace_parameter (const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state);
        std::string replace_field     (const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state);
        std::string replace_coordinate(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state);

        void*       pre_sr_velocity   (const std::vector<std::string>& args);

      };

  } // namespace macro_packages


#endif //__macropackage_background_H_
