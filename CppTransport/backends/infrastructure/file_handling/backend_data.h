//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
    double get_min_version() const { return(*this->minver); }

    //! get backend name
    const std::string& get_backend_name() const { return(*this->name); }

    //! get model type
    enum model_type get_model_type() const { return(this->type); }


    // INTERNAL DATA

  private:

    //! cache header line
    std::string line;

    //! minimum version
    boost::optional<double> minver;

    //! name of backend
    boost::optional<std::string> name;

    //! type of u-factory
    enum model_type type;

  };


#endif //CPPTRANSPORT_BACKEND_DATA_H
