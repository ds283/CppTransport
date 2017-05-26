//
// Created by David Seery on 24/05/2017.
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


#ifndef CPPTRANSPORT_GENERICS_DESCRIPTOR_H
#define CPPTRANSPORT_GENERICS_DESCRIPTOR_H


#include <string>
#include <sstream>

#include "parse_error.h"

#include "ginac/ginac.h"


template <typename SymbolChecker, typename SymbolInserter, typename AttributeBlock>
bool GenericInsertSymbol(SymbolChecker check, SymbolInserter insert,
                         const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, AttributeBlock& a,
                         std::string err_msg, std::string dupl_msg)
  {
    // search for an existing entry in the symbol table
    auto record = check(n);

    if(record)    // symbol already exists
      {
        std::ostringstream msg;
        msg << err_msg << " '" << n << "'";
        l.error(msg.str());

        std::ostringstream orig_decl;
        orig_decl << dupl_msg << " '" << n << "'";
        record.get().get_declaration_point().warn(orig_decl.str());

        throw parse_error(msg.str());
      }

    insert(n, s, l, a);
    return true;
  };


// version without GiNaC symbol
template <typename SymbolChecker, typename SymbolInserter, typename AttributeBlock>
bool GenericInsertSymbol(SymbolChecker check, SymbolInserter insert,
                         const std::string& n, const y::lexeme_type& l, AttributeBlock& a,
                         std::string err_msg, std::string dupl_msg)
  {
    // search for an existing entry in the symbol table
    auto record = check(n);

    if(record)    // symbol already exists
      {
        std::ostringstream msg;
        msg << err_msg << " '" << n << "'";
        l.error(msg.str());

        std::ostringstream orig_decl;
        orig_decl << dupl_msg << " '" << n << "'";
        record.get().get_declaration_point().warn(orig_decl.str());

        throw parse_error(msg.str());
      }

    insert(n, l, a);
    return true;
  };


// sort a symbol table by unique ID and return a list of some specified value,
// determined by the ValueGetter functor
template <typename TableType, typename ValueGetter>
auto UnzipSortedZipList(TableType& table, ValueGetter get) -> std::vector< typename std::result_of<ValueGetter(typename TableType::mapped_type::element_type&)>::type >
  {
    typedef typename std::result_of<ValueGetter(typename TableType::mapped_type::element_type&)>::type value_type;
    typedef std::vector< std::pair<unsigned int, value_type> > zip_list;

    zip_list zip;
    std::vector<value_type> unzip;

    // zip up a list of unique IDs and values
    for(const auto& t : table)
      {
        zip.push_back(std::make_pair(t.second->get_unique_id(), get(*t.second)));
      }

    // sort on unique ID
    struct Sorter
      {
        // sort on unique ID
        bool operator()(const typename zip_list::value_type& a, const typename zip_list::value_type& b)
          {
            return a.first < b.first;
          }
      };

    std::sort(zip.begin(), zip.end(), Sorter());

    // unzip values from the sorted list
    for(const auto& t : zip)
      {
        unzip.push_back(t.second);
      }

    return unzip;
  };


#endif //CPPTRANSPORT_GENERICS_DESCRIPTOR_H
