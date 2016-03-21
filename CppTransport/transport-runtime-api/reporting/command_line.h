//
// Created by David Seery on 20/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPORTING_COMMAND_LINE_H
#define CPPTRANSPORT_REPORTING_COMMAND_LINE_H


#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/utilities/asciitable.h"
#include "transport-runtime-api/utilities/formatter.h"


namespace transport
  {

    namespace reporting
      {

        class command_line
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            command_line()
              : show_status(false)
              {
              }

            //! destructor is default
            ~command_line() = default;


            // INTERFACE

          public:

            //! report on overall status of repository
            void status() { this->show_status = true; }

            //! report on individual repository items
            void info(const std::vector<std::string>& items) { this->info_items = items; }

            //! produce reports
            template <typename number>
            void report(repository<number>& repo);


            // INTERNAL API

          protected:

            //! produce status report
            template <typename number>
            void report_status(repository<number>& repo);

            //! produce individual item reports
            template <typename number>
            void report_info(repository<number>& repo);


            // REPOSITORY STATUS REPORT

          protected:

            //! report on tasks available in a repository
            template <typename number>
            void report_repository_tasks(repository<number>& repo);

            //! report on inflight content in a repository
            template <typename number>
            void report_repository_inflight(repository<number>& repo);


            // INTERNAL DATA

          private:

            //! emit repository status report
            bool show_status;

            //! list of repostitory items to provide detailed reports for
            std::vector<std::string> info_items;

          };


        template <typename number>
        void command_line::report(repository<number>& repo)
          {
            if(this->show_status) this->report_status(repo);
            if(this->info_items.size() > 0) this->report_info(repo);
          }


        template <typename number>
        void command_line::report_status(repository<number>& repo)
          {
            this->report_repository_tasks(repo);
            std::cout << '\n';
            this->report_repository_inflight(repo);
          }


        template <typename number>
        void command_line::report_repository_tasks(repository<number>& repo)
          {
            typename task_db<number>::type db = repo.enumerate_tasks();

            std::cout << CPPTRANSPORT_REPORT_STATUS_TASKS << '\n';

            std::vector<column_descriptor> columns;
            std::vector<std::string> task;
            std::vector<std::string> last_edit;
            std::vector<std::string> type;
            std::vector<std::string> num_groups;

            for(const typename task_db<number>::value_type& tk : db)
              {
                task.push_back(tk.first);
                last_edit.push_back(boost::posix_time::to_simple_string(tk.second->get_last_edit_time()));
                type.push_back(task_type_to_string(tk.second->get_type()));
                num_groups.push_back(boost::lexical_cast<std::string>(tk.second->get_output_groups().size()));
              }

            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_TASK_NAME, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_TYPE, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_LAST_EDIT, column_justify::right);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_NUM_GROUPS, column_justify::right);

            std::vector< std::vector<std::string> > table;
            table.push_back(task);
            table.push_back(type);
            table.push_back(last_edit);
            table.push_back(num_groups);

            asciitable at(std::cout);
            at.write(columns, table);
          }


        template <typename number>
        void command_line::report_repository_inflight(repository<number>& repo)
          {
            inflight_db db = repo.enumerate_inflight();

            if(db.empty()) return;

            std::cout << CPPTRANSPORT_REPORT_STATUS_INFLIGHT << '\n';

            std::vector<column_descriptor> columns;
            std::vector<std::string> name;
            std::vector<std::string> parent;
            std::vector<std::string> init;
            std::vector<std::string> duration;

            for(const inflight_db_value_type& group : db)
              {
                name.push_back(group.first);
                parent.push_back(group.second->task_name);

                boost::posix_time::ptime init_time = boost::posix_time::from_iso_string(group.second->posix_time);
                boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

                boost::posix_time::time_duration runtime = now - init_time;

                init.push_back(boost::posix_time::to_simple_string(init_time));
                duration.push_back(format_time(runtime.total_nanoseconds()));
              }

            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_CONTENT, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_PARENT, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_INITIATED, column_justify::right);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_DURATION, column_justify::right);

            std::vector< std::vector<std::string> > table;
            table.push_back(name);
            table.push_back(parent);
            table.push_back(init);
            table.push_back(duration);

            asciitable at(std::cout);
            at.write(columns, table);
          }


        template <typename number>
        void command_line::report_info(repository<number>& repo)
          {

          }

      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_COMMAND_LINE_H
