//
// Created by David Seery on 21/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//



#ifndef CPPTRANSPORT_SEMANTIC_DATA_H
#define CPPTRANSPORT_SEMANTIC_DATA_H


#include <iostream>
#include <string>
#include <memory>

#include "filestack.h"
#include "lexeme.h"
#include "contexted_value.h"
#include "y_common.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


class attributes
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    attributes() = default;

    ~attributes() = default;


		// INTERFACE

  public:

    const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }

    void set_latex(const std::string& ltx, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy attribute blocks

    std::shared_ptr< contexted_value<std::string> > latex;

	};


inline void attributes::set_latex(const std::string& ltx, const y::lexeme_type& l)
  {
    if(this->latex)
      {
        l.error(ERROR_LATEX_REDECLARATION);
        this->latex->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->latex = std::make_unique< contexted_value<std::string> >(ltx, l.get_error_context());
      }
  }


class subexpr
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

		subexpr()
      {
      }

		~subexpr() = default;


		// INTERFACE

  public:

		const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }

    void set_latex(const std::string& ltx, const y::lexeme_type& l);

		GiNaC::ex get_value() const { if(this->value) return *this->value; else return GiNaC::ex(0); }

    void set_value(GiNaC::ex v, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy a subexpr block

    std::shared_ptr< contexted_value<std::string> > latex;

    std::shared_ptr< contexted_value<GiNaC::ex> > value;

	};


inline void subexpr::set_value(GiNaC::ex v, const y::lexeme_type& l)
  {
    if(this->value)
      {
        l.error(ERROR_VALUE_REDECLARATION);
        this->value->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->value = std::make_unique< contexted_value<GiNaC::ex> >(v, l.get_error_context());
      }
  }


inline void subexpr::set_latex(const std::string& ltx, const y::lexeme_type& l)
  {
    if(this->latex)
      {
        l.error(ERROR_LATEX_REDECLARATION);
        this->latex->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->latex = std::make_unique< contexted_value<std::string> >(ltx, l.get_error_context());
      }
  }


class author
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    author() = default;

    ~author() = default;


    // INTERFACE

  public:

    const std::string get_name() const { if(this->name) return *this->name; else return std::string(); }

    void set_name(const std::string& n, const y::lexeme_type& l);

    const std::string get_email() const { if(this->email) return *this->email; else return std::string(); }

    void set_email(const std::string& e, const y::lexeme_type& l);

    const std::string get_institute() const { if(this->institute) return *this->institute; else return std::string(); }

    void set_institute(const std::string& i, const y::lexeme_type& l);


    // INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy an author block

    std::shared_ptr< contexted_value<std::string> > name;

    std::shared_ptr< contexted_value<std::string> > email;

    std::shared_ptr< contexted_value<std::string> > institute;

  };


inline void author::set_institute(const std::string& i, const y::lexeme_type& l)
  {
    if(this->institute)
      {
        l.error(ERROR_INSTITUTE_REDECLARATION);
        this->institute->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->institute = std::make_unique< contexted_value<std::string> >(i, l.get_error_context());
      }
  }


inline void author::set_email(const std::string& e, const y::lexeme_type& l)
  {
    if(this->email)
      {
        l.error(ERROR_EMAIL_REDECLARATION);
        this->email->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->email = std::make_unique< contexted_value<std::string> >(e, l.get_error_context());
      }
  }


inline void author::set_name(const std::string& n, const y::lexeme_type& l)
  {
    if(this->name)
      {
        l.error(ERROR_AUTHORNAME_REDECLARATION);
        this->name->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->name = std::make_unique< contexted_value<std::string> >(n, l.get_error_context());
      }
  }


class string_array
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    string_array() = default;

    ~string_array() = default;


    // INTERFACE

  public:

    const std::vector< contexted_value<std::string> >& get_array() const { return(this->array); }

    void push_element(const std::string& s, const y::lexeme_type& l) { this->array.emplace_back(s, l.get_error_context()); }


    // INTERNAL DATA

  private:

    std::vector< contexted_value<std::string> > array;

  };


#endif //CPPTRANSPORT_SEMANTIC_DATA_H
