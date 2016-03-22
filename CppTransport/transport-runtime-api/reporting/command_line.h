//
// Created by David Seery on 20/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPORTING_COMMAND_LINE_H
#define CPPTRANSPORT_REPORTING_COMMAND_LINE_H


#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/reporting/repository_cache.h"

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
                newline(false),
                include_tasks(false),
                include_inflight(false)
              {
              }

            //! destructor is default
            ~command_line() = default;


            // SELECT REPORTS TO BE INCLUDED

          public:

            //! include list of tasks in report
            void task_list() { this->include_tasks = true; }

            //! include list of inflight jobs in report
            void inflight_list() { this->include_inflight = true; }

            //! report on individual repository items
            void info(const std::vector<std::string>& items) { this->info_items = items; }


            // GENERATE REPORTS

          public:

            //! produce reports
            template <typename number>
            void report(repository<number>& repo);


            // GENERATE REPORTS

          protected:

            //! report on tasks available in a repository
            template <typename number>
            void report_tasks(repository_cache<number>&);

            //! report on inflight content in a repository
            template <typename number>
            void report_inflight(repository_cache<number>&);

            //! produce individual item reports
            template <typename number>
            void report_info(repository_cache<number>&);


            // HANDLE NEWLINES BETWEEN REPORTS

          protected:

            //! check whether a newline is needed to separate reports
            void check_newline() { if(this->newline) { std::cout << '\n'; this->newline = false; } }

            //! force a newline before the next report
            void force_newline() { this->newline = true; }


            // INTERNAL DATA

          private:

            //! reference to local environment object
            local_environment& env;

            //! reference to argument cache
            argument_cache& arg_cache;


            // REPORTS TO DISPLAY

            //! include list of tasks
            bool include_tasks;

            //! include list of inflight jobs
            bool include_inflight;

            //! list of repostitory items to provide detailed reports for
            std::vector<std::string> info_items;


            // CACHE STATUS

            //! display newline before next report?
            bool newline;

          };


        template <typename number>
        void command_line::report(repository<number>& repo)
          {
            repository_cache<number> cache(repo);

            if(this->include_tasks) this->report_tasks<number>(cache);
            if(this->include_inflight) this->report_inflight<number>(cache);
            if(this->info_items.size() > 0) this->report_info<number>(cache);
          }


        template <typename number>
        void command_line::report_tasks(repository_cache<number>& cache)
          {
            this->check_newline();

            typename task_db<number>::type& db = cache.get_task_db();

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

            this->force_newline();
          }


        template <typename number>
        void command_line::report_inflight(repository_cache<number>& cache)
          {
            typename task_db<number>::type& tk_db = cache.get_task_db();
            inflight_db& group_db = cache.get_inflight_db();

            if(group_db.empty()) return;

            this->check_newline();

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

            this->force_newline();
          }


        template <typename number>
        void command_line::report_info(repository_cache<number>& cache)
          {
            this->check_newline();
            this->force_newline();
          }

      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_COMMAND_LINE_H
