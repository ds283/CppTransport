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

    std::string replacement_rule_package::replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(2*this->data_payload.get_number_fields());

        return((*map)[this->fl.flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(2*this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(2*this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label, k_label)]);
      }


    std::string replacement_rule_package::replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(this->data_payload.get_number_fields());

        return((*map)[this->fl.flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        this->fl.set_size(this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->fl.flatten(i_label, j_label, k_label)]);
      }

  }
