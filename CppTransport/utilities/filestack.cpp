//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "filestack.h"


filestack::filestack()
  : locked(false)
  {
  }


filestack::~filestack()
  {
  }


void filestack::lock()
  {
    this->locked = true;
  }