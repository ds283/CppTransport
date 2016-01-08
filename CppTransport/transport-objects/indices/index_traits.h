//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_INDEX_TRAITS_H
#define CPPTRANSPORT_INDEX_TRAITS_H


#include "indices.h"
#include "abstract_index.h"


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


    // INTERFACE -- CONCRETE INDEX INTROSPECTION

  public:

    //! determine if a concrete index is a species label
    bool is_species(const phase_index& index);

    //! determine if a concrete index is a momentum label
    bool is_momentum(const phase_index& index);


    // INTERFACE -- CONCRETE INDEX CONVERSION

  public:

    //! convert a concrete index to a concrete momentum index
    phase_index to_momentum(const field_index& index);

    //! convert a concrete index to a concrete momentum index
    phase_index to_momentum(const phase_index& index);

    //! convert a concrete index to a concrete species label
    field_index to_species(const phase_index& index);


    // INTERFACE -- ABSTRACT INDEX CONVERSION

  public:

    //! convert an abstract phase-space index to an abstract species index
    //! conversion is direct
    abstract_index species_to_species(const abstract_index& index);

    //! convert an abstract phase-space index to an abstract species index
    //! conversion is offset by -number_fields
    abstract_index momentum_to_species(const abstract_index& index);

    // INTERNAL DATA

  private:

    //! cache number of fields
    unsigned int num_fields;

  };


#endif //CPPTRANSPORT_INDEX_TRAITS_H
