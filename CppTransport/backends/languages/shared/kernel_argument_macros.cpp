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


#define BIND(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace shared
  {


    kernel_argument_macros::kernel_argument_macros(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : ::macro_packages::replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND(args_params, "PARAM_ARGS"));
        pre_package.emplace_back(BIND(args_1index, "COORD_ARGS"));
        pre_package.emplace_back(BIND(args_2index, "U2_ARGS"));
        pre_package.emplace_back(BIND(args_2index, "TWOPF_ARGS"));
        pre_package.emplace_back(BIND(args_3index, "U3_ARGS"));
        pre_package.emplace_back(BIND(args_3index, "THREEPF_ARGS"));
      }


    // *******************************************************************


    std::string args_params::evaluate(const macro_argument_list& args)
      {
        std::vector<std::string> list = this->data_payload.get_param_list();

        std::ostringstream out;

        for(int i = 0; i < list.size(); ++i)
          {
            out << (i > 0 ? ", " : "") << "double " << list[i];
          }

        return(out.str());
      }


    std::string args_1index::evaluate(const macro_argument_list& args)
      {
        std::string name = args[COORD_ARGS_NAME_ARGUMENT];

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


    std::string args_2index::evaluate(const macro_argument_list& args)
      {
        std::string name = args[TWOPF_ARGS_NAME_ARGUMENT];

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


    std::string args_3index::evaluate(const macro_argument_list& args)
      {
        std::string name = args[THREEPF_ARGS_NAME_ARGUMENT];

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

            for(const assignment_record& t : *assign)
              {
                out << "_" << t.get_numeric_value();
              }

            ++c;
          }

        return(out.str());
      }


  } // namespace shared
