//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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

    void set_latex(const std::string& ltx, const y::lexeme_type& l) { this->latex.reset(); this->latex = std::make_unique< contexted_value<std::string> >(ltx, l.get_error_context()); }


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy attribute blocks

    std::shared_ptr< contexted_value<std::string> > latex;

	};


class subexpr
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

		subexpr()
      : value(0)    // initialize value to sensible choice
      {
      }

		~subexpr() = default;


		// INTERFACE

  public:

		const std::string get_latex() const { if(this->latex) return *this->latex; else return std::string(); }

		void set_latex(const std::string& ltx, const y::lexeme_type& l) { this->latex.reset(); this->latex = std::make_unique< contexted_value<std::string> >(ltx, l.get_error_context()); }

		GiNaC::ex get_value() const { return this->value; }

		void set_value(GiNaC::ex v) { this->value = v; }


		// INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy a subexpr block

    std::shared_ptr< contexted_value<std::string> > latex;

    GiNaC::ex value;
	};


class author
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    author() = default;

    ~author() = default;


    // INTERFACE

  public:

    const std::string get_name() const { if(this->name) return *this->name; else return std::string(); }

    void set_name(const std::string& n, const y::lexeme_type& l) { this->name.reset(); this->name = std::make_unique< contexted_value<std::string> >(n, l.get_error_context()); }

    const std::string get_email() const { if(this->email) return *this->email; else return std::string(); }

    void set_email(const std::string& e, const y::lexeme_type& l) { this->email.reset(); this->email = std::make_unique< contexted_value<std::string> >(e, l.get_error_context()); }

    const std::string get_institute() const { if(this->institute) return *this->institute; else return std::string(); }

    void set_institute(const std::string& i, const y::lexeme_type& l) { this->institute.reset(); this->institute = std::make_unique< contexted_value<std::string> >(i, l.get_error_context()); }


    // INTERNAL DATA

  private:

    // storage is via shared_ptr<> because we sometimes copy an author block

    std::shared_ptr< contexted_value<std::string> > name;

    std::shared_ptr< contexted_value<std::string> > email;

    std::shared_ptr< contexted_value<std::string> > institute;

  };


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
