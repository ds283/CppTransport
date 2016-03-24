//
// Created by David Seery on 23/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPORTING_HTML_REPORT_H
#define CPPTRANSPORT_REPORTING_HTML_REPORT_H


#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/reporting/repository_cache.h"
#include "transport-runtime-api/reporting/content_group_data.h"
#include "transport-runtime-api/reporting/unique_tags.h"

#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/message_handlers.h"

#include "transport-runtime-api/utilities/formatter.h"


namespace transport
  {

    namespace reporting
      {

        constexpr auto CPPTRANSPORT_HTML_INDEX = "index.html";
        constexpr auto CPPTRANSPORT_HTML_STYLESHEET = "cpptransport.css";
        constexpr auto CPPTRANSPORT_HTML_JAVASCRIPT = "script.js";

        constexpr auto CPPTRANSPORT_HTML_BUTTON_TAG_PREFIX = "button";

        class HTML_report
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            HTML_report(local_environment& e, argument_cache& c,
                        error_handler eh, warning_handler wh, message_handler mh)
              : env(e),
                arg_cache(c),
                err(eh),
                warn(wh),
                msg(mh),
                button_id(0)
              {
              }

            //! destructor is default
            ~HTML_report() = default;


            // INTERFACE

          public:

            //! set root directory
            void set_root(boost::filesystem::path p);

            //! produce report
            template <typename number>
            void report(repository<number>& repo);


            // INTERNAL API

          protected:

            //! write cpptransport.css stylesheet
            void write_stylesheet();

            //! write index.html file
            template <typename number>
            void write_index(repository_cache<number>& cache, unique_tags<number>& tags);

            //! write HTML report
            template <typename number>
            void write_report(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write package tab
            template <typename number>
            void write_packages(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write integration tasks tab
            template <typename number>
            void write_integration(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write postintegration tasks tab
            template <typename number>
            void write_postintegration(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write output tasks tab
            template <typename number>
            void write_output(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write derived products tab
            template <typename number>
            void write_derived_products(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write integration content tabs
            template <typename number>
            void write_integration_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write postintegration content tabs
            template <typename number>
            void write_postintegration_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write output content tabs
            template <typename number>
            void write_output_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags);


            // UTILITY WRITE FUNCTIONS

          protected:

            //! write details for generic records
            template <typename RecordType>
            void write_generic_record(std::ofstream& out, std::ofstream& out_js, const RecordType& rec);

            //! compose a tag list into a set of HTML labels
            std::string compose_tag_list(const std::list<std::string>& tags);

            //! write details for generic output records
            template <typename number, typename Payload>
            void write_generic_output_record(std::ofstream& out, std::ofstream& out_js, const output_group_record<Payload>& rec,
                                             repository_cache<number>& cache, unique_tags<number>& tags);

            //! write a link-to-task button
            template <typename number>
            void write_task_button(std::ofstream& out, std::ofstream& out_js, const std::string& name,
                                   repository_cache<number>& cache, unique_tags<number>& tags);

            //! write a seeding label
            template <typename number, typename Payload>
            void write_seeded(std::ofstream& out, std::ofstream& out_js, const Payload& payload,
                              repository_cache<number>& cache, unique_tags<number>& tags);

            //! write a link-to-group button
            template <typename number>
            void write_content_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write a link-to-group button
            void write_content_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, const std::string& tag);

            //! write a link-to-package button
            template <typename number>
            void write_package_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, repository_cache<number>& cache, unique_tags<number>& tags);

            //! write a link-to-package button
            void write_package_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, const std::string& tag);

            //! write a collapsible notes section
            void write_notes_collapsible(std::ofstream& out, std::ofstream& out_js, const std::list<std::string>& notes, const std::string& tag);


            // MISCELLANEOUS

          protected:

            //! make a button tag
            std::string make_button_tag();


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


            // REPORT DATA

            //! root directory
            boost::filesystem::path root;

            //! current unique button id
            unsigned int button_id;

          };


        void HTML_report::set_root(boost::filesystem::path p)
          {
            if(!p.is_absolute())
              {
                // make relative to current working directory
                p = boost::filesystem::absolute(p);
              }

            if(boost::filesystem::exists(p))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_ROOT_EXISTS_A << " '" << p.string() << "' " << CPPTRANSPORT_HTML_ROOT_EXISTS_B;
                this->err(msg.str());
                this->root.clear();
              }

            this->root = p;
          }


        template <typename number>
        void HTML_report::report(repository <number>& repo)
          {
            if(this->root.empty()) return;

            try
              {
                boost::filesystem::create_directories(this->root);
              }
            catch(boost::filesystem::filesystem_error &xe)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_CREATE_FAILURE << " '" << this->root.string() << "'";
                this->err(msg.str());
                return;
              }

            // reset button id
            this->button_id = 0;

            repository_cache<number> cache(repo);
            unique_tags<number> tags;

            // write main index.html
            this->write_index(cache, tags);

            // write CSS stylesheet
            this->write_stylesheet();
          }


        void HTML_report::write_stylesheet()
          {
            boost::filesystem::path out_path = this->root / CPPTRANSPORT_HTML_STYLESHEET;

            std::ofstream out(out_path.string(), std::ios::out | std::ios::trunc);

            out << ".topskip {" << '\n';
            out << "    margin-top: 20px;" << '\n';
            out << "}" << '\n';
            out << ".imgproduct {" << '\n';
            out << "    margin: 15px;" << '\n';
            out << "    border: 2px solid #ccc;" << '\n';
            out << "}" << '\n';

            out.close();
          }


        template <typename number>
        void HTML_report::write_index(repository_cache<number>& cache, unique_tags<number>& tags)
          {
            boost::filesystem::path out_path = this->root / CPPTRANSPORT_HTML_INDEX;
            boost::filesystem::path out_js_path = this->root / CPPTRANSPORT_HTML_JAVASCRIPT;

            std::ofstream out(out_path.string(), std::ios::out | std::ios::trunc);
            std::ofstream out_js(out_js_path.string(), std::ios::out | std::ios::trunc);

            out << "<!DOCTYPE html>" << '\n';
            out << "<html>" << '\n';
            out << "<head>" << '\n';
            out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"./cpptransport.css\">" << '\n';
            out << "</head>" << '\n';
            out << "<body>" << '\n';

            out << "<title>CppTransport report</title>" << '\n';
            out << "<meta charset=\"utf-8\">" << '\n';
            out << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << '\n';

            out << "<div class =\"container-fluid\">" << '\n';

            this->write_report(out, out_js, cache, tags);

            out << "</div>" << '\n';

            // jQuery must be available before Bootstrap is loaded
            out << "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.1.4/jquery.min.js\"></script>" << '\n';
            out << "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\" type=\"text/css\">" << '\n';
            out << "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js\"></script>" << '\n';
            out << "<script src=\"" << CPPTRANSPORT_HTML_JAVASCRIPT << "\"></script>" << '\n';

            out << "</body>" << '\n';
            out << "</html>" << '\n';

            out.close();
            out_js.close();
          }


        template <typename number>
        void HTML_report::write_report(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            // WRITE NAVIGATION BAR

            out << "<ul id=\"tabs\", class=\"nav nav-pills nav-justified\", role=\"tablist\">" << '\n';
            out << "<li class=\"active\"><a data-toggle=\"pill\" href=\"#packages\">Packages</a></li>" << '\n';
            out << "<li><a data-toggle=\"pill\" href=\"#integration\">Integration tasks</a></li>" << '\n';

            // Write integration content menu
            out << "<li class=\"dropdown\"><a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\">Integration content <span class=\"caret\"></span></a>" << '\n';
            out << "<ul class=\"dropdown-menu scrollable-menu\" role=\"menu\">" << '\n';

            integration_content_db& int_db = cache.get_integration_content_db();
            for(const integration_content_db::value_type& group : int_db)
              {
                const output_group_record<integration_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<li><a href=\"#" << tag << "\" data-toggle=\"tab\">" << rec.get_name() << "</a></li>" << '\n';
              }

            out << "</ul>" << '\n';
            out <<"</li>" << '\n';

            out << "<li><a data-toggle=\"pill\" href=\"#postintegration\">Postintegration tasks</a></li>" << '\n';

            // Write postintegration content menu
            out << "<li class=\"dropdown\"><a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\">Postintegration content <span class=\"caret\"></span></a>" << '\n';
            out << "<ul class=\"dropdown-menu scrollable-menu\" role=\"menu\">" << '\n';

            postintegration_content_db& pint_db = cache.get_postintegration_content_db();
            for(const postintegration_content_db::value_type& group : pint_db)
              {
                const output_group_record<postintegration_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<li><a href=\"#" << tag << "\" data-toggle=\"tab\">" << rec.get_name() << "</a></li>" << '\n';
              }

            out << "</ul>" << '\n';
            out <<"</li>" << '\n';

            out << "<li><a data-toggle=\"pill\" href=\"#output\">Output tasks</a></li>" << '\n';

            // Write output content menu
            out << "<li class=\"dropdown\"><a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\">Output content <span class=\"caret\"></span></a>" << '\n';
            out << "<ul class=\"dropdown-menu scrollable-menu\" role=\"menu\">" << '\n';

            output_content_db& out_db = cache.get_output_content_db();
            for(const output_content_db::value_type& group : out_db)
              {
                const output_group_record<output_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<li><a href=\"#" << tag << "\" data-toggle=\"tab\">" << rec.get_name() << "</a></li>" << '\n';
              }

            out << "</ul>" << '\n';
            out <<"</li>" << '\n';

            out << "<li><a data-toggle=\"pill\" href=\"#derived\">Derived products</a></li>" << '\n';
            out << "</ul>" << '\n';

            out << "<br>" << '\n';


            // WRITE PANE CONTENT

            out << "<div class=\"tab-content\">" << '\n';

            out << "<div id=\"packages\" class=\"tab-pane fade in active\">" << '\n';
            this->write_packages(out, out_js, cache, tags);
            out << "</div>" << '\n';

            out << "<div id=\"integration\" class=\"tab-pane fade\">" << '\n';
            this->write_integration(out, out_js, cache, tags);
            out << "</div>" << '\n';

            this->write_integration_content(out, out_js, cache, tags);

            out << "<div id=\"postintegration\" class=\"tab-pane fade\">" << '\n';
            this->write_postintegration(out, out_js, cache, tags);
            out << "</div>" << '\n';

            this->write_postintegration_content(out, out_js, cache, tags);

            out << "<div id=\"output\" class=\"tab-pane fade\">" << '\n';
            this->write_output(out, out_js, cache, tags);
            out << "</div>" << '\n';

            this->write_output_content(out, out_js, cache, tags);

            out << "<div id=\"derived\" class=\"tab-pane fade\">" << '\n';
            this->write_derived_products(out, out_js, cache, tags);
            out << "</div>" << '\n';

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_packages(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename package_db<number>::type& db = cache.get_package_db();

            out << "<div class=\"list-group\">" << '\n';

            // step through all packages, writing them out as list elements elements
            for(const typename package_db<number>::value_type& pkg : db)
              {
                const package_record<number>& rec = *pkg.second;

                out << "<a href=\"#" << tags.get_id(rec) << "\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                this->write_generic_record(out, out_js, rec);

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Model <span class=\"label label-default\">" << rec.get_ics().get_model()->get_name() << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Authors <span class=\"label label-default\">" << rec.get_ics().get_model()->get_author() << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Tagline <span class=\"label label-default\">" << rec.get_ics().get_model()->get_tag() << "</span></div>" << '\n';
                out << "</div>" << '\n';
                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Initial time <span class=\"label label-default\">" << boost::lexical_cast<std::string>(rec.get_ics().get_N_initial()) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Horizon-crossing time <span class=\"label label-default\">" << boost::lexical_cast<std::string>(rec.get_ics().get_N_horizon_crossing()) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\"></div>" << '\n';
                out << "</div>" << '\n';

                const parameters<number>& params = rec.get_ics().get_params();
                const std::vector<number>& param_vec = params.get_vector();
                const std::vector<std::string>& param_names = rec.get_ics().get_model()->get_param_names();

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-6\">" << '\n';

                out << "<h4 class=\"list-group-item-text topskip\">Parameter values</h4>" << '\n';
                out << "<table class=\"table table-striped\">" << '\n';
                out << "<tbody>" << '\n';

                out << "<tr>" << '\n';
                out << "<td><b>Planck mass</b></td>" << '\n';
                out << "<td>" << boost::lexical_cast<std::string>(params.get_Mp()) << "</td>" << '\n';
                out << "</tr>" << '\n';

                for(unsigned int i = 0; i < param_vec.size() && i < param_names.size(); ++i)
                  {
                    out << "<tr>" << '\n';
                    out << "<td><b>" << param_names[i] << "</b></td>" << '\n';
                    out << "<td>" << boost::lexical_cast<std::string>(param_vec[i]) << "</td>" << '\n';
                    out << "</tr>" << '\n';
                  }

                out << "</tbody>" << '\n';
                out << "</table>" << '\n';

                out << "</div>" << '\n';

                const std::vector<number>& ics_vec = rec.get_ics().get_vector();
                const std::vector<std::string>& coord_names = rec.get_ics().get_model()->get_state_names();

                out << "<div class=\"col-md-6\">" << '\n';

                out << "<h4 class=\"list-group-item-text topskip\">Initial conditions</h4>" << '\n';
                out << "<table class=\"table table-striped\">" << '\n';

                out << "<tbody>" << '\n';

                for(unsigned int i = 0; i < ics_vec.size() && i < coord_names.size(); ++i)
                  {
                    out << "<tr>" << '\n';
                    out << "<td><b>" << coord_names[i] << "</b></td>" << '\n';
                    out << "<td>" << boost::lexical_cast<std::string>(ics_vec[i]) << "</td>" << '\n';
                    out << "</tr>" << '\n';
                  }

                out << "</tbody>" << '\n';
                out << "</table>" << '\n';

                out << "</div>" << '\n';
                out << "</div>" << '\n';

                out << "</a>" << '\n';
              }

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_integration(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename task_db<number>::type& db = cache.get_task_db();

            out << "<div class=\"list-group\">" << '\n';

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::integration)
                  {
                    const integration_task_record<number>& rec = dynamic_cast< const integration_task_record<number>& >(pre_rec);
                    const std::string tag = tags.get_id(rec);

                    out << "<a href=\"#" << tag << "\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                    out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                    this->write_generic_record(out, out_js, rec);

                    out << "<div class=\"row\">" << '\n';
                    out << "<div class=\"col-md-4\">Task type <span class=\"label label-default\">" << task_type_to_string(rec.get_task_type()) << "</span></div>" << '\n';
                    out << "<div class=\"col-md-4\">Uses package ";
                    this->write_package_button(out, out_js, rec.get_task()->get_ics().get_name(), cache, tags);
                    out << "</div>" << '\n';
                    out << "<div class=\"col-md-4\">k-config database <span class=\"label label-default\">" << rec.get_relative_kconfig_database_path().string() << "</span></div>" << '\n';
                    out << "</div>" << '\n';

                    // write table of output groups
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        out << "<button data-toggle=\"collapse\" data-target=\"#" << tag << "groups\" class=\"button btn-info button-lg topskip\">Output groups <span class=\"badge\">" << output_groups.size() << "</span></button>" << '\n';
                        out << "<div id=\"" << tag << "groups\" class=\"collapse\">" << '\n';
                        out << "<table class=\"table table-striped\">" << '\n';

                        out << "<thead>" << '\n';
                        out << "<tr>" << '\n';
                        out << "<th>Name</th>" << '\n';
                        out << "<th>Created</th>" << '\n';
                        out << "<th>Last updated</th>" << '\n';
                        out << "<th>Complete</th>" << '\n';
                        out << "<th>Size</th>" << '\n';
                        out << "</tr>" << '\n';
                        out << "</thead>" << '\n';

                        out << "<tbody>" << '\n';

                        integration_content_db& content_db = cache.get_integration_content_db();
                        for(const std::string& group : output_groups)
                          {
                            integration_content_db::const_iterator t = content_db.find(group);
                            out << "<tr>" << '\n';

                            if(t != content_db.end())
                              {
                                out << "<td>";
                                this->write_content_button(out, out_js, group, tags.get_id(*t->second));
                                out << "</td>" << '\n';
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_creation_time()) << "</td>";
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_last_edit_time()) << "</td>";
                                out << "<td>" << (t->second->get_payload().is_failed() ? "No" : "Yes") << "</td>";
                                out << "<td>" << format_memory(t->second->get_payload().get_size()) << "</td>";
                              }
                            else
                              {
                                out << "<td><span class=\"label label-warning\">" << group << "</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                              }

                            out << "</tr>" << '\n';
                          }

                        out << "</tbody>" << '\n';
                        out << "</table>" << '\n';
                        out << "</div>" << '\n';
                      }

                    out << "</a>" << '\n';
                  }
              }

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_postintegration(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename task_db<number>::type& db = cache.get_task_db();

            out << "<div class=\"list-group\">" << '\n';

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::postintegration)
                  {
                    const postintegration_task_record<number>& rec = dynamic_cast< const postintegration_task_record<number>& >(pre_rec);
                    const std::string tag = tags.get_id(rec);

                    out << "<a href=\"#" << tag << "\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                    out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                    this->write_generic_record(out, out_js, rec);

                    out << "<div class=\"row\">" << '\n';
                    out << "<div class=\"col-md-4\">Task type <span class=\"label label-default\">" << task_type_to_string(rec.get_task_type()) << "</span></div>" << '\n';
                    out << "<div class=\"col-md-4\">Parent task ";
                    this->write_task_button(out, out_js, rec.get_task()->get_parent_task()->get_name(), cache, tags);
                    out << "</div>" << '\n';
                    out << "<div class=\"col-md-4\"></div>" << '\n';
                    out << "</div>" << '\n';

                    // write table of output groups
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        out << "<button data-toggle=\"collapse\" data-target=\"#" << tag << "groups\" class=\"button btn-info button-lg topskip\">Output groups <span class=\"badge\">" << output_groups.size() << "</span></button>" << '\n';
                        out << "<div id=\"" << tag << "groups\" class=\"collapse\">" << '\n';
                        out << "<table class=\"table table-striped\">" << '\n';

                        out << "<thead>" << '\n';
                        out << "<tr>" << '\n';
                        out << "<th>Name</th>" << '\n';
                        out << "<th>Created</th>" << '\n';
                        out << "<th>Last updated</th>" << '\n';
                        out << "<th>Complete</th>" << '\n';
                        out << "<th>Size</th>" << '\n';
                        out << "</tr>" << '\n';
                        out << "</thead>" << '\n';

                        out << "<tbody>" << '\n';

                        postintegration_content_db& content_db = cache.get_postintegration_content_db();
                        for(const std::string& group : output_groups)
                          {
                            postintegration_content_db::const_iterator t = content_db.find(group);
                            out << "<tr>" << '\n';

                            if(t != content_db.end())
                              {
                                out << "<td>";
                                this->write_content_button(out, out_js, group, tags.get_id(*t->second));
                                out << "</td>" << '\n';
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_creation_time()) << "</td>";
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_last_edit_time()) << "</td>";
                                out << "<td>" << (t->second->get_payload().is_failed() ? "No" : "Yes") << "</td>";
                                out << "<td>" << format_memory(t->second->get_payload().get_size()) << "</td>";
                              }
                            else
                              {
                                out << "<td><span class=\"label label-warning\">" << group << "</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                              }

                            out << "</tr>" << '\n';
                          }

                        out << "</tbody>" << '\n';
                        out << "</table>" << '\n';
                        out << "</div>" << '\n';
                      }

                    out << "</a>" << '\n';
                  }
              }

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_output(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename task_db<number>::type& db = cache.get_task_db();

            out << "<div class=\"list-group\">" << '\n';

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::output)
                  {
                    const output_task_record<number>& rec = dynamic_cast< const output_task_record<number>& >(pre_rec);
                    const std::string tag = tags.get_id(rec);

                    out << "<a href=\"#" << tag << "\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                    out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                    this->write_generic_record(out, out_js, rec);

                    // write elements
                    const typename std::vector< output_task_element<number> >& elements = rec.get_task()->get_elements();
                    if(!elements.empty())
                      {
                        out << "<button data-toggle=\"collapse\" data-target=\"#" << tag << "elements\" class=\"button btn-info button-lg topskip\">Derived products <span class=\"badge\">" << elements.size() << "</span></button>" << '\n';
                        out << "<div id=\"" << tag << "elements\" class=\"collapse\">" << '\n';
                        out << "<table class=\"table table-striped\">" << '\n';

                        out << "<thead>" << '\n';
                        out << "<tr>" << '\n';
                        out << "<th>Name</th>" << '\n';
                        out << "<th>Type</th>" << '\n';
                        out << "<th>Requires tags</th>" << '\n';
                        out << "<th>Filename</th>" << '\n';
                        out << "</tr>" << '\n';
                        out << "</thead>" << '\n';

                        out << "<tbody>" << '\n';

                        for(const output_task_element<number>& element : elements)
                          {
                            out << "<tr>" << '\n';

                            out << "<td>" << element.get_product_name() << "</td>" << '\n';
                            out << "<td>" << derived_data::derived_product_type_to_string(element.get_product().get_type()) << "</td>" << '\n';
                            out << "<td>" << this->compose_tag_list(element.get_tags()) << "</td>" << '\n';
                            out << "<td>" << element.get_product().get_filename().string() << "</td>" << '\n';

                            out << "</tr>" << '\n';
                          }

                        out << "</tbody>" << '\n';
                        out << "</table>" << '\n';
                        out << "</div>" << '\n';
                      }

                    // write table of output groups
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        out << "<button data-toggle=\"collapse\" data-target=\"#" << tag << "groups\" class=\"button btn-info button-lg topskip\">Output groups <span class=\"badge\">" << output_groups.size() << "</span></button>" << '\n';
                        out << "<div id=\"" << tag << "groups\" class=\"collapse\">" << '\n';
                        out << "<table class=\"table table-striped\">" << '\n';

                        out << "<thead>" << '\n';
                        out << "<tr>" << '\n';
                        out << "<th>Name</th>" << '\n';
                        out << "<th>Created</th>" << '\n';
                        out << "<th>Last updated</th>" << '\n';
                        out << "</tr>" << '\n';
                        out << "</thead>" << '\n';

                        out << "<tbody>" << '\n';

                        output_content_db& content_db = cache.get_output_content_db();
                        for(const std::string& group : output_groups)
                          {
                            output_content_db::const_iterator t = content_db.find(group);
                            out << "<tr>" << '\n';

                            if(t != content_db.end())
                              {
                                out << "<td>";
                                this->write_content_button(out, out_js, group, tags.get_id(*t->second));
                                out << "</td>" << '\n';
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_creation_time()) << "</td>";
                                out << "<td>" << boost::posix_time::to_simple_string(t->second->get_last_edit_time()) << "</td>";
                              }
                            else
                              {
                                out << "<td><span class=\"label label-warning\">" << group << "</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                                out << "<td><span class=\"label label-warning\">NULL</span></td>" << '\n';
                              }

                            out << "</tr>" << '\n';
                          }

                        out << "</tbody>" << '\n';
                        out << "</table>" << '\n';
                        out << "</div>" << '\n';
                      }

                    out << "</a>" << '\n';
                  }
              }

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_derived_products(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache,
                                                 unique_tags<number>& tags)
          {
            typename derived_product_db<number>::type& db = cache.get_derived_product_db();

            out << "<div class=\"list-group\">" << '\n';

            for(const typename derived_product_db<number>::value_type& product : db)
              {
                const derived_product_record<number>& rec = *product.second;
                const std::string tag = tags.get_id(rec);

                out << "<a href=\"#" << tag << "\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                this->write_generic_record(out, out_js, rec);

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Product type <span class=\"label label-default\">" << derived_data::derived_product_type_to_string(rec.get_product()->get_type()) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Filename <span class=\"label label-default\">" << rec.get_product()->get_filename().string() << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\"></div>" << '\n';
                out << "</div>" << '\n';

                typename std::vector< derivable_task<number>* > task_list;
                rec.get_product()->get_task_list(task_list);

                if(!task_list.empty())
                  {
                    out << "<h4 class=\"list-group-item-text topskip\">Depends on tasks</h4>" << '\n';

                    unsigned int count = 0;
                    for(derivable_task<number>* tk : task_list)
                      {
                        if(count == 0) out << "<div class=\"row\">" << '\n';

                        out << "<div class=\"col-md-4\">";
                        if(tk != nullptr)
                          {
                            this->write_task_button(out, out_js, tk->get_name(), cache, tags);
                          }
                        else
                          {
                            out << "<span label=\"label label-danger\">ERROR</span>";
                          }
                        out << "</div>" << '\n';

                        ++count;
                        if(count >= 3)
                          {
                            out << "</div>" << '\n';
                            count = 0;
                          }
                      }

                    if(count != 0)
                      {
                        while(count < 3)
                          {
                            out << "<div class=\"col-md-4\"></div>";
                            ++count;
                          }
                        out << "</div>" << '\n';
                      }
                  }

                out << "</a>" << '\n';
              }

            out << "</div>" << '\n';
          }


        template <typename number>
        void HTML_report::write_task_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename task_db<number>::type& tk_db = cache.get_task_db();
            typename task_db<number>::type::const_iterator t = tk_db.find(name);

            if(t != tk_db.end())
              {
                // find task tag
                std::string tag = tags.get_id(*t->second);
                std::string button_tag = this->make_button_tag();

                out << "<button class=\"button btn-link\", id=\"" << button_tag << "\">" << name << " <span class=\"badge\">" << t->second->get_output_groups().size() << "</span></button>";

                std::string pane;
                switch(t->second->get_type())
                  {
                    case task_type::integration:
                      {
                        pane = "integration";
                        break;
                      }

                    case task_type::postintegration:
                      {
                        pane = "postintegration";
                        break;
                      }

                    case task_type::output:
                      {
                        pane = "output";
                        break;
                      }
                  }

                // we need a Javascript function which causes this button to activate the appropriate task pane when clicked
                out_js << "$(function(){" << '\n';
                out_js << "  $('#" << button_tag << "').click(function(e){" << '\n';
                out_js << "    e.preventDefault();" << '\n';
                out_js << "      $('#tabs a[href=\"#" << pane << "\"]').tab('show');" << '\n';
                out_js << "      var elem = $(\"a[id='" << tag << "']\");" << '\n';
                out_js << "      $('html, body').animate({ scrollTop: elem.offset().top }, 1000);" << '\n';
                out_js << "  })" << '\n';
                out_js << "})" << '\n';
              }
            else
              {
                out << "<span label=\"label label-danger\">" << name << "</span>";
              }
          }


        template <typename RecordType>
        void HTML_report::write_generic_record(std::ofstream& out, std::ofstream& out_js, const RecordType& rec)
          {
            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            out << "<div class=\"row\">" << '\n';
            out << "<div class=\"col-md-4\">Created <span class=\"label label-primary\">" << boost::posix_time::to_simple_string(created) << "</span></div>" << '\n';
            out << "<div class=\"col-md-4\">Last update <span class=\"label label-primary\">" << boost::posix_time::to_simple_string(edited) << "</span></div>" << '\n';
            out << "<div class=\"col-md-4\">Runtime version <span class=\"label label-primary\">" << boost::lexical_cast<std::string>(rec.get_runtime_API_version()) << "</span></div>" << '\n';
            out << "</div>" << '\n';
          }


        std::string HTML_report::compose_tag_list(const std::list<std::string>& tags)
          {
            std::ostringstream out;

            if(!tags.empty())
              {
                for(const std::string& tag : tags)
                  {
                    out << "<span class=\"label label-info\">" << tag << "</span>";
                  }
              }
            else
              {
                out << "<span class=\"label label-default\">None</span>";
              }

            return(out.str());
          }


        template <typename number>
        void HTML_report::write_integration_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache,
                                                    unique_tags<number>& tags)
          {
            integration_content_db& db = cache.get_integration_content_db();

            for(const integration_content_db::value_type& group : db)
              {
                const output_group_record<integration_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<div id=\"" << tag << "\" class=\"tab-pane fade\">" << '\n';
                out << "<div class=\"list-group\">" << '\n';
                out << "<a href=\"#\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                this->write_generic_output_record(out, out_js, rec, cache, tags);

                const integration_payload& payload = rec.get_payload();

                out << "<h4 class=\"list-group-item-text topskip\">Properties</h4>" << '\n';
                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-12\">Container <span class=\"label label-default\">" << payload.get_container_path().string() << "</span></div>" << '\n';
                out << "</div>" << '\n';

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Complete <span class=\"label label-default\">" << (payload.is_failed() ? "No" : "Yes") << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Workgroup <span class=\"label label-default\">" << payload.get_workgroup_number() << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Seeded ";
                this->write_seeded(out, out_js, payload, cache, tags);
                out << "</div>" << '\n';
                out << "</div>" << '\n';

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Statistics <span class=\"label label-default\">" << (payload.has_statistics() ? "Yes" : "No") << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Initial conditions <span class=\"label label-default\">" << (payload.has_initial_conditions() ? "Yes" : "No") << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Size <span class=\"label label-default\">" << format_memory(payload.get_size()) << "</span></div>" << '\n';
                out << "</div>" << '\n';

                const integration_metadata& metadata = payload.get_metadata();

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Wallclock time <span class=\"label label-default\">" << format_time(metadata.total_wallclock_time) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Total time <span class=\"label label-default\">" << format_time(metadata.total_integration_time) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Configurations <span class=\"label label-default\">" << metadata.total_configurations << "</span></div>" << '\n';
                out << "</div>" << '\n';

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Minimum integration <span class=\"label label-default\">" << format_time(metadata.global_min_integration_time) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Maximum integration <span class=\"label label-default\">" << format_time(metadata.global_max_integration_time) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Failures <span class=\"label label-default\">" << metadata.total_failures << "</span></div>" << '\n';
                out << "</div>" << '\n';

                out << "</a>" << '\n';
                out << "</div>" << '\n';
                out << "</div>" << '\n';
              }
          }


        template <typename number>
        void HTML_report::write_postintegration_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            postintegration_content_db& db = cache.get_postintegration_content_db();

            for(const postintegration_content_db::value_type& group : db)
              {
                const output_group_record<postintegration_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<div id=\"" << tag << "\" class=\"tab-pane fade\">" << '\n';
                out << "<div class=\"list-group\">" << '\n';
                out << "<a href=\"#\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                this->write_generic_output_record(out, out_js, rec, cache, tags);

                const postintegration_payload& payload = rec.get_payload();

                out << "<h4 class=\"list-group-item-text topskip\">Properties</h4>" << '\n';
                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-12\">Container <span class=\"label label-default\">" << payload.get_container_path().string() << "</span></div>" << '\n';
                out << "</div>" << '\n';

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Complete <span class=\"label label-default\">" << (payload.is_failed() ? "No" : "Yes") << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\">Parent ";
                this->write_content_button(out, out_js, payload.get_parent_group(), cache, tags);
                out << "</div>" << '\n';
                out << "<div class=\"col-md-4\">Seeded ";
                this->write_seeded(out, out_js, payload, cache, tags);
                out << "</div>" << '\n';
                out << "</div>" << '\n';

                out << "<div class=\"row\">" << '\n';
                out << "<div class=\"col-md-4\">Size <span class=\"label label-default\">" << format_memory(payload.get_size()) << "</span></div>" << '\n';
                out << "<div class=\"col-md-4\"></div>" << '\n';
                out << "<div class=\"col-md-4\"></div>" << '\n';
                out << "</div>" << '\n';

                out << "</a>" << '\n';
                out << "</div>" << '\n';
                out << "</div>" << '\n';
              }
          }


        template <typename number>
        void HTML_report::write_output_content(std::ofstream& out, std::ofstream& out_js, repository_cache<number>& cache, unique_tags<number>& tags)
          {
            output_content_db& db = cache.get_output_content_db();
            typename derived_product_db<number>::type& product_db = cache.get_derived_product_db();

            for(const output_content_db::value_type& group : db)
              {
                const output_group_record<output_payload>& rec = *group.second;
                std::string tag = tags.get_id(rec);

                out << "<div id=\"" << tag << "\" class=\"tab-pane fade\">" << '\n';
                out << "<div class=\"list-group\">" << '\n';
                out << "<a href=\"#\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<h3 class=\"list-group-item-heading\">" << rec.get_name() << "</h3>" << '\n';

                this->write_generic_output_record(out, out_js, rec, cache, tags);

                const output_payload& payload = rec.get_payload();
                const std::list<derived_content>& content = payload.get_derived_content();

                if(!content.empty())
                  {
                    boost::filesystem::path relative_content_path = rec.get_name();
                    bool include_content = true;

                    try
                      {
                        boost::filesystem::create_directories(this->root / relative_content_path);
                      }
                    catch(boost::filesystem::filesystem_error& xe)
                      {
                        include_content = false;
                      }

                    out << "<div class=\"list-group\">" << '\n';

                    unsigned int count = 0;
                    for(const derived_content& item : content)
                      {
                        boost::filesystem::path filename = item.get_filename();
                        boost::filesystem::path extension = filename.extension();

                        out << "<a href=\"#\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                        out << "<h4 class=\"list-group-item-text topskip\">" << filename.string() << "</h4>" << '\n';

                        boost::filesystem::path abs_product_location = rec.get_abs_output_path() / item.get_filename();
                        boost::filesystem::path relative_copy_location = relative_content_path / item.get_filename();
                        boost::filesystem::path abs_copy_location = this->root / relative_copy_location;

                        bool write_content = include_content && (extension.string() == ".pdf" || extension.string() == ".png" || extension.string() == ".svg" || extension.string() == ".svgz");
                        if(write_content)
                          {
                            try
                              {
                                boost::filesystem::copy_file(abs_product_location, abs_copy_location);
                              }
                            catch(boost::filesystem::filesystem_error& xe)
                              {
                                write_content = false;
                              }

                            if(write_content)
                              {
                                // Safari will display PDFs in an <img> tag, but Chrome and Firefox will not
                                if(extension.string() == ".pdf")
                                  {
                                    out << "<div style=\"width: 700px; height: 600px\">" << '\n';
                                    out << "<object data=\"" << relative_copy_location.string() << "\" type=\"application/pdf\" width=\"100%\" height=\"100%\">" << '\n';
                                    out << "<p>PDF embedding not supported in this browser</p>" << '\n';
                                    out << "</object>" << '\n';
                                    out << "</div>" << '\n';
                                  }
                                else
                                  {
                                    out << "<img class=\"img-responsive imgproduct\" src=\"" << relative_copy_location.string() << "\" alt=\"" << filename << "\">" << '\n';
                                  }
                              }
                          }

                        typename derived_product_db<number>::type::const_iterator t = product_db.find(item.get_parent_product());

                        out << "<div class=\"row\">" << '\n';
                        if(t != product_db.end())
                          {
                            out << "<div class=\"col-md-4\">Type <span class=\"label label-default\">" << derived_data::derived_product_type_to_string(t->second->get_product()->get_type()) << "</span></div>" << '\n';
                          }
                        else
                          {
                            out << "<div class=\"col-md-4\">Type <span class=\"label label-danger\">NULL</span></div>" << '\n';
                          }
                        out << "<div class=\"col-md-4\">Created <span class=\"label label-default\">" << boost::posix_time::to_simple_string(item.get_creation_time()) << "</span></div>" << '\n';
                        out << "<div class=\"col-md-4\">Tags " << this->compose_tag_list(item.get_tags()) << "</div>" << '\n';
                        out << "</div>" << '\n';

                        // Write out notes if present
                        this->write_notes_collapsible(out, out_js, item.get_notes(), tag);

                        const std::list<std::string>& groups = item.get_content_groups();
                        if(!groups.empty())
                          {
                            std::string group_tag = tag + "_" + boost::lexical_cast<std::string>(count);
                            ++count;

                            out << "<button data-toggle=\"collapse\" data-target=\"#" << group_tag << "\" class=\"button btn-info button-lg topskip\">Data provenance <span class=\"badge\">" << groups.size() << "</span></button>" << '\n';
                            out << "<div id=\"" << group_tag << "\" class=\"collapse\">" << '\n';
                            out << "<table class=\"table table-striped\">" << '\n';

                            out << "<thead>" << '\n';
                            out << "<tr>" << '\n';
                            out << "<th>Name</th>" << '\n';
                            out << "<th>Task</th>" << '\n';
                            out << "<th>Type</th>" << '\n';
                            out << "<th>Last updated</th>" << '\n';
                            out << "</tr>" << '\n';
                            out << "</thead>" << '\n';

                            out << "<tbody>" << '\n';

                            for(const std::string& group : groups)
                              {
                                out << "<tr>" << '\n';
                                out << "<td>";
                                this->write_content_button(out, out_js, group, cache, tags);
                                out << "</td>" << '\n';

                                content_group_data<number> data(group, cache);

                                out << "<td>";
                                this->write_task_button(out, out_js, data.get_task(), cache, tags);
                                out << "</td>" << '\n';

                                out << "<td>" << data.get_type() << "</td>" << '\n';
                                out << "<td>" << data.get_last_edit() << "</td>" << '\n';

                                out << "</tr>" << '\n';
                              }

                            out << "</table>" << '\n';
                            out << "</div>" << '\n';
                          }

                        out << "</a>" << '\n';
                      }

                    out << "</div>" << '\n';
                  }

                out << "</a>" << '\n';
                out << "</div>" << '\n';
                out << "</div>" << '\n';
              }
          }


        template <typename number, typename Payload>
        void HTML_report::write_generic_output_record(std::ofstream& out, std::ofstream& out_js, const output_group_record<Payload>& rec,
                                                      repository_cache<number>& cache, unique_tags<number>& tags)
          {
            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            out << "<div class=\"row\">" << '\n';
            out << "<div class=\"col-md-4\">Created <span class=\"label label-primary\">" << boost::posix_time::to_simple_string(created) << "</span></div>" << '\n';
            out << "<div class=\"col-md-4\">Last update <span class=\"label label-primary\">" << boost::posix_time::to_simple_string(edited) << "</span></div>" << '\n';
            out << "<div class=\"col-md-4\">Runtime version <span class=\"label label-primary\">" << boost::lexical_cast<std::string>(rec.get_runtime_API_version()) << "</span></div>" << '\n';
            out << "</div>" << '\n';

            out << "<div class=\"row\">" << '\n';
            out << "<div class=\"col-md-4\">Parent task ";
            this->write_task_button(out, out_js, rec.get_task_name(), cache, tags);
            out << "</div>" << '\n';
            out << "<div class=\"col-md-4\">Locked <span class=\"label label-primary\">" << (rec.get_lock_status() ? "Yes" : "No") << "</span></div>" << '\n';
            out << "<div class=\"col-md-4\">Tags " << this->compose_tag_list(rec.get_tags()) << "</div>" << '\n';
            out << "</div>" << '\n';

            this->write_notes_collapsible(out, out_js, rec.get_notes(), tags.get_id(rec));
          }


        template <typename number, typename Payload>
        void HTML_report::write_seeded(std::ofstream& out, std::ofstream& out_js, const Payload& payload,
                                       repository_cache<number>& cache, unique_tags<number>& tags)
          {
            if(payload.is_seeded())
              {
                this->write_task_button(out, out_js, payload.get_seed_group(), cache, tags);
              }
            else
              {
                out << "<span class=\"label label-default\">No</span>";
              }
          }


        template <typename number>
        void HTML_report::write_content_button(std::ofstream& out, std::ofstream& out_js, const std::string& name,
                                               repository_cache<number>& cache, unique_tags<number>& tags)
          {
            integration_content_db& int_db = cache.get_integration_content_db();
            postintegration_content_db& pint_db = cache.get_postintegration_content_db();
            output_content_db& output_db = cache.get_output_content_db();

            integration_content_db::const_iterator int_t = int_db.find(name);
            postintegration_content_db::const_iterator pint_t = pint_db.find(name);
            output_content_db::const_iterator out_t = output_db.find(name);

            std::string tag;
            if(int_t != int_db.end()) tag = tags.get_id(*int_t->second);
            else if(pint_t != pint_db.end()) tag = tags.get_id(*pint_t->second);
            else if(out_t != output_db.end()) tag = tags.get_id(*out_t->second);

            this->write_content_button(out, out_js, name, tag);
          }


        void HTML_report::write_content_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, const std::string& tag)
          {
            if(!tag.empty())
              {
                std::string button_id = this->make_button_tag();
                out << "<button class=\"button btn-link\" id=\"" << button_id << "\">" << name << "</button>";

                // we need a Javascript function which causes this button to activate the appropriate contents pane when clicked
                out_js << "$(function(){" << '\n';
                out_js << "  $('#" << button_id << "').click(function(e){" << '\n';
                out_js << "    e.preventDefault();" << '\n';
                out_js << "      $('#tabs a[href=\"#" << tag << "\"]').tab('show');" << '\n';
                out_js << "  })" << '\n';
                out_js << "})" << '\n';
              }
            else
              {
                out << "<span class=\"label label-danger\">" << name << "</span>";
              }
          }


        template <typename number>
        void HTML_report::write_package_button(std::ofstream& out, std::ofstream& out_js, const std::string& name,
                                               repository_cache<number>& cache, unique_tags<number>& tags)
          {
            typename package_db<number>::type& db = cache.get_package_db();
            typename package_db<number>::type::const_iterator t = db.find(name);

            std::string tag;
            if(t != db.end()) tag = tags.get_id(*t->second);

            this->write_package_button(out, out_js, name, tag);
          }


        void HTML_report::write_package_button(std::ofstream& out, std::ofstream& out_js, const std::string& name, const std::string& tag)
          {
            if(!tag.empty())
              {
                std::string button_id = this->make_button_tag();
                out << "<button class=\"button btn-link\" id=\"" << button_id << "\">" << name << "</button>";

                // we need a Javascript function which causes this button to activate the packages pane when clicked
                out_js << "$(function(){" << '\n';
                out_js << "  $('#" << button_id << "').click(function(e){" << '\n';
                out_js << "    e.preventDefault();" << '\n';
                out_js << "      $('#tabs a[href=\"#packages\"]').tab('show');" << '\n';
                out_js << "      var elem = $(\"a[id='" << tag << "']\");" << '\n';
                out_js << "      $('html, body').animate({ scrollTop: elem.offset().top }, 1000);" << '\n';
                out_js << "  })" << '\n';
                out_js << "})" << '\n';
              }
            else
              {
                out << "<span class=\"label label-danger\">" << name << "</span>";
              }
          }


        void HTML_report::write_notes_collapsible(std::ofstream& out, std::ofstream& out_js, const std::list<std::string>& notes, const std::string& tag)
          {
            if(notes.empty()) return;

            out << "<button data-toggle=\"collapse\" data-target=\"#" << tag << "notes\" class=\"button btn-info button-lg topskip\">Notes <span class=\"badge\">" << notes.size() << "</span></button" << '\n';
            out << "<div id=\"" << tag << "notes\" class=\"collapse\">" << '\n';
            out << "<div class=\"list-group\">" << '\n';

            for(const std::string& note : notes)
              {
                out << "<a href=\"#\" class=\"list-group-item\" onclick=\"return false;\">" << '\n';
                out << "<p>" << note << "</p>" << '\n';
                out << "</a>";
              }

            out << "</div>" << '\n';
            out << "</div>" << '\n';
          }


        std::string HTML_report::make_button_tag()
          {
            std::ostringstream tag;
            tag << CPPTRANSPORT_HTML_BUTTON_TAG_PREFIX << this->button_id++;
            return(tag.str());
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_HTML_REPORT_H
