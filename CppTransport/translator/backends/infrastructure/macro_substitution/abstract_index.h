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
    abstract_index(char l, enum index_class c, unsigned int f, unsigned int p);

    //! destructor is default
    ~abstract_index() = default;


    // INTERFACE -- REQUIRED FOR INDEX_DATABASE

  public:

    //! return label
    char get_label() const { return(this->label); }

    //! return classification
    enum index_class get_class() const { return(this->classification); }


    // INTERFACE -- TURN SELF INTO A FOR-LOOP VARIABLE

  public:

    //! return equivalent 'for'-loop variable for this abstract index
    std::string get_loop_variable() const { return(this->pre_string + this->label + this->post_string); }


    // INTERFACE -- SPECIFIC TO INDEX_ABSTRACT

  public:

    //! return total numeric range of this index
    unsigned int numeric_range() const;

    //! return total number of fields
    unsigned int get_number_fields() const { return(this->fields); }

    //! return total number of parameters
    unsigned int get_number_parameters() const { return(this->params); }

    //! set post string; used when subtracting values to convert momenta to fields
    void set_post_string(std::string s) { this->post_string = std::move(s); }

    // INTERNAL DATA

  private:

    //! index label
    char label;

    //! index classification
    enum index_class classification;

    //! cache total number of fields
    unsigned int fields;

    //! cache total number of parameters
    unsigned int params;

    //! pre-string for conversion to loop variable
    std::string pre_string;

    //! post-string for conversion to loop variable
    std::string post_string;

  };


//! set up typedef for database of abstract_index records
typedef index_database<abstract_index> abstract_index_list;



#endif //CPPTRANSPORT_ABSTRACT_INDEX_H
