//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_INDEX_TRAITS_H
#define CPPTRANSPORT_INDEX_TRAITS_H


#include "indices.h"


class index_traits
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_traits(unsigned int nf)
      : num_fields(nf)
      {
      }

    //! destructor is default
    ~index_traits() = default;


    // INTERFACE -- INDEX INTROSPECTION

  public:

    //! determine if an index is a species label
    bool is_species(phase_index);

    //! determine if an index is a momentum label
    bool is_momentum(phase_index);


    // INTERFACE -- INDEX CONVERSION

  public:

    //! convert an index to a momentum label
    phase_index to_momentum(field_index);

    //! convert an index to a momentum label
    phase_index to_momentum(phase_index);

    //! convert an index to a species label
    field_index to_species(phase_index);


    // INTERNAL DATA

  private:

    //! cache number of fields
    unsigned int num_fields;

  };


#endif //CPPTRANSPORT_INDEX_TRAITS_H
