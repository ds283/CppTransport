//
// Created by David Seery on 22/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
