//
// Created by David Seery on 06/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_INDEX_ABSTRACT_H
#define CPPTRANSPORT_INDEX_ABSTRACT_H


#include "index_traits.h"
#include "index_database.h"


class index_abstract
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor -- takes best-guess at classification of index
    index_abstract(char l, unsigned int f, unsigned int p);

    //! constructor -- with assigned classification
    index_abstract(char l, enum index_class c, unsigned int f, unsigned int p);

    //! destructor is default
    ~index_abstract() = default;


    // INTERFACE -- REQUIRED FOR INDEX_DATABASE

  public:

    //! return label
    char get_label() const { return(this->label); }

    //! return classification
    enum index_class get_class() const { return(this->classification); }


    // INTERFACE -- SPECIFIC TO INDEX_ABSTRACT

  public:

    //! return total numeric range of this index
    unsigned int numeric_range() const;

    //! return total number of fields
    unsigned int get_number_fields() const { return(this->fields); }


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

  };


//! set up typedef for database of index_abstract records
typedef index_database<index_abstract> abstract_index_list;



#endif //CPPTRANSPORT_INDEX_ABSTRACT_H
