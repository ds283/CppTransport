//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H
#define CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H


#include "ginac/ginac.h"

#include "flatten.h"
#include "language_printer.h"
#include "cse.h"
#include "cse_map.h"
#include "error.h"
#include "replacement_rule_definitions.h"
#include "index_assignment.h"
#include "translator_data.h"


// need forward reference to avoid circularity
class u_tensor_factory;


namespace macro_packages
  {

    // abstract replacement_rule_package class
    class replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        replacement_rule_package(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn),
            u_factory(uf),
            fl(f),
            cse_worker(cw)
          {
          }

        // provide virtual destructor so that derived classes delete correctly
        virtual ~replacement_rule_package() = default;


        // INTERFACE

      public:

        // these methods must be overridden by derived classes which implement the replacement_rule_package concept

        //! get pre-rules
        virtual const std::vector<simple_rule> get_pre_rules  () = 0;

        //! get post-rules
        virtual const std::vector<simple_rule> get_post_rules () = 0;

        //! get index rules
        virtual const std::vector<index_rule>  get_index_rules() = 0;


        // INTERFACE -- END OF INPUT

      public:

        //! report end of input; default implementation is empty, but can be overridden by
        //! implementation classes if needed
        virtual void report_end_of_input() { return; }


        // INTERFACE -- INTERNAL API
        // COMMON UTILITY FUNCTIONS FOR MACRO REPLACEMENT

      protected:

        std::string replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);


        // INTERNAL DATA

      protected:

        translator_data&  data_payload;
        language_printer& printer;

        u_tensor_factory& u_factory;
        flattener&        fl;
        cse&              cse_worker;

      };

  }


#endif // CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H
