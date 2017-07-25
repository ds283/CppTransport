//
// Created by David Seery on 18/12/2015.
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

#ifndef CPPTRANSPORT_BACKEND_DATA_H
#define CPPTRANSPORT_BACKEND_DATA_H


#include <iostream>
#include <fstream>
#include <unordered_map>

#include "error_context.h"
#include "model_settings.h"

#include "contexted_value.h"

#include "boost/optional.hpp"
#include "boost/filesystem/operations.hpp"
#include "translator_data.h"


class backend_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    backend_data(const boost::filesystem::path& path, filestack& s, error_context::error_handler e,
                 error_context::warning_handler w, finder& f, argument_cache& a);

    //! destructor
    ~backend_data() = default;


    // INTERFACE

  public:

    //! returns true if all backend data is present and the template can be
    //! read, otherwise returns false
    operator bool() const { return(this->name && this->minver); }

    //! perform validation
    bool validate(translator_data& payload) const;
    

    // QUERY DATA

  public:

    //! get minimum version
    unsigned int get_min_version() const { if(this->minver) return *this->minver; else throw std::runtime_error(ERROR_UNSET_BACKEND_DATA);  }

    //! get backend name
    const std::string get_backend_name() const { if(this->name) return *this->name; else throw std::runtime_error(ERROR_UNSET_BACKEND_DATA); }

    //! get model type; default to canonical if unset
    //! (this field wasn't available prior to 2017.01, so templates for CppTransport versions before this won't set it)
    model_type get_model_type() const { if(this->type) return *this->type; else return model_type::canonical; }


    // INTERNAL DATA

  private:
    
    //! cache template name
    const boost::filesystem::path input;

    //! minimum version
    std::unique_ptr< contexted_value<unsigned int> > minver;

    //! name of backend
    std::unique_ptr< contexted_value<std::string> > name;

    //! type of u-factory
    std::unique_ptr< contexted_value<model_type> > type;

  };


#endif //CPPTRANSPORT_BACKEND_DATA_H
