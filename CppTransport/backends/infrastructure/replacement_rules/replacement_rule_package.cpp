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

    std::string replacement_rule_package::replace_1index_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].get_class() == index_class::full);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(2*this->data_payload.get_number_fields());

        return((*map)[this->fl->flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 2);
        assert(indices[0].get_class() == index_class::full);
        assert(indices[1].get_class() == index_class::full);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(2*this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->fl->flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 3);
        assert(indices[0].get_class() == index_class::full);
        assert(indices[1].get_class() == index_class::full);
        assert(indices[2].get_class() == index_class::full);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(2*this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->fl->flatten(i_label, j_label, k_label)]);
      }


    std::string replacement_rule_package::replace_1index_field_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].get_class() == index_class::field_only);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(this->data_payload.get_number_fields());

        return((*map)[this->fl->flatten(indices[0].get_numeric_value())]);
      }


    std::string replacement_rule_package::replace_2index_field_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 2);
        assert(indices[0].get_class() == index_class::field_only);
        assert(indices[1].get_class() == index_class::field_only);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();

        return((*map)[this->fl->flatten(i_label, j_label)]);
      }


    std::string replacement_rule_package::replace_3index_field_tensor(const macro_argument_list& args, const assignment_list& indices, void* state)
      {
        assert(indices.size() == 3);
        assert(indices[0].get_class() == index_class::field_only);
        assert(indices[1].get_class() == index_class::field_only);
        assert(indices[2].get_class() == index_class::field_only);

        assert(state != nullptr);
        cse_map* map = static_cast<cse_map*>(state);

        this->fl->set_size(this->data_payload.get_number_fields());

        unsigned int i_label = indices[0].get_numeric_value();
        unsigned int j_label = indices[1].get_numeric_value();
        unsigned int k_label = indices[2].get_numeric_value();

        return((*map)[this->fl->flatten(i_label, j_label, k_label)]);
      }


    void replacement_rule_package::generic_post_hook(void* state)
      {
        assert(state != nullptr);

        delete static_cast<cse_map*>(state);
      }

  }
