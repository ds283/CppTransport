//
// Created by David Seery on 08/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROS_VEXCL_KERNELS_H
#define CPPTRANSPORT_MACROS_VEXCL_KERNELS_H


#include "replacement_rule_package.h"


namespace vexcl
  {

    class import_kernel : public ::macro_packages::replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        import_kernel(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~import_kernel() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! language printer
        language_printer& printer;

      };


    class vexcl_kernels : public ::macro_packages::replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vexcl_kernels(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~vexcl_kernels() = default;


        // INTERFACE

      public:

        const std::vector<macro_packages::index_rule>  get_index_rules();

      };

  } // namespace vexcl


#endif //CPPTRANSPORT_MACROS_VEXCL_KERNELS_H
