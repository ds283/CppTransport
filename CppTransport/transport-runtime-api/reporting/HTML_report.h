//
// Created by David Seery on 23/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPORTING_HTML_REPORT_H
#define CPPTRANSPORT_REPORTING_HTML_REPORT_H


#include "transport-runtime-api/repository/repository.h"
#include "transport-runtime-api/data/data_manager.h"
#include "transport-runtime-api/derived-products/derived_product_type.h"

#include "transport-runtime-api/reporting/content_group_data.h"
#include "transport-runtime-api/reporting/HTML_report_bundle.h"

#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/message_handlers.h"

#include "transport-runtime-api/utilities/formatter.h"
#include "transport-runtime-api/utilities/plot_environment.h"


namespace transport
  {

    namespace reporting
      {

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
            void write_integration(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write postintegration tasks tab
            template <typename number>
            void write_postintegration(HTML_report_bundle<number>& bundle, HTML_node& parent);

            //! write output tasks tab
            template <typename number>
            void write_output(HTML_report_bundle<number>& bundle, HTML_node& parent);

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

          protected:

            //! create a worker table for an integration content record
            template <typename number>
            void write_worker_table(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec, HTML_node& parent);

            //! typedef for list of configurations-per-worker
            typedef std::map< std::pair<unsigned int, unsigned int>, unsigned int > count_list;

            //! count configurations per worker and store in an interable container
            count_list count_configurations_per_worker(timing_db& data);

            //! create visual report of timing statistics
            template <typename number>
            void write_integration_analysis(HTML_report_bundle<number>& bundle,
                                            const output_group_record<integration_payload> rec, HTML_node& parent);

            //! produce bar chart showing number of configurations processed per worker
            template <typename number>
            void write_worker_chart(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
                                    HTML_node& parent, count_list& counts);

            //! produce histogram showing distribution of integration times
            template <typename number>
            void write_timing_histogram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
                                        HTML_node& parent, timing_db& timing_data);

            //! write specialized integration analysis for 3pf
            template <typename number>
            void write_3pf_integration_analysis(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
                                                const integration_task_record<number>& irec,
                                                worker_information_db& worker_db, timing_db& timing_data,
                                                HTML_node& parent);

            //! plot a generic scatter diagram */
            template <typename number, typename Payload>
            void plot_scatter(HTML_report_bundle<number>& bundle, const output_group_record<Payload>& rec,
                              std::string script, std::string image,
                              const std::list< std::tuple<double, double, double> >& dataset,
                              std::string xlabel, std::string ylabel, std::string colour_label,
                              std::string css_class, std::string popover_title, std::string popover_text,
                              bool xlog, bool ylog, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. k_t
            template <typename number>
            void write_time_kt_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
                                       const integration_task_record<number>& irec,
                                       worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. alpha
            template <typename number>
            void write_time_alpha_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
                                          const integration_task_record<number>& irec,
                                          worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! for 3pf tasks, write a plot of integration time vs. beta
            template <typename number>
            void write_time_beta_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
                                         const integration_task_record<number>& irec,
                                         worker_information_db& worker_db, timing_db& timing_data, HTML_node& parent);

            //! write standardized matplotlib preamble
            template <typename number>
            void write_matplotlib_preamble(std::ofstream& out, HTML_report_bundle<number>& bundle);

            //! write integration task details for twopf task
            template <typename number>
            void write_task_data(HTML_report_bundle<number>& bundle, twopf_task<number>& tk,
                                 HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write integration task details for threepf task
            template <typename number>
            void write_task_data(HTML_report_bundle<number>& bundle, threepf_task<number>& tk,
                                 HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

            //! write task details for a generic integrationt ask
            template <typename number>
            void write_generic_task_data(HTML_report_bundle<number>& bundle, twopf_db_task<number>& tk,
                                         HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list);

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
            void write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& element, HTML_node& parent);

            //! write a link-to-derived product button
            template <typename number>
            void write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, HTML_node& parent);

            //! write a link-to-derived product button
            template <typename number>
            void write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& element, HTML_node& parent);


            // WRITE GENERIC RECORD DETAILS

          protected:

            //! write details for generic records
            template <typename number, typename RecordType>
            void write_generic_record(HTML_report_bundle<number>& bundle, const RecordType& rec, HTML_node& parent);

            //! write details for generic output records
            template <typename number, typename Payload>
            void write_generic_output_record(HTML_report_bundle<number>& bundle, const output_group_record<Payload>& rec,
                                             HTML_node& parent);

            //! write a seeding label
            template <typename number, typename Payload>
            void write_seeded(HTML_report_bundle<number>& bundle, const Payload& payload, HTML_node& parent);

            //! write a collapsible notes section
            void write_notes_collapsible(const std::list<std::string>& notes, const std::string& tag, HTML_node& parent);

            //! write a collapsible activity log
            void write_activity_collapsible(const std::list<metadata_history>& activity, const std::string& tag, HTML_node& parent);


            // WRITE JAVASCRIPT HANDLERS

          protected:

            //! write JavaScript button handler which changes pane and scrolls to a given element
            template <typename number>
            void write_JavaScript_button_scroll(HTML_report_bundle<number>& bundle, std::string button, std::string pane, std::string element);

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
            template <typename number, typename DatabaseType, typename PayloadType>
            HTML_node make_content_menu_tab(const DatabaseType& db, HTML_report_bundle<number>& bundle, std::string name);

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

          };


        void HTML_report::set_root(boost::filesystem::path p)
          {
            // check whether root path is absolute, and if not make it relative to current working directory
            if(!p.is_absolute())
              {
                // not absolute, so use CWD
                p = boost::filesystem::absolute(p);
              }

            // check whether root directory already exists, and if it does, raise an error
            if(boost::filesystem::exists(p))
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_ROOT_EXISTS_A << " '" << p.string() << "' " << CPPTRANSPORT_HTML_ROOT_EXISTS_B;
                this->err(msg.str());
                this->root.clear();
                return;
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
            // add jQuery and bootstrap scripts; note jQuery must come first, before bootstrap is loaded
            bundle.emplace_JavaScript_asset("jQuery/jquery-2.2.2.min.js", "jquery-2.2.2.min.js");
            bundle.emplace_JavaScript_asset("bootstrap/js/bootstrap.min.js", "bootstrap.min.js");

            // add plugin to enable history on bootstrap tabs
            bundle.emplace_JavaScript_asset("bootstrap-tab-history/bootstrap-tab-history.js", "bootstrap-tab-history.js");

            // add our own JavaScript file utility.js which handles eg. resizing navigation bar on window resize
            bundle.emplace_JavaScript_asset("HTML_assets/utility.js", "utility.js");

            // add bootstrap CSS file
            bundle.emplace_CSS_asset("bootstrap/css/bootstrap.min.css", "bootstrap.min.css");

            // add our own CSS file
            bundle.emplace_CSS_asset("HTML_assets/cpptransport.css", "cpptransport.css");

            // now generate the main body of the report
            HTML_node body("div");
            body.add_attribute("class", "container-fluid");
            this->generate_report_body(bundle, body);

            // emplace the report body inside the bundle
            bundle.emplace_HTML_body(body);
          }


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
            HTML_node package_tab = this->make_menu_tab("packages", "Packages");
            // mark this tab as active by default
            package_tab.add_attribute("class", "active");

            // Integration tasks tab
            HTML_node integration_tasks_tab = this->make_menu_tab("integration", "Integration tasks");

            // Integration content tab
            HTML_node integration_content_tab = this->make_content_menu_tab<number, integration_content_db, integration_payload>(
              bundle.get_integration_content_db(), bundle, "Integration content");

            // Postintegration tasks tab
            HTML_node postintegration_tasks_tab = this->make_menu_tab("postintegration", "Postintegration tasks");

            // Postintegration content tab
            HTML_node postintegration_content_tab = this->make_content_menu_tab<number, postintegration_content_db, postintegration_payload>(
              bundle.get_postintegration_content_db(), bundle, "Postintegration content");

            // Output tasks tab
            HTML_node output_tasks_tab = this->make_menu_tab("output", "Output tasks");

            // Output content tab
            HTML_node output_content_tab = this->make_content_menu_tab<number, output_content_db, output_payload>(
              bundle.get_output_content_db(), bundle, "Output_content");

            // Derived products tab
            HTML_node derived_products_tab = this->make_menu_tab("derived", "Derived products");

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

            HTML_node package_pane("div");
            package_pane.add_attribute("id", "packages").add_attribute("class", "tab-pane fade in active");
            this->write_packages(bundle, package_pane);
            tab_panes.add_element(package_pane);

            HTML_node integration_task_pane("div");
            integration_task_pane.add_attribute("id", "integration").add_attribute("class", "tab-pane fade");
            this->write_integration(bundle, integration_task_pane);
            tab_panes.add_element(integration_task_pane);

            this->write_integration_content(bundle, tab_panes);

            HTML_node postintegration_task_pane("div");
            postintegration_task_pane.add_attribute("id", "postintegration").add_attribute("class", "tab-pane fade");
            this->write_postintegration(bundle, postintegration_task_pane);
            tab_panes.add_element(postintegration_task_pane);

            this->write_postintegration_content(bundle, tab_panes);

            HTML_node output_task_pane("div");
            output_task_pane.add_attribute("id", "output").add_attribute("class", "tab-pane fade");
            this->write_output(bundle, output_task_pane);
            tab_panes.add_element(output_task_pane);

            this->write_output_content(bundle, tab_panes);

            HTML_node derived_products_pane("div");
            derived_products_pane.add_attribute("id", "derived").add_attribute("class", "tab-pane fade");
            this->write_derived_products(bundle, derived_products_pane);
            tab_panes.add_element(derived_products_pane);

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


        template <typename number, typename DatabaseType, typename PayloadType>
        HTML_node HTML_report::make_content_menu_tab(const DatabaseType& db, HTML_report_bundle<number>& bundle, std::string name)
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
                    const output_group_record<PayloadType>& rec = *group.second;
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

            HTML_node value("dd", v);

            parent.add_element(label).add_element(value);
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

            if(db.empty()) return;

            HTML_node list("div");
            list.add_attribute("class", "list-group");

            // step through all packages, writing them out as list elements
            for(const typename package_db<number>::value_type& pkg : db)
              {
                const package_record<number>& rec = *pkg.second;

                HTML_node item("a");
                item.add_attribute("href", "#" + bundle.get_id(rec)).add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
                this->make_list_item_label(rec.get_name(), item);

                // write generic repository information for this record
                this->write_generic_record(bundle, rec, item);

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");
                
                HTML_node panel_heading("div", "Model, parameters and initial conditions");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");
                
                HTML_node row("div");
                row.add_attribute("class", "row");
                
                HTML_node col1("div");
                col1.add_attribute("class", "col-md-4");
                HTML_node col1_list("dl");
                col1_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Model", rec.get_ics().get_model()->get_name(), col1_list);
                this->make_data_element("Authors", rec.get_ics().get_model()->get_author(), col1_list);

                col1.add_element(col1_list);

                HTML_node col2("div");
                col2.add_attribute("class", "col-md-4");
                HTML_node col2_list("dl");
                col2_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Initial time", format_number(rec.get_ics().get_N_initial()) + " e-folds", col2_list);
                this->make_data_element("Horizon-crossing time", format_number(rec.get_ics().get_N_horizon_crossing()) + " e-folds", col2_list);

                col2.add_element(col2_list);

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");
                HTML_node col3_list("dl");
                col3_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Citation data", rec.get_ics().get_model()->get_tag(), col3_list);

                col3.add_element(col3_list);

                row.add_element(col1).add_element(col2).add_element(col3);
                panel_body.add_element(row);
                panel.add_element(panel_heading).add_element(panel_body);
                item.add_element(panel);

                HTML_node params_column("div");
                params_column.add_attribute("class", "col-md-6 topskip");

                const parameters<number>& params = rec.get_ics().get_params();
                const std::vector<number>& param_vec = params.get_vector();
                const std::vector<std::string>& param_names = rec.get_ics().get_model()->get_param_names();

                HTML_node params_panel("div");
                params_panel.add_attribute("class", "panel panel-info");
                HTML_node params_panel_heading("div", "Parameter values");
                params_panel_heading.add_attribute("class", "panel-heading");

                HTML_node params_table("table");
                params_table.add_attribute("class", "table table-striped table-condensed");
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

                    HTML_node value("td", format_number(static_cast<double>(param_vec[i]), 6));

                    table_row.add_element(label).add_element(value);
                    params_table_body.add_element(table_row);
                  }

                params_table.add_element(params_table_body);
                params_panel.add_element(params_panel_heading).add_element(params_table);
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

                HTML_node ics_table("table");
                ics_table.add_attribute("class", "table table-striped table-condensed");
                HTML_node ics_table_body("tbody");

                for(unsigned int i = 0; i < ics_vec.size() && i < coord_names.size(); ++i)
                  {
                    HTML_node table_row("tr");

                    HTML_node label("td");
                    HTML_string label_text(coord_names[i]);
                    label_text.bold();
                    label.add_element(label_text);

                    HTML_node value("td", format_number(static_cast<double>(ics_vec[i] / params.get_Mp()), 6) + " M<sub>P</sub>");    // using tags in string is hacky, but simple

                    table_row.add_element(label).add_element(value);
                    ics_table_body.add_element(table_row);
                  }

                ics_table.add_element(ics_table_body);
                ics_panel.add_element(ics_panel_heading).add_element(ics_table);
                ics_column.add_element(ics_panel);

                data_grid.add_element(params_column).add_element(ics_column);
                item.add_element(data_grid);

                list.add_element(item);
              }

            parent.add_element(list);
          }


        template <typename number>
        void HTML_report::write_integration(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename task_db<number>::type& db = bundle.get_task_db();

            if(db.empty()) return;

            HTML_node list("div");
            list.add_attribute("class", "list-group");

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::integration)
                  {
                    const integration_task_record<number>& rec = dynamic_cast< const integration_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node item("a");
                    item.add_attribute("href", "#" + tag).add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

                    HTML_node panel("div");
                    panel.add_attribute("class", "panel panel-default");

                    HTML_node panel_heading("div", "Task information");
                    panel_heading.add_attribute("class", "panel-heading");

                    HTML_node panel_body("div");
                    panel_body.add_attribute("class", "panel-body");

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

                    // write table of output groups, if any are present
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Output groups", output_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Output groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        table_wrapper.add_attribute("class", "table-responsive");

                        HTML_node table("table");
                        table.add_attribute("class", "table table-striped table-condensed");

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
                        for(const std::string& group : output_groups)
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
                  }
              }

            parent.add_element(list);
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
            this->make_data_element("3pf configurations", boost::lexical_cast<std::string>(threepf_db.size()), col3_list);

            this->make_data_element("Integrable", tk.is_integrable() ? "Yes" : "No", col3_list);
            if(tk.is_integrable())
              {
                this->make_data_element("Voxel size", format_number(tk.voxel_size()), col3_list);
              }
          }


        template <typename number>
        void HTML_report::write_generic_task_data(HTML_report_bundle<number>& bundle, twopf_db_task<number>& tk,
                                                  HTML_node& col1_list, HTML_node& col2_list, HTML_node& col3_list)
          {
            this->make_data_element("Initial conditions", format_number(tk.get_N_initial()) + " e-folds", col1_list);
            this->make_data_element("<var>N</var><sub>*</sub>", format_number(tk.get_N_horizon_crossing()) + " e-folds", col1_list);
            this->make_data_element("Subhorizon e-folds", format_number(tk.get_N_subhorizon_efolds()) + " e-folds", col1_list);
            this->make_data_element("Maximum refinements", boost::lexical_cast<std::string>(tk.get_max_refinements()), col1_list);

            this->make_data_element("End of inflation", format_number(tk.get_N_end_of_inflation()) + " e-folds", col2_list);
            this->make_data_element("ln <var>a</var><sub>*</sub>", format_number(tk.get_astar_normalization()), col2_list);
            this->make_data_element("Adaptive ICs", tk.get_fast_forward() ? format_number(tk.get_fast_forward_efolds()) + " e-folds" : "No", col2_list);
            this->make_data_element("Collect ICs", tk.get_collect_initial_conditions() ? "Yes" : "No", col2_list);

            const time_config_database& time_db = tk.get_stored_time_config_database();
            this->make_data_element("Time samples", boost::lexical_cast<std::string>(time_db.size()), col3_list);

            const twopf_kconfig_database& twopf_db = tk.get_twopf_database();
            this->make_data_element("2pf configurations", boost::lexical_cast<std::string>(twopf_db.size()), col3_list);
          }


        template <typename number>
        void HTML_report::write_postintegration(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename task_db<number>::type& db = bundle.get_task_db();

            if(db.empty()) return;

            HTML_node list("div");
            list.add_attribute("class", "list-group");

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::postintegration)
                  {
                    const postintegration_task_record<number>& rec = dynamic_cast< const postintegration_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node item("a");
                    item.add_attribute("href", "#" + tag).add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

                    HTML_node panel("div");
                    panel.add_attribute("class", "panel panel-default");

                    HTML_node panel_heading("div", "Task information");
                    panel_heading.add_attribute("class", "panel-heading");

                    HTML_node panel_body("div");
                    panel_body.add_attribute("class", "panel-body");

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

                    // write table of output groups, if any are present
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target",
                                                                                      "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Output groups", output_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Output groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        table_wrapper.add_attribute("class", "table-responsive");

                        HTML_node table("table");
                        table.add_attribute("class", "table table-striped table-condensed");

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
                        for(const std::string& group : output_groups)
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
                  }
              }

            parent.add_element(list);
          }


        template <typename number>
        void HTML_report::write_output(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename task_db<number>::type& db = bundle.get_task_db();

            if(db.empty()) return;

            HTML_node list("div");
            list.add_attribute("class", "list-group");

            for(const typename task_db<number>::value_type& task : db)
              {
                const task_record<number>& pre_rec = *task.second;

                if(pre_rec.get_type() == task_type::output)
                  {
                    const output_task_record<number>& rec = dynamic_cast< const output_task_record<number>& >(pre_rec);
                    const std::string tag = bundle.get_id(rec);

                    HTML_node item("a");
                    item.add_attribute("href", "#" + tag).add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
                    this->make_list_item_label(rec.get_name(), item);

                    // write generic repository information for this record
                    this->write_generic_record(bundle, rec, item);

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
                        table_wrapper.add_attribute("class", "table-responsive");

                        HTML_node table("table");
                        table.add_attribute("class", "table table-striped table-condensed");

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

                    // write table of output groups
                    const std::list<std::string>& output_groups = rec.get_output_groups();
                    if(!output_groups.empty())
                      {
                        HTML_node button("button");
                        button.add_attribute("data-toggle", "collapse").add_attribute("data-target",
                                                                                      "#" + tag + "groups");
                        button.add_attribute("type", "button").add_attribute("class", "btn btn-info topskip");
                        this->make_badged_text("Output groups", output_groups.size(), button);

                        HTML_node group_list("div");
                        group_list.add_attribute("id", tag + "groups").add_attribute("class", "collapse");

                        HTML_node tbl_panel("div");
                        tbl_panel.add_attribute("class", "panel panel-info topskip");

                        HTML_node tbl_panel_head("div", "Output groups belonging to this task");
                        tbl_panel_head.add_attribute("class", "panel-heading");

                        HTML_node table_wrapper("div");
                        table_wrapper.add_attribute("class", "table-responsive");

                        HTML_node table("table");
                        table.add_attribute("class", "table table-striped table-condensed");

                        HTML_node head("thead");
                        HTML_node head_row("tr");

                        HTML_node name_label("th", "Name");
                        HTML_node created_label("th", "Created");
                        HTML_node edited_label("th", "Last updated");

                        head_row.add_element(name_label).add_element(created_label).add_element(edited_label);
                        head.add_element(head_row);

                        HTML_node body("tbody");

                        output_content_db& content_db = bundle.get_output_content_db();
                        for(const std::string& group : output_groups)
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
                  }
              }

            parent.add_element(list);
          }


        template <typename number>
        void HTML_report::write_derived_products(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            typename derived_product_db<number>::type& db = bundle.get_derived_product_db();
            typename task_db<number>::type& tk_db = bundle.get_task_db();

            if(db.empty()) return;

            HTML_node list("div");
            list.add_attribute("class", "list-group");

            for(const typename derived_product_db<number>::value_type& product : db)
              {
                const derived_product_record<number>& rec = *product.second;
                const std::string tag = bundle.get_id(rec);

                HTML_node item("a");
                item.add_attribute("href", "#" + tag).add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
                this->make_list_item_label(rec.get_name(), item);

                // write generic repository information for this record
                this->write_generic_record(bundle, rec, item);

                HTML_node panel("div");
                panel.add_attribute("class", "panel panel-default");

                HTML_node panel_heading("div", "Derived product information");
                panel_heading.add_attribute("class", "panel-heading");

                HTML_node panel_body("div");
                panel_body.add_attribute("class", "panel-body");

                HTML_node row("div");
                row.add_attribute("class", "row");

                HTML_node col1("div");
                col1.add_attribute("class", "col-md-4");
                HTML_node col1_list("dl");
                col1_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Product type", derived_data::derived_product_type_to_string(rec.get_product()->get_type()), col1_list);

                col1.add_element(col1_list);

                HTML_node col2("div");
                col2.add_attribute("class", "col-md-4");
                HTML_node col2_list("dl");
                col2_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Filename", rec.get_product()->get_filename().string(), col2_list);

                col2.add_element(col2_list);

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");

                row.add_element(col1).add_element(col2).add_element(col3);
                panel_body.add_element(row);
                panel.add_element(panel_heading).add_element(panel_body);
                item.add_element(panel);

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
                    table_wrapper.add_attribute("class", "table-responsive");

                    HTML_node table("table");
                    table.add_attribute("class", "table table-striped table-condensed");

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
                    item.add_element(button).add_element(group_list);
                  }

                list.add_element(item);
              }

            parent.add_element(list);
          }


        template <typename number>
        void HTML_report::write_integration_content(HTML_report_bundle<number>& bundle, HTML_node& parent)
          {
            integration_content_db& db = bundle.get_integration_content_db();

            for(const integration_content_db::value_type& group : db)
              {
                const output_group_record<integration_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("div");
                list.add_attribute("class", "list-group");

                HTML_node anchor("a");
                anchor.add_attribute("href", "#").add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");

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

                col2.add_element(col2_list);

                HTML_node col3("div");
                col3.add_attribute("class", "col-md-4");
                HTML_node col3_list("dl");
                col3_list.add_attribute("class", "dl-horizontal");

                this->make_data_element("Size", format_memory(payload.get_size()), col3_list);
                this->make_data_element("Shortest integration", format_time(metadata.global_min_integration_time), col3_list);
                this->make_data_element("Longest integration", format_time(metadata.global_max_integration_time), col3_list);
                this->make_data_element("Mean integration", format_time(metadata.total_integration_time / metadata.total_configurations), col3_list);

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
        void HTML_report::write_worker_table(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec, HTML_node& parent)
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

            HTML_node panel_head("div", "Worker summary for this output group");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node table_wrapper("div");
            table_wrapper.add_attribute("class", "table-responsive");

            HTML_node table("table");
            table.add_attribute("class", "table table-striped table-condensed");

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

            head_row.add_element(os_name_label);
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
                HTML_node backg_tol("td", format_number(btol.first, 2) + ", " + format_number(btol.second, 2));

                std::pair<double, double> ptol = info.get_pert_tol();
                HTML_node pert_tol("td", format_number(ptol.first, 2) + ", " + format_number(ptol.second, 2));

                HTML_node os_name("td", info.get_os_name());

                table_row.add_element(identifier).add_element(hostname).add_element(backend);
                table_row.add_element(backg_step).add_element(backg_tol);
                table_row.add_element(pert_step).add_element(pert_tol);

                if(has_statistics)
                  {
                    unsigned int count = (*counts)[std::make_pair(info.get_workgroup(), info.get_worker())];
                    double percent = 100.0 * static_cast<double>(count) / static_cast<double>(configurations);

                    HTML_node configs("td", boost::lexical_cast<std::string>(count) + " (" + format_number(percent) + "%)");
                    table_row.add_element(configs);
                  }

                table_row.add_element(os_name);
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
                                                     const output_group_record<integration_payload> rec, HTML_node& parent)
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

            HTML_node panel_head("div", "Integration analysis for this output group");
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
        void HTML_report::write_worker_chart(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
                                             HTML_node& parent, count_list& counts)
          {
            boost::filesystem::path relative_asset_loc = bundle.make_asset_directory(rec.get_name());

            boost::filesystem::path relative_script_loc = relative_asset_loc / "_worker_chart.py";
            boost::filesystem::path relative_image_loc = relative_asset_loc / "_worker_chart.png";

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
            out << "]" << '\n';


            count = 0;
            out << "label = [ ";
            for(const count_list::value_type& item : counts)
              {
                if(count > 0) out << ", ";
                out << "'" << item.first.first << "," << item.first.second << "'";
                ++count;
              }
            out << "]" << '\n';

            if(counts.size() <= 20)
              {
                out << "plt.bar(left, height, tick_label=label)" << '\n';
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
        void HTML_report::write_timing_histogram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
                                                 HTML_node& parent, timing_db& timing_data)
          {
            boost::filesystem::path relative_asset_loc = bundle.make_asset_directory(rec.get_name());

            boost::filesystem::path relative_script_loc = relative_asset_loc / "_timing_histogram.py";
            boost::filesystem::path relative_image_loc = relative_asset_loc / "_timing_histogram.png";

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
                chart.add_attribute("data-content", "Shows the distribution of integration times in this output group");
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
        void HTML_report::write_3pf_integration_analysis(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload> rec,
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
        void HTML_report::plot_scatter(HTML_report_bundle<number>& bundle, const output_group_record<Payload>& rec,
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
            out << "x = [ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";
                out << format_number(std::get<0>(item));
                ++count;
              }
            out << " ]" << '\n';

            count = 0;
            out << "y = [ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";
                out << format_number(std::get<1>(item));
                ++count;
              }
            out << " ]" << '\n';

            count = 0;
            out << "colours = [ ";
            for(const std::tuple<double, double, double>& item : dataset)
              {
                if(count > 0) out << ", ";
                out << format_number(std::get<2>(item));
                ++count;
              }
            out << " ]" << '\n';

            out << "colour_map = plt.get_cmap('autumn')" << '\n';
            out << "plt.scatter(x, y, marker='o', c=colours, cmap=colour_map, norm=col.LogNorm())" << '\n';
            out << "plt.colorbar(cmap=colour_map, label=r'" << colour_label << "')" << '\n';
            if(xlog) out << "plt.xscale('log')" << '\n';
            if(ylog) out << "plt.yscale('log')" << '\n';
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
        void HTML_report::write_time_kt_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
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

            this->plot_scatter(bundle, rec, "_timing_kt.py", "_timing_kt.png", dataset, "configuration scale $k_t$",
                               "integration time in seconds", "$k_3 / k_t$",
                               "report-chart", "Time dependence: scale",
                               "Shows the dependence of the integration time on the overall configuration scale",
                               true, true, parent);
          }


        template <typename number>
        void HTML_report::write_time_alpha_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
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

            this->plot_scatter(bundle, rec, "_timing_alpha.py", "_timing_alpha.png", dataset, "shape parameter $\\alpha$",
                               "integration time in seconds", "$k_t$",
                               "report-chart", "Time dependence: shape",
                               "Shows the dependence of the integration time on the shape parameter alpha",
                               false, true, parent);
          }


        template <typename number>
        void HTML_report::write_time_beta_diagram(HTML_report_bundle<number>& bundle, const output_group_record<integration_payload>& rec,
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

            this->plot_scatter(bundle, rec, "_timing_beta.py", "_timing_beta.png", dataset, "shape parameter $k_3 / k_t$",
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
                const output_group_record<postintegration_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("div");
                list.add_attribute("class", "list-group");

                HTML_node anchor("a");
                anchor.add_attribute("href", "#").add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");

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

            for(const output_content_db::value_type& group : db)
              {
                const output_group_record<output_payload>& rec = *group.second;
                std::string tag = bundle.get_id(rec);

                HTML_node pane("div");
                pane.add_attribute("id", tag).add_attribute("class", "tab-pane fade");

                HTML_node list("div");
                list.add_attribute("class", "list-group");

                HTML_node anchor("a");
                anchor.add_attribute("href", "#").add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");

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
                        it.add_attribute("href", "#").add_attribute("class", "list-group-item").add_attribute("onclick", "return false;");
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
                        else if(extension.string() == ".txt")
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
                            std::string group_tag = tag + "_" + boost::lexical_cast<std::string>(count);
                            ++count;

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
                            table_wrapper.add_attribute("class", "table-responsive");

                            HTML_node table("table");
                            table.add_attribute("class", "table table-striped table-condensed");

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
                unsigned int count = 0;
                for(const std::string& tag : tags)
                  {
                    if(count > 0)
                      {
                        HTML_string pad("&nbsp;");
                        parent.add_element(pad);
                      }

                    HTML_node tg("span", tag);
                    tg.add_attribute("class", "label label-info");

                    parent.add_element(tg);
                    ++count;
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

            this->make_data_element("Runtime version", boost::lexical_cast<std::string>(rec.get_runtime_API_version()), col3_list);

            col3.add_element(col3_list);

            row.add_element(col1).add_element(col2).add_element(col3);
            panel_body.add_element(row);

            this->write_activity_collapsible(rec.get_history(), bundle.get_id(rec), panel_body);

            panel.add_element(panel_heading).add_element(panel_body);
            parent.add_element(panel);
          }


        template <typename number, typename Payload>
        void HTML_report::write_generic_output_record(HTML_report_bundle<number>& bundle,
                                                      const output_group_record<Payload>& rec, HTML_node& parent)
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

            this->make_data_element("Runtime version", boost::lexical_cast<std::string>(rec.get_runtime_API_version()), col3_list);
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
                this->write_task_button(bundle, payload.get_seed_group(), parent);
              }
            else
              {
                HTML_node no("span", "No");
                parent.add_element(no);
              }
          }


        template <typename number>
        void HTML_report::write_JavaScript_button_scroll(HTML_report_bundle<number>& bundle, std::string button, std::string pane, std::string element)
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
                std::string element_id = bundle.get_id(*t->second);
                std::string button_id = this->make_button_tag();

                HTML_node button("button");
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);
                this->make_badged_text(name, t->second->get_output_groups().size(), button);

                parent.add_element(button);

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

                this->write_JavaScript_button_scroll(bundle, button_id, pane, element_id);
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
        void HTML_report::write_package_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& element, HTML_node& parent)
          {
            if(!element.empty())
              {
                std::string button_id = this->make_button_tag();

                HTML_node button("button", name);
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);

                parent.add_element(button);

                this->write_JavaScript_button_scroll(bundle, button_id, "packages", element);
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

            std::string element;
            if(t != db.end()) element = bundle.get_id(*t->second);

            this->write_derived_product_button(bundle, name, element, parent);
          }


        template <typename number>
        void HTML_report::write_derived_product_button(HTML_report_bundle<number>& bundle, const std::string& name, const std::string& element, HTML_node& parent)
          {
            if(!element.empty())
              {
                std::string button_id = this->make_button_tag();

                HTML_node button("button", name);
                button.add_attribute("type", "button").add_attribute("class", "btn btn-link").add_attribute("id", button_id);

                parent.add_element(button);

                this->write_JavaScript_button_scroll(bundle, button_id, "derived", element);
              }
            else
              {
                HTML_node fail("span", name);
                fail.add_attribute("class", "label label-danger");

                parent.add_element(fail);
              }
          }


        void HTML_report::write_notes_collapsible(const std::list<std::string>& notes, const std::string& tag, HTML_node& parent)
          {
            if(notes.empty()) return;

            HTML_node button("button");
            button.add_attribute("type", "button").add_attribute("class", "btn btn-info");
            button.add_attribute("data-toggle", "collapse").add_attribute("data-target", "#" + tag + "notes");
            this->make_badged_text("Notes", notes.size(), button);

            HTML_node content("div");
            content.add_attribute("id", tag + "notes").add_attribute("class", "collapse");

            HTML_node panel("div");
            panel.add_attribute("class", "panel panel-info topskip");

            HTML_node panel_head("div", "Notes attached to this record");
            panel_head.add_attribute("class", "panel-heading");

            HTML_node list("ol");
            list.add_attribute("class", "list-group");

            for(const std::string& note : notes)
              {
                HTML_node item("li", note);
                item.add_attribute("class", "list-group-item");
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
            panel.add_attribute("class", "panel panel-info topskip");

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
            tag << CPPTRANSPORT_HTML_BUTTON_TAG_PREFIX << this->button_id++;
            return(tag.str());
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_REPORTING_HTML_REPORT_H
