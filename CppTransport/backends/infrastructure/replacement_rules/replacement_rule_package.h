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

        //! constructor
        replacement_rule_package(u_tensor_factory& uf, cse& cw, translator_data& p, language_printer& prn);

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

        //! generic CSE-map replacement rule for 1-index full tensor
        std::string replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        //! generic CSE-map replacement rule for 2-index full tensopr
        std::string replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        //! generic CSE-map replacement rule for 3-index full tensor
        std::string replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        //! generic CSE-map replacement rule for 1-index field-space tensor
        std::string replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        //! generic CSE-map replacement rule for 2-index field-space tensor
        std::string replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        //! generic CSE-map replacement rule for 3-index field-space tensor
        std::string replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map);


        // INTERNAL DATA

      protected:

        // REFERENCES TO EXTERNALLY-SUPPLIED AGENTS

        //! data payload from parent translator
        translator_data& data_payload;

        //! language-printer
        language_printer& printer;

        //! u-tensor factory
        u_tensor_factory& u_factory;

        // CSE worker object
        cse& cse_worker;


        // INTERNAL AGENTS

        //! index_flattener -- full
        right_order_flattener fl;

        //! index flattener -- field space
        right_order_flattener field_fl;

      };

  }


#endif // CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H
