//
// Created by David Seery on 20/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPORTING_COMMAND_LINE_H
#define CPPTRANSPORT_REPORTING_COMMAND_LINE_H


#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/argument_cache.h"

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
            command_line(local_environment& e, argument_cache& c)
              : env(e),
                arg_cache(c),
                show_status(false)
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
            void report_repository_tasks(typename task_db<number>::type& db);

            //! report on inflight content in a repository
            template <typename number>
            void report_repository_inflight(typename task_db<number>::type& tk_db, inflight_db& group_db);


            // INTERNAL DATA

          private:

            //! reference to local environment object
            local_environment& env;

            //! reference to argument cache
            argument_cache& arg_cache;

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
            typename task_db<number>::type tk_db = repo.enumerate_tasks();
            inflight_db group_db = repo.enumerate_inflight();

            this->report_repository_tasks<number>(tk_db);
            this->report_repository_inflight<number>(tk_db, group_db);
          }


        template <typename number>
        void command_line::report_repository_tasks(typename task_db<number>::type& db)
          {
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_magenta);
            std::cout << CPPTRANSPORT_REPORT_STATUS_TASKS << '\n';
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);

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

            asciitable at(std::cout, this->env, this->arg_cache);
            at.set_terminal_output(true);
            at.write(columns, table);
          }


        template <typename number>
        void command_line::report_repository_inflight(typename task_db<number>::type& tk_db, inflight_db& group_db)
          {
            if(group_db.empty()) return;

            std::cout << '\n';

            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_magenta);
            std::cout << CPPTRANSPORT_REPORT_STATUS_INFLIGHT << '\n';
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);

            std::vector<column_descriptor> columns;
            std::vector<std::string> name;
            std::vector<std::string> parent;
            std::vector<std::string> type;
            std::vector<std::string> init;
            std::vector<std::string> duration;
            std::vector<std::string> cores;
            std::vector<std::string> complete;

            for(const inflight_db_value_type& group : group_db)
              {
                name.push_back(group.first);
                parent.push_back(group.second->task_name);

                typename task_db<number>::type::const_iterator t = tk_db.find(group.second->task_name);
                if(t != tk_db.end())
                  {
                    type.push_back(task_type_to_string(t->second->get_type()));
                  }
                else
                  {
                    type.push_back("--");
                  }

                boost::posix_time::ptime init_time = boost::posix_time::from_iso_string(group.second->posix_time);
                boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

                boost::posix_time::time_duration runtime = now - init_time;

                init.push_back(boost::posix_time::to_simple_string(init_time));
                duration.push_back(format_time(runtime.total_nanoseconds()));

                cores.push_back(boost::lexical_cast<std::string>(group.second->cores));

                if(group.second->completion == CPPTRANSPORT_DEFAULT_COMPLETION_UNSET)
                  {
                    complete.push_back("--");
                  }
                else
                  {
                    boost::posix_time::ptime complete_time = boost::posix_time::from_iso_string(group.second->completion);
                    complete.push_back(boost::posix_time::to_simple_string(complete_time));
                  }

//                std::cout << "'" << name.back() << "'" << '\n';
//                std::cout << "'" << parent.back() << "'" << '\n';
//                std::cout << "'" << type.back() << "'" << '\n';
//                std::cout << "'" << init.back() << "'" << '\n';
//                std::cout << "'" << duration.back() << "'" << '\n';
//                std::cout << "'" << cores.back() << "'" << '\n';
//                std::cout << "'" << complete.back() << "'" << '\n';
              }

            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_CONTENT, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_PARENT, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_TYPE, column_justify::left);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_INITIATED, column_justify::right);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_DURATION, column_justify::right);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_CORES, column_justify::right);
            columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_COMPLETION, column_justify::right);

            std::vector< std::vector<std::string> > table;
            table.push_back(name);
            table.push_back(parent);
            table.push_back(type);
            table.push_back(init);
            table.push_back(duration);
            table.push_back(cores);
            table.push_back(complete);

            asciitable at(std::cout, this->env, this->arg_cache);
            at.set_terminal_output(true);
            at.write(columns, table);
          }


        template <typename number>
        void command_line::report_info(repository<number>& repo)
          {

          }

      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_COMMAND_LINE_H
