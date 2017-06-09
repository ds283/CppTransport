//
// Created by David Seery on 06/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#include "index_literal.h"


std::unique_ptr<index_literal_database> to_database(const index_literal_list& indices)
  {
    auto db = std::make_unique<index_literal_database>();

    for(const auto& T : indices)
      {
        const index_literal& rec = *T;
        const abstract_index& idx = rec;

        if(db->count(idx.get_label()) > 0)
          {
            throw duplicate_index(std::string(1, idx.get_label()), rec.get_declaration_point());
          }

        db->emplace(std::make_pair(idx.get_label(), T));
      }

    return std::move(db);
  }


std::string index_literal::to_string() const
  {
    std::string str;

    // write variance modifier, if one is present
    switch(this->type)
      {
        case variance::none:
          {
            break;
          }

        case variance::contravariant:
          {
            str = "^";
            break;
          }

        case variance::covariant:
          {
            str = "_";
          }
      }

    // append index kernel letter
    str += this->idx.get().get_label();

    return str;
  }
