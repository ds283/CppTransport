//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_SEMANTIC_DATA_H
#define CPPTRANSPORT_SEMANTIC_DATA_H


#include <iostream>
#include <string>
#include <memory>

#include "filestack.h"
#include "lexeme.h"

#include "ginac/ginac.h"


class attributes
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    attributes();

    ~attributes() = default;


		// INTERFACE

  public:

    const std::string& get_latex() const;

    void set_latex(const std::string& l);

    void unset_latex();


		// INTERNAL DATA

  private:

    bool        latex_set;
    std::string latex;

	};


class subexpr
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

		subexpr();

		~subexpr() = default;


		// INTERFACE

  public:

		const std::string& get_latex() const;

		void set_latex(const std::string& l);

		void unset_latex();

		GiNaC::ex get_value() const;

		void set_value(GiNaC::ex v);

		void unset_value();


		// INTERNAL DATA

  private:

    bool        latex_set;
    std::string latex;

    GiNaC::ex   value;
    bool        value_set;
	};


#endif //CPPTRANSPORT_SEMANTIC_DATA_H
