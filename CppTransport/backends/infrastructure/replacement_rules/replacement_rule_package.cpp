//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>

#include "replacement_rule_package.h"

#include "macro.h"
#include "flatten.h"
#include "cse.h"
#include "translation_unit.h"
#include "error.h"


namespace macro_packages
  {

    replacement_rule_package::replacement_rule_package(u_tensor_factory& uf, cse& cw, translator_data& p,
                                                       language_printer& prn)
      : data_payload(p),
        printer(prn),
        u_factory(uf),
        cse_worker(cw),
        fl(2 * p.get_number_fields()),
        field_fl(p.get_number_fields())
      {
      }


    std::string replacement_rule_package::replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        return((*map)[this->fl.flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label, k_label)]);
      }


    std::string replacement_rule_package::replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        return((*map)[this->field_fl.flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->field_fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->field_fl.flatten(i_label, j_label, k_label)]);
      }

  }
