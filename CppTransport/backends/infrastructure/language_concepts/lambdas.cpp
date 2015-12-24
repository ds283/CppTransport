//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "lambdas.h"


lambda::lambda(const abstract_index_list& list)
  : index_list(list)
  {
  }


lambda::lambda(const abstract_index& i)
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
  }


lambda::lambda(const abstract_index& i, const abstract_index& j)
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
    index_list.emplace_back(std::make_pair(j.get_label(), std::make_shared<abstract_index>(j)));
  }


lambda::lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k)
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
    index_list.emplace_back(std::make_pair(j.get_label(), std::make_shared<abstract_index>(j)));
    index_list.emplace_back(std::make_pair(k.get_label(), std::make_shared<abstract_index>(k)));
  }
