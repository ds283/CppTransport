//
// Created by David Seery on 06/12/2015.
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

#ifndef CPPTRANSPORT_ABSTRACT_INDEX_H
#define CPPTRANSPORT_ABSTRACT_INDEX_H


#include "index_class.h"
#include "index_database.h"


class abstract_index
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor -- takes best-guess at classification of index
    abstract_index(char l, unsigned int f, unsigned int p);

    //! constructor -- with assigned classification
    abstract_index(char l, index_class c, unsigned int f, unsigned int p);

    //! destructor is default
    ~abstract_index() = default;


    // INTERFACE -- REQUIRED FOR INDEX_DATABASE

  public:

    //! return label
    char get_label() const { return(this->label); }

    //! return classification
    index_class get_class() const { return(this->classification); }


    // INTERFACE -- TURN SELF INTO A FOR-LOOP VARIABLE

  public:

    //! return equivalent 'for'-loop variable for this abstract index
    std::string get_loop_variable() const;


    // INTERFACE -- SPECIFIC TO INDEX_ABSTRACT

  public:

    //! return total numeric range of this index
    unsigned int numeric_range() const;

    //! return total number of fields
    unsigned int get_number_fields() const { return(this->fields); }

    //! return total number of parameters
    unsigned int get_number_parameters() const { return(this->params); }

    //! add a new post modifier; used when subtracting values to convert momenta to fields
    void push_post_modifier(std::string s);

    //! remove the last post modifier
    void pop_post_modifier();

    //! add a new pre modifier
    void push_pre_modifier(std::string s);

    //! remove the last pre modifier
    void push_pre_modifier();


    // INTERNAL DATA

  private:

    //! index label
    char label;

    //! index classification
    index_class classification;

    //! cache total number of fields
    unsigned int fields;

    //! cache total number of parameters
    unsigned int params;

    //! set of "pre" modifiers to be applied during conversion to a loop variable
    std::list<std::string> pre_string;

    //! set of "post" modifiers to be applied during conversion to a loop variable
    std::list<std::string> post_string;

  };


namespace std
  {

    // provide std::equal_to implementation for abstract_index
    template <>
    struct equal_to<abstract_index>
      {

        bool operator()(const abstract_index& lhs, const abstract_index& rhs) const
          {
            return (lhs.get_label() == rhs.get_label()) && (lhs.get_class() == rhs.get_class());
          }

      };

    // provide std::hash implementation for abstract_index, so it can be used in a std::unordered_map
    template <>
    struct hash<abstract_index>
      {

        size_t operator()(const abstract_index& idx) const
          {
            return static_cast<size_t>(idx.get_label());
          }

      };

  }   // namespace std


//! set up typedef for database of abstract_index records
typedef index_database<abstract_index> abstract_index_list;



#endif //CPPTRANSPORT_ABSTRACT_INDEX_H
