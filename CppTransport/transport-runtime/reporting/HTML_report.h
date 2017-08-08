//
// Created by David Seery on 23/03/2016.
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

#ifndef CPPTRANSPORT_REPORTING_HTML_REPORT_H
#define CPPTRANSPORT_REPORTING_HTML_REPORT_H


#include "transport-runtime/repository/repository.h"
#include "transport-runtime/data/data_manager.h"
#include "transport-runtime/derived-products/derived_product_type.h"

#include "transport-runtime/reporting/content_group_data.h"
#include "transport-runtime/reporting/HTML_report_bundle.h"

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/utilities/formatter.h"
#include "transport-runtime/utilities/plot_environment.h"

#include "transport-runtime/defaults.h"


namespace transport
  {

    namespace reporting
      {

        class HTML_report
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            HTML_report(local_environment& e, argument_cache& c)
              : env(e),
                arg_cache(c),
                err(error_handler(e, c)),
                warn(warning_handler(e, c)),
                msg(message_handler(e, c)),
                button_id(0),
                misc_precision(CPPTRANSPORT_DEFAULT_HTML_MISC_PRECISION),
                efolds_precision(CPPTRANSPORT_DEFAULT_HTML_EFOLDS_PRECISION),
                database_precision(CPPTRANSPORT_DEFAULT_HTML_DATABASE_PRECISION)
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
            void report(repository<number>& repo, data_manager<number>& dmgr);


            // INTERNAL API

          protected:

            //! generate HTML report
            template <typename number>
            void generate_report(HTML_report_bundle<number>& bundle);

            //! write HTML report
            template <typename number>
            void generate_report_body(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write package tab
            template <typename number>
            void write_packages(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write integration tasks tab
            template <typename number>
            void write_integration(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent);

            //! write postintegration tasks tab
            template <typename number>
            void write_postintegration(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent);

            //! write output tasks tab
            template <typename number>
            void write_output(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent);

            //! write derived products tab
            template <typename number>
            void write_derived_products(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write integration content tabs
            template <typename number>
            void write_integration_content(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write postintegration content tabs
            template <typename number>
            void write_postintegration_content(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write output content tabs
            template <typename number>
            void write_output_content(HTML_report_bundle<number>& bundle, HTML_node& parent);


            // INTEGRATION TASKS

          protected:

            //! write integration task details for twopf task
            template <typename number>
            void write_task_data(HTML_report_bundle<number>& bundle, twopf_task<number>& tk,
                                 HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write integration task details for threepf task
            template <typename number>
            void write_task_data(HTML_report_bundle<number>& bundle, threepf_task<number>& tk,
                                 HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write task details for a generic integration task
            template <typename number>
            void write_generic_task_data(HTML_report_bundle<number>& bundle, twopf_db_task<number>& tk,
                                         HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);


            // INTEGRATION CONTENT

          protected:

            //! create a worker table for an integration content record
            template <typename number>
            void write_worker_table(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec, HTML_node& parent);

            //! typedef for list of configurations-per-worker
            typedef std::map< std::pair<unsigned int, unsigned int>, unsigned int > count_list;

            //! count configurations per worker and store in an interable container
            count_list count_configurations_per_worker(timing_db& data);

            //! create visual report of timing statistics
            template <typename number>
            void write_integration_analysis(HTML_report_bundle<number>& bundle,
                                            const content_group_record<integration_payload> rec, HTML_node& parent);

            //! produce bar chart showing number of configurations processed per worker
            template <typename number>
            void write_worker_chart(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                    HTML_node& parent, count_list& counts);

            //! produce histogram showing distribution of integration times
            template <typename number>
            void write_timing_histogram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                        HTML_node& parent, timing_db& timing_data);

            //! write specialized integration analysis for 3pf
            template <typename number>
            void write_3pf_integration_analysis(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                                const integration_task_record<number>& irec,
                                                worker_information_db& worker_db, timing_db& timing_data,
                                                HTML_node& parent);

            //! plot a generic scatter diagram */
            template <typename number, typename Payload>
            void plot_scatter(HTML_report_bundle<number>& bundle, const content_group_record<Payload>& rec,
                              std::string script, std::string image,
                              const std::list< std::tuple<double, double, double> >& dataset,
                              std::string xlabel, std::string ylabel, std::string colour_label,
                              std::string css_class, std::string popover_title, std::string popover_text,
                              bool xlog, bool ylog, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. k_t
            template <typename number>
            void write_time_kt_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                       const integration_task_record<number>& irec,
                                       worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. alpha
            template <typename number>
            void write_time_alpha_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                          const integration_task_record<number>& irec,
                                          worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. beta
            template <typename number>
            void write_time_beta_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                         const integration_task_record<number>& irec,
                                         worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! write standardized matplotlib preamble
            template <typename number>
            void write_matplotlib_preamble(std::ofstream& out, HTML_report_bundle<number>& bundle);


            // DERIVED PRODUCTS

          protected:

            //! write details for line_plot2d product
            template <typename number>
            void write_derived_product(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                       const derived_data::line_plot2d<number>& product, HTML_node& parent);

            //! write details for line_asciitable products
            template <typename number>
            void write_derived_product(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                       const derived_data::line_asciitable<number>& product, HTML_node& parent);

            //! write details for generic derived product
            template <typename number>
            void write_generic_derived_product(HTML_report_bundle<number>& bundle, const derived_data::derived_product<number>& product,
                                               HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write details for generic line collection
            template <typename number>
            void write_line_collection(HTML_report_bundle<number>& bundle, const derived_data::line_collection<number>& product,
                                       HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write a collapsible list of lines in a line collection
            template <typename number>
            void write_line_list(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                 const derived_data::line_collection<number>& line_collection, HTML_node& parent);


            // DERIVED LINES

          public:

            // CATEGORIES

            //! write a title for a derived line data element
            void derived_line_title(std::string title, HTML_node& parent);

            //! write details for a generic derived line in a line collection
            template <typename number>
            void write_generic_derived_line(HTML_report_bundle<number>& bundle, const derived_data::derived_line<number>& line, HTML_node& parent);

            //! write details for a generic wavenumber series line line
            template <typename number>
            void write_wavenumber_series_line(HTML_report_bundle<number>& bundle, const derived_data::wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a twopf line
            template <typename number>
            void write_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_line<number>& line, HTML_node& parent);

            //! write details for a threepf line
            template <typename number>
            void write_threepf_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_line<number>& line, HTML_node& parent);

            //! write details for a zeta twopf line
            template <typename number>
            void write_zeta_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_line<number>& line, HTML_node& parent);

            //! write details for a zeta threepf line
            template <typename number>
            void write_zeta_threepf_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_line<number>& line, HTML_node& parent);

            //! write details for a zeta reduced bispectrum line
            template <typename number>
            void write_zeta_redbsp_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_line<number>& line, HTML_node& parent);

            //! write details for a tensor twopf line
            template <typename number>
            void write_tensor_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_line<number>& line, HTML_node& parent);

            //! write details for an fNL line
            template <typename number>
            void write_fNL_line(HTML_report_bundle<number>& bundle, const derived_data::fNL_line<number>& line, HTML_node& parent);

            //! write details for an r line
            template <typename number>
            void write_r_line(HTML_report_bundle<number>& bundle, const derived_data::r_line<number>& line, HTML_node& parent);

            // SQL QUERIES

            //! construct a generic SQL language block
            void write_SQL_block(const std::string& SQL, HTML_node& parent);

            //! embed an SQL block in a titled panel
            void write_SQL_panel(std::string title, const std::string& SQL, HTML_node& parent);

            //! write details for an SQL time configuration query
            template <typename number>
            void write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_time_query& query, HTML_node& parent);

            //! write details for an SQL twopf momentum-configuration query
            template <typename number>
            void write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_twopf_query& query, HTML_node& parent);

            //! write details for an SQL threepf momentum-configuration query
            template <typename number>
            void write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_threepf_query& query, HTML_node& parent);


            // SPECIFIC LINES

            //! write details for a background time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::background_time_series<number>& line, HTML_node& parent);

            //! write details for a twopf time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_time_series<number>& line, HTML_node& parent);

            //! write details for a threepf time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_time_series<number>& line, HTML_node& parent);

            //! write details for a tensor twopf time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_time_series<number>& line, HTML_node& parent);

            //! write details for a zeta twopf time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_time_series<number>& line, HTML_node& parent);

            //! write details for a zeta threepf time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_time_series<number>& line, HTML_node& parent);

            //! write details for a zeta reduced bispectrum time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_time_series<number>& line, HTML_node& parent);

            //! write details for a twopf wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a threepf wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a tensor twopf wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a zeta twopf wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a zeta threepf wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a zeta reduced bispectrum wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for an fNL time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::fNL_time_series<number>& line, HTML_node& parent);

            //! write details for an r time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::r_time_series<number>& line, HTML_node& parent);

            //! write details for an r wavenumber series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::r_wavenumber_series<number>& line, HTML_node& parent);

            //! write details for a background time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::background_line<number>& line, HTML_node& parent);

            //! write details for a u2 time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::u2_line<number>& line, HTML_node& parent);

            //! write details for a u3 time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::u3_line<number>& line, HTML_node& parent);

            //! write details for a largest-u2-element time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::largest_u2_line<number>& line, HTML_node& parent);

            //! write details for a largest-u3-element time series line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::largest_u3_line<number>& line, HTML_node& parent);

            //! write details for a cost-per-momentum-configuration integration analysis line
            template <typename number>
            void write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::cost_wavenumber<number>& line, HTML_node& parent);


            // MAKE BUTTONS

          protected:

            //! write a link-to-task button
            template <typename number>
            void write_task_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent);

            //! write a link-to-group button
            template <typename number>
            void write_content_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent);

            //! write a link-to-group button
            template <typename number>
            void write_content_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent);

            //! write a link-to-package button
            template <typename number>
            void write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent);

            //! write a link-to-package button
            template <typename number>
            void write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent);

            //! write a link-to-derived product button
            template <typename number>
            void write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent);

            //! write a link-to-derived product button
            template <typename number>
            void write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent);


            // WRITE GENERIC RECORD DETAILS

          protected:

            //! write details for generic records
            template <typename number, typename RecordType>
            void write_generic_record(HTML_report_bundle<number>& bundle, const RecordType& rec, HTML_node& parent);

            //! write details for generic output records
            template <typename number, typename Payload>
            void write_generic_output_record(HTML_report_bundle<number>& bundle, const content_group_record<Payload>& rec,
                                             HTML_node& parent);

            //! write a seeding label
            template <typename number, typename Payload>
            void write_seeded(HTML_report_bundle<number>& bundle, const Payload& payload, HTML_node& parent);

            //! write a collapsible notes section
            void write_notes_collapsible(const std::list<note>& notes, const std::string& tag, HTML_node& parent);

            //! write a collapsible activity log
            void write_activity_collapsible(const std::list<metadata_history>& activity, const std::string& tag, HTML_node& parent);


            // WRITE CONFIGURATION DATABASES

          protected:

            //! write time configuration database
            template <typename number>
            void write_time_db_modal(HTML_report_bundle<number>& bundle, const time_config_database& db,
                                     HTML_node& parent);

            //! write k configuration database
            template <typename number>
            void write_kconfig_db_modal(HTML_report_bundle<number>& bundle, const twopf_kconfig_database& db,
                                        HTML_node& parent);

            //! write k configuration database
            template <typename number>
            void write_kconfig_db_modal(HTML_report_bundle<number>& bundle, const threepf_kconfig_database& db,
                                        HTML_node& parent);

            //! utility function to write a close button
            void aria_close_button(HTML_node& parent);


            // WRITE JAVASCRIPT HANDLERS

          protected:

            //! write JavaScript button handler which changes pane and scrolls to a given element
            template <typename number>
            void write_JavaScript_button(HTML_report_bundle<number>& bundle, std::string button, std::string pane,
                                         std::string element);

            //! write JavaScript button handler which simply changes pane
            template <typename number>
            void write_JavaScript_button(HTML_report_bundle<number>& bundle, std::string button, std::string pane);


            // MISCELLANEOUS

          protected:

            //! make a button tag
            std::string make_button_tag();

            //! build an ordinary menu tab
            HTML_node make_menu_tab(std::string pane, std::string name);

            //! build a content-group menu tab
            template <typename number, typename DatabaseType>
            HTML_node make_dropdown_menu_tab(const DatabaseType& db, HTML_report_bundle<number>& bundle,
                                             std::string name);

            //! write a grid data element
            void make_data_element(std::string l, std::string v, HTML_node& parent);

            //! make a list item heading
            void make_list_item_label(std::string label, HTML_node& parent);

            //! make badged text
            void make_badged_text(std::string text, unsigned int number, HTML_node& parent);

            //! compose a tag list into a set of HTML labels
            std::string compose_tag_list(const std::list<std::string>& tags, HTML_node& parent);


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

            //! numerical prevision for formatting miscellaneous quantities
            unsigned int misc_precision;

            //! numerical precision for formatting e-folds
            unsigned int efolds_precision;

            //! numerical precision for formatting database entries
            unsigned int database_precision;

          };


        void HTML_report::set_root(boost::filesystem::path p)
          {
            // check whether root path is absolute, and if not make it relative to current working directory
            if(!p.is_absolute())
              {
                // not absolute, so use CWD
                p = boost::filesystem::absolute(p);
              }

            while(p.filename().string() == ".")
              {
                p = p.parent_path();
              }

            // check whether root directory already exists, and if it does, raise an error
            if(boost::filesystem::exists(p))
              {
                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

                boost::filesystem::path out_dir = p;
                out_dir.remove_filename();

                boost::filesystem::path out_leaf = p.filename();
                boost::filesystem::path out_ext = p.extension();
                out_leaf.replace_extension("");

                std::ostringstream new_leaf;
                new_leaf << out_leaf.string() << "-" << boost::posix_time::to_iso_string(now);

                out_leaf = boost::filesystem::path(new_leaf.str());
                out_leaf.replace_extension(out_ext);

                boost::filesystem::path new_p = out_dir / out_leaf;

                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_ROOT_EXISTS_A << " '" << p.string() << "' " << CPPTRANSPORT_HTML_ROOT_EXISTS_B << " '" << new_p.string() << "\"";
                this->warn(msg.str());

                p = new_p;
              }

            this->root = p;
          }


        template <typename number>
        void HTML_report::report(repository<number>& repo, data_manager<number>& dmgr)
          {
            // has a root directory been set? if not, no need to generate a report
            if(this->root.empty()) return;

            try
              {
                // reset button id
                this->button_id = 0;

                // set up a HTML bundle for this report; this encapsulates all the files needed
                // and also manages any assets
                HTML_report_bundle<number> bundle(this->env, this->arg_cache, repo, dmgr, this->root);

                // write main index.html
                this->generate_report(bundle);
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == exception_type::REPORTING_ERROR)
                  {
#ifdef TRACE_OUTPUT
                    std::cout << "TRACE_OUTPUT M" << '\n';
#endif
                    this->err(xe.what());
                    return;
                  }
                else throw;
              }
          }


        template <typename number>
        void HTML_report::generate_report(HTML_report_bundle<number>& bundle)
          {
            // add jQuery; note jQuery must come first, before bootstrap is loaded
            bundle.emplace_JavaScript_asset("jQuery/jquery-2.2.2.min.js", "jquery-2.2.2.min.js");

            // add DataTables javascript; apparently this must become before bootstrap too,
            // otherwise we get a lot of confusing 'is not a function' errors
            bundle.emplace_JavaScript_asset("DataTables/datatables.min.js", "datatables.min.js");

            // add bootstrap JavaScript
            bundle.emplace_JavaScript_asset("bootstrap/js/bootstrap.min.js", "bootstrap.min.js");

            // add bootstrap-tab-history plugin to enable history on bootstrap tabs
            bundle.emplace_JavaScript_asset("bootstrap-tab-history/bootstrap-tab-history.js", "bootstrap-tab-history.js");

            // add prism.js for syntax highlighting
            bundle.emplace_JavaScript_asset("prism/prism.min.js", "prism.min.js");

            // add our own JavaScript file utility.js which handles eg. resizing navigation bar on window resize
            bundle.emplace_JavaScript_asset("HTML_assets/utility.js", "utility.js");

            // add DataTables CSS file
            bundle.emplace_CSS_asset("DataTables/datatables.min.css", "datatables.min.css");

            // add bootstrap CSS file
            bundle.emplace_CSS_asset("bootstrap/css/bootstrap.min.css", "bootstrap.min.css");
            bundle.emplace_CSS_asset("bootstrap/css/bootstrap.min.css.map", "bootstrap.min.css.map");

            // add bootstrap glyphicon fonts
            bundle.emplace_font_asset("bootstrap/fonts/glyphicons-halflings-regular.eot", "glyphicons-halflings-regular.eot");
            bundle.emplace_font_asset("bootstrap/fonts/glyphicons-halflings-regular.svg", "glyphicons-halflings-regular.svg");
            bundle.emplace_font_asset("bootstrap/fonts/glyphicons-halflings-regular.ttf", "glyphicons-halflings-regular.ttf");
            bundle.emplace_font_asset("bootstrap/fonts/glyphicons-halflings-regular.woff", "glyphicons-halflings-regular.woff");
            bundle.emplace_font_asset("bootstrap/fonts/glyphicons-halflings-regular.woff2", "glyphicons-halflings-regular.woff2");

            // add prism.js CSS file
            bundle.emplace_CSS_asset("prism/prism.css", "prism.css");

            // add our own CSS file
            bundle.emplace_CSS_asset("HTML_assets/cpptransport.css", "cpptransport.css");

            // now generate the main body of the report
            HTML_node body("div");
            body.add_attribute("class", "container-fluid");
            this->generate_report_body(bundle, body);

            // emplace the report body inside the bundle
            bundle.emplace_HTML_body(body);
          }


        namespace HTML_report_impl
          {

            // duplicate a database record
            // (such records are not copyable because of std::unique_ptr<>'s disabled copy constructor)
            template <typename Iterator, typename DestinationContainer, typename Predicate>
            void duplicate_if(Iterator current, Iterator end, DestinationContainer& ctr, Predicate pred)
              {
                while(current != end)
                  {
                    // curent points to a std::pair< const std::string, std::unique_ptr<RecordType> >
                    if(pred(*current))
                      {
                        const typename DestinationContainer::value_type::second_type::element_type& record = *(current->second);

                        std::unique_ptr<typename DestinationContainer::value_type::second_type::element_type> new_ptr(dynamic_cast<typename DestinationContainer::value_type::second_type::element_type*>(record.clone()));

                        ctr.emplace(std::make_pair(current->first, std::move(new_ptr)));
                      }

                    ++current;
                  }
              }

          }   // namespace HTML_report_impl

        using namespace HTML_report_impl;


        template <typename number>
        void HTML_report::generate_report_body(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            // NAVIGATION BAR

            HTML_node navbar("nav");
            navbar.add_attribute("class", "navbar navbar-default navbar-fixed-top");

            HTML_node navbar_container("div");
            navbar_container.add_attribute("class", "container-fluid");

            HTML_node navbar_header("div");
            navbar_header.add_attribute("class", "navbar-header");

            HTML_node navbar_brand("a", bundle.get_title());
            navbar_brand.add_attribute("class", "navbar-brand").add_attribute("href", "#");

            navbar_header.add_element(navbar_brand);

            // Tablist
            HTML_node tablist("ul");
            tablist.add_attribute("id", "tabs").add_attribute("class", "nav navbar-nav").add_attribute("role", "tablist");

            // Package tab
            HTML_node package_tab = this->make_dropdown_menu_tab(bundle.get_package_db(), bundle, "Packages");

            typename task_db<number>::type i_tasks;
            typename task_db<number>::type p_tasks;
            typename task_db<number>::type o_tasks;

            const typename task_db<number>::type & tasks = bundle.get_task_db();

            duplicate_if(tasks.begin(), tasks.end(), i_tasks, [&](const typename task_db<number>::type::value_type& t) -> bool { return(t.second && t.second->get_type() == task_type::integration); });
            duplicate_if(tasks.begin(), tasks.end(), p_tasks, [&](const typename task_db<number>::type::value_type& t) -> bool { return(t.second && t.second->get_type() == task_type::postintegration); } );
            duplicate_if(tasks.begin(), tasks.end(), o_tasks, [&](const typename task_db<number>::type::value_type& t) -> bool { return(t.second && t.second->get_type() == task_type::output); } );

            // Integration tasks tab
            HTML_node integration_tasks_tab = this->make_dropdown_menu_tab(i_tasks, bundle, "Integration tasks");

            // Integration content tab
            HTML_node integration_content_tab = this->make_dropdown_menu_tab<number>(bundle.get_integration_content_db(), bundle, "Integration content");

            // Postintegration tasks tab
            HTML_node postintegration_tasks_tab = this->make_dropdown_menu_tab(p_tasks, bundle, "Postintegration tasks");

            // Postintegration content tab
            HTML_node postintegration_content_tab = this->make_dropdown_menu_tab<number>(bundle.get_postintegration_content_db(), bundle, "Postintegration content");

            // Output tasks tab
            HTML_node output_tasks_tab = this->make_dropdown_menu_tab(o_tasks, bundle, "Output tasks");

            // Output content tab
            HTML_node output_content_tab = this->make_dropdown_menu_tab<number>(bundle.get_output_content_db(), bundle, "Output content");

            // Derived products tab
            HTML_node derived_products_tab = this->make_dropdown_menu_tab<number>(bundle.get_derived_product_db(), bundle, "Derived products");

            tablist.add_element(package_tab);
            tablist.add_element(integration_tasks_tab);
            tablist.add_element(integration_content_tab);
            tablist.add_element(postintegration_tasks_tab);
            tablist.add_element(postintegration_content_tab);
            tablist.add_element(output_tasks_tab);
            tablist.add_element(output_content_tab);
            tablist.add_element(derived_products_tab);

            navbar_container.add_element(navbar_header).add_element(tablist);

            navbar.add_element(navbar_container);

            // WRITE PANE CONTENT

            HTML_node tab_panes("div");
            tab_panes.add_attribute("class", "tab-content");

            this->write_packages(bundle, tab_panes);
            this->write_integration(bundle, i_tasks, tab_panes);
            this->write_integration_content(bundle, tab_panes);
            this->write_postintegration(bundle, p_tasks, tab_panes);
            this->write_postintegration_content(bundle, tab_panes);
            this->write_output(bundle, o_tasks, tab_panes);
            this->write_output_content(bundle, tab_panes);
            this->write_derived_products(bundle, tab_panes);

            parent.add_element(navbar);
            parent.add_element(tab_panes);
          }


        HTML_node HTML_report::make_menu_tab(std::string pane, std::string name)
          {
            HTML_node tab("li");
            HTML_node anchor("a", name);
            anchor.add_attribute("data-toggle", "tab").add_attribute("href", "#" + pane);
            anchor.add_attribute("data-tab-history", "true").add_attribute("data-tab-history-changer", "push").add_attribute("data-tab-history-update-url", "true");
            tab.add_element(anchor);

            return tab;
          }


        template <typename number, typename DatabaseType>
        HTML_node HTML_report::make_dropdown_menu_tab(const DatabaseType& db, HTML_report_bundle<number>& bundle,
                                                      std::string name)
          {
            HTML_node tab("li");
            HTML_node anchor("a", name);
            anchor.add_attribute("href", "#").add_attribute("class", "dropdown-toggle").add_attribute("data-toggle", "dropdown");

            if(db.size() > 0)
              {
                tab.add_attribute("class", "dropdown");

                HTML_node caret("span");
                caret.add_attribute("class", "caret");
                anchor.add_element(caret);

                HTML_node dropdown_menu("ul");
                dropdown_menu.add_attribute("class", "dropdown-menu scrollable-menu").add_attribute("role", "menu");

                for(const typename DatabaseType::value_type& group : db)
                  {
                    // DatabaseType::value_type is a std::pair< std::string, pointer-to-record >
                    const typename DatabaseType::value_type::second_type::element_type& rec = *group.second;
                    std::string tag = bundle.get_id(rec);

                    HTML_node menu_item("li");
                    HTML_node menu_anchor("a", rec.get_name());
                    menu_anchor.add_attribute("href", "#" + tag).add_attribute("data-toggle", "tab");
                    menu_anchor.add_attribute("data-tab-history", "true").add_attribute("data-tab-history-changer", "push").add_attribute("data-tab-history-update-url", "true");

                    menu_item.add_element(menu_anchor);
                    dropdown_menu.add_element(menu_item);
                  }

                tab.add_element(anchor);
                tab.add_element(dropdown_menu);
              }
            else
              {
                tab.add_attribute("class", "dropdown disabled");
                tab.add_element(anchor);
              }

            return tab;
          }


        void HTML_report::make_data_element(std::string l, std::string v, HTML_node& parent)
          {
            HTML_node label("dt", l);

            if(!v.empty())
              {
                HTML_node value("dd", v);
                parent.add_element(label).add_element(value);
              }
            else
              {
                HTML_node value("dd");
                HTML_node lbl("span", "UNSET");
                lbl.add_attribute("class", "label label-default");
                value.add_element(lbl);
                parent.add_element(label).add_element(value);
              }
          }


        void HTML_report::make_list_item_label(std::string label, HTML_node& parent)
          {
            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-primary");

            HTML_node header("div");
            header.add_attribute("class", "panel-heading");

            HTML_node text("h4", label);

            header.add_element(text);
            panel.add_element(header);
            parent.add_element(panel);
          }


        void HTML_report::make_badged_text(std::string text, unsigned int number, HTML_node& parent)
          {
            HTML_string text_label(text);

            HTML_node badge("span", boost::lexical_cast<std::string>(number));
            badge.add_attribute("class", "badge");

            parent.add_element(text_label);
            parent.add_element(badge);
          }


        template <typename number>
        void HTML_report::write_packages(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename package_db<number>::type& db = bundle.get_package_db();

            bool marked_active = false;

            // step through all packages, writing them out as list elements
            for(const typename package_db<number>::value_type& pkg : db)
              {
                const package_record<number>& rec = *pkg.second;
                const initial_conditions<number>& ics = rec.get_ics();
                model<number>* mdl = ics.get_model();

                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag);
                if(!marked_active)
                  {
                    // mark first tab as active by default
                    pane.add_attribute("class", "tab-pane active");
                    marked_active = true;
                  }
                else
                  {
                    pane.add_attribute("class", "tab-pane fade");
                  }

                HTML_node list("ul");
                list.add_attribute("class", "list-group");

                HTML_node item("li");
                item.add_attribute("class", "list-group-item");
                this->make_list_item_label(rec.get_name(), item);

                // write generic repository information for this record
                this->write_generic_record(bundle, rec, item);

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");

                HTML_node panel_heading("div", "Model, parameters and initial conditions");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");

                const std::string& description = mdl->get_description();
                if(!description.empty())
                  {
                    HTML_node well("div", description);
                    well.add_attribute("class", "well");
                    panel_body.add_element(well);
                  }

                HTML_node row("div");
                row.add_attribute("class", "row");

                HTML_node col1("div");
                col1.add_attribute("class", "col-md-4");
                HTML_node col1_list("dl");
                col1_list.add_attribute("class", "dl-horizontal");

                HTML_node col2("div");
                col2.add_attribute("class", "col-md-4");
                HTML_node col2_list("dl");
                col2_list.add_attribute("class", "dl-horizontal");

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");
                HTML_node col3_list("dl");
                col3_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Model", mdl->get_name(), col1_list);
                this->make_data_element("License", mdl->get_license(), col1_list);

                this->make_data_element("Initial time", format_number(ics.get_N_initial(), this->efolds_precision) + " e-folds", col2_list);
                this->make_data_element("Horizon-crossing time", format_number(ics.get_N_horizon_crossing(), this->efolds_precision) + " e-folds", col2_list);

                this->make_data_element("Citation data", mdl->get_citeguide(), col3_list);
                this->make_data_element("Revision", boost::lexical_cast<std::string>(mdl->get_revision()), col3_list);

                col1.add_element(col1_list);
                col2.add_element(col2_list);
                col3.add_element(col3_list);

                row.add_element(col1).add_element(col2).add_element(col3);
                panel_body.add_element(row);
                panel.add_element(panel_heading).add_element(panel_body);
                item.add_element(panel);

                const author_db& authors = mdl->get_authors();
                if(!authors.empty())
                  {
                    HTML_node author_panel("div");
                    author_panel.add_attribute("class", "panel panel-default topskip");

                    HTML_node heading("div", "Authors");
                    heading.add_attribute("class", "panel-heading");

                    HTML_node table_wrapper("div");
                    HTML_node table("table");

                    if(authors.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                      {
                        table_wrapper.add_attribute("class", "table-responsive padded");
                        table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                      }
                    else
                      {
                        table_wrapper.add_attribute("class", "table-responsive");
                        table.add_attribute("class", "table table-striped table-condensed sortable");
                      }

                    HTML_node head("thead");
                    HTML_node hrow("tr");

                    HTML_node name_label("th", "Name");
                    HTML_node email_label("th", "Email");
                    HTML_node institute_label("th", "Institute");
                    hrow.add_element(name_label).add_element(email_label).add_element(institute_label);
                    head.add_element(hrow);

                    HTML_node body("tbody");
                    for(const author_db::value_type& author : authors)
                      {
                        if(author.second)
                          {
                            const author_record& record = *author.second;

                            HTML_node brow("tr");
                            HTML_node name("td", record.get_name());
                            HTML_node email("td");
                            HTML_node email_anchor("a", record.get_email());
                            email_anchor.add_attribute("href", record.format_email());
                            email.add_element(email_anchor);
                            HTML_node institute("td", record.get_institute());
                            brow.add_element(name).add_element(email).add_element(institute);
                            body.add_element(brow);
                          }
                      }

                    table.add_element(head).add_element(body);
                    table_wrapper.add_element(table);
                    author_panel.add_element(heading).add_element(table_wrapper);
                    item.add_element(author_panel);
                  }

                HTML_node params_column("div");
                params_column.add_attribute("class", "col-md-6 topskip");

                const parameters<number>& params = rec.get_ics().get_params();
                const std::vector<number>& param_vec = params.get_vector();
                const std::vector<std::string>& param_names = rec.get_ics().get_model()->get_param_names();

                HTML_node params_panel("div");
                params_panel.add_attribute("class", "panel panel-info");
                HTML_node params_panel_heading("div", "Parameter values");
                params_panel_heading.add_attribute("class", "panel-heading");

                HTML_node params_wrapper("div");
                HTML_node params_table("table");

                if(param_vec.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                  {
                    params_wrapper.add_attribute("class", "table-responsive padded");
                    params_table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                  }
                else
                  {
                    params_wrapper.add_attribute("class", "table-responsive");
                    params_table.add_attribute("class", "table table-striped table-condensed sortable");
                  }

                HTML_node params_table_head("thead");
                HTML_node params_head_row("tr");
                HTML_node params_name_label("th", "Name");
                HTML_node params_value_label("th", "Value");
                params_head_row.add_element(params_name_label).add_element(params_value_label);
                params_table_head.add_element(params_head_row);

                HTML_node params_table_body("tbody");

                // add separate row for value of Planck mass
                HTML_node Mp_row("tr");
                HTML_node Mp_label("td");
                HTML_string Mp_label_text("Planck mass");
                Mp_label_text.bold();
                Mp_label.add_element(Mp_label_text);
                HTML_node Mp_value("td", boost::lexical_cast<std::string>(params.get_Mp()));
                Mp_row.add_element(Mp_label).add_element(Mp_value);
                params_table_body.add_element(Mp_row);

                for(unsigned int i = 0; i < param_vec.size() && i < param_names.size(); ++i)
                  {
                    HTML_node table_row("tr");

                    HTML_node label("td");
                    HTML_string label_text(param_names[i]);
                    label_text.bold();
                    label.add_element(label_text);

                    HTML_node value("td", format_number(static_cast<double>(param_vec[i]), this->database_precision));

                    table_row.add_element(label).add_element(value);
                    params_table_body.add_element(table_row);
                  }

                params_table.add_element(params_table_head).add_element(params_table_body);
                params_wrapper.add_element(params_table);
                params_panel.add_element(params_panel_heading).add_element(params_wrapper);
                params_column.add_element(params_panel);

                HTML_node data_grid("div");
                data_grid.add_attribute("class", "row");

                HTML_node ics_column("div");
                ics_column.add_attribute("class", "col-md-6 topskip");

                const std::vector<number>& ics_vec = rec.get_ics().get_vector();
                const std::vector<std::string>& coord_names = rec.get_ics().get_model()->get_state_names();

                HTML_node ics_panel("div");
                ics_panel.add_attribute("class", "panel panel-info");
                HTML_node ics_panel_heading("div", "Initial conditions");
                ics_panel_heading.add_attribute("class", "panel-heading");

                HTML_node ics_wrapper("div");
                HTML_node ics_table("table");

                if(ics_vec.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                  {
                    ics_wrapper.add_attribute("class", "table-responsive padded");
                    ics_table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                  }
                else
                  {
                    ics_wrapper.add_attribute("class", "table-responsive");
                    ics_table.add_attribute("class", "table table-striped table-condensed sortable");
                  }

                HTML_node ics_table_head("thead");
                HTML_node ics_head_row("tr");
                HTML_node ics_name_label("th", "Name");
                HTML_node ics_value_label("th", "Value");
                ics_head_row.add_element(ics_name_label).add_element(ics_value_label);
                ics_table_head.add_element(ics_head_row);

                HTML_node ics_table_body("tbody");

                for(unsigned int i = 0; i < ics_vec.size() && i < coord_names.size(); ++i)
                  {
                    HTML_node table_row("tr");

                    HTML_node label("td");
                    HTML_string label_text(coord_names[i]);
                    label_text.bold();
                    label.add_element(label_text);

                    // using tags in string is hacky, but simple
                    HTML_node value("td", format_number(static_cast<double>(ics_vec[i] / params.get_Mp()), this->database_precision) + " M<sub>P</sub>");

                    table_row.add_element(label).add_element(value);
                    ics_table_body.add_element(table_row);
                  }

                ics_table.add_element(ics_table_head).add_element(ics_table_body);
                ics_wrapper.add_element(ics_table);
                ics_panel.add_element(ics_panel_heading).add_element(ics_wrapper);
                ics_column.add_element(ics_panel);

                data_grid.add_element(params_column).add_element(ics_column);
                item.add_element(data_grid);

                const std::vector<std::string>& references = mdl->get_references();
                if(!references.empty())
                  {
                    HTML_node button("button");
                    button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "references");
                    button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                    this->make_badged_text("References", references.size(), button);

                    HTML_node group_list("div");
                    group_list.add_attribute("id", tag + "references").add_attribute("class", "collapse");

                    HTML_node tbl_panel("div");
                    tbl_panel.add_attribute("class", "panel panel-info scrollable-panel topskip");

                    HTML_node panel_head("div", "References associated with this model");
                    panel_head.add_attribute("class", "panel-heading");

                    HTML_node item_list("ul");
                    item_list.add_attribute("class", "list-group");

                    for(const std::string& s : references)
                      {
                        HTML_node list_item("li", s);
                        list_item.add_attribute("class", "list-group-item");
                        item_list.add_element(list_item);
                      }

                    tbl_panel.add_element(panel_head).add_element(item_list);
                    group_list.add_element(tbl_panel);
                    item.add_element(button).add_element(group_list);
                  }

                const std::vector<std::string>& urls = mdl->get_urls();
                if(!urls.empty())
                  {
                    HTML_node button("button");
                    button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "urls");
                    button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                    this->make_badged_text("URLs", urls.size(), button);

                    HTML_node group_list("div");
                    group_list.add_attribute("id", tag + "urls").add_attribute("class", "collapse");

                    HTML_node tbl_panel("div");
                    tbl_panel.add_attribute("class", "panel panel-info scrollable-panel topskip");

                    HTML_node panel_head("div", "URLs associated with this model");
                    panel_head.add_attribute("class", "panel-heading");

                    HTML_node item_list("ul");
                    item_list.add_attribute("class", "list-group");

                    for(const std::string& s : urls)
                      {
                        HTML_node list_item("li");
                        list_item.add_attribute("class", "list-group-item");
                        HTML_node anchor("a", s);
                        anchor.add_attribute("href", s);
                        list_item.add_element(anchor);
                        item_list.add_element(list_item);
                      }

                    tbl_panel.add_element(panel_head).add_element(item_list);
                    group_list.add_element(tbl_panel);
                    item.add_element(button).add_element(group_list);
                  }

                list.add_element(item);
                pane.add_element(list);
                parent.add_element(pane);
              }
          }


        template <typename number>
        void HTML_report::write_integration(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent)
          {
            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::integration)
                  {
                    const integration_task_record<number>& rec = dynamic_cast< const integration_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node pane("div");
                    pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                    HTML_node list("ul");
                    list.add_attribute("class", "list-group");

                    HTML_node item("li");
                    item.add_attribute("class", "list-group-item");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

                    HTML_node panel("div");
                    panel.add_attribute("class", "panel panel-default");

                    HTML_node panel_heading("div", "Task information");
                    panel_heading.add_attribute("class", "panel-heading");

                    HTML_node panel_body("div");
                    panel_body.add_attribute("class", "panel-body");

                    const std::string& description = rec.get_task()->get_description();
                    if(!description.empty())
                      {
                        HTML_node well("div", description);
                        well.add_attribute("class", "well");
                        panel_body.add_element(well);
                      }

                    HTML_node row("div");
                    row.add_attribute("class", "row");

                    HTML_node col1("div");
                    col1.add_attribute("class", "col-md-4");
                    HTML_node col1_list("dl");
                    col1_list.add_attribute("class", "dl-horizontal");

                    this->make_data_element("Task type", task_type_to_string(rec.get_task_type()), col1_list);

                    HTML_node col2("div");
                    col2.add_attribute("class", "col-md-4");
                    HTML_node col2_list("dl");
                    col2_list.add_attribute("class", "dl-horizontal");

                    HTML_node pkg_dt("dt", "Uses package");
                    HTML_node pkg_dd("dd");
                    this->write_package_button(bundle, rec.get_task()->get_ics().get_name(), pkg_dd);
                    col2_list.add_element(pkg_dt).add_element(pkg_dd);

                    HTML_node col3("div");
                    col3.add_attribute("class", "col-md-4");
                    HTML_node col3_list("dl");
                    col3_list.add_attribute("class", "dl-horizontal");

                    this->make_data_element("k-config database", rec.get_relative_kconfig_database_path().string(), col3_list);

                    switch(rec.get_task_type())
                      {
                        case integration_task_type::twopf:
                          {
                            twopf_task<number>& tk = dynamic_cast< twopf_task<number>& >(*rec.get_task());
                            this->write_task_data(bundle, tk, col1_list, col2_list, col3_list);
                            break;
                          }

                        case integration_task_type::threepf:
                          {
                            threepf_task<number>& tk = dynamic_cast< threepf_task<number>& >(*rec.get_task());
                            this->write_task_data(bundle, tk, col1_list, col2_list, col3_list);
                          }
                      }

                    col1.add_element(col1_list);
                    col2.add_element(col2_list);
                    col3.add_element(col3_list);

                    row.add_element(col1).add_element(col2).add_element(col3);
                    panel_body.add_element(row);
                    panel.add_element(panel_heading).add_element(panel_body);
                    item.add_element(panel);

                    // write table of content groups, if any are present
                    const std::list<std::string>& content_groups = rec.get_content_groups();
                    if(!content_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Content groups", content_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Content groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        HTML_node table("table");

                        if(content_groups.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                          {
                            table_wrapper.add_attribute("class", "table-responsive padded");
                            table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                          }
                        else
                          {
                            table_wrapper.add_attribute("class", "table-responsive");
                            table.add_attribute("class", "table table-striped table-condensed sortable");
                          }

                        HTML_node head("thead");
                        HTML_node head_row("tr");

                        HTML_node name_label("th", "Name");
                        HTML_node created_label("th", "Created");
                        HTML_node edited_label("th", "Last updated");
                        HTML_node complete_label("th", "Complete");
                        HTML_node size_label("th", "Size");

                        head_row.add_element(name_label).add_element(created_label).add_element(edited_label).add_element(complete_label).add_element(size_label);
                        head.add_element(head_row);

                        HTML_node body("tbody");

                        integration_content_db& content_db = bundle.get_integration_content_db();
                        for(const std::string& group : content_groups)
                          {
                            integration_content_db::const_iterator t = content_db.find(group);

                            HTML_node table_row("tr");

                            if(t != content_db.end())
                              {
                                HTML_node name("td");
                                this->write_content_button(bundle, group, name);

                                HTML_node created("td", boost::posix_time::to_simple_string(t->second->get_creation_time()));
                                HTML_node edited("td", boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                                HTML_node complete("td", (t->second->get_payload().is_failed() ? "No" : "Yes"));
                                HTML_node size("td", format_memory(t->second->get_payload().get_size()));

                                table_row.add_element(name).add_element(created).add_element(edited).add_element(complete).add_element(size);
                              }
                            else
                              {
                                HTML_node name("td");
                                HTML_node label("span", group);
                                label.add_attribute("class", "label label-danger");
                                name.add_element(label);

                                HTML_node null("td");
                                HTML_node null_label("span", "NULL");
                                null_label.add_attribute("class", "label label-danger");
                                null.add_element(null_label);

                                table_row.add_element(name).add_element(null).add_element(null).add_element(null).add_element(null);
                              }

                            body.add_element(table_row);
                          }

                        table.add_element(head).add_element(body);
                        table_wrapper.add_element(table);
                        tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                        group_list.add_element(tbl_panel);
                        item.add_element(button).add_element(group_list);
                      }

                    list.add_element(item);
                    pane.add_element(list);
                    parent.add_element(pane);
                  }
              }
          }


        template <typename number>
        void HTML_report::write_task_data(HTML_report_bundle<number>& bundle, twopf_task<number>& tk,
                                          HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->write_generic_task_data(bundle, tk, col1_list, col2_list, col3_list);
          }


        template <typename number>
        void HTML_report::write_task_data(HTML_report_bundle<number>& bundle, threepf_task<number>& tk,
                                          HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->write_generic_task_data(bundle, tk, col1_list, col2_list, col3_list);

            const threepf_kconfig_database& threepf_db = tk.get_threepf_database();
            HTML_node k_dt("dt", "3pf configurations");
            HTML_node k_dd("dd");
            HTML_string k_str(boost::lexical_cast<std::string>(threepf_db.size()));
            k_dd.add_element(k_str);
            if(threepf_db.size() <= CPPTRANSPORT_DEFAULT_HTML_DATABASE_MAX_SIZE) this->write_kconfig_db_modal(bundle, threepf_db, k_dd);
            col3_list.add_element(k_dt).add_element(k_dd);

            this->make_data_element("Integrable", tk.is_integrable() ? "Yes" : "No", col3_list);
            if(tk.is_integrable())
              {
                this->make_data_element("Voxel size", format_number(tk.voxel_size(), this->misc_precision), col3_list);
              }
          }


        template <typename number>
        void HTML_report::write_generic_task_data(HTML_report_bundle<number>& bundle, twopf_db_task<number>& tk,
                                                  HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->make_data_element("Initial conditions", format_number(tk.get_N_initial(), this->efolds_precision) + " e-folds", col1_list);
            this->make_data_element("<var>N</var><sub>*</sub>", format_number(tk.get_N_horizon_crossing(), this->efolds_precision) + " e-folds", col1_list);
            this->make_data_element("Subhorizon e-folds", format_number(tk.get_N_subhorizon_efolds(), this->efolds_precision) + " e-folds", col1_list);
            this->make_data_element("Maximum refinements", boost::lexical_cast<std::string>(tk.get_max_refinements()), col1_list);
    
            try
              {
                this->make_data_element("End of inflation", format_number(tk.get_N_end_of_inflation(), this->efolds_precision) + " e-folds", col2_list);
              }
            catch(end_of_inflation_not_found& xe)
              {
                // silently ignore
              }
    
            this->make_data_element("ln <var>a</var><sub>*</sub>", format_number(tk.get_astar_normalization(), this->misc_precision), col2_list);
            this->make_data_element("Adaptive ICs", tk.get_adaptive_ics() ? format_number(tk.get_adaptive_ics_efolds(), this->efolds_precision) + " e-folds" : "No", col2_list);
            this->make_data_element("Collect ICs", tk.get_collect_initial_conditions() ? "Yes" : "No", col2_list);

            const time_config_database& time_db = tk.get_stored_time_config_database();
            HTML_node t_dt("dt", "Time samples");
            HTML_node t_dd("dd");
            HTML_string t_str(boost::lexical_cast<std::string>(time_db.size()));
            t_dd.add_element(t_str);
            if(time_db.size() <= CPPTRANSPORT_DEFAULT_HTML_DATABASE_MAX_SIZE) this->write_time_db_modal(bundle, time_db, t_dd);
            col3_list.add_element(t_dt).add_element(t_dd);

            const twopf_kconfig_database& twopf_db = tk.get_twopf_database();
            HTML_node k_dt("dt", "2pf configurations");
            HTML_node k_dd("dd");
            HTML_string k_str(boost::lexical_cast<std::string>(twopf_db.size()));
            k_dd.add_element(k_str);
            if(twopf_db.size() <= CPPTRANSPORT_DEFAULT_HTML_DATABASE_MAX_SIZE) this->write_kconfig_db_modal(bundle, twopf_db, k_dd);
            col3_list.add_element(k_dt).add_element(k_dd);
          }


        void HTML_report::aria_close_button(HTML_node& parent)
          {
            HTML_node button("button");
            button.add_attribute("class", "close").add_attribute("data-dismiss", "modal");
            button.add_attribute("aria-label", "Close");

            HTML_node span("span", "&times;");
            span.add_attribute("aria-hidden", "true");
            button.add_element(span);

            parent.add_element(button);
          }


        template <typename number>
        void HTML_report::write_time_db_modal(HTML_report_bundle<number>& bundle, const time_config_database& db,
                                              HTML_node& parent)
          {
            std::string button_id = this->make_button_tag();
            std::string modal_id = button_id + "modal";
            std::string modal_label= button_id + "label";

            HTML_node button("button", "(show)");
            button.add_attribute("class", "btn btn-link").add_attribute("data-toggle", "modal");
            button.add_attribute("data-target", "#" + modal_id);
            parent.add_element(button);

            HTML_node modal("div");
            modal.add_attribute("class", "modal fade").add_attribute("id", modal_id).add_attribute("tabindex", "-1");
            modal.add_attribute("role", "dialog").add_attribute("aria-labelledby", modal_label);

            HTML_node dialog("div");
            dialog.add_attribute("class", "modal-dialog modal-lg").add_attribute("role", "document");

            HTML_node content("div");
            content.add_attribute("class", "modal-content");

            HTML_node header("div");
            header.add_attribute("class", "modal-header");

            this->aria_close_button(header);

            HTML_node title("h4", "Time sample points");
            title.add_attribute("class", "modal-title").add_attribute("id", modal_label);

            HTML_node body("div");
            body.add_attribute("class", "modal-body");

            HTML_node table_wrapper("div");
            table_wrapper.add_attribute("class", "table-responsive");

            // generate table as shared_ptr<> to prevent long copy times if the label is large
            std::shared_ptr<HTML_node> table  = std::make_shared<HTML_node>("table");
            if(db.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
              {
                table->add_attribute("class", "table table-striped table-condensed sortable-pageable");
              }
            else
              {
                table->add_attribute("class", "table table-striped table-condensed sortable");
              }

            HTML_node thead("thead");
            HTML_node head_row("tr");

            HTML_node serial_label("th", "Serial");
            HTML_node time_label("th", "Time");
            head_row.add_element(serial_label).add_element(time_label);
            thead.add_element(head_row);

            std::shared_ptr<HTML_node> tbody = std::make_shared<HTML_node>("tbody");
            for(time_config_database::const_config_iterator t = db.config_cbegin(); t != db.config_cend(); ++t)
              {
                std::shared_ptr<HTML_node> trow = std::make_shared<HTML_node>("tr");
                HTML_node serial("td", boost::lexical_cast<std::string>(t->serial));
                HTML_node time("td", format_number(t->t, this->database_precision));
                trow->add_element(serial).add_element(time);
                tbody->add_element(trow);
              }
            table->add_element(thead).add_element(tbody);
            table_wrapper.add_element(table);
            body.add_element(table_wrapper);

            HTML_node footer("div");
            footer.add_attribute("class", "modal-footer");

            HTML_node dismiss("button", "Close");
            dismiss.add_attribute("type", "button").add_attribute("class", "btn btn-default").add_attribute("data-dismiss", "modal");

            header.add_element(title);
            footer.add_element(dismiss);
            content.add_element(header).add_element(body).add_element(footer);
            dialog.add_element(content);
            modal.add_element(dialog);

            bundle.emplace_modal(modal);
          }


        template <typename number>
        void HTML_report::write_kconfig_db_modal(HTML_report_bundle<number>& bundle, const twopf_kconfig_database& db,
                                                 HTML_node& parent)
          {
            std::string button_id = this->make_button_tag();
            std::string modal_id = button_id + "modal";
            std::string modal_label= button_id + "label";

            HTML_node button("button", "(show)");
            button.add_attribute("class", "btn btn-link").add_attribute("data-toggle", "modal");
            button.add_attribute("data-target", "#" + modal_id);
            parent.add_element(button);

            HTML_node modal("div");
            modal.add_attribute("class", "modal fade").add_attribute("id", modal_id).add_attribute("tabindex", "-1");
            modal.add_attribute("role", "dialog").add_attribute("aria-labelledby", modal_label);

            HTML_node dialog("div");
            dialog.add_attribute("class", "modal-dialog modal-lg").add_attribute("role", "document");

            HTML_node content("div");
            content.add_attribute("class", "modal-content");

            HTML_node header("div");
            header.add_attribute("class", "modal-header");

            this->aria_close_button(header);

            HTML_node title("h4", "2pf-configuration samples");
            title.add_attribute("class", "modal-title").add_attribute("id", modal_label);

            HTML_node body("div");
            body.add_attribute("class", "modal-body");

            HTML_node table_wrapper("div");
            table_wrapper.add_attribute("class", "table-responsive");

            // generate table as shared_ptr<> to prevent long copy times if the label is large
            std::shared_ptr<HTML_node> table = std::make_shared<HTML_node>("table");
            if(db.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
              {
                table->add_attribute("class", "table table-striped table-condensed sortable-pageable");
              }
            else
              {
                table->add_attribute("class", "table table-striped table-condensed sortable");
              }

            HTML_node thead("thead");
            HTML_node head_row("tr");

            HTML_node serial_label("th", "Serial");
            HTML_node k_label("th", "k");
            HTML_node texit_label("th", "t<sub>exit</sub>");
            HTML_node tmassless_label("th", "t<sub>massless</sub>");
            head_row.add_element(serial_label).add_element(k_label).add_element(texit_label).add_element(tmassless_label);
            thead.add_element(head_row);

            std::shared_ptr<HTML_node> tbody = std::make_shared<HTML_node>("tbody");
            for(twopf_kconfig_database::const_config_iterator t = db.config_cbegin(); t != db.config_cend(); ++t)
              {
                std::shared_ptr<HTML_node> trow = std::make_shared<HTML_node>("tr");
                HTML_node serial("td", boost::lexical_cast<std::string>(t->serial));
                HTML_node k("td", format_number(t->k_conventional, this->database_precision));
                HTML_node texit("td", format_number(t->t_exit, this->database_precision));
                HTML_node tmassless("td", format_number(t->t_massless, this->database_precision));
                trow->add_element(serial).add_element(k).add_element(texit).add_element(tmassless);
                tbody->add_element(trow);
              }
            table->add_element(thead).add_element(tbody);
            table_wrapper.add_element(table);
            body.add_element(table_wrapper);

            HTML_node footer("div");
            footer.add_attribute("class", "modal-footer");

            HTML_node dismiss("button", "Close");
            dismiss.add_attribute("type", "button").add_attribute("class", "btn btn-default").add_attribute("data-dismiss", "modal");

            header.add_element(title);
            footer.add_element(dismiss);
            content.add_element(header).add_element(body).add_element(footer);
            dialog.add_element(content);
            modal.add_element(dialog);

            bundle.emplace_modal(modal);
          }


        template <typename number>
        void HTML_report::write_kconfig_db_modal(HTML_report_bundle<number>& bundle, const threepf_kconfig_database& db,
                                                 HTML_node& parent)
          {
            std::string button_id = this->make_button_tag();
            std::string modal_id = button_id + "modal";
            std::string modal_label= button_id + "label";

            HTML_node button("button", "(show)");
            button.add_attribute("class", "btn btn-link").add_attribute("data-toggle", "modal");
            button.add_attribute("data-target", "#" + modal_id);
            parent.add_element(button);

            HTML_node modal("div");
            modal.add_attribute("class", "modal fade").add_attribute("id", modal_id).add_attribute("tabindex", "-1");
            modal.add_attribute("role", "dialog").add_attribute("aria-labelledby", modal_label);

            HTML_node dialog("div");
            dialog.add_attribute("class", "modal-dialog modal-lg").add_attribute("role", "document");

            HTML_node content("div");
            content.add_attribute("class", "modal-content");

            HTML_node header("div");
            header.add_attribute("class", "modal-header");

            this->aria_close_button(header);

            HTML_node title("h4", "3pf-configuration samples");
            title.add_attribute("class", "modal-title").add_attribute("id", modal_label);

            HTML_node body("div");
            body.add_attribute("class", "modal-body");

            HTML_node table_wrapper("div");
            table_wrapper.add_attribute("class", "table-responsive");

            // generate table as shared_ptr<> to prevent long copy times if the label is large
            std::shared_ptr<HTML_node> table = std::make_shared<HTML_node>("table");
            if(db.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
              {
                table->add_attribute("class", "table table-striped table-condensed sortable-pageable");
              }
            else
              {
                table->add_attribute("class", "table table-striped table-condensed sortable");
              }

            HTML_node thead("thead");
            HTML_node head_row("tr");

            HTML_node serial_label("th", "Serial");
            HTML_node k_label("th", "k");
            HTML_node alpha_label("th", "&alpha;");
            HTML_node beta_label("th", "&beta;");
            HTML_node k1_label("th", "k<sub>1</sub>");
            HTML_node k2_label("th", "k<sub>2</sub>");
            HTML_node k3_label("th", "k<sub>3</sub>");
            HTML_node texit_label("th", "t<sub>exit</sub>");
            HTML_node tmassless_label("th", "t<sub>massless</sub>");
            head_row.add_element(serial_label).add_element(k_label).add_element(alpha_label).add_element(beta_label);
            head_row.add_element(k1_label).add_element(k2_label).add_element(k3_label);
            head_row.add_element(texit_label).add_element(tmassless_label);
            thead.add_element(head_row);

            std::shared_ptr<HTML_node> tbody = std::make_shared<HTML_node>("tbody");
            for(threepf_kconfig_database::const_config_iterator t = db.config_cbegin(); t != db.config_cend(); ++t)
              {
                std::shared_ptr<HTML_node> trow = std::make_shared<HTML_node>("tr");
                HTML_node serial("td", boost::lexical_cast<std::string>(t->serial));
                HTML_node k("td", format_number(t->kt_conventional, this->database_precision));
                HTML_node alpha("td", format_number(t->alpha, this->database_precision));
                HTML_node beta("td", format_number(t->beta, this->database_precision));
                HTML_node k1("td", format_number(t->k1_conventional, this->database_precision));
                HTML_node k2("td", format_number(t->k2_conventional, this->database_precision));
                HTML_node k3("td", format_number(t->k3_conventional, this->database_precision));
                HTML_node texit("td", format_number(t->t_exit, this->database_precision));
                HTML_node tmassless("td", format_number(t->t_massless, this->database_precision));
                trow->add_element(serial).add_element(k).add_element(alpha).add_element(beta);
                trow->add_element(k1).add_element(k2).add_element(k3);
                trow->add_element(texit).add_element(tmassless);
                tbody->add_element(trow);
              }
            table->add_element(thead).add_element(tbody);
            table_wrapper.add_element(table);
            body.add_element(table_wrapper);

            HTML_node footer("div");
            footer.add_attribute("class", "modal-footer");

            HTML_node dismiss("button", "Close");
            dismiss.add_attribute("type", "button").add_attribute("class", "btn btn-default").add_attribute("data-dismiss", "modal");

            header.add_element(title);
            footer.add_element(dismiss);
            content.add_element(header).add_element(body).add_element(footer);
            dialog.add_element(content);
            modal.add_element(dialog);

            bundle.emplace_modal(modal);
          }


        template <typename number>
        void HTML_report::write_postintegration(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent)
          {
            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::postintegration)
                  {
                    const postintegration_task_record<number>& rec = dynamic_cast< const postintegration_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node pane("div");
                    pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                    HTML_node list("ul");
                    list.add_attribute("class", "list-group");

                    HTML_node item("li");
                    item.add_attribute("class", "list-group-item");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

                    HTML_node panel("div");
                    panel.add_attribute("class", "panel panel-default");

                    HTML_node panel_heading("div", "Task information");
                    panel_heading.add_attribute("class", "panel-heading");

                    HTML_node panel_body("div");
                    panel_body.add_attribute("class", "panel-body");

                    const std::string& description = rec.get_task()->get_description();
                    if(!description.empty())
                      {
                        HTML_node well("div", description);
                        well.add_attribute("class", "well");
                        panel_body.add_element(well);
                      }

                    HTML_node row("div");
                    row.add_attribute("class", "row");

                    HTML_node col1("div");
                    col1.add_attribute("class", "col-md-4");
                    HTML_node col1_list("dl");
                    col1_list.add_attribute("class", "dl-horizontal");

                    this->make_data_element("Task type",
                                            task_type_to_string(rec.get_task_type()), col1_list);

                    col1.add_element(col1_list);

                    HTML_node col2("div");
                    col2.add_attribute("class", "col-md-4");
                    HTML_node col2_list("dl");
                    col2_list.add_attribute("class", "dl-horizontal");

                    HTML_node parent_tk_dt("dt", "Parent task");
                    HTML_node parent_tk_dd("dd");
                    this->write_task_button(bundle, rec.get_task()->get_parent_task()->get_name(), parent_tk_dd);
                    col2_list.add_element(parent_tk_dt).add_element(parent_tk_dd);

                    col2.add_element(col2_list);

                    HTML_node col3("div");
                    col3.add_attribute("class", "col-md-4");

                    row.add_element(col1).add_element(col2).add_element(col3);
                    panel_body.add_element(row);
                    panel.add_element(panel_heading).add_element(panel_body);
                    item.add_element(panel);

                    // write table of content groups, if any are present
                    const std::list<std::string>& content_groups = rec.get_content_groups();
                    if(!content_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target",
                                                                                      "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Content groups", content_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Content groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        HTML_node table("table");

                        if(content_groups.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                          {
                            table_wrapper.add_attribute("class", "table-responsive padded");
                            table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                          }
                        else
                          {
                            table_wrapper.add_attribute("class", "table-responsive");
                            table.add_attribute("class", "table table-striped table-condensed sortable");
                          }

                        HTML_node head("thead");
                        HTML_node head_row("tr");

                        HTML_node name_label("th", "Name");
                        HTML_node created_label("th", "Created");
                        HTML_node edited_label("th", "Last updated");
                        HTML_node complete_label("th", "Complete");
                        HTML_node size_label("th", "Size");

                        head_row.add_element(name_label).add_element(created_label).add_element(edited_label).add_element(complete_label).add_element(size_label);
                        head.add_element(head_row);

                        HTML_node body("tbody");

                        postintegration_content_db& content_db = bundle.get_postintegration_content_db();
                        for(const std::string& group : content_groups)
                          {
                            postintegration_content_db::const_iterator t = content_db.find(group);

                            HTML_node table_row("tr");

                            if(t != content_db.end())
                              {
                                HTML_node name("td");
                                this->write_content_button(bundle, group, name);

                                HTML_node created("td", boost::posix_time::to_simple_string(t->second->get_creation_time()));
                                HTML_node edited("td", boost::posix_time::to_simple_string(t->second->get_last_edit_time()));
                                HTML_node complete("td", (t->second->get_payload().is_failed() ? "No" : "Yes"));
                                HTML_node size("td", format_memory(t->second->get_payload().get_size()));

                                table_row.add_element(name).add_element(created).add_element(edited).add_element(complete).add_element(size);
                              }
                            else
                              {
                                HTML_node name("td");
                                HTML_node label("span", group);
                                label.add_attribute("class", "label label-danger");
                                name.add_element(label);

                                HTML_node null("td");
                                HTML_node null_label("span", "NULL");
                                null_label.add_attribute("class", "label label-danger");
                                null.add_element(null_label);

                                table_row.add_element(name).add_element(null).add_element(null).add_element(null).add_element(null);
                              }

                            body.add_element(table_row);
                          }

                        table.add_element(head).add_element(body);
                        table_wrapper.add_element(table);
                        tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                        group_list.add_element(tbl_panel);
                        item.add_element(button).add_element(group_list);
                      }

                    list.add_element(item);
                    pane.add_element(list);
                    parent.add_element(pane);
                  }
              }
          }


        template <typename number>
        void HTML_report::write_output(HTML_report_bundle<number>& bundle, const typename task_db<number>::type& db, HTML_node& parent)
          {
            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::output)
                  {
                    const output_task_record<number>& rec = dynamic_cast< const output_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node pane("div");
                    pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                    HTML_node list("ul");
                    list.add_attribute("class", "list-group");

                    HTML_node item("li");
                    item.add_attribute("class", "list-group-item");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

                    const std::string& description = rec.get_task()->get_description();
                    if(!description.empty())
                      {
                        HTML_node well("div", description);
                        well.add_attribute("class", "well");
                        item.add_element(well);
                      }

                    // write elements
                    const typename std::vector< output_task_element<number> >& elements = rec.get_task()->get_elements();
                    if(!elements.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "elements");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Derived products", elements.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "elements").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Derived products used by this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        HTML_node table("table");

                        if(elements.size() > 10)
                          {
                            table_wrapper.add_attribute("class", "table-responsive padded");
                            table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                          }
                        else
                          {
                            table_wrapper.add_attribute("class", "table-responsive");
                            table.add_attribute("class", "table table-striped table-condensed sortable");
                          }

                        HTML_node head("thead");
                        HTML_node head_row("tr");

                        HTML_node name_label("th", "Name");
                        HTML_node type_label("th", "Type");
                        HTML_node requires_label("th", "Requires tags");
                        HTML_node filename_label("th", "Filename");

                        head_row.add_element(name_label).add_element(type_label).add_element(requires_label).add_element(filename_label);
                        head.add_element(head_row);

                        HTML_node body("tbody");

                        for(const output_task_element<number>& element : elements)
                          {
                            HTML_node row("tr");

                            HTML_node name("td");
                            this->write_derived_product_button(bundle, element.get_product_name(), name);

                            HTML_node type("td", derived_data::derived_product_type_to_string(element.get_product().get_type()));
                            HTML_node requires("td");
                            this->compose_tag_list(element.get_tags(), requires);

                            HTML_node filename("td", element.get_product().get_filename().string());

                            row.add_element(name).add_element(type).add_element(requires).add_element(filename);
                            body.add_element(row);
                          }

                        table.add_element(head).add_element(body);
                        table_wrapper.add_element(table);
                        tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                        group_list.add_element(tbl_panel);
                        item.add_element(button).add_element(group_list);
                      }

                    // write table of content groups
                    const std::list<std::string>& content_groups = rec.get_content_groups();
                    if(!content_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target",
                                                                                      "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Content groups", content_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Content groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        HTML_node table("table");

                        if(content_groups.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                          {
                            table_wrapper.add_attribute("class", "table-responsive padded");
                            table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                          }
                        else
                          {
                            table_wrapper.add_attribute("class", "table-responsive");
                            table.add_attribute("class", "table table-striped table-condensed sortable");
                          }

                        HTML_node head("thead");
                        HTML_node head_row("tr");

                        HTML_node name_label("th", "Name");
                        HTML_node created_label("th", "Created");
                        HTML_node edited_label("th", "Last updated");

                        head_row.add_element(name_label).add_element(created_label).add_element(edited_label);
                        head.add_element(head_row);

                        HTML_node body("tbody");

                        output_content_db& content_db = bundle.get_output_content_db();
                        for(const std::string& group : content_groups)
                          {
                            output_content_db::const_iterator t = content_db.find(group);

                            HTML_node row("tr");

                            if(t != content_db.end())
                              {
                                HTML_node name("td");
                                this->write_content_button(bundle, group, name);

                                HTML_node created("td", boost::posix_time::to_simple_string(t->second->get_creation_time()));
                                HTML_node edited("td", boost::posix_time::to_simple_string(t->second->get_last_edit_time()));

                                row.add_element(name).add_element(created).add_element(edited);
                              }
                            else
                              {
                                HTML_node name("td");
                                HTML_node label("span", group);
                                label.add_attribute("class", "label label-danger");
                                name.add_element(label);

                                HTML_node null("td");
                                HTML_node null_label("span", "NULL");
                                null_label.add_attribute("class", "label label-danger");
                                null.add_element(null_label);

                                row.add_element(name).add_element(null).add_element(null);
                              }

                            body.add_element(row);
                          }

                        table.add_element(head).add_element(body);
                        table_wrapper.add_element(table);
                        tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                        group_list.add_element(tbl_panel);
                        item.add_element(button).add_element(group_list);
                      }

                    list.add_element(item);
                    pane.add_element(list);
                    parent.add_element(pane);
                  }
              }
          }


        template <typename number>
        void HTML_report::write_derived_products(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename derived_product_db<number>::type& db = bundle.get_derived_product_db();
            typename task_db<number>::type& tk_db = bundle.get_task_db();

            for(const typename derived_product_db<number>::value_type& product : db)
              {
                const derived_product_record<number>& rec = *product.second;
                const std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("ul");
                list.add_attribute("class", "list-group");

                HTML_node anchor("li");
                anchor.add_attribute("class", "list-group-item");

                this->make_list_item_label(rec.get_name(), anchor);
                this->write_generic_record(bundle, rec, anchor);

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");

                HTML_node panel_heading("div", "Derived product information");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");

                const std::string& description = rec.get_product()->get_description();
                if(!description.empty())
                  {
                    HTML_node well("div", description);
                    well.add_attribute("class", "well");
                    panel_body.add_element(well);
                  }

                switch(rec.get_product()->get_type())
                  {
                    case derived_data::derived_product_type::line_2dplot:
                      {
                        const derived_data::line_plot2d<number>& item = dynamic_cast< const derived_data::line_plot2d<number>& >(*rec.get_product());
                        this->write_derived_product(bundle, rec, item, panel_body);
                        break;
                      }

                    case derived_data::derived_product_type::line_table:
                      {
                        const derived_data::line_asciitable<number>& item = dynamic_cast< const derived_data::line_asciitable<number>& >(*rec.get_product());
                        this->write_derived_product(bundle, rec, item, panel_body);
                        break;
                      }
                  }

                panel.add_element(panel_heading).add_element(panel_body);
                anchor.add_element(panel);

                typename std::list< derivable_task<number>* > task_list;
                rec.get_product()->get_task_list(task_list);

                if(!task_list.empty())
                  {
                    HTML_node button("button");
                    button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "tasks");
                    button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                    this->make_badged_text("Depends on tasks", task_list.size(), button);

                    HTML_node group_list("div");
                    group_list.add_attribute("id", tag + "tasks").add_attribute("class", "collapse");

                    HTML_node tbl_panel("div");
                    tbl_panel.add_attribute("class", "panel panel-info topskip");

                    HTML_node tbl_panel_head("div", "Tasks used by this derived product");
                    tbl_panel_head.add_attribute("class", "panel-heading");

                    HTML_node table_wrapper("div");
                    HTML_node table("table");

                    if(task_list.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                      {
                        table_wrapper.add_attribute("class", "table-responsive padded");
                        table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                      }
                    else
                      {
                        table_wrapper.add_attribute("class", "table-responsive");
                        table.add_attribute("class", "table table-striped table-condensed sortable");
                      }

                    HTML_node head("thead");
                    HTML_node head_row("tr");

                    HTML_node name_label("th", "Name");
                    HTML_node type_label("th", "Type");
                    HTML_node edited_label("th", "Last updated");

                    head_row.add_element(name_label);
                    head_row.add_element(type_label);
                    head_row.add_element(edited_label);
                    head.add_element(head_row);

                    HTML_node body("tbody");

                    for(derivable_task<number>* tk : task_list)
                      {
                        HTML_node table_row("tr");

                        if(tk != nullptr)
                          {
                            typename task_db<number>::type::const_iterator t = tk_db.find(tk->get_name());

                            if(t != tk_db.end())
                              {

                                HTML_node name("td");
                                this->write_task_button(bundle, tk->get_name(), name);

                                HTML_node type("td", task_type_to_string(tk->get_type()));
                                HTML_node edited("td", boost::posix_time::to_simple_string(t->second->get_last_edit_time()));

                                table_row.add_element(name);
                                table_row.add_element(type);
                                table_row.add_element(edited);
                              }
                          }
                        body.add_element(table_row);
                      }

                    table.add_element(head).add_element(body);
                    table_wrapper.add_element(table);
                    tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                    group_list.add_element(tbl_panel);
                    anchor.add_element(button).add_element(group_list);
                  }

                list.add_element(anchor);
                pane.add_element(list);
                parent.add_element(pane);
              }
          }


        template <typename number>
        void HTML_report::write_derived_product(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                                const derived_data::line_plot2d<number>& product, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->write_generic_derived_product(bundle, product, col1_list, col2_list, col3_list);
            this->write_line_collection(bundle, product, col1_list, col2_list, col3_list);

            this->make_data_element("Reverse x-axis", product.get_reverse_x() ? "Yes" : "No", col1_list);
            this->make_data_element("Reverse y-axis", product.get_reverse_y() ? "Yes" : "No", col1_list);

            HTML_node tdt("dt", "Title");
            HTML_node tdd("dd");
            if(product.get_title() && !product.get_title_text().empty())
              {
                HTML_node s("code", product.get_title_text(), true, false);
                s.add_attribute("class", "language-latex");
                tdd.add_element(s);
              }
            else
              {
                HTML_node s("span", "NULL");
                s.add_attribute("class", "label label-default");
                tdd.add_element(s);
              }
            col2_list.add_element(tdt).add_element(tdd);

            HTML_node xdt("dt", "x-axis label");
            HTML_node xdd("dd");
            if(product.get_x_label() && !product.get_x_label_text().empty())
              {
                HTML_node s("code", product.get_x_label_text(), true, false);
                s.add_attribute("class", "language-latex");
                xdd.add_element(s);
              }
            else
              {
                HTML_node s("span", "NULL");
                s.add_attribute("class", "label label-default");
                xdd.add_element(s);
              }
            col2_list.add_element(xdt).add_element(xdd);

            HTML_node ydt("dt", "y-axis label");
            HTML_node ydd("dd");
            if(product.get_y_label() && !product.get_y_label_text().empty())
              {
                HTML_node s("code", product.get_y_label_text(), true, false);
                s.add_attribute("class", "language-latex");
                ydd.add_element(s);
              }
            else
              {
                HTML_node s("span", "NULL");
                s.add_attribute("class", "label label-default");
                ydd.add_element(s);
              }
            col2_list.add_element(ydt).add_element(ydd);

            this->make_data_element("Legend", product.get_legend() ? derived_data::legend_pos_to_string(product.get_legend_position()) : "No", col3_list);
            this->make_data_element("LaTeX typsetting", product.get_typeset_with_LaTeX() ? "Yes" : "No", col3_list);
            this->make_data_element("Dash second axis", product.get_dash_second_axis() ? "Yes" :  "No", col3_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);

            this->write_line_list(bundle, rec, product, parent);
          }


        template <typename number>
        void HTML_report::write_derived_product(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                                const derived_data::line_asciitable<number>& product, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->write_generic_derived_product(bundle, product, col1_list, col2_list, col3_list);
            this->write_line_collection(bundle, product, col1_list, col2_list, col3_list);

            this->make_data_element("x-axis label", product.get_x_label(), col3_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);

            this->write_line_list(bundle, rec, product, parent);
          }


        template <typename number>
        void HTML_report::write_generic_derived_product(HTML_report_bundle<number>& bundle, const derived_data::derived_product<number>& product,
                                                        HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->make_data_element("Product type", derived_data::derived_product_type_to_string(product.get_type()), col1_list);
            this->make_data_element("Filename", product.get_filename().string(), col2_list);
          }


        template <typename number>
        void HTML_report::write_line_collection(HTML_report_bundle<number>& bundle, const derived_data::line_collection<number>& product,
                                                HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->make_data_element("x-axis values", derived_data::axis_value_to_string(product.get_x_axis_value()), col1_list);
            this->make_data_element("LaTeX labels", product.get_use_LaTeX() ? "Yes" : "No", col1_list);

            this->make_data_element("log x values", product.get_log_x() ? "Yes" : "No", col2_list);

            this->make_data_element("log y values", product.get_log_y() ? "Yes" : "No", col3_list);
            this->make_data_element("abs y values", product.get_abs_y() ? "Yes" : "No", col3_list);
          }


        template <typename number>
        void HTML_report::write_line_list(HTML_report_bundle<number>& bundle, const derived_product_record<number>& rec,
                                          const derived_data::line_collection<number>& line_collection, HTML_node& parent)
          {
            const std::list< std::unique_ptr< derived_data::derived_line<number> > >& lines = line_collection.get_lines();

            if(lines.empty()) return;

            std::string tag = bundle.get_id(rec);

            HTML_node button("button");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "lines");
            button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
            this->make_badged_text("Derived lines", lines.size(), button);

            HTML_node content("div");
            content.add_attribute("id", tag + "lines").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info topskip");

            HTML_node panel_head("div", "Derived lines in this collection");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node line_list("ul");
            line_list.add_attribute("class", "list-group");

            unsigned int count = 0;
            for(const std::unique_ptr< derived_data::derived_line<number> >& line : lines)
              {
                HTML_node item("li");
                item.add_attribute("class", "list-group-item");

                std::ostringstream lbl_text;
                lbl_text << "Line " << ++count;
                HTML_node lbl("span", lbl_text.str());
                lbl.add_attribute("class", "label label-primary");
                item.add_element(lbl);

                if(line)
                  {
                    try
                      {
                        switch(line->get_line_type())
                          {
                            case derived_data::derived_line_type::background:
                              {
                                const derived_data::background_time_series<number>& ln = dynamic_cast< derived_data::background_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::twopf_time:
                              {
                                const derived_data::twopf_time_series<number>& ln = dynamic_cast< derived_data::twopf_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::threepf_time:
                              {
                                const derived_data::threepf_time_series<number>& ln = dynamic_cast< derived_data::threepf_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::tensor_twopf_time:
                              {
                                const derived_data::tensor_twopf_time_series<number>& ln = dynamic_cast< derived_data::tensor_twopf_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_twopf_time:
                              {
                                const derived_data::zeta_twopf_time_series<number>& ln = dynamic_cast< derived_data::zeta_twopf_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_threepf_time:
                              {
                                const derived_data::zeta_threepf_time_series<number>& ln = dynamic_cast< derived_data::zeta_threepf_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_redbsp_time:
                              {
                                const derived_data::zeta_reduced_bispectrum_time_series<number>& ln = dynamic_cast< derived_data::zeta_reduced_bispectrum_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::twopf_wavenumber:
                              {
                                const derived_data::twopf_wavenumber_series<number>& ln = dynamic_cast< derived_data::twopf_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::threepf_wavenumber:
                              {
                                const derived_data::threepf_wavenumber_series<number>& ln = dynamic_cast< derived_data::threepf_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::tensor_twopf_wavenumber:
                              {
                                const derived_data::tensor_twopf_wavenumber_series<number>& ln = dynamic_cast< derived_data::tensor_twopf_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_twopf_wavenumber:
                              {
                                const derived_data::zeta_twopf_wavenumber_series<number>& ln = dynamic_cast< derived_data::zeta_twopf_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_threepf_wavenumber:
                              {
                                const derived_data::zeta_threepf_wavenumber_series<number>& ln = dynamic_cast< derived_data::zeta_threepf_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::zeta_redbsp_wavenumber:
                              {
                                const derived_data::zeta_reduced_bispectrum_wavenumber_series<number>& ln = dynamic_cast< derived_data::zeta_reduced_bispectrum_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::fNL_time:
                              {
                                const derived_data::fNL_time_series<number>& ln = dynamic_cast< derived_data::fNL_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::r_time:
                              {
                                const derived_data::r_time_series<number>& ln = dynamic_cast< derived_data::r_time_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::r_wavenumber:
                              {
                                const derived_data::r_wavenumber_series<number>& ln = dynamic_cast< derived_data::r_wavenumber_series<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::background_line:
                              {
                                const derived_data::background_line<number>& ln = dynamic_cast< derived_data::background_line<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::u2:
                              {
                                const derived_data::u2_line<number>& ln = dynamic_cast< derived_data::u2_line<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::u3:
                              {
                                const derived_data::u3_line<number>& ln = dynamic_cast< derived_data::u3_line<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::largest_u2:
                              {
                                const derived_data::largest_u2_line<number>& ln = dynamic_cast< derived_data::largest_u2_line<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::largest_u3:
                              {
                                const derived_data::largest_u3_line<number>& ln = dynamic_cast< derived_data::largest_u3_line<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }

                            case derived_data::derived_line_type::integration_cost:
                              {
                                const derived_data::cost_wavenumber<number>& ln = dynamic_cast< derived_data::cost_wavenumber<number>& >(*line);
                                this->write_derived_line(bundle, ln, item);
                                break;
                              }
                          }
                      }
                    catch(std::bad_cast& xe)
                      {
                        std::ostringstream msg;
                        msg << CPPTRANSPORT_REPO_RECORD_CAST_FAILED << " [" << rec.get_name() << " : " << count << "]";
                        this->err(msg.str());
                      }

                  }
                line_list.add_element(item);
              }

            panel.add_element(panel_head).add_element(line_list);
            content.add_element(panel);
            parent.add_element(button).add_element(content);
          }


        template <typename number>
        void HTML_report::write_wavenumber_series_line(HTML_report_bundle<number>& bundle, const derived_data::wavenumber_series<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Spectral index", line.is_spectral_index() ? "Yes" : "No", col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("2pf type", derived_data::twopf_type_to_string(line.get_type()), col1_list);
            this->make_data_element("Dimensionless", line.is_dimensionless() ? "Yes" : "No", col2_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_threepf_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Dimensionless", line.is_dimensionless() ? "Yes" : "No", col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_zeta_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Dimensionless", line.is_dimensionless() ? "Yes" : "No", col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_zeta_threepf_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Dimensionless", line.is_dimensionless() ? "Yes" : "No", col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_zeta_redbsp_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_line<number>& line, HTML_node& parent)
          {
            // not currently used
          }


        template <typename number>
        void HTML_report::write_tensor_twopf_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Dimensionless", line.is_dimensionless() ? "Yes" : "No", col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_fNL_line(HTML_report_bundle<number>& bundle, const derived_data::fNL_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Template", derived_data::template_type_to_string(line.get_template()), col1_list);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_r_line(HTML_report_bundle<number>& bundle, const derived_data::r_line<number>& line, HTML_node& parent)
          {
            // not currently used
          }


        void HTML_report::write_SQL_block(const std::string& SQL, HTML_node& parent)
          {
            HTML_node pre("pre");

            HTML_node code("code", SQL, true, false);
            code.add_attribute("class", "language-sql");
            pre.add_element(code);

            parent.add_element(pre);
          }


        void HTML_report::write_SQL_panel(std::string title, const std::string& SQL, HTML_node& parent)
          {
            HTML_node t("h6", title);
            t.add_attribute("class", "topskip-small");
            parent.add_element(t);

            this->write_SQL_block(SQL, parent);
          }


        template <typename number>
        void HTML_report::write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_time_query& query, HTML_node& parent)
          {
            this->write_SQL_panel("SQL query for time sample points", query.get_query_string(), parent);
          }


        template <typename number>
        void HTML_report::write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_twopf_query& query, HTML_node& parent)
          {
            this->write_SQL_panel("SQL query for 2-point function momentum-configuration points", query.get_query_string(), parent);
          }


        template <typename number>
        void HTML_report::write_SQL_query(HTML_report_bundle<number>& bundle, const derived_data::SQL_threepf_query& query, HTML_node& parent)
          {
            this->write_SQL_panel("SQL query for 3-point function momentum-configuration sample points", query.get_query_string(), parent);
          }


        void HTML_report::derived_line_title(std::string title, HTML_node& parent)
          {
            HTML_node t("h4", title);
            parent.add_element(t);
          }


        template <typename number>
        void HTML_report::write_generic_derived_line(HTML_report_bundle<number>& bundle, const derived_data::derived_line<number>& line, HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row topskip-small");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Derivatives/momenta", derived_data::dot_type_to_string(line.get_dot_meaning()), col1_list);
            this->make_data_element("k-labels", derived_data::klabel_type_to_string(line.get_klabel_meaning()), col2_list);
            this->make_data_element("Add identifiers to labels", line.get_label_tags() ? "Yes" : "No", col3_list);

            HTML_node tt("dt", "Data from task");
            HTML_node td("dd");
            derivable_task<number>* ptk = line.get_parent_task();
            if(ptk != nullptr)
              {
                this->write_task_button(bundle, ptk->get_name(), td);
                col1_list.add_element(tt).add_element(td);
              }

            HTML_node lt("dt", "Label");
            HTML_node ld("dd");
            if(line.is_label_set() && !line.get_non_LaTeX_label().empty())
              {
                HTML_node s("code", line.get_non_LaTeX_label(), true, false);
                s.add_attribute("class", "language-latex");
                ld.add_element(s);
              }
            else
              {
                HTML_node s("span", "DEFAULT");
                s.add_attribute("class", "label label-default");
                ld.add_element(s);
              }
            col2_list.add_element(lt).add_element(ld);

            col1.add_element(col1_list);
            col2.add_element(col2_list);
            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            parent.add_element(row);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::background_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("background &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("2-point function &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_twopf_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("3-point function &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_threepf_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("tensor 2-point function &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_tensor_twopf_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta 2-point function &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_twopf_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta 3-point function &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_threepf_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta reduced bispectrum &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_redbsp_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::twopf_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("2-point function &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_twopf_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::threepf_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("3-point function &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_threepf_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::tensor_twopf_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("3-point function &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_tensor_twopf_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_twopf_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta 2-point function &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_twopf_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_threepf_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta 3-point function &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_threepf_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::zeta_reduced_bispectrum_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("zeta reduced bispectrum &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_zeta_redbsp_line(bundle, line, parent);
            this->write_wavenumber_series_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::fNL_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("f<sub>NL</sub>-like quantity &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_fNL_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::r_time_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("tensor-to-scalar ratio &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_r_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::r_wavenumber_series<number>& line, HTML_node& parent)
          {
            this->derived_line_title("tensor-to-scalar ratio &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_r_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::background_line<number>& line, HTML_node& parent)
          {
            this->derived_line_title("background quantity &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);

            HTML_node row("div");
            row.add_attribute("class", "row");
            HTML_node col("div");
            col.add_attribute("class", "col-md-12");
            this->make_data_element("Quantity", derived_data::background_quantity_to_string(line.get_quantity()), parent);
            row.add_element(col);
            parent.add_element(row);

            this->write_SQL_query(bundle, line.get_time_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::u2_line<number>& line, HTML_node& parent)
          {
            this->derived_line_title("u<sub>2</sub> tensor &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::u3_line<number>& line, HTML_node& parent)
          {
            this->derived_line_title("u<sub>3</sub> tensor &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::largest_u2_line<number>& line, HTML_node& parent)
          {
            this->derived_line_title("largest element of u<sub>2</sub> tensor &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::largest_u3_line<number>& line, HTML_node& parent)
          {
            this->derived_line_title("largest element of u<sub>3</sub> tensor &mdash; time data", parent);

            this->write_generic_derived_line(bundle, line, parent);
            this->write_SQL_query(bundle, line.get_time_query(), parent);
            this->write_SQL_query(bundle, line.get_k_query(), parent);
          }


        template <typename number>
        void HTML_report::write_derived_line(HTML_report_bundle<number>& bundle, const derived_data::cost_wavenumber<number>& line, HTML_node& parent)
          {
            this->derived_line_title("integration analysis &mdash; momentum-configuration data", parent);

            this->write_generic_derived_line(bundle, line, parent);
          }


        template <typename number>
        void HTML_report::write_integration_content(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            integration_content_db& db = bundle.get_integration_content_db();

            for(const integration_content_db::value_type& group : db)
              {
                const content_group_record<integration_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("ul");
                list.add_attribute("class", "list-group");

                HTML_node anchor("li");
                anchor.add_attribute("class", "list-group-item");

                this->make_list_item_label(rec.get_name(), anchor);
                this->write_generic_output_record(bundle, rec, anchor);

                const integration_payload& payload = rec.get_payload();

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");

                HTML_node panel_heading("div", "Summary");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");

                HTML_node ctr_row("div");
                ctr_row.add_attribute("class", "row");
                HTML_node ctr_col("div");
                ctr_col.add_attribute("class", "col-md-12");
                HTML_node ctr_list("dl");
                ctr_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Container", payload.get_container_path().string(), ctr_list);

                const integration_metadata& metadata = payload.get_metadata();

                HTML_node row("div");
                row.add_attribute("class", "row");

                HTML_node col1("div");
                col1.add_attribute("class", "col-md-4");
                HTML_node col1_list("dl");
                col1_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Complete", (payload.is_failed() ? "No" : "Yes"), col1_list);
                this->make_data_element("Workgroup", boost::lexical_cast<std::string>(payload.get_workgroup_number()), col1_list);
                this->make_data_element("Wallclock time", format_time(metadata.total_wallclock_time), col1_list);
                this->make_data_element("CPU time", format_time(metadata.total_integration_time), col1_list);
                this->make_data_element("Failures", boost::lexical_cast<std::string>(metadata.total_failures), col1_list);

                col1.add_element(col1_list);

                HTML_node col2("div");
                col2.add_attribute("class", "col-md-4");
                HTML_node col2_list("dl");
                col2_list.add_attribute("class", "dl-horizontal");

                HTML_node seeded_dt("dt", "Seeded");
                HTML_node seeded_dd("dd");
                this->write_seeded(bundle, payload, seeded_dd);
                col2_list.add_element(seeded_dt).add_element(seeded_dd);

                this->make_data_element("Statistics", (payload.has_statistics() ? "Yes" : "No"), col2_list);
                this->make_data_element("Initial conditions", (payload.has_initial_conditions() ? "Yes" : "No"), col2_list);
                this->make_data_element("Configurations", boost::lexical_cast<std::string>(metadata.total_configurations), col2_list);
                this->make_data_element("Data type", payload.get_data_type(), col2_list);

                col2.add_element(col2_list);

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");
                HTML_node col3_list("dl");
                col3_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Size", format_memory(payload.get_size()), col3_list);
                this->make_data_element("Shortest integration", format_time(metadata.global_min_integration_time), col3_list);
                this->make_data_element("Longest integration", format_time(metadata.global_max_integration_time), col3_list);
                if(metadata.total_configurations > 0)
                  {
                    this->make_data_element("Mean integration", format_time(metadata.total_integration_time / metadata.total_configurations), col3_list);
                  }
                else
                  {
                    this->make_data_element("Mean integration", "", col3_list);
                  }

                col3.add_element(col3_list);

                ctr_col.add_element(ctr_list);
                ctr_row.add_element(ctr_col);

                row.add_element(col1).add_element(col2).add_element(col3);

                panel_body.add_element(ctr_row).add_element(row);
                panel.add_element(panel_heading).add_element(panel_body);

                anchor.add_element(panel);

                this->write_worker_table(bundle, rec, anchor);
                if(rec.get_payload().has_statistics()) this->write_integration_analysis(bundle, rec, anchor);

                list.add_element(anchor);
                pane.add_element(list);
                parent.add_element(pane);
              }
          }


        template <typename number>
        void HTML_report::write_worker_table(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec, HTML_node& parent)
          {
            worker_information_db worker_db = bundle.read_worker_database(rec.get_payload().get_container_path());
            std::string tag = bundle.get_id(rec);

            HTML_node button("button");
            button.add_attribute("type", "button");

            if(worker_db.size() == 0)
              {
                button.add_attribute("class", "btn btn-info disabled");
                HTML_string label("Worker information");
                button.add_element(label);
                parent.add_element(button);
                return;
              }

            button.add_attribute("class", "btn btn-info");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "workers");
            this->make_badged_text("Worker information", worker_db.size(), button);

            HTML_node content("div");
            content.add_attribute("id", tag + "workers").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info topskip");

            HTML_node panel_head("div", "Worker summary for this content group");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node table_wrapper("div");
            HTML_node table("table");

            if(worker_db.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
              {
                table_wrapper.add_attribute("class", "table-responsive padded");
                table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
              }
            else
              {
                table_wrapper.add_attribute("class", "table-responsive");
                table.add_attribute("class", "table table-striped table-condensed sortable");
              }

            HTML_node head("thead");
            HTML_node head_row("tr");

            HTML_node identifier_label("th", "Identifier");
            identifier_label.add_attribute("data-toggle", "tooltip").add_attribute("data-container", "body");
            identifier_label.add_attribute("data-placement", "top").add_attribute("title", "workgroup, worker number");
            HTML_node hostname_label("th", "Hostname");
            HTML_node backend_label("th", "Backend");
            HTML_node backg_step_label("th", "Background");
            HTML_node backg_tol_label("th", "Tolerances");
            backg_tol_label.add_attribute("data-toggle", "tooltip").add_attribute("data-container", "body");
            backg_tol_label.add_attribute("data-placement", "top").add_attribute("title", "atol, rtol");
            HTML_node pert_step_label("th", "Perturbations");
            HTML_node pert_tol_label("th", "Tolerances");
            pert_tol_label.add_attribute("data-toggle", "tooltip").add_attribute("data-container", "body");
            pert_tol_label.add_attribute("data-placement", "top").add_attribute("title", "atol, rtol");
            HTML_node configurations_label("th", "Configurations");
            HTML_node os_name_label("th", "Operating system");
            HTML_node CPU_brand_label("th", "CPU");

            head_row.add_element(identifier_label).add_element(hostname_label).add_element(backend_label);
            head_row.add_element(backg_step_label).add_element(backg_tol_label);
            head_row.add_element(pert_step_label).add_element(pert_tol_label);

            // cache timing data if it is available
            // (used later to compute how many configurations were processed by each worker)
            boost::optional< timing_db > timing_data;
            boost::optional< count_list > counts;
            bool has_statistics = rec.get_payload().has_statistics();
            unsigned int configurations = 1;
            if(has_statistics)
              {
                head_row.add_element(configurations_label);

                // will throw an exception if statistics table cannot be found
                timing_data = bundle.read_timing_database(rec.get_payload().get_container_path());

                // bin data into an aggregate number of configurations processed per worker
                counts = this->count_configurations_per_worker(*timing_data);

                configurations = rec.get_payload().get_metadata().total_configurations;
              }

            head_row.add_element(os_name_label).add_element(CPU_brand_label);
            head.add_element(head_row);

            HTML_node body("tbody");

            for(const worker_information_db::value_type& worker : worker_db)
              {
                const worker_record& info = *worker.second;
                HTML_node table_row("tr");

                HTML_node identifier("td", boost::lexical_cast<std::string>(info.get_workgroup()) + ", " + boost::lexical_cast<std::string>(info.get_worker()));
                HTML_node hostname("td", info.get_hostname());
                HTML_node backend("td", info.get_backend());
                HTML_node backg_step("td", info.get_backg_stepper());
                HTML_node pert_step("td", info.get_pert_stepper());

                std::pair<double, double> btol = info.get_backg_tol();
                HTML_node backg_tol("td", format_number(btol.first, this->misc_precision) + ", " + format_number(btol.second, this->misc_precision));

                std::pair<double, double> ptol = info.get_pert_tol();
                HTML_node pert_tol("td", format_number(ptol.first, this->misc_precision) + ", " + format_number(ptol.second, this->misc_precision));

                HTML_node os_name("td", info.get_os_name());

                table_row.add_element(identifier).add_element(hostname).add_element(backend);
                table_row.add_element(backg_step).add_element(backg_tol);
                table_row.add_element(pert_step).add_element(pert_tol);

                if(has_statistics)
                  {
                    unsigned int count = (*counts)[std::make_pair(info.get_workgroup(), info.get_worker())];
                    double percent = configurations > 0 ? (100.0 * static_cast<double>(count) / static_cast<double>(configurations)) : 0.0;

                    HTML_node configs("td", boost::lexical_cast<std::string>(count) + " (" + format_number(percent) + "%)");
                    table_row.add_element(configs);
                  }

                table_row.add_element(os_name);
    
                const std::string CPU_id = info.get_cpu_brand_string();
                if(!CPU_id.empty())
                  {
                    HTML_node CPU_brand("td", CPU_id);
                    table_row.add_element(CPU_brand);
                  }
                else
                  {
                    HTML_node CPU_brand("td", "unknown");
                    CPU_brand.add_attribute("class", "disabled");
                    table_row.add_element(CPU_brand);
                  }

                body.add_element(table_row);
              }

            table.add_element(head).add_element(body);
            table_wrapper.add_element(table);
            panel.add_element(panel_head).add_element(table_wrapper);
            content.add_element(panel);
            parent.add_element(button).add_element(content);
          }


        HTML_report::count_list HTML_report::count_configurations_per_worker(timing_db& data)
          {
            count_list list;

            for(timing_db::value_type& record : data)
              {
                const timing_record& rec = *record.second;

                // get reference to count for this worker; if we haven't encountered this worker before
                // then a suitable entry is inserted.
                // This value associated with this entry is guaranteed to be zero-initialized
                unsigned int& count = list[std::make_pair(rec.get_workgroup(), rec.get_worker())];

                // increment it
                count++;
              }

            return(list);
          }


        template <typename number>
        void HTML_report::write_integration_analysis(HTML_report_bundle<number>& bundle,
                                                     const content_group_record<integration_payload> rec, HTML_node& parent)
          {
            worker_information_db worker_db = bundle.read_worker_database(rec.get_payload().get_container_path());
            std::string tag = bundle.get_id(rec);

            // will throw an exception if statistics table cannot be found
            timing_db timing_data = bundle.read_timing_database(rec.get_payload().get_container_path());

            // bin data into an aggregate number of configurations processed per worker
            count_list counts = this->count_configurations_per_worker(timing_data);

            // Produce report

            // GLOBAL SECTION
            // Any kind of task gets a histogram showing the distribution of configurations among workers
            // and the overall distribution of integration time

            HTML_node button("button", "Integration report");
            button.add_attribute("type", "button");

            if(worker_db.size() == 0 || timing_data.size() == 0 || !bundle.can_produce_plots())
              {
                button.add_attribute("class", "btn btn-info disabled");
                parent.add_element(button);
                return;
              }

            button.add_attribute("class", "btn btn-info");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "timing");

            HTML_node content("div");
            content.add_attribute("id", tag + "timing").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info topskip");

            HTML_node panel_head("div", "Integration analysis for this content group");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node panel_body("div");
            panel_body.add_attribute("class", "panel-body");

            HTML_node chart_row("div");
            chart_row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-6");
            this->write_worker_chart(bundle, rec, col1, counts);

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-6");
            this->write_timing_histogram(bundle, rec, col2, timing_data);

            chart_row.add_element(col1).add_element(col2);
            panel_body.add_element(chart_row);

            // ADD CONTENT FOR TASKS OF A SPECIFIC TYPE

            // get record for owning task
            typename task_db<number>::type& tk_db = bundle.get_task_db();
            typename task_db<number>::type::const_iterator tk_t = tk_db.find(rec.get_task_name());
            if(tk_t != tk_db.end())
              {
                const task_record<number>& trec = *tk_t->second;
                if(trec.get_type() == task_type::integration)
                  {
                    const integration_task_record<number>& irec = dynamic_cast< const integration_task_record<number>& >(trec);

                    switch(irec.get_task_type())
                      {
                        case integration_task_type::threepf:
                          {
                            // 3pf tasks get a special analysis
                            this->write_3pf_integration_analysis(bundle, rec, irec, worker_db, timing_data, panel_body);
                            break;
                          }

                        // other tasks get no special treatment
                        default:
                          break;
                      }
                  }
              }

            panel.add_element(panel_head).add_element(panel_body);
            content.add_element(panel);
            parent.add_element(button).add_element(content);
          }


        template <typename number>
        void HTML_report::write_worker_chart(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                             HTML_node& parent, count_list& counts)
          {
            boost::filesystem::path relative_asset_loc = bundle.make_asset_directory(rec.get_name());

            boost::filesystem::path relative_script_loc = relative_asset_loc / "_worker_chart.py";
            boost::filesystem::path relative_image_loc = relative_asset_loc / "_worker_chart.svg";

            boost::filesystem::path script_path = this->root / relative_script_loc;
            boost::filesystem::path image_path = this->root / relative_image_loc;

            std::ofstream out(script_path.string(), std::ios::out | std::ios::trunc);
            if(out.fail() || !out.is_open()) return;

            this->write_matplotlib_preamble(out, bundle);
            out << "plt.figure()" << '\n';

            unsigned int count = 0;
            out << "left = [ ";
            for(const count_list::value_type& item : counts)
              {
                if(count > 0) out << ", ";
                out << count;
                ++count;
              }
            out << " ]" << '\n';

            count = 0;
            out << "height = [ ";
            for(const count_list::value_type& item : counts)
              {
                if(count > 0) out << ", ";
                out << item.second;
                ++count;
              }
            out << " ]" << '\n';

            if(this->env.has_matplotlib_tick_label())
              {
                count = 0;
                out << "label = [ ";
                for(const count_list::value_type& item : counts)
                  {
                    if(count > 0) out << ", ";
                    out << "'" << item.first.first << "," << item.first.second << "'";
                    ++count;
                  }
                out << " ]" << '\n';
              }

            if(counts.size() <= 20)
              {
                out << "plt.bar(left, height";
                if(this->env.has_matplotlib_tick_label())
                  {
                    out << ", tick_label=label";
                  }
                out << ")" << '\n';
              }
            else // don't include tick labels if they would be too crowded
              {
                out << "plt.bar(left, height)" << '\n';
              }
            out << "plt.xlabel('worker')" << '\n';
            out << "plt.ylabel('configurations processed')" << '\n';
            out << "plt.savefig('" << image_path.string() << "')" << '\n';
            out << "plt.close()" << '\n';

            out.close();

            local_environment& env = bundle.get_environment();
            bool success = env.execute_python(script_path) == 0;

            if(success)
              {
                boost::filesystem::remove(script_path);
                HTML_node chart("img", false);
                chart.add_attribute("src", relative_image_loc.string()).add_attribute("class", "report-chart");
                chart.add_attribute("class", "report-chart");
                chart.add_attribute("data-toggle", "popover").add_attribute("data-placement", "top").add_attribute("title", "Worker activity");
                chart.add_attribute("data-content", "Shows the number of configurations processed by each worker");
                parent.add_element(chart);
              }
            else
              {
                if(boost::filesystem::exists(image_path)) boost::filesystem::remove(image_path);
                HTML_node no_chart("div", "Could not generate configurations-per-worker chart");
                no_chart.add_attribute("class", "label label-danger");
                HTML_node br("br", false);
                parent.add_element(no_chart).add_element(br);
              }
          }


        template <typename number>
        void HTML_report::write_timing_histogram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                                 HTML_node& parent, timing_db& timing_data)
          {
            boost::filesystem::path relative_asset_loc = bundle.make_asset_directory(rec.get_name());

            boost::filesystem::path relative_script_loc = relative_asset_loc / "_timing_histogram.py";
            boost::filesystem::path relative_image_loc = relative_asset_loc / "_timing_histogram.svg";

            boost::filesystem::path script_path = this->root / relative_script_loc;
            boost::filesystem::path image_path = this->root / relative_image_loc;

            std::ofstream out(script_path.string(), std::ios::out | std::ios::trunc);
            if(out.fail() || !out.is_open()) return;

            this->write_matplotlib_preamble(out, bundle);
            out << "plt.figure()" << '\n';

            out << "timings = np.array([ ";
            unsigned int count = 0;
            for(const timing_db::value_type& item : timing_data)
              {
                const timing_record& record = *item.second;
                if(count > 0) out << ", ";

                out << format_number(static_cast<double>(record.get_integration_time()) / 1E9);

                ++count;
              }
            out << " ])" << '\n';

            out << "bin_values = np.logspace(np.log10(timings.min()), np.log10(timings.max()), num=20, base=10.0)" << '\n';
            out << "plt.hist(timings, bins=bin_values, normed=True)" << '\n';
            out << "plt.yscale('log')" << '\n';
            out << "plt.xscale('log')" << '\n';
            out << "plt.xlabel('integration time in seconds')" << '\n';
            out << "plt.ylabel('frequency')" << '\n';
            out << "plt.savefig('" << image_path.string() << "')" << '\n';
            out << "plt.close()" << '\n';

            out.close();

            local_environment& env = bundle.get_environment();
            bool success = env.execute_python(script_path) == 0;

            if(success)
              {
                boost::filesystem::remove(script_path);
                HTML_node chart("img", false);
                chart.add_attribute("src", relative_image_loc.string()).add_attribute("class", "report-chart");
                chart.add_attribute("data-toggle", "popover").add_attribute("data-placement", "top").add_attribute("title", "Timing distribution");
                chart.add_attribute("data-content", "Shows the distribution of integration times in this content group");
                parent.add_element(chart);
              }
            else
              {
                if(boost::filesystem::exists(image_path)) boost::filesystem::remove(image_path);
                HTML_node no_chart("div", "Could not generate timing histogram");
                no_chart.add_attribute("class", "label label-danger");
                HTML_node br("br", false);
                parent.add_element(no_chart).add_element(br);
              }
          }


        template <typename number>
        void HTML_report::write_3pf_integration_analysis(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload> rec,
                                                         const integration_task_record<number>& irec,
                                                         worker_information_db& worker_db, timing_db& timing_data,
                                                         HTML_node& parent)
          {
            HTML_node row("div");
            row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-6");

            HTML_node col2("div");
            col1.add_attribute("class", "col-md-6");

            this->write_time_kt_diagram(bundle, rec, irec, worker_db, timing_data, col1);
            this->write_time_alpha_diagram(bundle, rec, irec, worker_db, timing_data, col1);
            this->write_time_beta_diagram(bundle, rec, irec, worker_db, timing_data, col2);

            row.add_element(col1).add_element(col2);
            parent.add_element(row);
          }


        template <typename number, typename Payload>
        void HTML_report::plot_scatter(HTML_report_bundle<number>& bundle, const content_group_record<Payload>& rec,
                                       std::string script, std::string image,
                                       const std::list< std::tuple<double, double, double> >& dataset,
                                       std::string xlabel, std::string ylabel, std::string colour_label,
                                       std::string css_class, std::string popover_title, std::string popover_text,
                                       bool xlog, bool ylog, HTML_node& parent)
          {
            boost::filesystem::path relative_asset_loc = bundle.make_asset_directory(rec.get_name());

            boost::filesystem::path relative_script_loc = relative_asset_loc / script;
            boost::filesystem::path relative_image_loc = relative_asset_loc / image;

            boost::filesystem::path script_path = this->root / relative_script_loc;
            boost::filesystem::path image_path = this->root / relative_image_loc;

            std::ofstream out(script_path.string(), std::ios::out | std::ios::trunc);
            if(out.fail() || !out.is_open()) return;

            this->write_matplotlib_preamble(out, bundle);
            out << "plt.figure()" << '\n';

            unsigned count = 0;
            double xmin = std::numeric_limits<double>::max();
            double xmax = std::numeric_limits<double>::min();
            out << "x = np.array([ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";

                double x = std::get<0>(item);
                if(x > xmax) xmax = x;
                if(x < xmin) xmin = x;

                out << format_number(x);
                ++count;
              }
            out << " ])" << '\n';

            count = 0;
            double ymin = std::numeric_limits<double>::max();
            double ymax = std::numeric_limits<double>::min();
            out << "y = np.array([ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";

                double y = std::get<1>(item);
                if(y > ymax) ymax = y;
                if(y < ymin) ymin = y;

                out << format_number(y);
                ++count;
              }
            out << " ])" << '\n';

            count = 0;
            double col_min = std::numeric_limits<double>::max();
            double col_max = std::numeric_limits<double>::min();
            out << "colours = np.array([ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";

                double col = std::get<2>(item);
                if(col > col_max) col_max = col;
                if(col < col_min) col_min = col;

                out << format_number(col);
                ++count;
              }
            out << " ])" << '\n';

            out << "colour_map = plt.get_cmap('autumn')" << '\n';
            out << "plt.scatter(x, y, marker='o', c=colours, cmap=colour_map, norm=col.LogNorm())" << '\n';
            out << "plt.colorbar(cmap=colour_map, label=r'" << colour_label << "')" << '\n';

            if(xlog)
              {
                out << "plt.xscale('log')" << '\n';

                int max_point = static_cast<int>(std::ceil(std::log10(xmax)));
                int min_point = static_cast<int>(std::floor(std::log10(xmin)));

                out << "plt.xlim(1E" << min_point << ", 1E" << max_point << ")" << '\n';
              }
            else
              {
                // allow 5% padding at top and bottom
                double span = xmax - xmin;

                double lim_min = xmin - 0.05*span;
                double lim_max = xmax + 0.05*span;
                out << "plt.xlim(" << lim_min << ", " << lim_max << ")" << '\n';
              }

            if(ylog)
              {
                out << "plt.yscale('log')" << '\n';

                int max_point = static_cast<int>(std::ceil(std::log10(ymax)));
                int min_point = static_cast<int>(std::floor(std::log10(ymin)));

                out << "plt.ylim(1E" << min_point << ", 1E" << max_point << ")" << '\n';
              }
            else
              {
                // allow 5% padding at top and bottom
                double span = ymax - ymin;

                double lim_min = ymin - 0.05*span;
                double lim_max = ymax + 0.05*span;
                out << "plt.ylim(" << lim_min << "*, " << lim_max << ")" << '\n';
              }

            if(!xlabel.empty()) out << "plt.xlabel(r'" << xlabel << "')" << '\n';
            if(!ylabel.empty()) out << "plt.ylabel(r'" << ylabel << "')" << '\n';

            out << "plt.savefig('" << image_path.string() << "')" << '\n';
            out << "plt.close()" << '\n';

            out.close();

            local_environment& env = bundle.get_environment();
            bool success = env.execute_python(script_path) == 0;

            if(success)
              {
                boost::filesystem::remove(script_path);
                HTML_node chart("img", false);
                chart.add_attribute("src", relative_image_loc.string()).add_attribute("class", css_class);
                if(!popover_title.empty() && !popover_text.empty())
                  {
                    chart.add_attribute("data-toggle", "popover").add_attribute("data-placement", "top").add_attribute("title", popover_title);
                    chart.add_attribute("data-content", popover_text);
                  }
                parent.add_element(chart);
              }
            else
              {
                if(boost::filesystem::exists(image_path)) boost::filesystem::remove(image_path);
                HTML_node no_chart("div", "Could not generate chart");
                no_chart.add_attribute("class", "label label-danger");
                HTML_node br("br", false);
                parent.add_element(no_chart).add_element(br);
              }
          }


        template <typename number>
        void HTML_report::write_time_kt_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                                const integration_task_record<number>& irec,
                                                worker_information_db& worker_db, timing_db& timing_data,
                                                HTML_node& parent)
          {
            integration_task<number>& raw_tk = *irec.get_task();
            threepf_task<number>& tk = dynamic_cast< threepf_task<number>& >(raw_tk);
            const threepf_kconfig_database& kconfig_db = tk.get_threepf_database();

            std::list< std::tuple<double, double, double> > dataset;
            for(const timing_db::value_type& item : timing_data)
              {
                const timing_record& data_point = *item.second;
                threepf_kconfig_database::const_record_iterator krec = kconfig_db.lookup(data_point.get_serial());

                dataset.emplace_back(std::make_tuple((*krec)->kt_conventional, static_cast<double>(data_point.get_integration_time()) / 1E9, (1.0-(*krec)->beta)/2.0));
              }

            this->plot_scatter(bundle, rec, "_timing_kt.py", "_timing_kt.svg", dataset, "configuration scale $k_t$",
                               "integration time in seconds", "$k_3 / k_t$",
                               "report-chart", "Time dependence: scale",
                               "Shows the dependence of the integration time on the overall configuration scale",
                               true, true, parent);
          }


        template <typename number>
        void HTML_report::write_time_alpha_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                                   const integration_task_record<number>& irec,
                                                   worker_information_db& worker_db, timing_db& timing_data,
                                                   HTML_node& parent)
          {
            integration_task<number>& raw_tk = *irec.get_task();
            threepf_task<number>& tk = dynamic_cast< threepf_task<number>& >(raw_tk);
            const threepf_kconfig_database& kconfig_db = tk.get_threepf_database();

            std::list< std::tuple<double, double, double> > dataset;
            for(const timing_db::value_type& item : timing_data)
              {
                const timing_record& data_point = *item.second;
                threepf_kconfig_database::const_record_iterator krec = kconfig_db.lookup(data_point.get_serial());

                dataset.emplace_back(std::make_tuple((*krec)->alpha, static_cast<double>(data_point.get_integration_time()) / 1E9, (*krec)->kt_conventional));
              }

            this->plot_scatter(bundle, rec, "_timing_alpha.py", "_timing_alpha.svg", dataset, "shape parameter $\\alpha$",
                               "integration time in seconds", "$k_t$",
                               "report-chart", "Time dependence: shape",
                               "Shows the dependence of the integration time on the shape parameter alpha",
                               false, true, parent);
          }


        template <typename number>
        void HTML_report::write_time_beta_diagram(HTML_report_bundle<number>& bundle, const content_group_record<integration_payload>& rec,
                                                  const integration_task_record<number>& irec,
                                                  worker_information_db& worker_db, timing_db& timing_data,
                                                  HTML_node& parent)
          {
            integration_task<number>& raw_tk = *irec.get_task();
            threepf_task<number>& tk = dynamic_cast< threepf_task<number>& >(raw_tk);
            const threepf_kconfig_database& kconfig_db = tk.get_threepf_database();

            std::list< std::tuple<double, double, double> > dataset;
            for(const timing_db::value_type& item : timing_data)
              {
                const timing_record& data_point = *item.second;
                threepf_kconfig_database::const_record_iterator krec = kconfig_db.lookup(data_point.get_serial());

                dataset.emplace_back(std::make_tuple((1.0-(*krec)->beta)/2.0, static_cast<double>(data_point.get_integration_time()) / 1E9, (*krec)->kt_conventional));
              }

            this->plot_scatter(bundle, rec, "_timing_beta.py", "_timing_beta.svg", dataset, "shape parameter $k_3 / k_t$",
                               "integration time in seconds", "$k_t$",
                               "report-chart", "Time dependence: shape",
                               "Shows the dependence of the integration time on the shape parameter k3/kt",
                               true, true, parent);
          }


        template <typename number>
        void HTML_report::write_matplotlib_preamble(std::ofstream& out, HTML_report_bundle<number>& bundle)
          {
            out << "import numpy as np" << '\n';
            plot_environment plot_env(bundle.get_environment(), bundle.get_argument_cache());
            plot_env.write_environment(out);
          }


        template <typename number>
        void HTML_report::write_postintegration_content(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            postintegration_content_db& db = bundle.get_postintegration_content_db();

            for(const postintegration_content_db::value_type& group : db)
              {
                const content_group_record<postintegration_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("ul");
                list.add_attribute("class", "list-group");

                HTML_node anchor("li");
                anchor.add_attribute("class", "list-group-item");

                this->make_list_item_label(rec.get_name(), anchor);
                this->write_generic_output_record(bundle, rec, anchor);

                const postintegration_payload& payload = rec.get_payload();

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");

                HTML_node panel_heading("div", "Summary");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");

                HTML_node ctr_row("div");
                ctr_row.add_attribute("class", "row");
                HTML_node ctr_col("div");
                ctr_col.add_attribute("class", "col-md-12");
                HTML_node ctr_list("dl");
                ctr_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Container", payload.get_container_path().string(), ctr_list);

                HTML_node row("div");
                row.add_attribute("class", "row");

                HTML_node col1("div");
                col1.add_attribute("class", "col-md-4");
                HTML_node col1_list("dl");
                col1_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Complete", (payload.is_failed() ? "No" : "Yes"), col1_list);
                this->make_data_element("Size", format_memory(payload.get_size()), col1_list);

                col1.add_element(col1_list);

                HTML_node col2("div");
                col2.add_attribute("class", "col-md-4");
                HTML_node col2_list("dl");
                col2_list.add_attribute("class", "dl-horizontal");

                HTML_node parent_dt("dt", "Parent");
                HTML_node parent_dd("dd");
                this->write_content_button(bundle, payload.get_parent_group(), parent_dd);
                col2_list.add_element(parent_dt).add_element(parent_dd);

                col2.add_element(col2_list);

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");
                HTML_node col3_list("dl");
                col3_list.add_attribute("class", "dl-horizontal");

                HTML_node seeded_dt("dt", "Seeded");
                HTML_node seeded_dd("dd");
                this->write_seeded(bundle, payload, seeded_dd);
                col3_list.add_element(seeded_dt).add_element(seeded_dd);

                col3.add_element(col3_list);

                ctr_col.add_element(ctr_list);
                ctr_row.add_element(ctr_col);

                row.add_element(col1).add_element(col2).add_element(col3);

                panel_body.add_element(ctr_row).add_element(row);
                panel.add_element(panel_heading).add_element(panel_body);

                anchor.add_element(panel);
                list.add_element(anchor);
                pane.add_element(list);
                parent.add_element(pane);
              }
          }


        template <typename number>
        void HTML_report::write_output_content(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            output_content_db& db = bundle.get_output_content_db();
            typename derived_product_db<number>::type& product_db = bundle.get_derived_product_db();

            std::unique_ptr< repository_graphkit<number> > graphkit = bundle.get_graphkit(this->err, this->warn, this->msg);

            for(const output_content_db::value_type& group : db)
              {
                const content_group_record<output_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                boost::filesystem::path asset_dir = bundle.make_asset_directory(rec.get_name());

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("ul");
                list.add_attribute("class", "list-group");

                HTML_node anchor("li");
                anchor.add_attribute("class", "list-group-item");

                this->make_list_item_label(rec.get_name(), anchor);
                this->write_generic_output_record(bundle, rec, anchor);

                const output_payload& payload = rec.get_payload();
                const std::list<derived_content>& content = payload.get_derived_content();

                if(!content.empty())
                  {
                    // make_asset_directory() will throw an exception if it cannot succeed
                    boost::filesystem::path asset_directory = bundle.make_asset_directory(rec.get_name());

                    HTML_node content_list("ul");
                    content_list.add_attribute("class", "list-group topskip");

                    unsigned int count = 0;
                    for(const derived_content& item : content)
                      {
                        boost::filesystem::path filename = item.get_filename();
                        boost::filesystem::path extension = filename.extension();

                        HTML_node it("li");
                        it.add_attribute("class", "list-group-item");
                        HTML_node title("h4", filename.string());
                        title.add_attribute("class", "list-group-item-text lead");
                        it.add_element(title);

                        boost::filesystem::path abs_product_location = rec.get_abs_output_path() / filename;
                        boost::filesystem::path relative_asset_location = bundle.emplace_asset(abs_product_location, rec.get_name(), filename);

                        if(extension.string() == ".pdf")
                          {
                            // Safari will display PDFs in an <img> tag, but Chrome and Firefox will not so we choose to emebed
                            // as PDF objects instead
                            HTML_node pdf("div");
                            pdf.add_attribute("class", "pdfproduct topskip");

                            HTML_node obj("object");
                            obj.add_attribute("data", relative_asset_location.string()).add_attribute("type", "application/pdf");
                            obj.add_attribute("width", "100%").add_attribute("height", "100%");

                            HTML_node fallback("p", "PDF embedding not supported in this browser");
                            obj.add_element(fallback);

                            pdf.add_element(obj);
                            it.add_element(pdf);
                          }
                        else if(extension.string() == ".png" || extension.string() == ".svg" || extension.string() == ".svgz")
                          {
                            HTML_node img("img", false);
                            img.add_attribute("class", "img-responsive imgproduct").add_attribute("src", relative_asset_location.string()).add_attribute("alt", filename.string());
                            it.add_element(img);
                          }
                        else if(extension.string() == ".txt" || extension.string() == ".dat" || extension.string() == ".data"
                                || extension.string() == ".csv" || extension.string() == ".tsv")
                          {
                            HTML_node well("div");
                            well.add_attribute("class", "well topskip");
                            HTML_node frame("iframe", filename.string());
                            frame.add_attribute("src", relative_asset_location.string()).add_attribute("class", "iframeproduct");
                            well.add_element(frame);
                            it.add_element(well);
                          }
                        else
                          {
                            HTML_node well("div", "Cannot embed this content");
                            well.add_attribute("class", "well");
                            it.add_element(well);
                          }

                        typename derived_product_db<number>::type::const_iterator t = product_db.find(item.get_parent_product());

                        HTML_node panel("div");
                        panel.add_attribute("class", "panel panel-default topskip");

                        HTML_node panel_heading("div", "Summary");
                        panel_heading.add_attribute("class", "panel-heading");

                        HTML_node panel_body("div");
                        panel_body.add_attribute("class", "panel-body");

                        HTML_node row("div");
                        row.add_attribute("class", "row");

                        HTML_node col1("div");
                        col1.add_attribute("class", "col-md-4");
                        HTML_node col1_list("dl");
                        col1_list.add_attribute("class", "dl-horizontal");

                        if(t != product_db.end())
                          {
                            HTML_node link_dt("dt", "Derived product");
                            HTML_node link_dd("dd");
                            this->write_derived_product_button(bundle, t->second->get_product()->get_name(), link_dd);
                            col1_list.add_element(link_dt).add_element(link_dd);

                            this->make_data_element("Type", derived_data::derived_product_type_to_string(t->second->get_product()->get_type()), col1_list);
                          }
                        else
                          {
                            HTML_node null_dt("dt", "Type");
                            HTML_node null_dd("dd");
                            HTML_node null_label("span", "NULL");
                            null_label.add_attribute("class", "label label-danger");
                            null_dd.add_element(null_label);
                            col1_list.add_element(null_dt).add_element(null_dd);
                          }

                        col1.add_element(col1_list);

                        HTML_node col2("div");
                        col2.add_attribute("class", "col-md-4");
                        HTML_node col2_list("dl");
                        col2_list.add_attribute("class", "dl-horizontal");

                        this->make_data_element("Created", boost::posix_time::to_simple_string(item.get_creation_time()), col2_list);

                        col2.add_element(col2_list);

                        HTML_node col3("div");
                        col3.add_attribute("class", "col-md-4");
                        HTML_node col3_list("dl");
                        col3_list.add_attribute("class", "dl-horizontal");

                        HTML_node tg_dt("dt", "Tags");
                        HTML_node tg_dd("dd");
                        this->compose_tag_list(item.get_tags(), tg_dd);
                        col3_list.add_element(tg_dt).add_element(tg_dd);

                        col3.add_element(col3_list);

                        row.add_element(col1).add_element(col2).add_element(col3);

                        panel_body.add_element(row);

                        // Write out notes if present
                        this->write_notes_collapsible(item.get_notes(), tag, panel_body);

                        panel.add_element(panel_heading).add_element(panel_body);

                        it.add_element(panel);

                        const std::list<std::string>& groups = item.get_content_groups();
                        if(!groups.empty())
                          {
                            std::string group_tag = tag + "_" + boost::lexical_cast<std::string>(count) + "_table";

                            HTML_node button("button");
                            button.add_attribute("type", "button").add_attribute("class", "btn btn-info");
                            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + group_tag);
                            this->make_badged_text("Data provenance", groups.size(), button);

                            HTML_node data("div");
                            data.add_attribute("id", group_tag).add_attribute("class", "collapse");

                            HTML_node tbl_panel("div");
                            tbl_panel.add_attribute("class", "panel panel-info topskip");

                            HTML_node tbl_panel_head("div", "Provenance report for this product");
                            tbl_panel_head.add_attribute("class", "panel-heading");

                            HTML_node table_wrapper("div");
                            HTML_node table("table");

                            if(groups.size() > CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE)
                              {
                                table_wrapper.add_attribute("class", "table-responsive padded");
                                table.add_attribute("class", "table table-striped table-condensed sortable-pageable");
                              }
                            else
                              {
                                table_wrapper.add_attribute("class", "table-responsive");
                                table.add_attribute("class", "table table-striped table-condensed sortable");
                              }

                            HTML_node head("thead");
                            HTML_node head_row("tr");

                            HTML_node name_label("th", "Name");
                            HTML_node task_label("th", "Task");
                            HTML_node type_label("th", "Type");
                            HTML_node edited_label("th", "Last updated");

                            head_row.add_element(name_label).add_element(task_label).add_element(type_label).add_element(edited_label);
                            head.add_element(head_row);

                            HTML_node body("tbody");

                            for(const std::string& gp : groups)
                              {
                                HTML_node table_row("tr");

                                HTML_node name("td");
                                this->write_content_button(bundle, gp, name);

                                content_group_data<number> properties(gp, bundle.get_cache());

                                HTML_node task("td");
                                this->write_task_button(bundle, properties.get_task(), task);

                                HTML_node type("td", properties.get_type());
                                HTML_node edited("td", properties.get_last_edit());

                                table_row.add_element(name).add_element(task).add_element(type).add_element(edited);
                                body.add_element(table_row);
                              }

                            table.add_element(head).add_element(body);
                            table_wrapper.add_element(table);
                            tbl_panel.add_element(tbl_panel_head).add_element(table_wrapper);
                            data.add_element(tbl_panel);
                            it.add_element(button).add_element(data);

                            // make a dependency diagram if Graphviz is available
                            if(this->env.has_dot())
                              {
                                std::unique_ptr<repository_dependency_graph> depends = graphkit->derived_content_dependency(filename.string(), groups);

                                boost::filesystem::path graph_filename(item.get_filename().stem().string() + "-graph");
                                boost::filesystem::path relative_dot_script = asset_dir / graph_filename;
                                relative_dot_script.replace_extension("dot");

                                boost::filesystem::path relative_dot_product = relative_dot_script;
                                relative_dot_product.replace_extension("svg");

                                boost::filesystem::path absolute_dot_script = this->root / relative_dot_script;
                                boost::filesystem::path absolute_dot_product = this->root / relative_dot_product;

                                depends->write_graphviz(absolute_dot_script);
                                if(this->env.execute_dot(absolute_dot_script, absolute_dot_product, "svg") == 0)
                                  {
                                    std::string group_tag2 = tag + "_" + boost::lexical_cast<std::string>(count) + "_graph";

                                    HTML_node button2("button", "Dependency diagram");
                                    button2.add_attribute("type", "button").add_attribute("class", "btn btn-info");
                                    button2.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + group_tag2);

                                    // remove unneeded script file
                                    boost::filesystem::remove(absolute_dot_script);

                                    HTML_node diagram("div");
                                    diagram.add_attribute("id", group_tag2).add_attribute("class", "collapse");
                                    HTML_node graph("img", false);
                                    graph.add_attribute("class", "img-responsive topskip").add_attribute("src", relative_dot_product.string()).add_attribute("alt", relative_dot_product.string());
                                    diagram.add_element(graph);

                                    it.add_element(button2).add_element(diagram);
                                  }
                                else
                                  {
                                    HTML_node button2("button", "Dependency diagram");
                                    button2.add_attribute("type", "button").add_attribute("class", "btn btn-info disabled");
                                    it.add_element(button2);
                                  }
                              }

                            ++count;
                          }

                        content_list.add_element(it);
                      }

                    anchor.add_element(content_list);
                  }

                list.add_element(anchor);
                pane.add_element(list);
                parent.add_element(pane);
              }
          }


        std::string HTML_report::compose_tag_list(const std::list<std::string>& tags, HTML_node& parent)
          {
            std::ostringstream out;

            if(!tags.empty())
              {
                for(const std::string& tag : tags)
                  {
                    HTML_node tg("span");
                    tg.add_attribute("class", "label label-info rightskip");

                    HTML_node icon("span");
                    icon.add_attribute("class", "glyphicon glyphicon-tag");

                    HTML_string tg_text(tag);
                    tg.add_element(icon).add_element(tg_text);

                    parent.add_element(tg);
                  }
              }
            else
              {
                HTML_node tg("span", "None");
                tg.add_attribute("class", "label label-default");

                parent.add_element(tg);
              }

            return(out.str());
          }


        template <typename number, typename RecordType>
        void HTML_report::write_generic_record(HTML_report_bundle<number>& bundle,
                                               const RecordType& rec, HTML_node& parent)
          {
            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-default");

            HTML_node panel_heading("div", "Repository metadata");
            panel_heading.add_attribute("class", "panel-heading");

            HTML_node panel_body("div");
            panel_body.add_attribute("class", "panel-body");

            HTML_node row("div");
            row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Created", boost::posix_time::to_simple_string(created), col1_list);

            col1.add_element(col1_list);

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Last updated", boost::posix_time::to_simple_string(edited), col2_list);

            col2.add_element(col2_list);

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Runtime version", format_version(rec.get_runtime_API_version()), col3_list);

            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            panel_body.add_element(row);

            this->write_activity_collapsible(rec.get_history(), bundle.get_id(rec), panel_body);

            panel.add_element(panel_heading).add_element(panel_body);
            parent.add_element(panel);
          }


        template <typename number, typename Payload>
        void HTML_report::write_generic_output_record(HTML_report_bundle<number>& bundle,
                                                      const content_group_record<Payload>& rec, HTML_node& parent)
          {
            boost::posix_time::ptime created = rec.get_creation_time();
            boost::posix_time::ptime edited = rec.get_last_edit_time();

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-default");

            HTML_node panel_heading("div", "Repository metadata");
            panel_heading.add_attribute("class", "panel-heading");

            HTML_node panel_body("div");
            panel_body.add_attribute("class", "panel-body");

            HTML_node row("div");
            row.add_attribute("class", "row");

            HTML_node col1("div");
            col1.add_attribute("class", "col-md-4");
            HTML_node col1_list("dl");
            col1_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Created", boost::posix_time::to_simple_string(created), col1_list);

            HTML_node pt_dt("dt", "Parent task");
            HTML_node pt_dd("dd");
            this->write_task_button(bundle, rec.get_task_name(), pt_dd);
            col1_list.add_element(pt_dt).add_element(pt_dd);

            col1.add_element(col1_list);

            HTML_node col2("div");
            col2.add_attribute("class", "col-md-4");
            HTML_node col2_list("dl");
            col2_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Last updated", boost::posix_time::to_simple_string(edited), col2_list);
            this->make_data_element("Locked", (rec.get_lock_status() ? "Yes" : "No"), col2_list);

            col2.add_element(col2_list);

            HTML_node col3("div");
            col3.add_attribute("class", "col-md-4");
            HTML_node col3_list("dl");
            col3_list.add_attribute("class", "dl-horizontal");

            this->make_data_element("Runtime version", format_version(rec.get_runtime_API_version()), col3_list);
            HTML_node tags_dt("dt", "Tags");
            HTML_node tags_dd("dd");
            this->compose_tag_list(rec.get_tags(), tags_dd);
            col3_list.add_element(tags_dt).add_element(tags_dd);

            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            panel_body.add_element(row);

            this->write_notes_collapsible(rec.get_notes(), bundle.get_id(rec), panel_body);
            this->write_activity_collapsible(rec.get_history(), bundle.get_id(rec), panel_body);

            panel.add_element(panel_heading).add_element(panel_body);
            parent.add_element(panel);
          }


        template <typename number, typename Payload>
        void HTML_report::write_seeded(HTML_report_bundle<number>& bundle, const Payload& payload, HTML_node& parent)
          {
            if(payload.is_seeded())
              {
                this->write_content_button(bundle, payload.get_seed_group(), parent);
              }
            else
              {
                HTML_node no("span", "No");
                parent.add_element(no);
              }
          }


        template <typename number>
        void HTML_report::write_JavaScript_button(HTML_report_bundle<number>& bundle, std::string button,
                                                  std::string pane, std::string element)
          {
            // we need a Javascript function which causes this button to activate the appropriate task pane when clicked
            bundle.write_JavaScript("$(function(){");
            bundle.write_JavaScript("    $('#" + button + "').click(function(e){");
            bundle.write_JavaScript("        e.preventDefault();");
            bundle.write_JavaScript("        var tab_element = $('#tabs').find('a[href=\"#" + pane + "\"]');");
            bundle.write_JavaScript("        tab_element.tab('show');");
            bundle.write_JavaScript("        tab_element.one('shown.bs.tab', function(e2){");
            bundle.write_JavaScript("            var list_element = $('#" + pane + "').find('a[href=\"#" + element + "\"]');");
            bundle.write_JavaScript("            $('html, body').animate({ scrollTop: list_element.offset().top - $(\".navbar\").height() - 40 }, 1000);");
            bundle.write_JavaScript("        });");
            bundle.write_JavaScript("    });");
            bundle.write_JavaScript("});");
          }


        template <typename number>
        void HTML_report::write_JavaScript_button(HTML_report_bundle<number>& bundle, std::string button, std::string pane)
          {
            // we need a Javascript function which causes this button to activate the appropriate contents pane when clicked
            bundle.write_JavaScript("$(function(){");
            bundle.write_JavaScript("    $('#" + button + "').click(function(e){");
            bundle.write_JavaScript("        e.preventDefault();");
            bundle.write_JavaScript("        $('#tabs').find('a[href=\"#" + pane + "\"]').tab('show');");
            bundle.write_JavaScript("    })");
            bundle.write_JavaScript("});");
          }


        template <typename number>
        void HTML_report::write_task_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent)
          {
            typename task_db<number>::type& tk_db = bundle.get_task_db();
            typename task_db<number>::type::const_iterator t = tk_db.find(name);

            if(t != tk_db.end())
              {
                // find task tag
                std::string pane_id = bundle.get_id(*t->second);
                std::string button_id = this->make_button_tag();

                HTML_node button("button");
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);
                this->make_badged_text(name, t->second->get_content_groups().size(), button);

                parent.add_element(button);
                this->write_JavaScript_button(bundle, button_id, pane_id);
              }
            else
              {
                HTML_node fail("span", name);
                fail.add_attribute("class", "label label-danger");

                parent.add_element(fail);
              }
          }


        template <typename number>
        void HTML_report::write_content_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent)
          {
            integration_content_db& int_db = bundle.get_integration_content_db();
            postintegration_content_db& pint_db = bundle.get_postintegration_content_db();
            output_content_db& output_db = bundle.get_output_content_db();

            integration_content_db::const_iterator int_t = int_db.find(name);
            postintegration_content_db::const_iterator pint_t = pint_db.find(name);
            output_content_db::const_iterator out_t = output_db.find(name);

            std::string pane;
            if(int_t != int_db.end()) pane = bundle.get_id(*int_t->second);
            else if(pint_t != pint_db.end()) pane = bundle.get_id(*pint_t->second);
            else if(out_t != output_db.end()) pane = bundle.get_id(*out_t->second);

            this->write_content_button(bundle, name, pane, parent);
          }


        template <typename number>
        void HTML_report::write_content_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent)
          {
            if(!pane.empty())
              {
                std::string button_id = this->make_button_tag();

                HTML_node button("button", name);
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);

                parent.add_element(button);

                this->write_JavaScript_button(bundle, button_id, pane);
              }
            else
              {
                HTML_node fail("span", name);
                fail.add_attribute("class", "label label-danger");

                parent.add_element(fail);
              }
          }


        template <typename number>
        void HTML_report::write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent)
          {
            typename package_db<number>::type& db = bundle.get_package_db();
            typename package_db<number>::type::const_iterator t = db.find(name);

            std::string element;
            if(t != db.end()) element = bundle.get_id(*t->second);

            this->write_package_button(bundle, name, element, parent);
          }


        template <typename number>
        void HTML_report::write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent)
          {
            if(!pane.empty())
              {
                std::string button_id = this->make_button_tag();

                HTML_node button("button", name);
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);

                parent.add_element(button);

                this->write_JavaScript_button(bundle, button_id, pane);
              }
            else
              {
                HTML_node fail("span", name);
                fail.add_attribute("class", "label label-danger");

                parent.add_element(fail);
              }
          }


        template <typename number>
        void HTML_report::write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent)
          {
            typename derived_product_db<number>::type& db = bundle.get_derived_product_db();
            typename derived_product_db<number>::type::const_iterator t = db.find(name);

            std::string pane;
            if(t != db.end()) pane = bundle.get_id(*t->second);

            this->write_derived_product_button(bundle, name, pane, parent);
          }


        template <typename number>
        void HTML_report::write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& pane, HTML_node& parent)
          {
            if(!pane.empty())
              {
                std::string button_id = this->make_button_tag();

                HTML_node button("button", name);
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);

                parent.add_element(button);

                this->write_JavaScript_button(bundle, button_id, pane);
              }
            else
              {
                HTML_node fail("span", name);
                fail.add_attribute("class", "label label-danger");

                parent.add_element(fail);
              }
          }


        void HTML_report::write_notes_collapsible(const std::list<note>& notes, const std::string& tag, HTML_node& parent)
          {
            if(notes.empty()) return;

            HTML_node button("button");
            button.add_attribute("type", "button").add_attribute("class", "btn btn-info");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "notes");
            this->make_badged_text("Notes", notes.size(), button);

            HTML_node content("div");
            content.add_attribute("id", tag + "notes").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info scrollable-panel topskip");

            HTML_node panel_head("div", "Notes attached to this record");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node list("ol");
            list.add_attribute("class", "list-group");

            for(const note& it : notes)
              {
                HTML_node item("li");
                item.add_attribute("class", "list-group-item");

                if(!it.get_uid().empty())
                  {
                    HTML_node uid_block("span");
                    uid_block.add_attribute("class", "label label-primary rightskip");

                    HTML_node uid_icon("span");
                    uid_icon.add_attribute("class", "glyphicon glyphicon-user");

                    HTML_string uid(it.get_uid());
                    uid_block.add_element(uid_icon).add_element(uid);

                    HTML_node timestamp_block("span");
                    timestamp_block.add_attribute("class", "label label-default");

                    HTML_node timestamp_icon("span");
                    timestamp_icon.add_attribute("class", "glyphicon glyphicon-calendar");

                    HTML_string timestamp(boost::posix_time::to_simple_string(it.get_timestamp()));
                    timestamp_block.add_element(timestamp_icon).add_element(timestamp);
                    item.add_element(uid_block).add_element(timestamp_block);
                  }
                else
                  {
                    HTML_node uid_block("span");
                    uid_block.add_attribute("class", "label label-danger");

                    HTML_node icon("span");
                    icon.add_attribute("class", "glyphicon glyphicon-user");

                    HTML_string uid(it.get_uid());

                    // omit timestamp on presumption it is meaningless
                    uid_block.add_element(icon).add_element(uid);
                    item.add_element(uid_block);
                  }

                HTML_node bk("br", false);
                HTML_string note_text(it.get_note());
                item.add_element(bk).add_element(note_text);

                list.add_element(item);
              }

            panel.add_element(panel_head).add_element(list);
            content.add_element(panel);
            parent.add_element(button).add_element(content);
          }


        void HTML_report::write_activity_collapsible(const std::list<metadata_history>& activity, const std::string& tag, HTML_node& parent)
          {
            if(activity.empty()) return;

            HTML_node button("button");
            button.add_attribute("type", "button").add_attribute("class", "btn btn-info");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "activity");
            this->make_badged_text("Activity log", activity.size(), button);

            HTML_node content("div");
            content.add_attribute("id", tag + "activity").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info scrollable-panel topskip");

            HTML_node panel_head("div", "Activity report");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node list("ol");
            list.add_attribute("class", "list-group");

            for(const metadata_history& log_item : activity)
              {
                HTML_node item("li", log_item.to_string());
                item.add_attribute("class", "list-group-item");
                list.add_element(item);
              }

            panel.add_element(panel_head).add_element(list);
            content.add_element(panel);
            parent.add_element(button).add_element(content);
          }


        std::string HTML_report::make_button_tag()
          {
            std::ostringstream tag;
            tag << CPPTRANSPORT_DEFAULT_HTML_BUTTON_TAG_PREFIX << this->button_id++;
            return(tag.str());
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_HTML_REPORT_H
