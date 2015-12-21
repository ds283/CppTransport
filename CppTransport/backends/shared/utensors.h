//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_UTENSORS_H
#define CPPTRANSPORT_MACROS_UTENSORS_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class utensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        utensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~utensors() = default;


        // INTERFACE

      public:

        //! return index- macros package
        const std::vector<index_rule>  get_index_rules() override;


        // INTERNAL API

      protected:

        std::unique_ptr<cse_map> pre_u1_predef(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_u2_predef(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_u3_predef(const macro_argument_list& args);


        // INTERNAL DATA

      private:

        //! u1 object
        std::unique_ptr<u1> u1_tensor;

        //! u2 object
        std::unique_ptr<u2> u2_tensor;

        //! u3 object
        std::unique_ptr<u3> u3_tensor;

      };
  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_UTENSORS_H
