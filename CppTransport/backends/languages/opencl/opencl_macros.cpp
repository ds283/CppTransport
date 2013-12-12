//
// Created by David Seery on 11/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "opencl_macros.h"
#include "translation_unit.h"
#include "msg_en.h"


#define BIND(X) std::bind(&opencl_macros::X, this, std::placeholders::_1)


namespace opencl
  {

    opencl_macros::opencl_macros(translation_unit* u, language_printer& p, std::string l)
      : ::macro_packages::replacement_rule_package(u, p), label(l)
      {
//        printer->set_label(label);
      }

    const std::vector<macro_packages::simple_rule> opencl_macros::get_pre_rules()
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

        for(int i = 0; i < rules.size(); i++)
          {
            macro_packages::simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<macro_packages::simple_rule> opencl_macros::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> opencl_macros::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string opencl_macros::args_params(const std::vector<std::string> &args)
      {
        std::vector<std::string> list = this->unit->get_param_list();

        std::ostringstream out;

        for(int i = 0; i < list.size(); i++)
          {
            out << (i > 0 ? ", " : "") << "double " << list[i];
          }

        return(out.str());
      }


    std::string opencl_macros::args_1index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string name     = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_ONEINDEX_NAME);
        unsigned int ncoords = 2*this->unit->get_number_fields();

        this->fl->set_size(ncoords);

        std::ostringstream out;

        for(int i = 0; i < ncoords; i++)
          {
            out << (i > 0 ? ", " : "") << "global double* " << name << "_" << this->fl->flatten(i);
          }

        return(out.str());
      }


    std::string opencl_macros::args_2index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_TWOINDEX_NAME);
        unsigned int ncoords = 2*this->unit->get_number_fields();

        this->fl->set_size(ncoords);

        std::ostringstream out;

        for(int i = 0; i < ncoords; i++)
          {
            for(int j = 0; j < ncoords; j++)
              {
                out << (i == 0 && j == 0 ? "" : ", ") << "global double* " << name << "_" << i << "_" << j;
              }
          }

        return(out.str());
      }


    std::string opencl_macros::args_3index(const std::vector<std::string>& args)
      {
        assert(args.size() == 1);

        std::string  name    = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_THREEINDEX_NAME);
        unsigned int ncoords = 2*this->unit->get_number_fields();

        this->fl->set_size(ncoords);

        std::ostringstream out;

        for(int i = 0; i < ncoords; i++)
          {
            for(int j = 0; j < ncoords; j++)
              {
                for(int k = 0; k < ncoords; k++)
                  {
                    out << (i == 0 && j == 0 && k == 0 ? "" : ", ") << "global double* " << name << "_" << i << "_" << j << "_" << k;
                  }
              }
          }

        return(out.str());
      }


  } // namespace opencl
