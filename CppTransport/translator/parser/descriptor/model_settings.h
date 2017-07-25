//
// Created by David Seery on 10/12/2013.
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


#ifndef CPPTRANSPORT_MODEL_SETTINGS_H
#define CPPTRANSPORT_MODEL_SETTINGS_H


#include <map>

#include "y_common.h"
#include "semantic_values.h"


// author declarations are slightly different; they do not inherit from the base
// declaration class since they are not really symbols in the normal sense
class author_declaration
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    author_declaration(const std::string& n, const y::lexeme_type& l, std::shared_ptr<author> a);

    //! destructor
    ~author_declaration() = default;


    // INTERFACE

  public:

    //! return name
    std::string get_name() const;

    //! return email
    std::string get_email() const;

    //! get institute
    std::string get_institute() const;

    //! return lexeme representing declaration point
    const y::lexeme_type& get_declaration_point() const { return(this->declaration_point); }


    // PRINT TO STANDARD STREAM

  public:

    void print(std::ostream& stream) const;


    // INTERNAL DATA

  protected:

    //! link to author record
    std::shared_ptr<author> auth;

    //! reference to declaration lexeme
    const y::lexeme_type& declaration_point;

  };


enum class index_order
  {
    left, right
  };


enum class model_type
  {
    canonical, nontrivial_metric
  };


inline std::string format(model_type T)
  {
    switch(T)
      {
        case model_type::canonical: return "canonical"; break;
        case model_type::nontrivial_metric: return "nontrivial_metric"; break;
      }
  }


typedef std::map< std::string, std::unique_ptr<author_declaration> > author_table;

typedef std::pair< bool, std::string > validation_message;
typedef std::list< std::unique_ptr<validation_message> > validation_exceptions;


#endif //CPPTRANSPORT_MODEL_SETTINGS_H
