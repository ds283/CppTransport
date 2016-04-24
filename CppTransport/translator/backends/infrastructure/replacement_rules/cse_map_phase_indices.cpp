//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#include "cse_map_phase_indices.h"


std::string macro_packages::cse_map_phase1::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    phase_index i_label = phase_index(indices[0].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label)]);
  }


std::string macro_packages::cse_map_phase2::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    phase_index i_label = phase_index(indices[0].get_numeric_value());
    phase_index j_label = phase_index(indices[1].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label, j_label)]);
  }


std::string macro_packages::cse_map_phase3::unroll(const macro_argument_list& args, const assignment_list& indices)
  {
    if(this->map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

    phase_index i_label = phase_index(indices[0].get_numeric_value());
    phase_index j_label = phase_index(indices[1].get_numeric_value());
    phase_index k_label = phase_index(indices[2].get_numeric_value());

    return((*this->map)[this->fl.flatten(i_label, j_label, k_label)]);
  }
