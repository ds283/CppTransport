//
// Created by David Seery on 13/11/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//

// based on GinacPrint by Doug Baker
// original copyright notice:

//***************************************************************************
//* @file GinacPrint.cc
//*
// * Source file for a class that prints GiNaC expressions in a form that
//* is efficient to compile and efficient to run.  The main work this
//* class does is identifying shared sub-expressions to generate code
//* that only computes them once.
//*
//* See GinacPrint.hh for details on usage.
//*
//* Copyright 2010
//*
//* National Robotics Engineering Center, Carnegie Mellon University
//* 10 40th Street, Pittsburgh, PA 15201
//* www.rec.ri.cmu.edu
//*
//* This program is free software; you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation; either version 2 of the License, or
//* (at your option) any later version.
//*
//* This program is distributed in the hope that it will be useful, but
//* WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//* General Public License for more details.
//*
//* This notice must appear in all copies of this file and its derivatives.
//*
//* History of Significant Contributions (don't put commit logs here):
//* 2010-05-20 Doug Baker, cape1232@yahoo.com: Created.
//*
//***************************************************************************


#ifndef __cse_H_
#define __cse_H_

#include <string>

#include "ginac/ginac.h"

#define DEFAULT_CSE_KERNEL_NAME "t"

class cse
  {
  public:
    cse(unsigned int s, std::string k=DEFAULT_CSE_KERNEL_NAME) : serial_number(s), kernel_name(k) {}

    void parse(const GiNaC::ex& expr);

    std::string temporaries(const std::string& t);
    std::string symbol(const GiNaC::ex& expr);

    void clear();

    const std::string& get_kernel_name()                     { return(this->kernel_name); }
    void               set_kernel_name(const std::string& k) { this->kernel_name = k; }

  protected:
    std::string print         (const GiNaC::ex& expr);
    std::string print_operands(const GiNaC::ex& expr, std::string op);

    std::string make_symbol   ();

    unsigned int serial_number;
    unsigned int symbol_counter;

    std::string  kernel_name;

    std::map<std::string, std::string>                 symbols;
    std::vector< std::pair<std::string, std::string> > decls;
  };


#endif //__cse_H_
