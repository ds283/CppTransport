//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
#define CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H


#include <string>

#include "replacement_rule_package.h"


namespace macro_packages
  {

    class flow_tensors : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        flow_tensors(u_tensor_factory& uf, cse& cw, translator_data& p, language_printer& prn)
          : replacement_rule_package(uf, cw, p, prn)
          {
          }

        //! destructor is default
        virtual ~flow_tensors() = default;


        // INTERFACE

      public:

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

        // INTERNAL API

      protected:

        std::string replace_V(const macro_argument_list& args);

        std::string replace_Hsq(const macro_argument_list& args);

        std::string replace_eps(const macro_argument_list& args);

        std::string replace_parameter(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_field(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_coordinate(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::unique_ptr<cse_map> pre_sr_velocity(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_dV(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_ddV(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_dddV(const macro_argument_list& args);

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
