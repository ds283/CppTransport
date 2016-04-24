//
// Created by David Seery on 21/03/2016.
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

#ifndef CPPTRANSPORT_INFLIGHT_H
#define CPPTRANSPORT_INFLIGHT_H

#include <string>
#include <map>

#include "boost/filesystem/operations.hpp"

namespace transport
  {

    //! details of inflight groups
    class inflight_group
      {
      public:
        std::string             name;
        std::string             task_name;
        boost::filesystem::path path;
        std::string             posix_time;
        unsigned int            cores;
        std::string             completion;
      };


    typedef std::map< std::string, std::unique_ptr<inflight_group> > inflight_db;
    typedef std::pair< const std::string, std::unique_ptr<inflight_group> > inflight_db_value_type;


    //! details of an inflight integration task
    class inflight_integration
      {
      public:
        std::string             content_group;
        std::string             task_name;
        boost::filesystem::path output;
        boost::filesystem::path container;
        boost::filesystem::path logdir;
        boost::filesystem::path tempdir;
        unsigned int            workgroup_number;
        bool                    is_seeded;
        std::string             seed_group;
        bool                    is_collecting_stats;
        bool                    is_collecting_ics;
      };


    typedef std::map< std::string, std::unique_ptr<inflight_integration> > inflight_integration_db;
    typedef std::pair< const std::string, std::unique_ptr<inflight_integration> > inflight_integration_db_value_type;


    //! details of an inflight postintegration task
    class inflight_postintegration
      {
      public:
        std::string             content_group;
        std::string             task_name;
        boost::filesystem::path output;
        boost::filesystem::path container;
        boost::filesystem::path logdir;
        boost::filesystem::path tempdir;
        bool                    is_paired;
        std::string             parent_group;
        bool                    is_seeded;
        std::string             seed_group;
      };


    typedef std::map< std::string, std::unique_ptr<inflight_postintegration> > inflight_postintegration_db;
    typedef std::pair< const std::string, std::unique_ptr<inflight_postintegration> > inflight_postintegration_db_value_type;


    //! details of an inflight output/derived-content task
    class inflight_derived_content
      {
      public:
        std::string             content_group;
        std::string             task_name;
        boost::filesystem::path output;
        boost::filesystem::path logdir;
        boost::filesystem::path tempdir;
      };


    typedef std::map< std::string, std::unique_ptr<inflight_derived_content> > inflight_derived_content_db;
    typedef std::pair< const std::string, std::unique_ptr<inflight_derived_content> > inflight_derived_content_db_value_type;

  }

#endif //CPPTRANSPORT_INFLIGHT_H
