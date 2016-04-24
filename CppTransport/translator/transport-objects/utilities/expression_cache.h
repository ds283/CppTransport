//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_EXPRESSION_CACHE_H
#define CPPTRANSPORT_EXPRESSION_CACHE_H


#include "ginac_cache.h"


enum class expression_item_types
  {
    sr_U_item, sr_U_lambda,
    U1_item, U1_lambda,
    U2_item, U2_lambda,
    U3_item, U3_lambda,
    A_item, A_lambda,
    B_item, B_lambda,
    C_item, C_lambda,
    M_item, M_lambda,
    zxfm1_item, zxfm1_lambda,
    zxfm2_item, zxfm2_lambda,
    dN1_item,
    dN2_item,
    V_item,
    dV_item, dV_lambda,
    ddV_item, ddV_lambda,
    dddV_item, dddV_lambda,
    Hubble2_item,
    epsilon_item
  };


typedef ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE> expression_cache;


#endif //CPPTRANSPORT_EXPRESSION_CACHE_H
