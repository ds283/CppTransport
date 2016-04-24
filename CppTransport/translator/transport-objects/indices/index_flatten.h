//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_INDEX_FLATTEN_H
#define CPPTRANSPORT_INDEX_FLATTEN_H


#include "indices.h"


class index_flatten
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_flatten(unsigned int p, unsigned int f)
      : num_params(p),
        num_fields(f)
      {
      }


    // INTERFACE

  public:

    //! get size of a flattened d-dimensional array
    template <typename IndexType>
    unsigned int get_flattened_size(unsigned int d) const;


    // INTERFACE -- FLATTEN FUNCTIONS

  public:

    //! flatten 1-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a) const;

    //! flatten 2-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a, const IndexType& b) const;

    //! flatten 3-index object
    template <typename IndexType>
    unsigned int flatten(const IndexType& a, const IndexType& b, const IndexType& c) const;


    // INTERNAL DATA

  private:

    //! number of parameters
    unsigned int num_params;

    //! number of fields
    unsigned int num_fields;

  };


#endif //CPPTRANSPORT_INDEX_FLATTEN_H
