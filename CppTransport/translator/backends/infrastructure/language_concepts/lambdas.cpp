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


#include <sstream>

#include "lambdas.h"
#include "language_printer.h"


generic_lambda::generic_lambda(const abstract_index_database& list, expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : index_list(list),
    type(t),
    tags(tg),
    working_type(std::move(ty))
  {
  }


generic_lambda::generic_lambda(const abstract_index& i, expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : type(t),
    tags(tg),
    working_type(std::move(ty))
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
  }


generic_lambda::generic_lambda(const abstract_index& i, const abstract_index& j, expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : type(t),
    tags(tg),
    working_type(std::move(ty))
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
    index_list.emplace_back(std::make_pair(j.get_label(), std::make_shared<abstract_index>(j)));
  }


generic_lambda::generic_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : type(t),
    tags(tg),
    working_type(std::move(ty))
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
    index_list.emplace_back(std::make_pair(j.get_label(), std::make_shared<abstract_index>(j)));
    index_list.emplace_back(std::make_pair(k.get_label(), std::make_shared<abstract_index>(k)));
  }


std::string atomic_lambda::make_temporary(language_printer& printer, unsigned int) const
  {
    std::ostringstream stmt;

    std::string open = printer.open_lambda(this->index_list, this->working_type);
    std::string close = printer.close_lambda();

    stmt << open << " " << printer.format_return(this->expr) << " " << close;

    return stmt.str();
  }


std::string map_lambda::make_temporary(language_printer& printer, unsigned int num_fields) const
  {
    std::ostringstream stmt;

    std::string open = printer.open_lambda(this->index_list, this->working_type);
    std::string close = printer.close_lambda();

    // can assume that the cached map is a suitable size for the supplied index list,
    // since this was checked during construction

    stmt << open << " ";

    unsigned int counter = 0;
    for(const GiNaC::ex& expr : this->map)
      {
        std::list< GiNaC::ex > conditions;
        unsigned int state = counter;

        // work through index list in reverse order, assuming this is the order given by the lambda_flatten() functions
        // calculate out the appropriate conditions for this index from the current position in the map, and push them to the list
        for(abstract_index_database::const_reverse_iterator t = this->index_list.crbegin(); t != this->index_list.crend(); ++t)
          {
            GiNaC::symbol sym(t->get_loop_variable());
            GiNaC::ex rel = state % 2 == 1 ? sym >= num_fields : sym < num_fields;
            conditions.push_back(rel);
            state = state / 2;
          }

        // each case can be a separate 'if' statement since the function exits whenever one is executed
        stmt << printer.format_if(conditions) << " " << printer.format_return(expr) << '\n';
        ++counter;
      }

    // kill compiler warning about possible void return
    GiNaC::ex null_value = 0;
    stmt << printer.format_return(null_value);

    stmt << " " << close;

    return stmt.str();
  }
