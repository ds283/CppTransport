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

#include "error_context.h"
#include "model_settings.h"

#include "boost/optional.hpp"
#include "boost/filesystem/operations.hpp"


class backend_data
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    backend_data(std::ifstream& inf, const boost::filesystem::path& path, error_context& err_context);

    //! destructor
    ~backend_data() = default;


    // QUERY WHETHER ALL BACKEND DATA OK

  public:

    operator bool() const { return(this->name && this->minver); }


    // QUERY DATA

  public:

    //! get minimum version
    unsigned int get_min_version() const { return(*this->minver); }

    //! get backend name
    const std::string& get_backend_name() const { return(*this->name); }

    //! get model type
    model_type get_model_type() const { return(this->type); }


    // INTERNAL DATA

  private:

    //! cache header line
    std::string line;

    //! minimum version
    boost::optional<unsigned int> minver;

    //! name of backend
    boost::optional<std::string> name;

    //! type of u-factory
    model_type type;

  };


#endif //CPPTRANSPORT_BACKEND_DATA_H
