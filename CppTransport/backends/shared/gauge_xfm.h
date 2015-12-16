//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_gauge_xfm_H_
#define __macropackage_gauge_xfm_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class gauge_xfm: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        gauge_xfm(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& p, language_printer& prn)
          : replacement_rule_package(uf, f, cw, p, prn)
          {
          }

        //! destructor is default
        virtual ~gauge_xfm() = default;


        // INTERFACE

      public:

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::unique_ptr<cse_map> pre_zeta_xfm_1(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_zeta_xfm_2(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_deltaN_xfm_1(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_deltaN_xfm_2(const macro_argument_list& args);

      };

  } // namespace macro_packages


#endif //__gauge_xfm_H_
