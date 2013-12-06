//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <assert.h>

#include "replacement_rule_package.h"

#include "macro.h"
#include "flatten.h"
#include "cse.h"


namespace macro_packages
  {

    unsigned int replacement_rule_package::get_index_label(struct index_assignment& index)
      {
        unsigned int label = 0;

        switch(index.trait)
          {
            case index_field:
              label = index.species;
            break;

            case index_momentum:
              label = index.species + index.num_fields;
            break;

            case index_parameter:
            default:
              assert(false);
          }

        return(label);
      }


    std::string replacement_rule_package::replace_1index_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(2*this->data.num_fields);

        unsigned int i_label = this->get_index_label(indices[0]);
        return((*map)[this->fl->flatten(i_label)]);
      }


    std::string replacement_rule_package::replace_2index_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 2);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());
        assert(indices[1].species < this->data.parse_tree->get_number_fields());

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(2*this->data.num_fields);

        unsigned int i_label = this->get_index_label(indices[0]);
        unsigned int j_label = this->get_index_label(indices[1]);

        return((*map)[this->fl->flatten(i_label,j_label)]);
      }


    std::string replacement_rule_package::replace_3index_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 3);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());
        assert(indices[1].species < this->data.parse_tree->get_number_fields());
        assert(indices[2].species < this->data.parse_tree->get_number_fields());

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(2*this->data.num_fields);

        unsigned int i_label = this->get_index_label(indices[0]);
        unsigned int j_label = this->get_index_label(indices[1]);
        unsigned int k_label = this->get_index_label(indices[2]);

        return((*map)[this->fl->flatten(i_label,j_label,k_label)]);
      }


    std::string replacement_rule_package::replace_1index_field_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 1);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());
        assert(indices[0].trait == index_field);

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(this->data.num_fields);

        unsigned int i_label = this->get_index_label(indices[0]);

        return((*map)[this->fl->flatten(i_label)]);
      }


    std::string replacement_rule_package::replace_2index_field_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 2);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());
        assert(indices[1].species < this->data.parse_tree->get_number_fields());
        assert(indices[0].trait == index_field);
        assert(indices[1].trait == index_field);

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(this->data.num_fields);

        unsigned int i_label = get_index_label(indices[0]);
        unsigned int j_label = get_index_label(indices[1]);

        return((*map)[this->fl->flatten(i_label,j_label)]);
      }


    std::string replacement_rule_package::replace_3index_field_tensor(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        assert(indices.size() == 3);
        assert(indices[0].species < this->data.parse_tree->get_number_fields());
        assert(indices[1].species < this->data.parse_tree->get_number_fields());
        assert(indices[2].species < this->data.parse_tree->get_number_fields());
        assert(indices[0].trait == index_field);
        assert(indices[1].trait == index_field);
        assert(indices[2].trait == index_field);

        assert(state != nullptr);
        cse_map* map = (cse_map*)state;

        this->fl->set_size(this->data.num_fields);

        unsigned int i_label = get_index_label(indices[0]);
        unsigned int j_label = get_index_label(indices[1]);
        unsigned int k_label = get_index_label(indices[2]);

        return((*map)[this->fl->flatten(i_label,j_label,k_label)]);
      }


    void replacement_rule_package::generic_post_hook(void* state)
      {
        assert(state != nullptr);

        delete (cse_map*)state;
      }

  }
