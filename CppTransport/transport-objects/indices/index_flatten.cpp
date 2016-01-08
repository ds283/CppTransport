//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include "index_flatten.h"


template <>
unsigned int index_flatten::get_flattened_size<field_index>(unsigned int d) const
  {
    unsigned int s = 1;
    while(d > 0)
      {
        s *= this->num_fields;
        --d;
      }
    return(s);
  }


template <>
unsigned int index_flatten::get_flattened_size<phase_index>(unsigned int d) const
  {
    unsigned int s = 1;
    while(d > 0)
      {
        s *= 2*this->num_fields;
        --d;
      }
    return(s);
  }


// allow only 1-index flattening of parameter indices
template <>
unsigned int index_flatten::flatten(const param_index& a) const
  {
    return static_cast<unsigned int>(a);
  }


// FIELD INDICES


template <>
unsigned int index_flatten::flatten(const field_index& a) const
  {
    unsigned int av = static_cast<unsigned int>(a);

    return(av);
  }


template <>
unsigned int index_flatten::flatten(const field_index& a, const field_index& b) const
  {
    unsigned int av = static_cast<unsigned int>(a);
    unsigned int bv = static_cast<unsigned int>(b);
    unsigned int N  = this->num_fields;

    return(N*av + bv);
  }


template <>
unsigned int index_flatten::flatten(const field_index& a, const field_index& b, const field_index& c) const
  {
    unsigned int av = static_cast<unsigned int>(a);
    unsigned int bv = static_cast<unsigned int>(b);
    unsigned int cv = static_cast<unsigned int>(c);
    unsigned int N  = this->num_fields;

    return(N*N*av + N*bv + cv);
  }


// PHASE SPACE INDICES


template <>
unsigned int index_flatten::flatten(const phase_index& a) const
  {
    unsigned int av = static_cast<unsigned int>(a);

    return(av);
  }


template <>
unsigned int index_flatten::flatten(const phase_index& a, const phase_index& b) const
  {
    unsigned int av = static_cast<unsigned int>(a);
    unsigned int bv = static_cast<unsigned int>(b);
    unsigned int N  = 2*this->num_fields;

    return(N*av + bv);
  }


template <>
unsigned int index_flatten::flatten(const phase_index& a, const phase_index& b, const phase_index& c) const
  {
    unsigned int av = static_cast<unsigned int>(a);
    unsigned int bv = static_cast<unsigned int>(b);
    unsigned int cv = static_cast<unsigned int>(c);
    unsigned int N  = 2*this->num_fields;

    return(N*N*av + N*bv + cv);
  }
