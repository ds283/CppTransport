//
// Created by David Seery on 26/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_AUTHOR_H
#define CPPTRANSPORT_AUTHOR_H


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


#endif //CPPTRANSPORT_AUTHOR_H
