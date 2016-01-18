//
// Created by David Seery on 12/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#include "symbol_factory.h"


GiNaC::symbol symbol_factory::get_symbol(const std::string& name)
	{
    std::string LaTeX;
		return(this->get_symbol(name, LaTeX));
	}


GiNaC::symbol symbol_factory::get_symbol(const std::string& name, const std::string& LaTeX)
	{
    std::unordered_map<std::string, GiNaC::symbol>::iterator t = this->cache.find(name);

		if(t != this->cache.end()) return(t->second);

    GiNaC::symbol rval(name, LaTeX);
		this->cache[name] = rval;

		return(rval);
	}
