//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "kernel_argument_macros.h"
#include "translation_unit.h"
#include "index_assignment.h"
#include "msg_en.h"


#define BIND(X) std::bind(&kernel_argument_macros::X, this, std::placeholders::_1)


namespace shared
  {

    kernel_argument_macros::kernel_argument_macros(translation_unit* u, language_printer& p, std::string q, std::string l)
      : ::macro_packages::replacement_rule_package(u, p), qualifier(q), label(l)
      {
//        printer->set_label(label);
      }

    const std::vector<macro_packages::simple_rule> kernel_argument_macros::get_pre_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
                                                     { BIND(args_params), BIND(args_1index),
                                                       BIND(args_2index), BIND(args_2index),
                                                       BIND(args_3index), BIND(args_3index)
                                                     };

        const std::vector<std::string> names =
                                         { "PARAM_ARGS", "COORD_ARGS",
                                           "U2_ARGS", "TWOPF_ARGS",
                                           "U3_ARGS", "THREEPF_ARGS"
                                         };

        const std::vector<unsigned int> args =
                                          { 0, 1,
                                            1, 1,
                                            1, 1
                                          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            macro_packages::simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<macro_packages::simple_rule> kernel_argument_macros::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> kernel_argument_macros::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string kernel_argument_macros::args_params(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_param_list();

        std::ostringstream out;

        for(int i = 0; i < list.size(); ++i)
          {
            out << (i > 0 ? ", " : "") << "double " << list[i];
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_1index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string name     = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_ONEINDEX_NAME);

        std::ostringstream out;

        std::vector<struct index_abstract> indices;
        struct index_abstract A;
        A.label  = 'A';
        A.range  = INDEX_RANGE_ALL;
        indices.push_back(A);

        assignment_package assigner(this->unit->get_number_fields(), this->unit->get_number_parameters(), this->unit->get_index_order());

        std::vector< std::vector<struct index_assignment> > assignment = assigner.assign(indices);

        for(std::vector< std::vector<struct index_assignment> >::iterator t = assignment.begin(); t != assignment.end(); ++t)
          {
            out << (t != assignment.begin() ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;
            for(std::vector<struct index_assignment>::iterator u = (*t).begin(); u != (*t).end(); ++u)
              {
                out << "_" << assigner.value(*u);
              }
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_2index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_TWOINDEX_NAME);

        std::ostringstream out;

        std::vector< struct index_abstract > indices;
        struct index_abstract A;
        A.label  = 'A';
        A.range  = INDEX_RANGE_ALL;
        indices.push_back(A);
        struct index_abstract B;
        B.label  = 'B';
        B.range  = INDEX_RANGE_ALL;
        indices.push_back(B);

        assignment_package assigner(this->unit->get_number_fields(), this->unit->get_number_parameters(), this->unit->get_index_order());

        std::vector< std::vector<struct index_assignment> > assignment = assigner.assign(indices);

        for(std::vector< std::vector<struct index_assignment> >::iterator t = assignment.begin(); t != assignment.end(); ++t)
          {
            out << (t != assignment.begin() ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;
            for(std::vector<struct index_assignment>::iterator u = (*t).begin(); u != (*t).end(); ++u)
              {
                out << "_" << assigner.value(*u);
              }
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_3index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_THREEINDEX_NAME);

        std::ostringstream out;

        std::vector< struct index_abstract > indices;
        struct index_abstract A;
        A.label  = 'A';
        A.range  = INDEX_RANGE_ALL;
        indices.push_back(A);
        struct index_abstract B;
        B.label  = 'B';
        B.range  = INDEX_RANGE_ALL;
        indices.push_back(B);
        struct index_abstract C;
        C.label  = 'C';
        C.range  = INDEX_RANGE_ALL;
        indices.push_back(C);

        assignment_package assigner(this->unit->get_number_fields(), this->unit->get_number_parameters(), this->unit->get_index_order());

        std::vector< std::vector<struct index_assignment> > assignment = assigner.assign(indices);

        for(std::vector< std::vector<struct index_assignment> >::iterator t = assignment.begin(); t != assignment.end(); ++t)
          {
            out << (t != assignment.begin() ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;
            for(std::vector<struct index_assignment>::iterator u = (*t).begin(); u != (*t).end(); ++u)
              {
                out << "_" << assigner.value(*u);
              }
          }

        return(out.str());
      }


  } // namespace shared
