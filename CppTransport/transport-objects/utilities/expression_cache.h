//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_EXPRESSION_CACHE_H
#define CPPTRANSPORT_EXPRESSION_CACHE_H


#include "ginac_cache.h"


enum class expression_item_types
  {
    sr_U_item,
    U1_item, U2_item, U3_item,
    A_item, B_item, C_item, M_item,
    zxfm1_item, zxfm2_item,
    dN1_item, dN2_item,
    V_item, dV_item, ddV_item, dddV_item,
    Hubble2_item, epsilon_item
  };


typedef ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE> expression_cache;


#endif //CPPTRANSPORT_EXPRESSION_CACHE_H
