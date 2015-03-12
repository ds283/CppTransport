//
// Created by David Seery on 12/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __symbol_factory_H_
#define __symbol_factory_H_


#include <unordered_map>

#include "ginac/ginac.h"


class symbol_factory
	{

		// CONSTRUCTOR

  public:

		symbol_factory() = default;

		~symbol_factory() = default;


		// INTERFACE

  public:

		//! get a symbol
		GiNaC::symbol get_symbol(const std::string& name);

		//! get a symbol, specifying a LaTeX equivalent
		GiNaC::symbol get_symbol(const std::string& name, const std::string& LaTeX);


		// INTERNAL DATA

  private:

		std::unordered_map<std::string, GiNaC::symbol> cache;

	};


#endif //__symbol_factory_H_
