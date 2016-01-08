//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "cse_map_field_indices.h"

#include "indices.h"


std::string macro_packages::cse_map_field1::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    field_index i_label = field_index(indices[0].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label)]);
  }


std::string macro_packages::cse_map_field2::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    field_index i_label = field_index(indices[0].get_numeric_value());
    field_index j_label = field_index(indices[1].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label, j_label)]);
  }


std::string macro_packages::cse_map_field3::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    field_index i_label = field_index(indices[0].get_numeric_value());
    field_index j_label = field_index(indices[1].get_numeric_value());
    field_index k_label = field_index(indices[2].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label, j_label, k_label)]);
  }
