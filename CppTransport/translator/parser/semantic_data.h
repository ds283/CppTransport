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
#include "msg_en.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


class attributes
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    attributes() = default;

    //! destructor
    ~attributes() = default;


		// INTERFACE

  public:

    //! get LaTeX name
    // TODO: move to boost::optional<>
    const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }

    //! set LaTeX name; returns true if value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_latex(const std::string& ltx, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy attribute blocks
    
    //! contexted value representing LaTeX name
    std::shared_ptr< contexted_value<std::string> > latex;

	};


inline bool attributes::set_latex(const std::string& ltx, const y::lexeme_type& l)
  {
    return SetContextedValue(this->latex, ltx, l, ERROR_LATEX_REDECLARATION);
  }


class subexpr
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
		subexpr() = default;

    //! destructor
		~subexpr() = default;


		// INTERFACE

  public:

    //! get LaTeX name
    // TODO: move to boost::optional<>
		const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }
    
    //! set LaTeX name; returns true if value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_latex(const std::string& ltx, const y::lexeme_type& l);

    //! get symbolic value
    // TODO: move to boost::optional<>
		GiNaC::ex get_value() const { if(this->value) return *this->value; else return GiNaC::ex(0); }

    //! set symbolic value; returns true if the value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_value(GiNaC::ex v, const y::lexeme_type& l);


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy a subexpr block

    //! contexted value representing LaTeX name
    std::shared_ptr< contexted_value<std::string> > latex;

    //! contexted value representing symbolic value
    std::shared_ptr< contexted_value<GiNaC::ex> > value;

	};


inline bool subexpr::set_value(GiNaC::ex v, const y::lexeme_type& l)
  {
    return SetContextedValue(this->value, v, l, ERROR_VALUE_REDECLARATION);
  }


inline bool subexpr::set_latex(const std::string& ltx, const y::lexeme_type& l)
  {
    return SetContextedValue(this->latex, ltx, l, ERROR_LATEX_REDECLARATION);
  }


class author
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    author() = default;

    //! destructor
    ~author() = default;


    // INTERFACE

  public:

    //! get author name
    const std::string get_name() const { if(this->name) return *this->name; else return std::string(); }

    //! set author name; returns true if the value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_name(const std::string& n, const y::lexeme_type& l);

    //! get author email
    const std::string get_email() const { if(this->email) return *this->email; else return std::string(); }

    //! set author email; returns true if the value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_email(const std::string& e, const y::lexeme_type& l);

    //! get author institute
    const std::string get_institute() const { if(this->institute) return *this->institute; else return std::string(); }

    //! set author institute; returns true if the value was set, otherwise reports an error
    //! if an existing value would be overwritten
    bool set_institute(const std::string& i, const y::lexeme_type& l);


    // INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy an author block

    //! contexted value representing author name
    std::shared_ptr< contexted_value<std::string> > name;

    //! contexted value representing author email
    std::shared_ptr< contexted_value<std::string> > email;

    //! contexted value representing author institutional affiliation
    std::shared_ptr< contexted_value<std::string> > institute;

  };


inline bool author::set_institute(const std::string& i, const y::lexeme_type& l)
  {
    return SetContextedValue(this->institute, i, l, ERROR_INSTITUTE_REDECLARATION);
  }


inline bool author::set_email(const std::string& e, const y::lexeme_type& l)
  {
    return SetContextedValue(this->email, e, l, ERROR_EMAIL_REDECLARATION);
  }


inline bool author::set_name(const std::string& n, const y::lexeme_type& l)
  {
    return SetContextedValue(this->name, n, l, ERROR_AUTHORNAME_REDECLARATION);
  }


class string_array
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    string_array() = default;

    //! destructor
    ~string_array() = default;


    // INTERFACE

  public:

    //! get array of contexted values representing string literals
    const std::vector< contexted_value<std::string> >& get_array() const { return(this->array); }

    //! push element to the array
    void push_element(const std::string& s, const y::lexeme_type& l) { this->array.emplace_back(s, l.get_error_context()); }


    // INTERNAL DATA

  private:

    //! array of contexted string literals
    std::vector< contexted_value<std::string> > array;

  };


#endif //CPPTRANSPORT_SEMANTIC_DATA_H
