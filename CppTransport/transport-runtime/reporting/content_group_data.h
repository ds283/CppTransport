//
// Created by David Seery on 22/03/2016.
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

#ifndef CPPTRANSPORT_REPORTING_CONTENT_GROUP_DATA_H
#define CPPTRANSPORT_REPORTING_CONTENT_GROUP_DATA_H


#include <string>

#include "transport-runtime/repository/repository_cache.h"

#include "boost/date_time.hpp"


namespace transport
  {

    namespace reporting
      {

        template <typename number>
        class content_group_data
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            content_group_data(const std::string& n, repository_cache<number>& c);

            //! destructor is default
            ~content_group_data() = default;


            // INTERFACE

          public:

            //! get task name
            const std::string& get_task() const { return(this->task); }

            //! get task type
            const std::string& get_type() const { return(this->type); }

            //! get last edit time
            const std::string& get_last_edit() const { return(this->last_edit); }


            // INTERNAL API

          protected:

            //! search a database for a named content gorup
            template <typename DatabaseType>
            bool check_database(const std::string& item, DatabaseType& db);

            //! match an integration content group
            void match_content(const content_group_record<integration_payload>& rec);

            //! match a postintegration content group
            void match_content(const content_group_record<postintegration_payload>& rec);

            //! match an output content group
            void match_content(const content_group_record<output_payload>& rec);


            // INTERNAL DATA

          private:

            //! name of content group
            std::string group;

            //! reference to repository cache
            repository_cache<number>& cache;


            // DATA

            //! name of task associated with this group
            std::string task;

            //! type of task associated with this group
            std::string type;

            //! last-edit time of this content group
            std::string last_edit;

          };


        template <typename number>
        content_group_data<number>::content_group_data(const std::string& n, repository_cache<number>& c)
          : group(n),
            cache(c),
            task("--"),
            type("--"),
            last_edit("--")
          {
            if(this->check_database(group, cache.get_integration_content_db())) return;
            if(this->check_database(group, cache.get_postintegration_content_db())) return;
            if(this->check_database(group, cache.get_output_content_db())) return;
          }


        template <typename number>
        template <typename DatabaseType>
        bool content_group_data<number>::check_database(const std::string& item, DatabaseType& db)
          {
            typename DatabaseType::const_iterator t = db.find(item);

            if(t != db.end())
              {
                this->match_content(*t->second);
                return(true);
              }

            return(false);
          }


        template <typename number>
        void content_group_data<number>::match_content(const content_group_record<integration_payload>& rec)
          {
            this->type = task_type_to_string(task_type::integration);
            this->task = rec.get_task_name();
            this->last_edit = boost::posix_time::to_simple_string(rec.get_last_edit_time());
          }


        template <typename number>
        void content_group_data<number>::match_content(const content_group_record<postintegration_payload>& rec)
          {
            this->type = task_type_to_string(task_type::postintegration);
            this->task = rec.get_task_name();
            this->last_edit = boost::posix_time::to_simple_string(rec.get_last_edit_time());
          }


        template <typename number>
        void content_group_data<number>::match_content(const content_group_record<output_payload>& rec)
          {
            this->type = task_type_to_string(task_type::output);
            this->task = rec.get_task_name();
            this->last_edit = boost::posix_time::to_simple_string(rec.get_last_edit_time());
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_CONTENT_GROUP_DATA_H
