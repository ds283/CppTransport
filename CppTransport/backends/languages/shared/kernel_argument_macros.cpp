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

    kernel_argument_macros::kernel_argument_macros(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& p, language_printer& prn,
                                                   std::string q, std::string l)
      : ::macro_packages::replacement_rule_package(uf, f, cw, p, prn),
        qualifier(q),
        label(l)
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
            package.emplace_back(names[i], rules[i], args[i]);
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
        std::vector<std::string> list = this->data_payload.get_param_list();

        std::ostringstream out;

        for(int i = 0; i < list.size(); ++i)
          {
            out << (i > 0 ? ", " : "") << "double " << list[i];
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_1index(const macro_argument_list& args)
      {
        assert(args.size() == 1);

        std::string name = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_ONEINDEX_NAME);

        std::ostringstream out;

        // build a set of assignments for a fake index 'A'
        // the assignments will range over all possible values which A can assume
        // we use these to construct a list of arguments, one for each possible value
        abstract_index_list indices;
        indices.emplace_back('A', std::make_unique<abstract_index>('A', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));

        assignment_set assignments(indices, this->data_payload.get_index_order());

        unsigned int c = 0;
        for(std::unique_ptr<assignment_list> assign : assignments)
          {
            out << (c != 0 ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;

            for(const assignment_record& t : *assign)
              {
                out << "_" << t.get_numeric_value();
              }

            ++c;
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_2index(const macro_argument_list& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_TWOINDEX_NAME);

        std::ostringstream out;

        abstract_index_list indices;
        indices.emplace_back('A', std::make_unique<abstract_index>('A', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));
        indices.emplace_back('B', std::make_unique<abstract_index>('B', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));

        assignment_set assignments(indices, this->data_payload.get_index_order());

        unsigned int c = 0;
        for(std::unique_ptr<assignment_list> assign : assignments)
          {
            out << (c != 0 ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;

            for(const assignment_record& t : *assign)
              {
                out << "_" << t.get_numeric_value();
              }

            ++c;
          }

        return(out.str());
      }


    std::string kernel_argument_macros::args_3index(const macro_argument_list& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_THREEINDEX_NAME);

        std::ostringstream out;

        abstract_index_list indices;
        indices.emplace_back('A', std::make_unique<abstract_index>('A', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));
        indices.emplace_back('B', std::make_unique<abstract_index>('B', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));
        indices.emplace_back('C', std::make_unique<abstract_index>('C', this->data_payload.get_number_fields(), this->data_payload.get_number_parameters()));

        assignment_set assignments(indices, this->data_payload.get_index_order());

        unsigned int c = 0;
        for(std::unique_ptr<assignment_list> assign : assignments)
          {
            out << (c != 0 ? ", " : "") << this->qualifier << (this->qualifier != "" ? " " : "") << "double* " << name;


            ++c;

            ++c;
          }

        return(out.str());
      }


  } // namespace shared
