//
// Created by David Seery on 13/05/2014.
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


#ifndef CPPTRANSPORT_PRODUCT_LINE_PLOT2D_H
#define CPPTRANSPORT_PRODUCT_LINE_PLOT2D_H


#include <iostream>
#include <fstream>
#include <functional>
#include <cstdlib>

#include <unordered_map>
#include <unordered_set>

#include "transport-runtime/derived-products/line-collections/line_collection.h"
#include "transport-runtime/derived-products/line-collections/data_line.h"

#include "transport-runtime/utilities/plot_environment.h"

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/log/utility/formatting_ostream.hpp"
#include "line_collection.h"


namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT          = "line-plot2d";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX      = "reverse-x";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY      = "reverse-y";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL        = "x-label";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT   = "x-label-text";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL        = "y-label";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT   = "y-label-text";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE         = "title";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT    = "title-text";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND        = "legend";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS    = "legend-position";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND   = "dash-second-axis";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_X_LABEL_SET   = "x-label-set";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_Y_LABEL_SET   = "y-label-set";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_SET     = "title-set";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR     = "top-right";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR     = "bottom-right";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL     = "bottom-left";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL     = "top-left";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R      = "right";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR     = "centre-right";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL     = "centre-left";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC     = "upper-centre";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC     = "lower-centre";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C      = "centre";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX = "typeset-with-latex";

        //! A line_plot2d is a specialization of a line_collection that
		    //! produces a plot of derived data lines against an axis.


		    namespace line_plot2d_impl
          {

            class script_data_package
              {
              public:

                //! constructor captures filenames and output stream
                script_data_package(std::ofstream s, boost::filesystem::path p, boost::filesystem::path f)
                  : out{std::move(s)},
                    plot_file{std::move(p)},
                    script_file{std::move(f)}
                  {
                  }

                //! move constructor
                script_data_package(script_data_package&& rhs)
                  : out{std::move(rhs.out)},
                    plot_file(std::move(rhs.plot_file)),
                    script_file(std::move(rhs.script_file))
                  {
                  }

                //! destructor should close output
                ~script_data_package()
                  {
                    if(this->out.is_open()) this->out.close();
                  }


                // ACCESS API

              public:

                //! get output stream
                std::ofstream& get_stream() { return this->out; }

                //! get plot file path
                const boost::filesystem::path& get_plot_path() const { return this->plot_file; }

                //! get script file path
                const boost::filesystem::path& get_script_path() const { return this->script_file; }


                // CLOSE STREAM

              public:

                //! close output stream
                void close() { if(this->out.is_open()) this->out.close(); }


                // STATUS

              public:

                //! allow explicit conversion too bool
                explicit operator bool() const { return static_cast<bool>(this->out); }

                //! query good() of underlying stream
                bool good() const { return this->out.good(); }

                //! query eof() of underlying stream
                bool eof() const { return this->out.eof(); }

                //! query fail() of underlying stream
                bool fail() const { return this->out.fail(); }


                // INTERNAL DATA

              private:

                //! output stream
                std::ofstream out;

                //! plot file path
                const boost::filesystem::path plot_file;

                //! script file path (maybe the same as plot file path if output is a script)
                const boost::filesystem::path script_file;
              };


            class line_id
              {
              public:

                //! constructor captures line id, label, and type
                line_id(unsigned int i, std::string l, data_line_type t)
                  : id{i},
                    label{std::move(l)},
                    type(t)
                  {
                  }

                //! destructor is default
                ~line_id() = default;


                // ACCESS API

              public:

                //! get id
                unsigned int get_id() const { return this->id; }

                //! get label
                const std::string& get_label() const { return this->label; }

                //! get type
                data_line_type get_type() const { return this->type; }


                // INTERNAL DATA

              private:

                //! line id
                const unsigned int id;

                //! label
                const std::string label;

                //! line type
                const data_line_type type;
              };

            using line_id_set = std::list<line_id>;
            using binned_plot_line_set = std::unordered_map< value_type, line_id_set >;

          }   // namespace line_plot2d_impl


		    template <typename number>
		    class line_plot2d: public line_collection<number>
			    {

		        // LINE_PLOT: CONSTRUCTOR, DESTRUCTOR

		      public:

						//! Basic user-facing constructor.
				    //! A line_plot2d is an instance of a line_collection; we limit
				    //! the maximum number of value types in the collection to two,
				    //! because that's as many different y-axes as we can have on the plot.
		        line_plot2d(const std::string& name, const boost::filesystem::path& filename)
		          : line_collection<number>(name, filename),
								reverse_x(false),
								reverse_y(false),
								x_label(false),
								y_label(false),
								title(false),
								legend(false),
								position(legend_pos::top_right),
								typeset_with_LaTeX(false),
								dash_second_axis(true),
								x_label_set(false),
								y_label_set(false),
								title_set(false)
			        {
								if(this->filename.extension().string() != ".pdf" &&
									 this->filename.extension().string() != ".png" &&
									 this->filename.extension().string() != ".ps" &&
									 this->filename.extension().string() != ".eps" &&
									 this->filename.extension().string() != ".svg" &&
									 this->filename.extension().string() != ".svgz" &&
									 this->filename.extension().string() != ".py")
									{
								    std::ostringstream msg;
										msg << CPPTRANSPORT_PRODUCT_LINE_PLOT2D_UNSUPPORTED_FORMAT << " " << filename.extension();
										throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
									}
			        }

				    //! Deserialization constructor
		        line_plot2d(const std::string& name, Json::Value& reader, task_finder<number> finder);

		        virtual ~line_plot2d() = default;


            // INTERFACE -- DERIVED PRODUCT

          public:

            //! Get type of this derived data product
            derived_product_type get_type() const override final { return(derived_product_type::line_2dplot); }


            // LINE MANAGEMENT

		      public:

		        //! Add a line to the collection
		        line_plot2d<number>& add_line(const derived_line<number>& line) override
              {
                this->line_collection<number>::add_line(line);
                this->apply_default_labels(!this->x_label_set, !this->y_label_set, !this->title_set);
                return *this;
              }

            //! overload += to do the same thing
            line_plot2d<number>& operator+=(const derived_line<number>& line) override { return this->add_line(line); }

            //! add a vector of derived lines (eg. produced by operator+ overload between derived lines)
            line_plot2d<number>& operator+=(const std::vector< std::shared_ptr< derived_line<number> > > list) override
              {
                this->line_collection<number>::operator+=(list);
                this->apply_default_labels(!this->x_label_set, !this->y_label_set, !this->title_set);
                return *this;
              }


		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels(bool x_label_set, bool y_label_set, bool title_set) {}

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() {}


				    // GENERATE PLOT -- implements a 'derived_product' interface

		      public:

				    //! Generate our derived output
				    content_group_name_set derive(datapipe<number>& pipe, const tag_list& tags,
                                          slave_message_buffer& messages, local_environment& env, argument_cache& args) override;


		      protected:

            //! Make plot
				    bool make_plot(datapipe<number>& pipe,
                           const typename line_collection<number>::binned_line_set& data,
                           local_environment& env, argument_cache& args) const;

            //! Internal API: write header for plot script
            line_plot2d_impl::script_data_package write_plot_script_header
              (const datapipe <number>& pipe, argument_cache& args, local_environment& env) const;

            //! Internal API: write footer for plot script
            void write_plot_script_footer(line_plot2d_impl::script_data_package& data) const;

            //! Internal API: write out value arrays into the plot script
            void write_line_value_arrays
              (line_plot2d_impl::script_data_package& pkg, const typename line_collection<number>::output_axis& axis,
               const line_plot2d_impl::line_id& line, const output_data_series& data_series) const;

            //! generate a plot line from a specified axis object
            void write_line_emplacement
              (line_plot2d_impl::script_data_package& pkg, unsigned int bin_number, const line_plot2d_impl::line_id& line,
               const std::string& axis_object, const std::string& line_style) const;

            //! Internal API: set plot properties
            // TODO: should record axis names separately and pass the list to this function, rather than
            //  having it 'guess' the names itself
            void write_plot_properties
              (line_plot2d_impl::script_data_package& pkg, unsigned int bins) const;

            //! Internal API: write plot legend
            // TODO: should record axis names separately and pass the list to this function, rather than
            //  having it 'guess' the names itself
            void write_plot_legend
              (line_plot2d_impl::script_data_package& pkg, unsigned int bins) const;

            //! Internal API: write plot labels
            // TODO: should record axis names separately and pass the list to this function, rather than
            //  having it 'guess' the names itself
            void write_plot_labels
              (line_plot2d_impl::script_data_package& pkg, const line_plot2d_impl::binned_plot_line_set& bins) const;

            //! Internal API: execute a completed script to generate a plot
            bool convert_script_to_plot
              (line_plot2d_impl::script_data_package& pkg, local_environment& env) const;


		        // GET AND SET BASIC PLOT ATTRIBUTES

		      public:

		        //! get reverse x-axis setting
		        bool get_reverse_x() const { return(this->reverse_x); }

		        //! set reverse x-axis setting
		        line_plot2d<number>& set_reverse_x(bool g) { this->reverse_x = g; return *this; }

		        //! get reverse y-axis setting
		        bool get_reverse_y() const { return(this->reverse_y); }

		        //! set reverse y-axis setting
            line_plot2d<number>& set_reverse_y(bool g) { this->reverse_y = g; return *this; }

		        //! get use x-axis label setting
		        bool get_x_label() const { return(this->x_label); }

            //! set use x-axis label setting
            line_plot2d<number>& set_x_label(bool g) { this->x_label = g; return *this; }

            //! get x-axis label text
            const std::string& get_x_label_text() const { return(this->x_label_text); }

		        //! set x-axis label text
            line_plot2d<number>& set_x_label_text(const std::string& text) { this->internal_set_x_label_text(text); this->x_label_set = true; return *this; }

		        //! clear x-axis label text
            line_plot2d<number>& clear_x_label_text() { this->internal_clear_x_label_text(); this->x_label_set = true; return *this; }

		      protected:

						void internal_set_x_label_text(const std::string& text)
							{
						    if(text.empty())  // assume intended to suppress x-axis label
							    {
						        this->x_label_text.clear();
						        this->x_label = false;
							    }
						    else
							    {
						        this->x_label_text = text;
						        this->x_label = true;
							    }
							}
				    void internal_clear_x_label_text() { this->x_label_text.clear(); }

		      public:

		        //! get use y-axis label setting
		        bool get_y_label() const { return(this->y_label); }

		        //! set use y-axis label setting
            line_plot2d<number>& set_y_label(bool g) { this->y_label = g; return *this; }

		        //! get y-axis label text
		        const std::string& get_y_label_text() const { return(this->y_label_text); }

		        //! set y-axis label text
            line_plot2d<number>& set_y_label_text(const std::string& text) { this->internal_set_y_label_text(text); this->y_label_set = true; return *this; }

		        //! clear y-axis label text
            line_plot2d<number>& clear_y_label_text() { this->internal_clear_y_label_text(); this->y_label_set = true; return *this; }

		      protected:

				    void internal_set_y_label_text(const std::string& text)
					    {
				        if(text.empty())  // assume intended to suppress y-axis label
					        {
				            this->y_label_text.clear();
				            this->y_label = false;
					        }
				        else
					        {
				            this->y_label_text = text;
				            this->y_label = true;
					        }
					    }
				    void internal_clear_y_label_text() { this->y_label_text.clear(); }

		      public:

		        //! get use title setting
		        bool get_title() const { return(this->title); }

		        //! set use title setting
            line_plot2d<number>& set_title(bool g) { this->title = g; return *this; }

		        //! get title text
		        const std::string& get_title_text() const { return(this->title_text); }

		        //! set title text
            line_plot2d<number>& set_title_text(const std::string& text) { this->internal_set_title_text(text); this->title_set = true; return *this; }

				    //! clear title text
            line_plot2d<number>& clear_title_text() { this->internal_clear_title_text(); this->title_set = true; return *this; }

		      protected:

				    void internal_set_title_text(const std::string& text)
					    {
				        if(text.empty())    // assume intended to suppress the title
					        {
				            this->title_text.clear();
				            this->title = false;
					        }
				        else
					        {
				            this->title_text = text;
				            this->title = true;
					        }
					    }
				    void internal_clear_title_text() { this->title_text.clear(); }

		      public:

				    //! get legend setting
				    bool get_legend() const { return(this->legend); }

				    //! set legend setting
            line_plot2d<number>& set_legend(bool g) { this->legend = g; return *this; }

				    //! get legend position
				    legend_pos get_legend_position() const { return(this->position); }

				    //! set legend position
            line_plot2d<number>& set_legend_position(legend_pos pos) { this->position = pos; return *this;}

				    //! get typeset with LaTeX setting
				    bool get_typeset_with_LaTeX() const { return(this->typeset_with_LaTeX); }

				    //! set typeset with LaTeX setting
            line_plot2d<number>& set_typeset_with_LaTeX(bool g) { this->typeset_with_LaTeX = g; this->apply_default_labels(!this->x_label_set, !this->y_label_set, !this->title_set); return *this; }

				    //! get dash second axis
				    bool get_dash_second_axis() const { return(this->dash_second_axis); }

				    //! set dash second axis
            line_plot2d<number>& set_dash_second_axis(bool g) { this->dash_second_axis = g; return *this; }


				    // CLONE

		      public:

				    //! Copy this object
				    line_plot2d<number>* clone() const override { return new line_plot2d<number>(static_cast<const line_plot2d<number>&>(*this)); }


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    void serialize(Json::Value& writer) const override;


				    // WRITE SELF TO A STANDARD STREAM

		      public:

				    template <typename Stream> void write(Stream& out);


		        // INTERNAL DATA

		      protected:

				    // STYLE ATTRIBUTES

				    //! reverse x-axis?
				    bool reverse_x;

				    //! reverse y-axis?
				    bool reverse_y;

		        //! generate an x-axis label?
		        bool x_label;

				    //! x-label text
		        std::string x_label_text;

		        //! generate a y-axis label?
		        bool y_label;

				    //! y-label text
		        std::string y_label_text;

		        //! generate a title?
		        bool title;

				    //! title text
		        std::string title_text;

		        //! add a legend?
		        bool legend;

				    //! location of legend
				    legend_pos position;

		        //! use LaTeX to typeset labels?
		        bool typeset_with_LaTeX;

				    //! uses dashes for lines plotted on second y-axis?
				    bool dash_second_axis;

				    //! which labels have been explicitly set?
				    bool x_label_set;
				    bool y_label_set;
				    bool title_set;

          };


				template <typename number>
		    line_plot2d<number>::line_plot2d(const std::string& name, Json::Value& reader, task_finder<number> finder)
			    : line_collection<number>(name, reader, finder)
			    {
				    reverse_x        = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX].asBool();
				    reverse_y        = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY].asBool();
				    x_label          = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL].asBool();
				    x_label_text     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT].asString();
				    y_label          = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL].asBool();
				    y_label_text     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT].asString();
				    title            = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE].asBool();
				    title_text       = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT].asString();
				    legend           = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND].asBool();
				    dash_second_axis = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND].asBool();
						x_label_set      = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_X_LABEL_SET].asBool();
						y_label_set      = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_Y_LABEL_SET].asBool();
						title_set        = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_SET].asBool();

		        std::string leg_pos = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS].asString();

		        if     (leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL) position = legend_pos::top_left;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR) position = legend_pos::top_right;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL) position = legend_pos::bottom_left;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR) position = legend_pos::bottom_right;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R)  position = legend_pos::right;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR) position = legend_pos::centre_right;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL) position = legend_pos::centre_left;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC) position = legend_pos::lower_centre;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC) position = legend_pos::upper_centre;
		        else if(leg_pos == CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C)  position = legend_pos::centre;
		        else
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_PRODUCT_LINE_PLOT2D_UNKNOWN_LEG_POS << " '" << leg_pos << "'";
		            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
			        }

				    typeset_with_LaTeX = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX].asBool();
			    }


				template <typename number>
        content_group_name_set line_plot2d<number>::derive(datapipe<number>& pipe, const tag_list& tags,
                                                           slave_message_buffer& messages, local_environment& env, argument_cache& args)
					{
            slave_message_context ctx(messages, this->name);

						// generate output from our constituent lines, writing any errors messages into the
						// slave message buffer
						auto data_lines = this->obtain_output(pipe, tags, messages);

						// merge this output onto a single axis
						// this turns our collection of data_lines into a collection of output_lines.
						// The functionality isn't really needed for line_plot2d (we don't have to plot every line
						// using the same x-axis), so later we'll strip out any masked values this generates
				    auto merged_lines = this->merge_lines(pipe, data_lines);

						// bin these lines into groups of the same value type, restricting to a maximum of two bins
						// TODO: matplotlib does not seem to have a maximum of two bins; it seems that multiple calls to
						//  twinx() are allowed, each of which generates a new y-axis with shared x-axis. So apparently there
						//  is no bar to having more than 2 y-value types, except space on the plot for all the y-axis scales.
						//  This means we should eventually remove this restriction on the number of bins.
				    auto binned_lines = this->bin_lines(merged_lines, messages, 2);

						// generate plot
				    bool success = this->make_plot(pipe, binned_lines, env, args);

            // get content groups which were used
            content_group_name_set used_groups = this->extract_content_groups(data_lines);

						// commit product (even if the plot failed to generate and we are committing the script - the next line
						// will prevent the whole content group being committed)
						pipe.commit(this, used_groups);

						if(!success) throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_PYTHON_FAIL);
            return(used_groups);
					}


        // data is a set of bins, with all lines in a bin sharing a common value type
				template <typename number>
				bool line_plot2d<number>::make_plot
				  (datapipe<number>& pipe, const typename line_collection<number>::binned_line_set& data,
           local_environment& env, argument_cache& args) const
					{
            line_plot2d_impl::script_data_package pkg{this->write_plot_script_header(pipe, args, env)};

            unsigned int current_id = 0;

						line_plot2d_impl::binned_plot_line_set plot_lines;

            const auto& axis = data.first;
            const auto& bins = data.second;   // type line_collection<number>::binned_lines

            // loop through all bins, writing out arrays representing the values in each line, and recording
						// which arrays correspond to which bin
						for(const auto& bin : bins)
              {
                const value_type& label = bin.first;
                const auto& lines = bin.second;         // type line_collection<number>::output_line_ref_set;

                auto& plot_bin = plot_lines[label];

                for(const auto& line : lines)      // line has type output_line_set::const_iterator, so points to instance of output_line
                  {
                    const auto this_line = plot_bin.emplace(plot_bin.end(), current_id++, line->get_label(), line->get_data_line_type());

                    const output_data_series& line_data = line->get_values();
                    assert(line_data.size() == axis.size());

                    this->write_line_value_arrays(pkg, axis, *this_line, line_data);
                  }
              }

						// now allocate lines to axes by issuing suitable plot directives
						unsigned int bin_number = 0;
				    auto& out = pkg.get_stream();
				    for(const auto& bin : plot_lines)
              {
                ++bin_number;
                std::string axis_name;
                std::string line_style;

                if(bin_number == 1)
                  {
                    out << "ax1 = plt.gca()" << '\n';
                    axis_name = "ax1";
                    line_style = "-";
                  }
                if(bin_number == 2)
                  {
                    out << "ax2 = ax1.twinx()" << '\n';
                    axis_name = "ax2";
                    if(this->dash_second_axis) line_style = "--"; else line_style = "-";
                  }

                const value_type& label = bin.first;
                const auto& lines = bin.second;   // type line_id_set

                for(const auto& line : lines)
                  {
                    this->write_line_emplacement(pkg, bin_number, line, axis_name, line_style);
                  }
              }

            // TODO: record axis names separately and pass that this to write_plot_properties(),
            //  write_plot_legend(), write_plot_labels(), rather than having them 'guess' the names of the axes
            this->write_plot_properties(pkg, plot_lines.size());
				    this->write_plot_legend(pkg, plot_lines.size());
            this->write_plot_labels(pkg, plot_lines);

						this->write_plot_script_footer(pkg);

				    return this->convert_script_to_plot(pkg, env);
					}


        template <typename number>
        line_plot2d_impl::script_data_package
        line_plot2d<number>::write_plot_script_header
          (const datapipe <number>& pipe, argument_cache& args, local_environment& env) const
          {
            // extract paths from the datapipe
            boost::filesystem::path temp_root = pipe.get_abs_tempdir_path();

            // obtain path for plot output
            boost::filesystem::path plot_file = temp_root / this->filename;

            boost::filesystem::path script_file = plot_file;

            // script file should have extension .py
            if(script_file.extension() != ".py")
              {
                script_file.replace_extension(".py");
              }

            // create script file
            std::ofstream out(script_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

            if(!out.is_open() || out.fail())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_DERIVED_PRODUCT_FAILED << " " << plot_file;
                throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
              }

            out << "import numpy as np" << '\n';
            plot_environment plot_env(env, args);
            plot_env.write_environment(out);

            if(typeset_with_LaTeX)
              {
                out << "plt.rc('text', usetex=True)" << '\n';
                out << "plt.rc('font',**{'family':'serif','serif':['Computer Modern Roman']})" << '\n';
                out << "plt.rc('font',**{'family':'sans-serif','sans-serif':['Computer Modern Sans serif']})" << '\n';
              }

            out << "plt.figure()" << '\n';

            return {std::move(out), plot_file, script_file};
          }


        template <typename number>
        void line_plot2d<number>::write_plot_script_footer(line_plot2d_impl::script_data_package& pkg) const
          {
            auto& out = pkg.get_stream();

            const auto& plot_file = pkg.get_plot_path();

            if(this->title) out << "plt.title(r'" << this->title_text << "')" << '\n';

            if(plot_file.extension() != ".py")
              {
                out << "plt.savefig('" << plot_file.string() << "')" << '\n';
              }
            else
              {
                boost::filesystem::path temp = plot_file;
                temp.replace_extension(".pdf");
                out << "plt.savefig('" << temp.string() << "')" << '\n';
              }
            out << "plt.close()" << '\n';
          }


        template <typename number>
        bool line_plot2d<number>::convert_script_to_plot
          (line_plot2d_impl::script_data_package& pkg, local_environment& env) const
          {
            bool rval = true;

            // close output stream
            pkg.close();

            const auto& plot_file = pkg.get_plot_path();
            const auto& script_file = pkg.get_script_path();

            // if output format wasn't Python, try to execute the script
            if(plot_file.extension() != ".py")
              {
                // determine if local environment has a Python interpreter
                bool success = env.has_python();

                // if yes, hand off to local environment to execute
                if(success) success = env.execute_python(script_file) == 0;

                // remove python script if worked ok, otherwise move script to destination and mark as failed;
                // caller may choose to throw an exception
                if(success)
                  {
                    boost::filesystem::remove(script_file);
                  }
                else
                  {
                    if(boost::filesystem::exists(plot_file)) boost::filesystem::remove(plot_file);
                    boost::filesystem::rename(script_file, plot_file);
                    rval = false;
                  }
              }

            return rval;
          }


        template <typename number>
        void line_plot2d<number>::write_line_value_arrays
          (line_plot2d_impl::script_data_package& pkg, const typename line_collection<number>::output_axis& axis,
           const line_plot2d_impl::line_id& line, const output_data_series& data_series) const
          {
            auto& out = pkg.get_stream();
            const auto id = line.get_id();

            // write x-array (we write one per line because we don't know which lines share
            // x-axis sample points)
            out << "x" << id << " = [ ";

            bool first = true;
            auto x = axis.cbegin();
            for(auto w = data_series.cbegin(); w != data_series.cend() && x != axis.cend(); ++w, ++x)
              {
                if(w->is_present())
                  {
                    out << (first ? "" : ", ") << *x;
                    first = false;
                  }
              }
            out << " ]" << '\n';

            // write y-array
            out << "y" << id << " = [ ";

            first = true;
            x = axis.cbegin();
            for(auto w = data_series.cbegin(); w != data_series.cend() && x != axis.cend(); ++w, ++x)
              {
                if(w->is_present())
                  {
                    out << (first ? "" : ", ");
                    w->format_python(out);
                    first = false;
                  }
              }

            out << " ]" << '\n';
          }


        template <typename number>
        void line_plot2d<number>::write_line_emplacement
          (line_plot2d_impl::script_data_package& pkg, unsigned int bin_number, const line_plot2d_impl::line_id& line,
           const std::string& axis_object, const std::string& line_style) const
          {
            auto& out = pkg.get_stream();

            const auto& id = line.get_id();
            const auto& type = line.get_type();
            const auto& label = line.get_label();

            // call suitable method from axis object to assign ln<id> to a plot line
            out << "ln" << id << " = " << axis_object << ".";

            // decide which method to call based on line type
            switch(type)
              {
                case data_line_type::continuous_data:
                  {
                    out << "errorbar";
                    break;
                  }

                case data_line_type::scattered_data:
                  {
                    out << "plot";
                    break;
                  }
              }

            out << "(x" << id << ", y" << id;

            if(type == data_line_type::scattered_data) out << ", 'o'";

            out << ", label=r'" << label << "'";
            out << ", linestyle='" << line_style << "'";

            out << ")" << '\n';
          }


        template <typename number>
        void line_plot2d<number>::write_plot_properties
          (line_plot2d_impl::script_data_package& pkg, unsigned int bins) const
          {
            auto& out = pkg.get_stream();

            // x-axis properties should be set only for the main axis
            if(this->reverse_x) out << "ax1.set_xlim(ax.get_xlim()[::-1])" << '\n';

            // y-axis properties should be set for main axis and each twinx() axis
            if(this->reverse_y)
              {
                for(unsigned int i = 1; i <= bins; ++i)
                  {
                    out << "ax" << i << ".set_ylim(ax.get_ylim()[::-1])" << '\n';
                  }
              }
            if(this->log_x) out << "ax1.set_xscale('log')" << '\n';
            if(this->log_y)
              {
                for(unsigned int i = 1; i <= bins; ++i)
                  {
                    out << "ax" << i << ".set_yscale('log')" << '\n';
                  }
              }
          }


        template <typename number>
        void line_plot2d<number>::write_plot_legend
          (line_plot2d_impl::script_data_package& pkg, unsigned int bins) const
          {
            auto& out = pkg.get_stream();

            if(this->legend)
              {
                for(unsigned int i = 1; i <= bins; ++i)
                  {
                    out << "handles" << i << ", labels" << i << " = ax" << i << ".get_legend_handles_labels()" << '\n';

                    out << "handles = ";
                    if(i > 1) out << "handles + ";
                    out << "handles" << i << '\n';

                    out << "labels = ";
                    if(i > 1) out << "labels + ";
                    out << "labels" << i << '\n';
                  }

                out << "ax1.legend(handles, labels, frameon=False, loc=";
                switch(this->position)
                  {
                    case legend_pos::top_right:    out << "1"; break;
                    case legend_pos::top_left:     out << "2"; break;
                    case legend_pos::bottom_left:  out << "3"; break;
                    case legend_pos::bottom_right: out << "4"; break;
                    case legend_pos::right:        out << "5"; break;
                    case legend_pos::centre_left:  out << "6"; break;
                    case legend_pos::centre_right: out << "7"; break;
                    case legend_pos::upper_centre: out << "8"; break;
                    case legend_pos::lower_centre: out << "9"; break;
                    case legend_pos::centre:       out << "10"; break;
                  }
                out << ")" << '\n';
              }
          }


        template <typename number>
        void line_plot2d<number>::write_plot_labels
          (line_plot2d_impl::script_data_package& pkg, const line_plot2d_impl::binned_plot_line_set& bins) const
          {
            auto& out = pkg.get_stream();

            if(this->x_label) out << "ax1.set_xlabel(r'" << this->x_label_text << "')" << '\n';

            if(this->y_label)
              {
                // if there is user-set text, apply this to the first axis;
                // otherwise, use default labels for each value axis
                if(this->y_label_text.length() > 0)
                  {
                    out << "ax1.set_ylabel(r'" << this->y_label_text << "')" << '\n';
                  }
                else
                  {
                    unsigned int i = 0;
                    for(const auto& bin : bins)
                      {
                        ++i;
                        const auto& bin_label = bin.first;

                        auto label = this->use_LaTeX ? value_type_to_string_LaTeX(bin_label) : value_type_to_string_non_LaTeX(bin_label);

                        out << "ax" << i << ".set_ylabel(r'" << label << "')" << '\n';
                      }
                  }
              }

          }


        template <typename number>
		    void line_plot2d<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_DERIVED_PRODUCT_TYPE] = std::string(CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D);

		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX]    = this->reverse_x;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY]    = this->reverse_y;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL]      = this->x_label;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT] = this->x_label_text;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL]      = this->y_label;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT] = this->y_label_text;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE]       = this->title;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT]  = this->title_text;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND]      = this->legend;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND] = this->dash_second_axis;
				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_X_LABEL_SET] = this->x_label_set;
				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_Y_LABEL_SET] = this->y_label_set;
				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_SET]   = this->title_set;

		        switch(this->position)
			        {
		            case legend_pos::top_left:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL);
		              break;

		            case legend_pos::top_right:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR);
		              break;

		            case legend_pos::bottom_left:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL);
		              break;

		            case legend_pos::bottom_right:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR);
		              break;

		            case legend_pos::right:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R);
		              break;

		            case legend_pos::centre_right:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR);
		              break;

		            case legend_pos::centre_left:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL);
		              break;

		            case legend_pos::upper_centre:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC);
		              break;

		            case legend_pos::lower_centre:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC);
		              break;

                case legend_pos::centre:
			            writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C);
		              break;
			        }

		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX] = this->typeset_with_LaTeX;

		        // call next serialization
		        this->line_collection<number>::serialize(writer);
			    }


		    template <typename number>
        template <typename Stream>
		    void line_plot2d<number>::write(Stream& out)
			    {
		        // call next writer
		        this->line_collection<number>::write(out);

		        unsigned int count = 0;

		        this->wrapper.wrap_list_item(out, this->reverse_x, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_REVERSEX, count);
		        this->wrapper.wrap_list_item(out, this->reverse_y, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_REVERSEY, count);

		        this->wrapper.wrap_list_item(out, this->x_label, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_XLABEL, count);
		        if(this->x_label) this->wrapper.wrap_value(out, this->x_label_text, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);

		        this->wrapper.wrap_list_item(out, this->y_label, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_YLABEL, count);
		        if(this->y_label) this->wrapper.wrap_value(out, this->y_label_text, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);

		        this->wrapper.wrap_list_item(out, this->title, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_TITLE, count);
		        if(this->title) this->wrapper.wrap_value(out, this->title_text, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);

		        this->wrapper.wrap_list_item(out, this->legend, CPPTRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LEGEND, count);

		        if(this->legend)
			        {
		            if(count > 0) out << ", ";
		            switch(this->position)
			            {
		                case legend_pos::top_left:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL;
		                  break;
		                case legend_pos::top_right:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR;
		                  break;
		                case legend_pos::bottom_left:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL;
		                  break;
		                case legend_pos::bottom_right:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR;
		                  break;
		                case legend_pos::right:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R;
		                  break;
		                case legend_pos::centre_right:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR;
		                  break;
		                case legend_pos::centre_left:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL;
		                  break;
		                case legend_pos::upper_centre:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC;
		                  break;
		                case legend_pos::lower_centre:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC;
		                  break;
                    case legend_pos::centre:
			                out << CPPTRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C;
		                  break;
			            }
			        }
			    }


		    template <typename number, typename Char, typename Traits>
		    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, line_plot2d<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


        template <typename number, typename Char, typename Traits, typename Allocator>
        boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, line_plot2d<number>& obj)
          {
            obj.write(out);
            return(out);
          }


			}   // namespace derived_data

	}   // namespace transport


#endif // CPPTRANSPORT_PRODUCT_LINE_PLOT2D_H
