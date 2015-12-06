//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef __replacement_rule_package_H_
#define __replacement_rule_package_H_


#include "ginac/ginac.h"

#include "flatten.h"
#include "language_printer.h"
#include "cse.h"
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

        replacement_rule_package(translator_data& p, language_printer& prn)
          : data_payload(p),
            printer(prn),
            u_factory(nullptr),         // initialize utility objects to nullptr
            fl(nullptr),                // these will be populated later, when this rule package
            cse_worker(nullptr)         // is registered in a group
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


        // INTERFACE -- SET WORKER OBJECTS

      public:

        //! set u-tensor factory object
        inline void set_u_factory(u_tensor_factory* uf) { this->u_factory = uf; }

        //! set flattener object
        inline void set_flattener(flattener* f) { this->fl = f; }

        //! set CSE worker object
        inline void set_cse_worker(cse* cw) { this->cse_worker = cw; }


        // INTERFACE -- END OF INPUT

      public:

        //! report end of input; default implementation is empty, but can be overridden by
        //! implementation classes if needed
        virtual void report_end_of_input() { return; }


        // INTERFACE -- INTERNAL API

      protected:

        std::string replace_1index_tensor(const std::vector<std::string>& args,
                                          std::vector<index_assignment> indices, void* state);

        std::string replace_2index_tensor(const std::vector<std::string>& args,
                                          std::vector<index_assignment> indices, void* state);

        std::string replace_3index_tensor(const std::vector<std::string>& args,
                                          std::vector<index_assignment> indices, void* state);

        std::string replace_1index_field_tensor(const std::vector<std::string>& args,
                                                std::vector<index_assignment> indices, void* state);

        std::string replace_2index_field_tensor(const std::vector<std::string>& args,
                                                std::vector<index_assignment> indices, void* state);

        std::string replace_3index_field_tensor(const std::vector<std::string>& args,
                                                std::vector<index_assignment> indices, void* state);

        void generic_post_hook(void* state);

        unsigned int get_index_label(index_assignment& index);


        // INTERNAL DATA

      protected:

        translator_data&  data_payload;
        language_printer& printer;

        u_tensor_factory* u_factory;
        flattener*        fl;
        cse*              cse_worker;

      };

  }


#endif // __replacement_rule_package_H_