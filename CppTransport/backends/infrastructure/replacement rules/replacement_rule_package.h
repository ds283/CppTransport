//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __replacement_rule_package_H_
#define __replacement_rule_package_H_


#include "ginac/ginac.h"

#include "macro.h"
#include "u_tensor_factory.h"
#include "flatten.h"
#include "language_printer.h"
#include "cse.h"
#include "replacement_data.h"


namespace macro_packages
  {

    class simple_rule
      {
      public:
        replacement_rule_simple  rule;
        unsigned int             args;
        std::string              name;
      };


    class index_rule
      {
      public:
        replacement_rule_index   rule;
        replacement_rule_pre     pre;
        replacement_rule_post    post;
        unsigned int             args;
        unsigned int             indices;
        unsigned int             range;
        std::string              name;
      };


    // abstract replacement_rule_package class
    class replacement_rule_package
      {
      public:
        replacement_rule_package(replacement_data& d, language_printer& p)
          : data(d), printer(p), u_factory(nullptr), fl(nullptr), cse_worker(nullptr)
          {
          }

        // these methods must be overridden by derived classes which implement the replacement_rule_package concept
        virtual const std::vector<simple_rule> get_pre_rules  ()   = 0;
        virtual const std::vector<simple_rule> get_post_rules ()  = 0;
        virtual const std::vector<index_rule>  get_index_rules() = 0;

        inline void                            set_u_factory  (u_tensor_factory* uf)  { this->u_factory = uf; }
        inline void                            set_flattener  (flattener* f)          { this->fl = f; }
        inline void                            set_cse_worker (cse* cw)               { this->cse_worker = cw; }

        inline void                            error          (std::string const msg) { ::error(msg, this->data.current_line, this->data.path); }
        inline void                            warn           (std::string const msg) { ::warn(msg, this->data.current_line, this->data.path); }

      protected:
        replacement_data& data;
        language_printer& printer;

        u_tensor_factory* u_factory;
        flattener*        fl;
        cse*              cse_worker;

        std::string       replace_1index_tensor      (const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);
        std::string       replace_2index_tensor      (const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);
        std::string       replace_3index_tensor      (const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);

        std::string       replace_1index_field_tensor(const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);
        std::string       replace_2index_field_tensor(const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);
        std::string       replace_3index_field_tensor(const std::vector<std::string> &args, std::vector<struct index_assignment> indices, void *state);

        void              generic_post_hook          (void* state);

        unsigned int      get_index_label            (struct index_assignment& index);
      };
  }


#endif // __replacement_rule_package_H_