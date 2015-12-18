//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MACROPACKAGE_RESOURCES_H
#define CPPTRANSPORT_MACROPACKAGE_RESOURCES_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class resources : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(u_tensor_factory& uf, cse& cw, translator_data& p, language_printer& prn)
          : replacement_rule_package(uf, cw, p, prn)
          {
          }

        //! destructor is default
        virtual ~resources() = default;


        // INTERFACE

      public:

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string set_params(const macro_argument_list& args);

        std::string set_coordinates(const macro_argument_list& args);

        std::string set_dV(const macro_argument_list& args);

        std::string set_ddV(const macro_argument_list& args);

        std::string set_dddV(const macro_argument_list& args);

        std::string set_connexion(const macro_argument_list& args);

        std::string set_Riemann(const macro_argument_list& args);

        std::string release(const macro_argument_list& args);

      };

  }


#endif //CPPTRANSPORT_MACROPACKAGE_RESOURCES_H
