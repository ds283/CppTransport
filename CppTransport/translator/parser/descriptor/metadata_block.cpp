//
// Created by David Seery on 24/05/2017.
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


#include "metadata_block.h"
#include "generics_descriptor.h"


// AUTHOR RECORD METHODS


author_declaration::author_declaration(const std::string& n, const y::lexeme_type& l, author& a)
  : declaration_point(l)
  {
    auth = std::make_unique<author>(a);
  }


std::string author_declaration::get_name() const
  {
    return this->auth->get_name();
  }


std::string author_declaration::get_email() const
  {
    return this->auth->get_email();
  }


std::string author_declaration::get_institute() const
  {
    return this->auth->get_institute();
  }


void author_declaration::print(std::ostream& stream) const
  {
    stream << "Author declaration for '" << this->get_name() << "', email = '" << this->get_email() << "', institute = '" << this->get_institute() << "'" << '\n';
  }


// BASIC METADATA


bool metadata_block::set_name(const std::string& n, const y::lexeme_type& l)
  {
    return SetContextedValue(this->name, n, l, ERROR_NAME_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > metadata_block::get_name() const
  {
    if(this->name) return(*this->name); else return(boost::none);
  }


bool metadata_block::set_citeguide(const std::string& t, const y::lexeme_type& l)
  {
    return SetContextedValue(this->citeguide, t, l, ERROR_CITEGUIDE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > metadata_block::get_citeguide() const
  {
    if(this->citeguide) return(*this->citeguide); else return(boost::none);
  }


bool metadata_block::set_description(const std::string& d, const y::lexeme_type& l)
  {
    return SetContextedValue(this->description, d, l, ERROR_DESCRIPTION_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > metadata_block::get_description() const
  {
    if(this->description) return *this->description; else return boost::none;
  }


bool metadata_block::set_revision(int r, const y::lexeme_type& l)
  {
    return SetContextedValue(this->revision, static_cast<unsigned>(std::abs(r)), l, ERROR_REVISION_DECLARATION);
  }


boost::optional< contexted_value<unsigned int>& > metadata_block::get_revision() const
  {
    if(this->revision) return *this->revision; else return boost::none;
  }


bool metadata_block::set_license(const std::string& lic, const y::lexeme_type& l)
  {
    return SetContextedValue(this->license, lic, l, ERROR_LICENSE_REDECLARATION);
  }


boost::optional< contexted_value<std::string>& > metadata_block::get_license() const
  {
    if(this->license) return *this->license; else return boost::none;
  }


bool metadata_block::set_references(const std::vector< contexted_value<std::string> >& refs)
  {
    return SetContextedValue(this->references, refs, ERROR_REFERENCES_REDECLARATION);
  }


boost::optional< std::vector< contexted_value<std::string> >& > metadata_block::get_references() const
  {
    if(this->references) return *this->references; else return boost::none;
  }


bool metadata_block::set_urls(const std::vector<contexted_value<std::string> >& urls)
  {
    return SetContextedValue(this->urls, urls, ERROR_URLS_REDECLARATION);
  }


boost::optional< std::vector< contexted_value<std::string> >& > metadata_block::get_urls() const
  {
    if(this->urls) return *this->urls; else return boost::none;
  }


bool metadata_block::add_author(const std::string& n, const y::lexeme_type& l, author& a)
  {
    auto check = [&](auto& name) -> auto
      {
        boost::optional<author_table::mapped_type::element_type&> rval;

        author_table::const_iterator t = this->authors.find(n);
        if(t != this->authors.cend()) rval = *t->second;

        return rval;
      };
    auto insert = [&](auto& name, auto& lexeme, auto& attr) -> auto
      {
        // add declaration to list
        this->authors.emplace(std::make_pair(name, std::make_unique<author_declaration>(name, lexeme, attr)));
      };

    return GenericInsertSymbol(check, insert, n, l, a, ERROR_AUTHOR_EXISTS, NOTIFY_DUPLICATE_AUTHOR_WAS);
  }


const author_table& metadata_block::get_author() const
  {
    return this->authors;
  }


validation_exceptions metadata_block::validate() const
  {
    validation_exceptions list;
    
    return list;
  }
