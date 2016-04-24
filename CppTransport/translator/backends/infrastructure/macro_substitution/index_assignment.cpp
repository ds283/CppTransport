//
// Created by David Seery on 27/06/2013.
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


#include <assert.h>
#include <sstream>
#include <stdexcept>
#include <ctype.h>

#include "index_assignment.h"


// **************************************************************************************


namespace index_assignment_impl
  {

    std::unique_ptr<assignment_list> assignment_set_iterator::operator*() const
      {
        std::unique_ptr<assignment_list> rval = std::make_unique<assignment_list>();

        switch(this->parent->order)
          {
            case index_order::left:
              {
                // arrange for insertion at the back, so indices are in correct order
                this->construct_assignment(this->parent->source_set.begin(), this->parent->source_set.end(), database_back_inserter<assignment_list>(*rval));
              }

            case index_order::right:
              {
                // arrange for insertion at the front, so indices remain in correct order
                this->construct_assignment(this->parent->source_set.rbegin(), this->parent->source_set.rend(), database_front_inserter<assignment_list>(*rval));
              }
          }

        return(rval);
      }


    template <typename IteratorType, typename Inserter>
    void assignment_set_iterator::construct_assignment(IteratorType begin, IteratorType end, Inserter ins) const
      {
        IteratorType it      = begin;
        unsigned int current = this->pos;

        while(it != end)
          {
            unsigned int range = it->numeric_range();

            ins.insert(std::make_pair(it->get_label(), std::make_shared<assignment_record>(*it, current % range)));
            current = current / range;

            it++;
          }
      }

  }   // namespace index_assignment_impl


assignment_set::assignment_set(const abstract_index_list& s, index_order o)
  : source_set(s),
    order(o),
    assignments_size(1)
  {
    for(const abstract_index& idx : source_set)
      {
        assignments_size *= idx.numeric_range();
      }
  }


assignment_set::iterator assignment_set::begin()
  {
    return iterator(this, 0);
  }


assignment_set::iterator assignment_set::end()
  {
    return iterator(this, this->assignments_size);
  }
