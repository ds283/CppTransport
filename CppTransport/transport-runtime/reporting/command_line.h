//
// Created by David Seery on 20/03/2016.
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

#ifndef CPPTRANSPORT_REPORTING_COMMAND_LINE_H
#define CPPTRANSPORT_REPORTING_COMMAND_LINE_H


#include "transport-runtime/repository/repository.h"

#include "transport-runtime/reporting/repository_cache.h"
#include "transport-runtime/reporting/key_value.h"
#include "transport-runtime/reporting/content_group_data.h"

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/utilities/asciitable.h"
#include "transport-runtime/utilities/formatter.h"
#include "transport-runtime/utilities/match.h"


namespace transport
  {

    namespace reporting
      {

        constexpr unsigned int CPPTRANSPORT_REPORT_MAX_TAG_LENGTH = 20;


        namespace command_line_impl
          {

            template <typename RecordType> struct record_traits;

            template <typename number>
            struct record_traits< package_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_PACKAGE); }
              };

            template <typename number>
            struct record_traits< task_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_TASK); }
              };

            template <typename number>
            struct record_traits< integration_task_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_INTEGRATION_TASK); }
              };

            template <typename number>
            struct record_traits< postintegration_task_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_POSTINTEGRATION_TASK); }
              };

            template <typename number>
            struct record_traits< output_task_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_OUTPUT_TASK); }
              };

            template <typename number>
            struct record_traits< derived_product_record<number> >
              {
                std::string type_string() const { return std::string(CPPTRANSPORT_REPORT_RECORD_DERIVED); }
              };

            template <>
            struct record_traits< content_group_record<integration_payload> >
              {
                static std::string type_string() { return std::string(CPPTRANSPORT_REPORT_RECORD_INTEGRATION); }
              };

            template <>
            struct record_traits< content_group_record<postintegration_payload> >
              {
                static std::string type_string() { return std::string(CPPTRANSPORT_REPORT_RECORD_POSTINTEGRATION); }
              };

            template <>
            struct record_traits< content_group_record<output_payload> >
              {
                static std::string type_string() { return std::string(CPPTRANSPORT_REPORT_RECORD_OUTPUT); }
              };

          }

        // pull in command_line_impl namespace for this translation unit
        using namespace command_line_impl;


        class command_line
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            command_line(local_environment& e, argument_cache& c)
              : env(e),
                arg_cache(c),
                err(error_handler(e, c)),
                warn(warning_handler(e, c)),
                msg(message_handler(e, c)),
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

            //! report on specified repository items
            void info(const std::vector<std::string>& items) { this->info_items = items; }

            //! report on provenance of specific content groups
            void provenance(const std::vector<std::string>& items) { this->provenance_items = items; }


            // GENERATE REPORTS

          public:

            //! produce reports
            template <typename number>
            void report(repository<number>& repo);


            // GENERATE REPORTS

          protected:

            //! report on tasks available in a repository
            template <typename number>
            void report_task_list(repository_cache<number>& cache);

            //! report on inflight content in a repository
            template <typename number>
            void report_inflight(repository_cache<number>& cache);

            //! produce reports on specified repository items
            template <typename number>
            void report_info(repository_cache<number>& cache);

            //! produce detailed provenance report on specified content groups
            template <typename number>
            void report_provenance(repository_cache<number>& cache);


            // SEARCH A DATABASE FOR OBJECTS TO REPORT

          protected:

            //! report on named items in a database
            template <typename number, typename DatabaseType>
            void report_database_items(std::map<std::string, bool>& items, DatabaseType& db, repository_cache<number>& cache);


            // REPORT ON SPECIFIED DATABASE OBJECTS

          protected:

            //! report on a package object
            template <typename number>
            void report_object(const package_record<number>& rec, repository_cache<number>& cache);

            //! report on a generic task object
            template <typename number>
            void report_object(const task_record<number>& rec, repository_cache<number>& cache);

            //! report on an integration task object
            template <typename number>
            void report_object(const integration_task_record<number>& rec, repository_cache<number>& cache);

            //! report on a postintegration task object
            template <typename number>
            void report_object(const postintegration_task_record<number>& rec, repository_cache<number>& cache);

            //! report on an output task object
            template <typename number>
            void report_object(const output_task_record<number>& rec, repository_cache<number>& cache);

            //! report on a derived product object
            template <typename number>
            void report_object(const derived_product_record<number>& rec, repository_cache<number>& cache);

            //! report on an integration content group
            template <typename number>
            void report_object(const content_group_record<integration_payload>& rec, repository_cache<number>& cache);

            //! report on a postintegration content group
            template <typename number>
            void report_object(const content_group_record<postintegration_payload>& rec, repository_cache<number>& cache);

            //! report on an output content group
            template <typename number>
            void report_object(const content_group_record<output_payload>& rec, repository_cache<number>& cache);

            //! report provenance data for specified content group
            template <typename number>
            void report_provenance(const content_group_record<output_payload>& rec, repository_cache<number>& cache);


            // GENERIC REPORTING FUNCTIONS

          protected:

            //! write out title for a generic record
            template <typename RecordType>
            void report_record_title(const RecordType& rec);

            //! report details for a generic record
            //! (creation time, last edit time, API level)
            void report_record_generic(const repository_record& rec);

            //! report details for a generic content group record
            //! (used instead of report_record_generic() and subsumes its functionality)
            template <typename Payload>
            void report_output_record_generic(const content_group_record<Payload>& rec, const std::string& type="");

            //! compose tags into a single string
            std::string compose_tag_list(const std::list<std::string>& tag_list);


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


            // MESSAGE HANDLERS

            //! error handler
            error_handler err;

            //! warning handler
            warning_handler warn;

            //! message handler
            message_handler msg;


            // REPORTS TO DISPLAY

            //! include list of tasks
            bool include_tasks;

            //! include list of inflight jobs
            bool include_inflight;

            //! list of repostitory items to provide detailed reports for
            std::vector<std::string> info_items;

            //! list of content groups to provided detailed provenance for
            std::vector<std::string> provenance_items;


            // CACHE STATUS

            //! display newline before next report?
            bool newline;

          };


        template <typename number>
        void command_line::report(repository<number>& repo)
          {
            repository_cache<number> cache(repo);

            if(this->include_tasks) this->report_task_list<number>(cache);
            if(this->include_inflight) this->report_inflight<number>(cache);
            if(!this->info_items.empty()) this->report_info<number>(cache);
            if(!this->provenance_items.empty()) this->report_provenance<number>(cache);
          }


        template <typename number>
        void command_line::report_task_list(repository_cache<number>& cache)
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
                num_groups.push_back(boost::lexical_cast<std::string>(tk.second->get_content_groups().size()));
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

            // for each inflight group, push its details into the vectors for each column;
            // then assemble the columns into a table and write them
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
                    type.emplace_back("--");
                  }

                boost::posix_time::ptime init_time = boost::posix_time::from_iso_string(group.second->posix_time);

                // writers are tagged with UTC, not local time, so we have to be sure to use UTC here to compute the elapsed time
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

                boost::posix_time::time_duration runtime = now - init_time;

                init.push_back(boost::posix_time::to_simple_string(init_time));
                duration.push_back(format_time(runtime.total_nanoseconds()));

                cores.push_back(boost::lexical_cast<std::string>(group.second->cores));

                if(group.second->completion == CPPTRANSPORT_DEFAULT_COMPLETION_UNSET)
                  {
                    complete.emplace_back("--");
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
            // make local copy of items; we will tag items in this list as 'processed' as we go
            std::map<std::string, bool> items;
            for(const std::string& item : this->info_items)
              {
                items[item] = false;
              }

            // work through packages
            this->report_database_items(items, cache.get_package_db(), cache);

            // work through tasks
            this->report_database_items(items, cache.get_task_db(), cache);

            // work through derived products
            this->report_database_items(items, cache.get_derived_product_db(), cache);

            // work through integration content
            this->report_database_items(items, cache.get_integration_content_db(), cache);

            // work through postintegration content
            this->report_database_items(items, cache.get_postintegration_content_db(), cache);

            // work through output content
            this->report_database_items(items, cache.get_output_content_db(), cache);

            for(const std::pair< const std::string, bool >& item : items)
              {
                if(!item.second)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_REPORT_MISSING_OBJECT << " '" << item.first << "'";
                    this->warn(msg.str());
                  }
              }
          }


        template <typename number, typename DatabaseType>
        void command_line::report_database_items(std::map< std::string, bool >& items, DatabaseType& db, repository_cache<number>& cache)
          {
            for(std::pair< const std::string, bool >& item : items)
              {
                // search for item
                for(const typename DatabaseType::value_type& record : db)
                  {
                    if(check_match(record.second->get_name(), item.first))
                      {
                        this->report_object(*record.second, cache);
                        item.second = true;   // mark as a match for this item
                      }
                  }
              }
          }


        template <typename RecordType>
        void command_line::report_record_title(const RecordType& rec)
          {
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_magenta);
            std::cout << rec.get_name();
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);

            std::cout << " -- ";

            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold);
            std::cout << record_traits<RecordType>().type_string();
            if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);

            std::cout << '\n';
          }


        template <typename number>
        void command_line::report_object(const package_record<number>& rec, repository_cache<number>& cache)
          {
            this->check_newline();
            this->report_record_title(rec);
            this->report_record_generic(rec);

            const initial_conditions<number>& ics = rec.get_ics();
            model<number>* mdl = ics.get_model();

            key_value kv(this->env, this->arg_cache);
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_NAME, mdl->get_name());
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_CITEGUIDE, mdl->get_citeguide());
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_LICENSE, mdl->get_license());
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_REVISION, boost::lexical_cast<std::string>(mdl->get_revision()));
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_N_INIT, boost::lexical_cast<std::string>(ics.get_N_initial()));
            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_N_CROSSING, boost::lexical_cast<std::string>(ics.get_N_horizon_crossing()));

            kv.set_tiling(true);
            kv.write(std::cout);
            std::cout << '\n';

            const std::string& description = mdl->get_description();
            if(!description.empty())
              {
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_PACKAGE_MODEL_DESCRIPTION;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
                std::cout << description << '\n' << '\n';
              }

            const author_db& db = mdl->get_authors();
            if(!db.empty())
              {
                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> email;
                std::vector<std::string> institute;

                for(const author_db::value_type& item : db)
                  {
                    if(item.second)
                      {
                        const author_record& record = *item.second;
                        name.push_back(record.get_name());
                        email.push_back(record.get_email());
                        institute.push_back(record.get_institute());
                      }
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_EMAIL, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHOR_INSTITUTE, column_justify::left);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(email);
                table.push_back(institute);

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_PACKAGE_MODEL_AUTHORS;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
                std::cout << '\n';
              }

            const std::vector<std::string> references = mdl->get_references();
            if(!references.empty())
              {
                kv.reset();

                unsigned int serial = 1;
                for(const std::string& s : references)
                  {
                    kv.insert_back(boost::lexical_cast<std::string>(serial), s);
                    ++serial;
                  }

                kv.set_tiling(false);
                kv.set_title(CPPTRANSPORT_REPORT_PACKAGE_MODEL_REFERENCES);
                kv.write(std::cout);
                std::cout << '\n';
              }

            const std::vector<std::string> urls = mdl->get_urls();
            if(!urls.empty())
              {
                kv.reset();

                unsigned int serial = 1;
                for(const std::string& s : urls)
                  {
                    kv.insert_back(boost::lexical_cast<std::string>(serial), s);
                    ++serial;
                  }

                kv.set_tiling(false);
                kv.set_title(CPPTRANSPORT_REPORT_PACKAGE_MODEL_URLS);
                kv.write(std::cout);
                std::cout << '\n';
              }

            kv.reset();
            const parameters<number>& params = ics.get_params();
            const std::vector<number>& param_vec = params.get_vector();
            const std::vector<std::string>& param_names = mdl->get_param_names();

            kv.insert_back(CPPTRANSPORT_REPORT_PACKAGE_MP, boost::lexical_cast<std::string>(params.get_Mp()));
            for(unsigned int i = 0; i < param_vec.size() && i < param_names.size(); ++i)
              {
                kv.insert_back(param_names[i], boost::lexical_cast<std::string>(param_vec[i]));
              }

            kv.set_tiling(true);
            kv.set_title(CPPTRANSPORT_REPORT_PACKAGE_PARAMETERS);
            kv.write(std::cout);
            std::cout << '\n';

            kv.reset();
            const std::vector<number>& ics_vec = ics.get_vector();
            const std::vector<std::string>& coord_names = mdl->get_state_names();

            for(unsigned int i = 0; i < ics_vec.size() && i < coord_names.size(); ++i)
              {
                kv.insert_back(coord_names[i], boost::lexical_cast<std::string>(ics_vec[i]));
              }

            kv.set_tiling(true);
            kv.set_title(CPPTRANSPORT_REPORT_PACKAGE_ICS);
            kv.write(std::cout);

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const task_record<number>& rec, repository_cache<number>& cache)
          {
            switch(rec.get_type())
              {
                case task_type::integration:
                  {
                    this->report_object(dynamic_cast< const integration_task_record<number>& >(rec), cache);
                    break;
                  }

                case task_type::postintegration:
                  {
                    this->report_object(dynamic_cast< const postintegration_task_record<number>& >(rec), cache);
                    break;
                  }

                case task_type::output:
                  {
                    this->report_object(dynamic_cast< const output_task_record<number>& >(rec), cache);
                    break;
                  }
              }
          }


        template <typename number>
        void command_line::report_object(const integration_task_record<number>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_record_generic(rec);

            key_value kv(this->env, this->arg_cache);
            kv.insert_back(CPPTRANSPORT_REPORT_TASK_TYPE, task_type_to_string(rec.get_task_type()));
            kv.insert_back(CPPTRANSPORT_REPORT_USES_PACKAGE, rec.get_task()->get_ics().get_name());
            kv.insert_back(CPPTRANSPORT_REPORT_KCONFIG_DB, rec.get_relative_kconfig_database_path().string());

            kv.write(std::cout);

            const std::string& description = rec.get_task()->get_description();
            if(!description.empty())
              {
                std::cout << '\n';

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_TASK_DESCRIPTION;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
                std::cout << description << '\n';
              }

            // write table of content groups
            const std::list<std::string>& content_groups = rec.get_content_groups();

            if(!content_groups.empty())
              {
                std::cout << '\n';

                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> created;
                std::vector<std::string> updated;
                std::vector<std::string> failed;
                std::vector<std::string> size;

                integration_content_db& db = cache.get_integration_content_db();
                for(const std::string& group : content_groups)
                  {
                    integration_content_db::const_iterator t = db.find(group);
                    name.push_back(group);

                    if(t != db.end())
                      {
                        created.push_back(boost::posix_time::to_simple_string(t->second->get_creation_time()));
                        updated.push_back(boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                        failed.emplace_back(t->second->get_payload().is_failed() ? CPPTRANSPORT_REPORT_NO : CPPTRANSPORT_REPORT_YES); // label is 'complete', so yes/no other way
                        size.push_back(format_memory(t->second->get_payload().get_size()));
                      }
                    else
                      {
                        created.emplace_back("--");
                        updated.emplace_back("--");
                        failed.emplace_back("--");
                        size.emplace_back("--");
                      }
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_CREATED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_EDITED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_FAILED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_SIZE, column_justify::right);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(created);
                table.push_back(updated);
                table.push_back(failed);
                table.push_back(size);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
              }

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const postintegration_task_record<number>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_record_generic(rec);

            key_value kv(this->env, this->arg_cache);
            kv.insert_back(CPPTRANSPORT_REPORT_TASK_TYPE, task_type_to_string(rec.get_task_type()));
            kv.insert_back(CPPTRANSPORT_REPORT_PARENT_TASK, rec.get_task()->get_parent_task()->get_name());

            kv.write(std::cout);

            const std::string& description = rec.get_task()->get_description();
            if(!description.empty())
              {
                std::cout << '\n';

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_TASK_DESCRIPTION;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
                std::cout << description << '\n';
              }

            // write table of content groups
            const std::list<std::string>& content_groups = rec.get_content_groups();

            if(!content_groups.empty())
              {
                std::cout << '\n';

                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> created;
                std::vector<std::string> updated;
                std::vector<std::string> failed;
                std::vector<std::string> size;

                postintegration_content_db& db = cache.get_postintegration_content_db();
                for(const std::string& group : content_groups)
                  {
                    postintegration_content_db::const_iterator t = db.find(group);
                    name.push_back(group);

                    if(t != db.end())
                      {
                        created.push_back(boost::posix_time::to_simple_string(t->second->get_creation_time()));
                        updated.push_back(boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                        failed.emplace_back(t->second->get_payload().is_failed() ? CPPTRANSPORT_REPORT_NO : CPPTRANSPORT_REPORT_YES); // label is 'complete', so yes/no other way
                        size.push_back(format_memory(t->second->get_payload().get_size()));
                      }
                    else
                      {
                        created.emplace_back("--");
                        updated.emplace_back("--");
                        failed.emplace_back("--");
                        size.emplace_back("--");
                      }
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_CREATED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_EDITED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_FAILED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_SIZE, column_justify::right);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(created);
                table.push_back(updated);
                table.push_back(failed);
                table.push_back(size);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
              }

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const output_task_record<number>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_record_generic(rec);

            const std::string& description = rec.get_task()->get_description();
            if(!description.empty())
              {
                std::cout << '\n';

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_TASK_DESCRIPTION;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
                std::cout << description << '\n';
              }

            // write table of derived products in this output task
            const typename std::vector< output_task_element<number> >& elements = rec.get_task()->get_elements();

            if(!elements.empty())
              {
                std::cout << '\n';

                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> type;
                std::vector<std::string> tags;
                std::vector<std::string> filename;

                for(const output_task_element<number>& element : elements)
                  {
                    name.push_back(element.get_product_name());

                    // introspect derived product type
                    type.push_back(derived_data::derived_product_type_to_string(element.get_product().get_type()));

                    tags.push_back(this->compose_tag_list(element.get_tags()));
                    filename.push_back(element.get_product().get_filename().string());
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_PRODUCT_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_PRODUCT_TYPE, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_PRODUCT_TAGS, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_PRODUCT_FILENAME, column_justify::left);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(type);
                table.push_back(tags);
                table.push_back(filename);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);

                this->force_newline();
              }

            // write table of content groups
            const std::list<std::string>& content_groups = rec.get_content_groups();

            if(!content_groups.empty())
              {
                this->check_newline();

                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> created;
                std::vector<std::string> updated;

                output_content_db& db = cache.get_output_content_db();
                for(const std::string& group : content_groups)
                  {
                    output_content_db::const_iterator t = db.find(group);
                    name.push_back(group);

                    if(t != db.end())
                      {
                        created.push_back(boost::posix_time::to_simple_string(t->second->get_creation_time()));
                        updated.push_back(boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                      }
                    else
                      {
                        created.emplace_back("--");
                        updated.emplace_back("--");
                      }
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_CREATED, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_EDITED, column_justify::right);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(created);
                table.push_back(updated);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
              }

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const derived_product_record<number>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_record_generic(rec);

            key_value kv(this->env, this->arg_cache);
            kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_TYPE, derived_data::derived_product_type_to_string(rec.get_product()->get_type()));
            kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_FILENAME, rec.get_product()->get_filename().string());

            kv.write(std::cout);

            const std::string& description = rec.get_product()->get_description();
            if(!description.empty())
              {
                std::cout << '\n';

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_PRODUCT_DESCRIPTION;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
                std::cout << description << '\n';
              }

            this->force_newline();

            typename std::list< derivable_task<number>* > task_list;
            rec.get_product()->get_task_list(task_list);

            if(!task_list.empty())
              {
                this->check_newline();

                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_PRODUCT_DEPENDS_ON;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';

                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> type;
                std::vector<std::string> last_edit;
                std::vector<std::string> num_groups;

                typename task_db<number>::type& db = cache.get_task_db();

                for(derivable_task<number>* tk : task_list)
                  {
                    if(tk != nullptr)
                      {
                        typename task_db<number>::type::const_iterator t = db.find(tk->get_name());
                        name.push_back(tk->get_name());

                        if(t != db.end())
                          {
                            type.push_back(task_type_to_string(t->second->get_type()));
                            last_edit.push_back(boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                            num_groups.push_back(boost::lexical_cast<std::string>(t->second->get_content_groups().size()));
                          }
                        else
                          {
                            type.emplace_back("--");
                            last_edit.emplace_back("--");
                            num_groups.emplace_back("--");
                          }
                      }
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_TASK_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_TYPE, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_LAST_EDIT, column_justify::right);
                columns.emplace_back(CPPTRANSPORT_REPORT_STATUS_NUM_GROUPS, column_justify::right);

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(type);
                table.push_back(last_edit);
                table.push_back(num_groups);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
              }

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const content_group_record<integration_payload>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            // produce generic report on content group
            typename task_db<number>::type& db = cache.get_task_db();
            typename task_db<number>::type::const_iterator t = db.find(rec.get_task_name());

            this->report_record_title(rec);
            if(t != db.end() && t->second->get_type() == task_type::integration)
              {
                const auto& irec = dynamic_cast< const integration_task_record<number>& >(*t->second);
                this->report_output_record_generic(rec, task_type_to_string(irec.get_task_type()));
              }
            else
              {
                this->report_output_record_generic(rec, "--");
              }

            // now report on the payload
            const integration_payload& payload = rec.get_payload();

            key_value kv_payload(this->env, this->arg_cache);
            key_value kv_container(this->env, this->arg_cache);

            kv_container.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_CONTAINER, payload.get_container_path().string());
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_COMPLETE, payload.is_failed() ? CPPTRANSPORT_REPORT_NO : CPPTRANSPORT_REPORT_YES);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_WORKGROUP, boost::lexical_cast<std::string>(payload.get_workgroup_number()));
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SEEDED, payload.is_seeded() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_DATA_TYPE, payload.get_data_type());
            if(payload.is_seeded()) kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SEED_GROUP, payload.get_seed_group());
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_STATISTICS, payload.has_statistics() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_ICS, payload.has_initial_conditions() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_SPECTRAL, payload.has_spectral_data() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SIZE, format_memory(payload.get_size()));

            std::cout << '\n';
            kv_payload.set_tiling(false);
            kv_payload.write(std::cout);
            kv_container.set_tiling(false);
            kv_container.write(std::cout);

            // finally report on the metdata
            const integration_metadata& metadata = payload.get_metadata();

            key_value kv_metadata(this->env, this->arg_cache);
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_TOTAL_WALLCLOCK, format_time(metadata.total_wallclock_time));
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_TOTAL_TIME, format_time(metadata.total_integration_time));
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_GLOBAL_MIN, format_time(metadata.global_min_integration_time));
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_GLOBAL_MAX, format_time(metadata.global_max_integration_time));
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_CONFIGURATIONS, boost::lexical_cast<std::string>(metadata.total_configurations));
            kv_metadata.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_FAILURES, boost::lexical_cast<std::string>(metadata.total_failures));

            std::cout << '\n';
            kv_metadata.set_title(CPPTRANSPORT_REPORT_PAYLOAD_METADATA);
            kv_metadata.set_tiling(false);
            kv_metadata.write(std::cout);

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const content_group_record<postintegration_payload>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            // produce generic report on content group
            typename task_db<number>::type& db = cache.get_task_db();
            typename task_db<number>::type::const_iterator t = db.find(rec.get_task_name());

            this->report_record_title(rec);
            if(t != db.end() && t->second->get_type() == task_type::postintegration)
              {
                const auto& irec = dynamic_cast< const postintegration_task_record<number>& >(*t->second);
                this->report_output_record_generic(rec, task_type_to_string(irec.get_task_type()));
              }
            else
              {
                this->report_output_record_generic(rec, "--");
              }

            // now report on the payload
            const postintegration_payload& payload = rec.get_payload();

            key_value kv_payload(this->env, this->arg_cache);
            key_value kv_container(this->env, this->arg_cache);

            kv_container.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_CONTAINER, payload.get_container_path().string());
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_COMPLETE, payload.is_failed() ? CPPTRANSPORT_REPORT_NO : CPPTRANSPORT_REPORT_YES);
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_PARENT, payload.get_parent_group());
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SEEDED, payload.is_seeded() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            if(payload.is_seeded()) kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SEED_GROUP, payload.get_seed_group());
            kv_payload.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_SIZE, format_memory(payload.get_size()));

            std::cout << '\n';
            kv_payload.set_tiling(false);
            kv_payload.write(std::cout);
            kv_container.set_tiling(false);
            kv_container.write(std::cout);

            const precomputed_products& products = payload.get_precomputed_products();

            key_value kv_products(this->env, this->arg_cache);

            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_TWOPF, products.get_zeta_twopf() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_TWOPF_SPECTRAL, products.get_zeta_twopf_spectral() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_THREEPF, products.get_zeta_threepf() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_ZETA_REDBSP, products.get_zeta_redbsp() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_LOCAL, products.get_fNL_local() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_EQUILATERAL, products.get_fNL_equi() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_ORTHOGONAL, products.get_fNL_ortho() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv_products.insert_back(CPPTRANSPORT_REPORT_PAYLOAD_HAS_FNL_DBI, products.get_fNL_DBI() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);

            std::cout << '\n';
            kv_products.set_title(CPPTRANSPORT_REPORT_PAYLOAD_PRECOMPUTED);
            kv_products.set_tiling(false);
            kv_products.write(std::cout);

            this->force_newline();
          }


        template <typename number>
        void command_line::report_object(const content_group_record<output_payload>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_output_record_generic(rec);

            const output_payload& payload = rec.get_payload();
            const std::list<std::string>& groups = payload.get_content_groups_summary();

            if(!groups.empty())
              {
                std::vector<column_descriptor> columns;
                std::vector<std::string> name;
                std::vector<std::string> task;
                std::vector<std::string> type;
                std::vector<std::string> last_edit;

                for(const std::string& group : groups)
                  {
                    name.push_back(group);

                    content_group_data<number> data(group, cache);

                    task.push_back(data.get_task());
                    type.push_back(data.get_type());
                    last_edit.push_back(data.get_last_edit());
                  }

                columns.emplace_back(CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK_TYPE, column_justify::left);
                columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_EDITED, column_justify::right);

                std::cout << '\n';
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                std::cout << CPPTRANSPORT_REPORT_PAYLOAD_CONTENT_GROUPS;
                if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                std::cout << '\n';

                std::vector< std::vector<std::string> > table;
                table.push_back(name);
                table.push_back(task);
                table.push_back(type);
                table.push_back(last_edit);

                asciitable at(std::cout, this->env, this->arg_cache);
                at.set_terminal_output(true);
                at.write(columns, table);
              }

            this->force_newline();
          }


        void command_line::report_record_generic(const repository_record& rec)
          {
            key_value kv(this->env, this->arg_cache);

            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            kv.insert_back(CPPTRANSPORT_REPORT_CREATION_DATE, boost::posix_time::to_simple_string(created));
            kv.insert_back(CPPTRANSPORT_REPORT_LAST_EDIT_DATE, boost::posix_time::to_simple_string(edited));
            kv.insert_back(CPPTRANSPORT_REPORT_API_VERSION, format_version(rec.get_runtime_API_version()));

            kv.set_tiling(true);
            kv.write(std::cout);
          }


        template <typename Payload>
        void command_line::report_output_record_generic(const content_group_record<Payload>& rec, const std::string& type)
          {
            key_value kv(this->env, this->arg_cache);

            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            kv.insert_back(CPPTRANSPORT_REPORT_CREATION_DATE, boost::posix_time::to_simple_string(created));
            kv.insert_back(CPPTRANSPORT_REPORT_LAST_EDIT_DATE, boost::posix_time::to_simple_string(edited));
            kv.insert_back(CPPTRANSPORT_REPORT_API_VERSION, format_version(rec.get_runtime_API_version()));
            kv.insert_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK, rec.get_task_name());
            if(!type.empty()) kv.insert_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK_TYPE, type);
            kv.insert_back(CPPTRANSPORT_REPORT_OUTPUT_LOCKED, rec.get_lock_status() ? CPPTRANSPORT_REPORT_YES : CPPTRANSPORT_REPORT_NO);
            kv.insert_back(CPPTRANSPORT_REPORT_OUTPUT_TAGS, this->compose_tag_list(rec.get_tags()));

            kv.set_tiling(true);
            kv.write(std::cout);

            const std::list<note>& notes = rec.get_notes();
            if(!notes.empty())
              {
                kv.reset();

                unsigned int serial = 0;
                for(const note& it : notes)
                  {
                    std::ostringstream msg;
                    std::string uid = it.get_uid();
                    const std::string time = boost::posix_time::to_simple_string(it.get_timestamp());
                    msg << (!uid.empty() ? uid.append(" ").append(time).append(": ") : "") << it.get_note();
                    kv.insert_back(boost::lexical_cast<std::string>(serial), msg.str());
                    ++serial;
                  }

                std::cout << '\n';
                kv.set_title(CPPTRANSPORT_REPORT_OUTPUT_NOTES);
                kv.set_tiling(false);
                kv.write(std::cout);
              }
          }


        std::string command_line::compose_tag_list(const std::list<std::string>& tag_list)
          {
            // compose tags into a single string
            std::ostringstream composed_list;
            unsigned int tag_count = 0;

            for(const std::string& tag : tag_list)
              {
                if(tag_count > 0) composed_list << ", ";
                composed_list << tag;
                ++tag_count;
                if(composed_list.str().length() > CPPTRANSPORT_REPORT_MAX_TAG_LENGTH) break;
              }
            if(tag_count < tag_list.size()) composed_list << ", ...";

            if(composed_list.str().length() > 0) return composed_list.str();
            else                                 return std::string("--");
          }


        template <typename number>
        void command_line::report_provenance(repository_cache<number>& cache)
          {
            // make a local copy of items; we remove items from this list as they are processed
            std::list<std::string> items;
            std::copy(this->provenance_items.begin(), this->provenance_items.end(), std::back_inserter(items));

            output_content_db& db = cache.get_output_content_db();

            for(auto item_t = items.begin(); item_t != items.end(); /* intentionally no update statement */ )
              {
                output_content_db::const_iterator t = db.find(*item_t);
                if(t != db.end())
                  {
                    this->report_provenance(*t->second, cache);
                    items.erase(item_t++);
                  }
                else
                  {
                    ++item_t;
                  }
              }

            for(const std::string& item : items)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPORT_PROVENANCE_FAILURE << " '" << item << "'";
                this->warn(msg.str());
              }
          }


        template <typename number>
        void command_line::report_provenance(const content_group_record<output_payload>& rec, repository_cache<number>& cache)
          {
            this->check_newline();

            this->report_record_title(rec);
            this->report_output_record_generic(rec);

            this->force_newline();

            const output_payload& payload = rec.get_payload();
            const std::list<derived_content>& content = payload.get_derived_content();

            asciitable at(std::cout, this->env, this->arg_cache);
            key_value kv(this->env, this->arg_cache);
            at.set_terminal_output(true);

            typename derived_product_db<number>::type& db = cache.get_derived_product_db();

            if(!content.empty())
              {
                for(const derived_content& item : content)
                  {
                    this->check_newline();

                    if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold_green);
                    std::cout << item.get_parent_product();
                    if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                    std::cout << '\n';

                    kv.reset();
                    kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_FILENAME, item.get_filename().string());

                    typename derived_product_db<number>::type::const_iterator t = db.find(item.get_parent_product());
                    if(t != db.end()) kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_TYPE, derived_data::derived_product_type_to_string(t->second->get_product()->get_type()));
                    else              kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_TYPE, "--");

                    kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_CREATED, boost::posix_time::to_simple_string(item.get_creation_time()));
                    kv.insert_back(CPPTRANSPORT_REPORT_PRODUCT_TAG_SET, this->compose_tag_list(item.get_tags()));

                    kv.set_tiling(true);
                    kv.write(std::cout);

                    const std::list<note>& notes = item.get_notes();
                    if(!notes.empty())
                      {
                        kv.reset();

                        unsigned int serial = 0;
                        for(const note& it : notes)
                          {
                            std::ostringstream msg;
                            std::string uid = it.get_uid();
                            const std::string time = boost::posix_time::to_simple_string(it.get_timestamp());
                            msg << (!uid.empty() ? uid.append(" ").append(time).append(": ") : "") << it.get_note();
                            kv.insert_back(boost::lexical_cast<std::string>(serial), msg.str());
                            ++serial;
                          }

                        std::cout << '\n';
                        kv.set_title(CPPTRANSPORT_REPORT_OUTPUT_NOTES);
                        kv.set_tiling(false);
                        kv.write(std::cout);
                      }

                    const std::list<std::string>& groups = item.get_content_groups();
                    if(!groups.empty())
                      {
                        std::vector<column_descriptor> columns;
                        std::vector<std::string> name;
                        std::vector<std::string> task;
                        std::vector<std::string> type;
                        std::vector<std::string> last_edit;

                        for(const std::string& group : groups)
                          {
                            name.push_back(group);

                            content_group_data<number> data(group, cache);

                            task.push_back(data.get_task());
                            type.push_back(data.get_type());
                            last_edit.push_back(data.get_last_edit());
                          }

                        columns.emplace_back(CPPTRANSPORT_REPORT_CONTENT_GROUP_NAME, column_justify::left);
                        columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK, column_justify::left);
                        columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_PARENT_TASK_TYPE, column_justify::left);
                        columns.emplace_back(CPPTRANSPORT_REPORT_OUTPUT_EDITED, column_justify::right);

                        std::cout << '\n';
                        if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::bold);
                        std::cout << CPPTRANSPORT_REPORT_PAYLOAD_CONTENT_GROUPS;
                        if(this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output()) std::cout << ColourCode(ANSI_colour::normal);
                        std::cout << '\n';

                        std::vector< std::vector<std::string> > table;
                        table.push_back(name);
                        table.push_back(task);
                        table.push_back(type);
                        table.push_back(last_edit);

                        at.write(columns, table);
                      }

                    this->force_newline();
                  }
              }

            this->force_newline();
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_COMMAND_LINE_H
