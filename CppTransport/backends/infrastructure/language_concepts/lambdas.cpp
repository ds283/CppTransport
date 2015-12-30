//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <sstream>

#include "lambdas.h"
#include "language_printer.h"


generic_lambda::generic_lambda(const abstract_index_list& list, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : index_list(list),
    type(t),
    tags(tg),
    working_type(std::move(ty))
  {
  }


generic_lambda::generic_lambda(const abstract_index& i, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : type(t),
    tags(tg),
    working_type(std::move(ty))
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
  }


generic_lambda::generic_lambda(const abstract_index& i, const abstract_index& j, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
  : type(t),
    tags(tg),
    working_type(std::move(ty))
  {
    index_list.emplace_back(std::make_pair(i.get_label(), std::make_shared<abstract_index>(i)));
    index_list.emplace_back(std::make_pair(j.get_label(), std::make_shared<abstract_index>(j)));
  }


generic_lambda::generic_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k, enum expression_item_types t, const ginac_cache_tags& tg, std::string ty)
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
        for(abstract_index_list::const_reverse_iterator t = this->index_list.crbegin(); t != this->index_list.crend(); ++t)
          {
            GiNaC::symbol sym(t->get_loop_variable());
            GiNaC::ex rel = state % 2 == 1 ? sym >= num_fields : sym < num_fields;
            conditions.push_back(rel);
            state = state / 2;
          }

        if(counter > 0) stmt << printer.format_elseif(conditions);
        else            stmt << printer.format_if(conditions);

        stmt << " " << printer.format_return(expr) << '\n';
        ++counter;
      }

    // kill compiler warning about possible void return
    GiNaC::ex null_value = 0;
    if(counter > 0) stmt << printer.format_else(null_value);

    stmt << " " << close;

    return stmt.str();
  }
