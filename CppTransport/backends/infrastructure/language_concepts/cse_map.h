//
// Created by David Seery on 14/12/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CSE_MAP_H
#define CPPTRANSPORT_CSE_MAP_H


#include "cse.h"


// utility class to make using CSE easier.
// On construction it takes a vector of GiNaC expressions.
// The resulting object can be subscripted in the same order as the input to produce the equivalent CSE get_symbol
class cse_map
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor; takes ownership of a std::unique_ptr<> to a list of GiNaC expressions
    //! and uses them to set up CSE
    cse_map(std::unique_ptr< std::vector<GiNaC::ex> > l, cse& c);

    //! destructor is default
    ~cse_map() = default;


    // INTERFACE

  public:

    // not returning a reference disallows using [] as an lvalue
    std::string operator[](unsigned int index);


    // INTERNAL DATA

  protected:

    //! parent CSE worker
    cse& cse_worker;

    //! list of GiNaC expressions used in the map; we take ownership of this list
    //! and it is destroyed when this object goes out of scope
    std::unique_ptr< std::vector<GiNaC::ex> > list;

  };


#endif //CPPTRANSPORT_CSE_MAP_H
