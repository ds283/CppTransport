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


#ifndef CPPTRANSPORT_METADATA_BLOCK_H
#define CPPTRANSPORT_METADATA_BLOCK_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"


class metadata_block
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    metadata_block(unsigned int& ec)
      : err_count(ec)
      {
      }

    //! destructor is default
    ~metadata_block() = default;
    
    
    // SERVICES
  
  public:
    
    //! perform validation
    validation_exceptions validate() const;


    // GET AND SET BASIC METADATA

  public:

    //! get model name (a textual string)
    bool set_name(const std::string& n, const y::lexeme_type& l);

    //! get model name as contexted value
    boost::optional< contexted_value<std::string>& > get_name() const;


    //! add an author
    bool add_author(const std::string& n, const y::lexeme_type& l, author& a);

    //! get author table
    const author_table& get_author() const;


    //! set citation guidance
    bool set_citeguide(const std::string& t, const y::lexeme_type& l);

    //! get citation guidance as contexted value
    boost::optional< contexted_value<std::string>& > get_citeguide() const;


    //! set model description
    bool set_description(const std::string& t, const y::lexeme_type& l);

    //! get model description as contexted value
    boost::optional< contexted_value<std::string>& > get_description() const;


    //! set model revision
    bool set_revision(int r, const y::lexeme_type& l);

    //! get model revision as contexted value
    boost::optional< contexted_value<unsigned int>& > get_revision() const;


    //! set model license string
    bool set_license(const std::string& t, const y::lexeme_type& l);

    //! get model license string as contexted value
    boost::optional< contexted_value<std::string>& > get_license() const;


    //! set model reference list
    bool set_references(const std::vector<contexted_value<std::string> >& refs);

    //! get model reference list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_references() const;


    //! set model URL list
    bool set_urls(const std::vector<contexted_value<std::string> >& urls);

    //! get model URL list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_urls() const;


    // INTERNAL DATA

  private:

    //! error count
    unsigned int& err_count;


    // METADATA

    //! 'name' is a human-readable name
    std::unique_ptr<contexted_value<std::string> > name;

    //! citation guidance
    std::unique_ptr<contexted_value<std::string> > citeguide;

    //! model description
    std::unique_ptr<contexted_value<std::string> > description;

    //! model revision
    std::unique_ptr<contexted_value<unsigned int> > revision;

    //! model license string
    std::unique_ptr<contexted_value<std::string> > license;

    //! list of references
    std::unique_ptr<std::vector<contexted_value<std::string> > > references;

    //! list of URLs
    std::unique_ptr<std::vector<contexted_value<std::string> > > urls;

    //! author table
    author_table authors;

  };


#endif //CPPTRANSPORT_METADATA_BLOCK_H
