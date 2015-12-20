//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_lagrangian_tensors_H_
#define __macropackage_lagrangian_tensors_H_


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class lagrangian_tensors: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        lagrangian_tensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~lagrangian_tensors() = default;


        // INTERFACE

      public:

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::unique_ptr<cse_map> pre_A_tensor(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_B_tensor(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_C_tensor(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_A_predef(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_B_predef(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_C_predef(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_M_tensor(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_M_predef(const macro_argument_list& args);


        // INTERNAL DATA

      private:

        //! A-tensor object
        std::unique_ptr<A> A_tensor;

        //! B-tensor object
        std::unique_ptr<B> B_tensor;

        //! C-tensor object
        std::unique_ptr<C> C_tensor;

        //! M-tensor object
        std::unique_ptr<M> M_tensor;

      };

  }


#endif //__macropackage_lagrangian_tensors_H_
