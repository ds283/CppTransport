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

    replacement_rule_package::replacement_rule_package(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
      : data_payload(p),
        printer(prn),
        fctry(f),
        cse_worker(cw),
        fl(p.get_number_parameters(), p.get_number_fields()),
        sym_factory(p.get_symbol_factory())
      {
      }


    std::string replacement_rule_package::replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        phase_index i_label = phase_index(indices[0].get_numeric_value());

        return((*map)[this->fl.flatten(i_label)]);
      }


    std::string replacement_rule_package::replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        phase_index i_label = phase_index(indices[0].get_numeric_value());
        phase_index j_label = phase_index(indices[1].get_numeric_value());

        return((*map)[this->fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        phase_index i_label = phase_index(indices[0].get_numeric_value());
        phase_index j_label = phase_index(indices[1].get_numeric_value());
        phase_index k_label = phase_index(indices[2].get_numeric_value());

        return((*map)[this->fl.flatten(i_label, j_label, k_label)]);
      }


    std::string replacement_rule_package::replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        field_index i_label = field_index(indices[0].get_numeric_value());

        return((*map)[this->fl.flatten(i_label)]);
      }


    std::string replacement_rule_package::replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        field_index i_label = field_index(indices[0].get_numeric_value());
        field_index j_label = field_index(indices[1].get_numeric_value());

        return((*map)[this->fl.flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, cse_map* map)
      {
        if(map == nullptr) throw rule_apply_fail(ERROR_NO_PRE_MAP);

        field_index i_label = field_index(indices[0].get_numeric_value());
        field_index j_label = field_index(indices[1].get_numeric_value());
        field_index k_label = field_index(indices[2].get_numeric_value());

        return((*map)[this->fl.flatten(i_label, j_label, k_label)]);
      }


  }
