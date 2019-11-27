//
// Created by David Seery on 26/11/2019.
// --@@
// Copyright (c) 2019 University of Sussex. All rights reserved.
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


#ifndef CPPTRANSPORT_READ_EXPRESSIONS_H
#define CPPTRANSPORT_READ_EXPRESSIONS_H

#include <initializer_list>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/filesystem/path.hpp>

#include "resource_failure.h"

#include "ginac/ginac.h"

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/operations.hpp"
#include <boost/range/adaptor/reversed.hpp>


namespace reader
  {

    namespace detail
      {

        template <typename ContainerObject>
        GiNaC::ex
        read_expr(std::ifstream& f, const GiNaC::symtab& table, ContainerObject indices)
          {
            if(f.fail() || f.eof()) throw resource_failure("Could not read from expression file");

            // read an entire line from the input file
            std::string line;
            std::getline(f, line);

            // break the line at the = operator
            std::vector<std::string> elements;
            boost::split(elements, line, boost::is_any_of("="));

            if(elements.size() != 2) throw resource_failure("Unexpected formatting in input expression file");

            // trim leading and trailing white space from all particles
            for(auto& elt : elements)
              {
                boost::trim(elt);
              }

            // break left-hand side into a type specified (usually 'ex') and a name
            std::vector<std::string> tags;
            boost::split(tags, elements[0], boost::is_any_of(" "));

            // trim leading and trailing white space ... again
            for(auto& tag : tags)
              {
                boost::trim(tag);
              }

            if(tags[0] != "ex") throw resource_failure("Expected expression type in input file to be 'ex'");

            auto& label = tags[1];
            // We need to reverse the indicies in this for loop as we read the indicies right-to-left in the input files
            for(const auto& item : boost::adaptors::reverse(indices))
              {
                if(label.size() <= 1) throw resource_failure("Unexpected formatting for left-hand side of expression");

                // extract and remove final number from string
                auto final_char = label.back();
                label.pop_back();

                // explicitly construct s as a string and then add final_char to it - previous conversion to string failed
                std::string s;
                s.push_back(final_char);

                if(s != std::to_string(item))
                  throw resource_failure("Unexpected formatting for index part of expression name. \n Expected " + s + ", but got " + std::to_string(item));
              }

            GiNaC::parser reader{table};
            return reader(elements[1]);
          }

      }

    // pre-prepare GiNaC reader() symbol table from field and parameter lists
    std::unique_ptr<GiNaC::symtab>
    build_total_symbol_table(const symbol_list& fields, const symbol_list& derivs, const symbol_list& params);

    // utility function to read the inverse metric from a file
    std::unique_ptr<GiNaC::matrix>
    read_inverse_metric(boost::filesystem::path p, const GiNaC::symtab& subst_table, size_t N);

  }


#endif //CPPTRANSPORT_READ_EXPRESSIONS_H
